/* Copyright 2014 Google Inc. All Rights Reserved.

   Distributed under MIT license.
   See file LICENSE for detail or copy at https://opensource.org/licenses/MIT
*/

/* Command line interface for Brotli library. */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#include "../common/constants.h"
#include "../common/version.h"
#include <brotli/decode.h>
#include <brotli/encode.h>

#if !defined(_WIN32)
#include <unistd.h>
#include <utime.h>
#define MAKE_BINARY(FILENO) (FILENO)
#else
#include <io.h>
#include <share.h>
#include <sys/utime.h>

#define MAKE_BINARY(FILENO) (_setmode((FILENO), _O_BINARY), (FILENO))

#if !defined(__MINGW32__)
#define STDIN_FILENO _fileno(stdin)
#define STDOUT_FILENO _fileno(stdout)
#define S_IRUSR S_IREAD
#define S_IWUSR S_IWRITE
#endif

#define fdopen _fdopen
#define isatty _isatty
#define unlink _unlink
#define utimbuf _utimbuf
#define utime _utime

#define fopen ms_fopen
#define open ms_open

#define chmod(F, P) (0)
#define chown(F, O, G) (0)

#if defined(_MSC_VER) && (_MSC_VER >= 1400)
#define fseek _fseeki64
#define ftell _ftelli64
#endif

static FILE* ms_fopen(const char* filename, const char* mode) {
  FILE* result = 0;
  fopen_s(&result, filename, mode);
  return result;
}

static int ms_open(const char* filename, int oflag, int pmode) {
  int result = -1;
  _sopen_s(&result, filename, oflag | O_BINARY, _SH_DENYNO, pmode);
  return result;
}
#endif  /* WIN32 */

typedef enum {
  COMMAND_COMPRESS,
  COMMAND_DECOMPRESS,
  COMMAND_HELP,
  COMMAND_INVALID,
  COMMAND_TEST_INTEGRITY,
  COMMAND_NOOP,
  COMMAND_VERSION
} Command;

#define DEFAULT_LGWIN 24
#define DEFAULT_SUFFIX ".br"
#define MAX_OPTIONS 20
#define DECODE_HEADER_SIZE 0x10
#define GAP_MEM_BLOCK 0x1000

typedef struct {
  /* Parameters */
  int gmem;
  int quality;
  int lgwin;
  BROTLI_BOOL force_overwrite;
  BROTLI_BOOL junk_source;
  BROTLI_BOOL copy_stat;
  BROTLI_BOOL verbose;
  BROTLI_BOOL write_to_stdout;
  BROTLI_BOOL test_integrity;
  BROTLI_BOOL decompress;
  BROTLI_BOOL large_window;
  const char* output_path;
  const char* suffix;
  int not_input_indices[MAX_OPTIONS];
  size_t longest_path_len;
  size_t input_count;

  /* Inner state */
  int argc;
  char** argv;
  char* modified_path;  /* Storage for path with appended / cut suffix */
  int iterator;
  int ignore;
  BROTLI_BOOL iterator_error;
  uint8_t* buffer;
  uint8_t* input;
  uint8_t* output;
  const char* current_input_path;
  const char* current_output_path;
  int64_t input_file_length;  /* -1, if impossible to calculate */
  FILE* fin;
  FILE* fout;

  /* I/O buffers */
  size_t available_in;
  const uint8_t* next_in;
  size_t available_out;
  uint8_t* next_out;
} Context;

/* Parse up to 5 decimal digits. */
static BROTLI_BOOL ParseInt(const char* s, int low, int high, int* result) {
  int value = 0;
  int i;
  for (i = 0; i < 5; ++i) {
    char c = s[i];
    if (c == 0) break;
    if (s[i] < '0' || s[i] > '9') return BROTLI_FALSE;
    value = (10 * value) + (c - '0');
  }
  if (i == 0) return BROTLI_FALSE;
  if (i > 1 && s[0] == '0') return BROTLI_FALSE;
  if (s[i] != 0) return BROTLI_FALSE;
  if (value < low || value > high) return BROTLI_FALSE;
  *result = value;
  return BROTLI_TRUE;
}

/* Returns "base file name" or its tail, if it contains '/' or '\'. */
static const char* FileName(const char* path) {
  const char* separator_position = strrchr(path, '/');
  if (separator_position) path = separator_position + 1;
  separator_position = strrchr(path, '\\');
  if (separator_position) path = separator_position + 1;
  return path;
}

/* Detect if the program name is a special alias that infers a command type. */
static Command ParseAlias(const char* name) {
  /* TODO: cast name to lower case? */
  const char* unbrotli = "unbrotli";
  size_t unbrotli_len = strlen(unbrotli);
  name = FileName(name);
  /* Partial comparison. On Windows there could be ".exe" suffix. */
  if (strncmp(name, unbrotli, unbrotli_len) == 0) {
    char terminator = name[unbrotli_len];
    if (terminator == 0 || terminator == '.') return COMMAND_DECOMPRESS;
  }
  return COMMAND_COMPRESS;
}

static Command ParseParams(Context* params) {
  int argc = params->argc;
  char** argv = params->argv;
  int i;
  int next_option_index = 0;
  size_t input_count = 0;
  size_t longest_path_len = 1;
  BROTLI_BOOL command_set = BROTLI_FALSE;
  BROTLI_BOOL gmem_set = BROTLI_FALSE;
  BROTLI_BOOL quality_set = BROTLI_FALSE;
  BROTLI_BOOL output_set = BROTLI_FALSE;
  BROTLI_BOOL keep_set = BROTLI_FALSE;
  BROTLI_BOOL lgwin_set = BROTLI_FALSE;
  BROTLI_BOOL suffix_set = BROTLI_FALSE;
  BROTLI_BOOL after_dash_dash = BROTLI_FALSE;
  Command command = ParseAlias(argv[0]);

  for (i = 1; i < argc; ++i) {
    const char* arg = argv[i];
    /* C99 5.1.2.2.1: "members argv[0] through argv[argc-1] inclusive shall
       contain pointers to strings"; NULL and 0-length are not forbidden. */
    size_t arg_len = arg ? strlen(arg) : 0;

    if (arg_len == 0) {
      params->not_input_indices[next_option_index++] = i;
      continue;
    }

    /* Too many options. The expected longest option list is:
       "-q 0 -w 10 -o f -D d -S b -d -f -k -n -v --", i.e. 16 items in total.
       This check is an additional guard that is never triggered, but provides
       a guard for future changes. */
    if (next_option_index > (MAX_OPTIONS - 2)) {
      fprintf(stderr, "too many options passed\n");
      return COMMAND_INVALID;
    }

    /* Input file entry. */
    if (after_dash_dash || arg[0] != '-' || arg_len == 1) {
      input_count++;
      if (longest_path_len < arg_len) longest_path_len = arg_len;
      continue;
    }

    /* Not a file entry. */
    params->not_input_indices[next_option_index++] = i;

    /* '--' entry stop parsing arguments. */
    if (arg_len == 2 && arg[1] == '-') {
      after_dash_dash = BROTLI_TRUE;
      continue;
    }

    /* Simple / coalesced options. */
    if (arg[1] != '-') {
      size_t j;
      for (j = 1; j < arg_len; ++j) {
        char c = arg[j];
        if (c >= '0' && c <= '9') {
          if (quality_set) {
            fprintf(stderr, "quality already set\n");
            return COMMAND_INVALID;
          }
          quality_set = BROTLI_TRUE;
          params->quality = c - '0';
          continue;
        } else if (c == 'c') {
          if (output_set) {
            fprintf(stderr, "write to standard output already set\n");
            return COMMAND_INVALID;
          }
          output_set = BROTLI_TRUE;
          params->write_to_stdout = BROTLI_TRUE;
          continue;
        } else if (c == 'd') {
          if (command_set) {
            fprintf(stderr, "command already set when parsing -d\n");
            return COMMAND_INVALID;
          }
          command_set = BROTLI_TRUE;
          command = COMMAND_DECOMPRESS;
          continue;
        } else if (c == 'f') {
          if (params->force_overwrite) {
            fprintf(stderr, "force output overwrite already set\n");
            return COMMAND_INVALID;
          }
          params->force_overwrite = BROTLI_TRUE;
          continue;
        } else if (c == 'h') {
          /* Don't parse further. */
          return COMMAND_HELP;
        } else if (c == 'j' || c == 'k') {
          if (keep_set) {
            fprintf(stderr, "argument --rm / -j or --keep / -n already set\n");
            return COMMAND_INVALID;
          }
          keep_set = BROTLI_TRUE;
          params->junk_source = TO_BROTLI_BOOL(c == 'j');
          continue;
        } else if (c == 'n') {
          if (!params->copy_stat) {
            fprintf(stderr, "argument --no-copy-stat / -n already set\n");
            return COMMAND_INVALID;
          }
          params->copy_stat = BROTLI_FALSE;
          continue;
        } else if (c == 't') {
          if (command_set) {
            fprintf(stderr, "command already set when parsing -t\n");
            return COMMAND_INVALID;
          }
          command_set = BROTLI_TRUE;
          command = COMMAND_TEST_INTEGRITY;
          continue;
        } else if (c == 'v') {
          if (params->verbose) {
            fprintf(stderr, "argument --verbose / -v already set\n");
            return COMMAND_INVALID;
          }
          params->verbose = BROTLI_TRUE;
          continue;
        } else if (c == 'V') {
          /* Don't parse further. */
          return COMMAND_VERSION;
        } else if (c == 'Z') {
          if (quality_set) {
            fprintf(stderr, "quality already set\n");
            return COMMAND_INVALID;
          }
          quality_set = BROTLI_TRUE;
          params->quality = 11;
          continue;
        }
        /* o/q/w/D/S with parameter is expected */
        if (c != 'o' && c != 'q' && c != 'w' && c != 'D' && c != 'S') {
          fprintf(stderr, "invalid argument -%c\n", c);
          return COMMAND_INVALID;
        }
        if (j + 1 != arg_len) {
          fprintf(stderr, "expected parameter for argument -%c\n", c);
          return COMMAND_INVALID;
        }
        i++;
        if (i == argc || !argv[i] || argv[i][0] == 0) {
          fprintf(stderr, "expected parameter for argument -%c\n", c);
          return COMMAND_INVALID;
        }
        params->not_input_indices[next_option_index++] = i;
        if (c == 'o') {
          if (output_set) {
            fprintf(stderr, "write to standard output already set (-o)\n");
            return COMMAND_INVALID;
          }
          params->output_path = argv[i];
        } else if (c == 'g') {
          gmem_set = ParseInt(argv[i], 1, 0x10, &params->gmem);
          if (!gmem_set) {
            fprintf(stderr, "error parsing gmem value [%s]\n", argv[i]);
            return COMMAND_INVALID;
          }
        } else if (c == 'q') {
          if (quality_set) {
            fprintf(stderr, "quality already set\n");
            return COMMAND_INVALID;
          }
          quality_set = ParseInt(argv[i], BROTLI_MIN_QUALITY,
                                 BROTLI_MAX_QUALITY, &params->quality);
          if (!quality_set) {
            fprintf(stderr, "error parsing quality value [%s]\n", argv[i]);
            return COMMAND_INVALID;
          }
        } else if (c == 'w') {
          if (lgwin_set) {
            fprintf(stderr, "lgwin parameter already set\n");
            return COMMAND_INVALID;
          }
          lgwin_set = ParseInt(argv[i], 0,
                               BROTLI_MAX_WINDOW_BITS, &params->lgwin);
          if (!lgwin_set) {
            fprintf(stderr, "error parsing lgwin value [%s]\n", argv[i]);
            return COMMAND_INVALID;
          }
          if (params->lgwin != 0 && params->lgwin < BROTLI_MIN_WINDOW_BITS) {
            fprintf(stderr,
                    "lgwin parameter (%d) smaller than the minimum (%d)\n",
                    params->lgwin, BROTLI_MIN_WINDOW_BITS);
            return COMMAND_INVALID;
          }
        } else if (c == 'S') {
          if (suffix_set) {
            fprintf(stderr, "suffix already set\n");
            return COMMAND_INVALID;
          }
          suffix_set = BROTLI_TRUE;
          params->suffix = argv[i];
        }
      }
    } else {  /* Double-dash. */
      arg = &arg[2];
      if (strcmp("best", arg) == 0) {
        if (quality_set) {
          fprintf(stderr, "quality already set\n");
          return COMMAND_INVALID;
        }
        quality_set = BROTLI_TRUE;
        params->quality = 11;
      } else if (strcmp("decompress", arg) == 0) {
        if (command_set) {
          fprintf(stderr, "command already set when parsing --decompress\n");
          return COMMAND_INVALID;
        }
        command_set = BROTLI_TRUE;
        command = COMMAND_DECOMPRESS;
      } else if (strcmp("force", arg) == 0) {
        if (params->force_overwrite) {
          fprintf(stderr, "force output overwrite already set\n");
          return COMMAND_INVALID;
        }
        params->force_overwrite = BROTLI_TRUE;
      } else if (strcmp("help", arg) == 0) {
        /* Don't parse further. */
        return COMMAND_HELP;
      } else if (strcmp("keep", arg) == 0) {
        if (keep_set) {
          fprintf(stderr, "argument --rm / -j or --keep / -n already set\n");
          return COMMAND_INVALID;
        }
        keep_set = BROTLI_TRUE;
        params->junk_source = BROTLI_FALSE;
      } else if (strcmp("no-copy-stat", arg) == 0) {
        if (!params->copy_stat) {
          fprintf(stderr, "argument --no-copy-stat / -n already set\n");
          return COMMAND_INVALID;
        }
        params->copy_stat = BROTLI_FALSE;
      } else if (strcmp("rm", arg) == 0) {
        if (keep_set) {
          fprintf(stderr, "argument --rm / -j or --keep / -n already set\n");
          return COMMAND_INVALID;
        }
        keep_set = BROTLI_TRUE;
        params->junk_source = BROTLI_TRUE;
      } else if (strcmp("stdout", arg) == 0) {
        if (output_set) {
          fprintf(stderr, "write to standard output already set\n");
          return COMMAND_INVALID;
        }
        output_set = BROTLI_TRUE;
        params->write_to_stdout = BROTLI_TRUE;
      } else if (strcmp("test", arg) == 0) {
        if (command_set) {
          fprintf(stderr, "command already set when parsing --test\n");
          return COMMAND_INVALID;
        }
        command_set = BROTLI_TRUE;
        command = COMMAND_TEST_INTEGRITY;
      } else if (strcmp("verbose", arg) == 0) {
        if (params->verbose) {
          fprintf(stderr, "argument --verbose / -v already set\n");
          return COMMAND_INVALID;
        }
        params->verbose = BROTLI_TRUE;
      } else if (strcmp("version", arg) == 0) {
        /* Don't parse further. */
        return COMMAND_VERSION;
      } else {
        /* key=value */
        const char* value = strrchr(arg, '=');
        size_t key_len;
        if (!value || value[1] == 0) {
          fprintf(stderr, "must pass the parameter as --%s=value\n", arg);
          return COMMAND_INVALID;
        }
        key_len = (size_t)(value - arg);
        value++;
        if (strncmp("lgwin", arg, key_len) == 0) {
          if (lgwin_set) {
            fprintf(stderr, "lgwin parameter already set\n");
            return COMMAND_INVALID;
          }
          lgwin_set = ParseInt(value, 0,
                               BROTLI_MAX_WINDOW_BITS, &params->lgwin);
          if (!lgwin_set) {
            fprintf(stderr, "error parsing lgwin value [%s]\n", value);
            return COMMAND_INVALID;
          }
          if (params->lgwin != 0 && params->lgwin < BROTLI_MIN_WINDOW_BITS) {
            fprintf(stderr,
                    "lgwin parameter (%d) smaller than the minimum (%d)\n",
                    params->lgwin, BROTLI_MIN_WINDOW_BITS);
            return COMMAND_INVALID;
          }
        } else if (strncmp("large_window", arg, key_len) == 0) {
          /* This option is intentionally not mentioned in help. */
          if (lgwin_set) {
            fprintf(stderr, "lgwin parameter already set\n");
            return COMMAND_INVALID;
          }
          lgwin_set = ParseInt(value, 0,
                               BROTLI_LARGE_MAX_WINDOW_BITS, &params->lgwin);
          if (!lgwin_set) {
            fprintf(stderr, "error parsing lgwin value [%s]\n", value);
            return COMMAND_INVALID;
          }
          if (params->lgwin != 0 && params->lgwin < BROTLI_MIN_WINDOW_BITS) {
            fprintf(stderr,
                    "lgwin parameter (%d) smaller than the minimum (%d)\n",
                    params->lgwin, BROTLI_MIN_WINDOW_BITS);
            return COMMAND_INVALID;
          }
        } else if (strncmp("output", arg, key_len) == 0) {
          if (output_set) {
            fprintf(stderr,
                    "write to standard output already set (--output)\n");
            return COMMAND_INVALID;
          }
          params->output_path = value;
        } else if (strncmp("gap", arg, key_len) == 0) {
          gmem_set = ParseInt(value, 1, 0x10, &params->gmem);
          if (!gmem_set) {
            fprintf(stderr, "error parsing gmem value [%s]\n", value);
            return COMMAND_INVALID;
          }
        } else if (strncmp("quality", arg, key_len) == 0) {
          if (quality_set) {
            fprintf(stderr, "quality already set\n");
            return COMMAND_INVALID;
          }
          quality_set = ParseInt(value, BROTLI_MIN_QUALITY,
                                 BROTLI_MAX_QUALITY, &params->quality);
          if (!quality_set) {
            fprintf(stderr, "error parsing quality value [%s]\n", value);
            return COMMAND_INVALID;
          }
        } else if (strncmp("suffix", arg, key_len) == 0) {
          if (suffix_set) {
            fprintf(stderr, "suffix already set\n");
            return COMMAND_INVALID;
          }
          suffix_set = BROTLI_TRUE;
          params->suffix = value;
        } else {
          fprintf(stderr, "invalid parameter: [%s]\n", arg);
          return COMMAND_INVALID;
        }
      }
    }
  }

  params->input_count = input_count;
  params->longest_path_len = longest_path_len;
  params->decompress = (command == COMMAND_DECOMPRESS);
  params->test_integrity = (command == COMMAND_TEST_INTEGRITY);

  if (input_count > 1 && output_set) return COMMAND_INVALID;
  if (params->test_integrity) {
    if (params->output_path) return COMMAND_INVALID;
    if (params->write_to_stdout) return COMMAND_INVALID;
  }
  if (strchr(params->suffix, '/') || strchr(params->suffix, '\\')) {
    return COMMAND_INVALID;
  }

  return command;
}

static void PrintVersion(void) {
  int major = BROTLI_VERSION >> 24;
  int minor = (BROTLI_VERSION >> 12) & 0xFFF;
  int patch = BROTLI_VERSION & 0xFFF;
  fprintf(stdout, "brotli %d.%d.%d\n", major, minor, patch);
}

static void PrintHelp(const char* name, BROTLI_BOOL error) {
  FILE* media = error ? stderr : stdout;
  /* String is cut to pieces with length less than 509, to conform C90 spec. */
  fprintf(media,
"Usage: %s [OPTION]... [FILE]...\n",
          name);
  fprintf(media,
"Options:\n"
"  -#                          compression level (0-9)\n"
"  -c, --stdout                write on standard output\n"
"  -d, --decompress            decompress\n"
"  -f, --force                 force output file overwrite\n"
"  -h, --help                  display this help and exit\n");
  fprintf(media,
"  -j, --rm                    remove source file(s)\n"
"  -k, --keep                  keep source file(s) (default)\n"
"  -n, --no-copy-stat          do not copy source file(s) attributes\n"
"  -o FILE, --output=FILE      output file (only if 1 input file)\n");
  fprintf(media,
"  -g NUM, --gap=NUM           scratch memory gap level (1-16)\n");
  fprintf(media,
"  -q NUM, --quality=NUM       compression level (%d-%d)\n",
          BROTLI_MIN_QUALITY, BROTLI_MAX_QUALITY);
  fprintf(media,
"  -t, --test                  test compressed file integrity\n"
"  -v, --verbose               verbose mode\n");
  fprintf(media,
"  -w NUM, --lgwin=NUM         set LZ77 window size (0, %d-%d)\n",
          BROTLI_MIN_WINDOW_BITS, BROTLI_MAX_WINDOW_BITS);
  fprintf(media,
"                              window size = 2**NUM - 16\n"
"                              0 lets compressor choose the optimal value\n");
  fprintf(media,
"  -S SUF, --suffix=SUF        output file suffix (default:'%s')\n",
          DEFAULT_SUFFIX);
  fprintf(media,
"  -V, --version               display version and exit\n"
"  -Z, --best                  use best compression level (11) (default)\n"
"Simple options could be coalesced, i.e. '-9kf' is equivalent to '-9 -k -f'.\n"
"With no FILE, or when FILE is -, read standard input.\n"
"All arguments after '--' are treated as files.\n");
}

static const char* PrintablePath(const char* path) {
  return path ? path : "con";
}

static BROTLI_BOOL OpenInputFile(const char* input_path, FILE** f) {
  *f = NULL;
  if (!input_path) {
    *f = fdopen(MAKE_BINARY(STDIN_FILENO), "rb");
    return BROTLI_TRUE;
  }
  *f = fopen(input_path, "rb");
  if (!*f) {
    fprintf(stderr, "failed to open input file [%s]: %s\n",
            PrintablePath(input_path), strerror(errno));
    return BROTLI_FALSE;
  }
  return BROTLI_TRUE;
}

static BROTLI_BOOL OpenOutputFile(const char* output_path, FILE** f,
                                  BROTLI_BOOL force) {
  int fd;
  *f = NULL;
  if (!output_path) {
    *f = fdopen(MAKE_BINARY(STDOUT_FILENO), "wb");
    return BROTLI_TRUE;
  }
  fd = open(output_path, O_CREAT | (force ? 0 : O_EXCL) | O_WRONLY | O_TRUNC,
            S_IRUSR | S_IWUSR);
  if (fd < 0) {
    fd = open(output_path, (force ? 0 : O_EXCL) | O_WRONLY | O_TRUNC,
              S_IRUSR | S_IWUSR);
    if (fd < 0) {
      fprintf(stderr, "failed to open output file [%s]: %s\n",
              PrintablePath(output_path), strerror(errno));
      return BROTLI_FALSE;
    }
  }

  *f = fdopen(fd, "wb");
  if (!*f) {
    fprintf(stderr, "failed to open output file [%s]: %s\n",
            PrintablePath(output_path), strerror(errno));
    return BROTLI_FALSE;
  }
  return BROTLI_TRUE;
}

static int64_t FileSize(const char* path) {
  FILE* f = fopen(path, "rb");
  int64_t retval;
  if (f == NULL) {
    return -1;
  }
  if (fseek(f, 0L, SEEK_END) != 0) {
    fclose(f);
    return -1;
  }
  retval = ftell(f);
  if (fclose(f) != 0) {
    return -1;
  }
  return retval;
}

/* Copy file times and permissions.
   TODO: this is a "best effort" implementation; honest cross-platform
   fully featured implementation is way too hacky; add more hacks by request. */
static void CopyStat(const char* input_path, const char* output_path) {
  struct stat statbuf;
  struct utimbuf times;
  int res;
  if (input_path == 0 || output_path == 0) {
    return;
  }
  if (stat(input_path, &statbuf) != 0) {
    return;
  }
  times.actime = statbuf.st_atime;
  times.modtime = statbuf.st_mtime;
  utime(output_path, &times);
  res = chmod(output_path, statbuf.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO));
  if (res != 0) {
    fprintf(stderr, "setting access bits failed for [%s]: %s\n",
            PrintablePath(output_path), strerror(errno));
  }
  res = chown(output_path, (uid_t)-1, statbuf.st_gid);
  if (res != 0) {
    fprintf(stderr, "setting group failed for [%s]: %s\n",
            PrintablePath(output_path), strerror(errno));
  }
  res = chown(output_path, statbuf.st_uid, (gid_t)-1);
  if (res != 0) {
    fprintf(stderr, "setting user failed for [%s]: %s\n",
            PrintablePath(output_path), strerror(errno));
  }
}

int64_t input_file_length = 0;

static BROTLI_BOOL NextFile(Context* context) {
  const char* arg;
  size_t arg_len;

  /* Iterator points to last used arg; increment to search for the next one. */
  context->iterator++;

  context->input_file_length = -1;

  /* No input path; read from console. */
  if (context->input_count == 0) {
    if (context->iterator > 1) return BROTLI_FALSE;
    context->current_input_path = NULL;
    /* Either write to the specified path, or to console. */
    context->current_output_path = context->output_path;
    return BROTLI_TRUE;
  }

  /* Skip option arguments. */
  while (context->iterator == context->not_input_indices[context->ignore]) {
    context->iterator++;
    context->ignore++;
  }

  /* All args are scanned already. */
  if (context->iterator >= context->argc) return BROTLI_FALSE;

  /* Iterator now points to the input file name. */
  arg = context->argv[context->iterator];
  arg_len = strlen(arg);
  /* Read from console. */
  if (arg_len == 1 && arg[0] == '-') {
    context->current_input_path = NULL;
    context->current_output_path = context->output_path;
    return BROTLI_TRUE;
  }

  if (context->current_input_path == NULL) {
    context->current_input_path = arg;
  }
  context->input_file_length = FileSize(context->current_input_path);
  context->current_output_path = context->output_path;
  if (!context->decompress) {
    input_file_length += context->input_file_length;
  }

  if (context->output_path) return BROTLI_TRUE;
  if (context->write_to_stdout) return BROTLI_TRUE;

  strcpy(context->modified_path, arg);
  context->current_output_path = context->modified_path;
  /* If output is not specified, input path suffix should match. */
  if (context->decompress) {
    size_t suffix_len = strlen(context->suffix);
    char* name = (char*)FileName(context->modified_path);
    char* name_suffix;
    size_t name_len = strlen(name);
    if (name_len < suffix_len + 1) {
      fprintf(stderr, "empty output file name for [%s] input file\n",
              PrintablePath(arg));
      context->iterator_error = BROTLI_TRUE;
      return BROTLI_FALSE;
    }
    name_suffix = name + name_len - suffix_len;
    if (strcmp(context->suffix, name_suffix) != 0) {
      fprintf(stderr, "input file [%s] suffix mismatch\n",
              PrintablePath(arg));
      context->iterator_error = BROTLI_TRUE;
      return BROTLI_FALSE;
    }
    name_suffix[0] = 0;
    return BROTLI_TRUE;
  } else {
    strcpy(context->modified_path + arg_len, context->suffix);
    return BROTLI_TRUE;
  }
}

static BROTLI_BOOL OpenFiles(Context* context) {
  BROTLI_BOOL is_ok = OpenInputFile(context->current_input_path, &context->fin);
  if (context->decompress) {
    //
    // skip the decoder data header
    //
    fseek(context->fin, DECODE_HEADER_SIZE, SEEK_SET);
  }
  if (!context->test_integrity && is_ok && context->fout == NULL) {
    is_ok = OpenOutputFile(
        context->current_output_path, &context->fout, context->force_overwrite);
  }
  if (!context->decompress) {
    //
    // append the decoder data header
    //
    fseek(context->fout, DECODE_HEADER_SIZE, SEEK_SET);
  }
  return is_ok;
}

static BROTLI_BOOL CloseFiles(Context* context, BROTLI_BOOL success) {
  BROTLI_BOOL is_ok = BROTLI_TRUE;
  if (!context->test_integrity && context->fout) {
    if (!success && context->current_output_path) {
      unlink(context->current_output_path);
    }
    if (fclose(context->fout) != 0) {
      if (success) {
        fprintf(stderr, "fclose failed [%s]: %s\n",
                PrintablePath(context->current_output_path), strerror(errno));
      }
      is_ok = BROTLI_FALSE;
    }

    /* TOCTOU violation, but otherwise it is impossible to set file times. */
    if (success && is_ok && context->copy_stat) {
      CopyStat(context->current_input_path, context->current_output_path);
    }
  }

  if (context->fin) {
    if (fclose(context->fin) != 0) {
      if (is_ok) {
        fprintf(stderr, "fclose failed [%s]: %s\n",
                PrintablePath(context->current_input_path), strerror(errno));
      }
      is_ok = BROTLI_FALSE;
    }
  }
  if (success && context->junk_source && context->current_input_path) {
    unlink(context->current_input_path);
  }

  context->fin = NULL;
  context->fout = NULL;

  return is_ok;
}

static const size_t kFileBufferSize = 1 << 16;

static void InitializeBuffers(Context* context) {
  context->available_in = 0;
  context->next_in = NULL;
  context->available_out = kFileBufferSize;
  context->next_out = context->output;
}

static BROTLI_BOOL HasMoreInput(Context* context) {
  return feof(context->fin) ? BROTLI_FALSE : BROTLI_TRUE;
}

static BROTLI_BOOL ProvideInput(Context* context) {
  context->available_in =
      fread(context->input, 1, kFileBufferSize, context->fin);
  context->next_in = context->input;
  if (ferror(context->fin)) {
    fprintf(stderr, "failed to read input [%s]: %s\n",
            PrintablePath(context->current_input_path), strerror(errno));
    return BROTLI_FALSE;
  }
  return BROTLI_TRUE;
}

/* Internal: should be used only in Provide-/Flush-Output. */
static BROTLI_BOOL WriteOutput(Context* context) {
  size_t out_size = (size_t)(context->next_out - context->output);
  if (out_size == 0) return BROTLI_TRUE;
  if (context->test_integrity) return BROTLI_TRUE;

  fwrite(context->output, 1, out_size, context->fout);
  if (ferror(context->fout)) {
    fprintf(stderr, "failed to write output [%s]: %s\n",
            PrintablePath(context->current_output_path), strerror(errno));
    return BROTLI_FALSE;
  }
  return BROTLI_TRUE;
}

static BROTLI_BOOL ProvideOutput(Context* context) {
  if (!WriteOutput(context)) return BROTLI_FALSE;
  context->available_out = kFileBufferSize;
  context->next_out = context->output;
  return BROTLI_TRUE;
}

static BROTLI_BOOL FlushOutput(Context* context) {
  if (!WriteOutput(context)) return BROTLI_FALSE;
  context->available_out = 0;
  return BROTLI_TRUE;
}

static BROTLI_BOOL DecompressFile(Context* context, BrotliDecoderState* s) {
  BrotliDecoderResult result = BROTLI_DECODER_RESULT_NEEDS_MORE_INPUT;
  InitializeBuffers(context);
  for (;;) {
    if (result == BROTLI_DECODER_RESULT_NEEDS_MORE_INPUT) {
      if (!HasMoreInput(context)) {
        fprintf(stderr, "corrupt input [%s]\n",
                PrintablePath(context->current_input_path));
        return BROTLI_FALSE;
      }
      if (!ProvideInput(context)) return BROTLI_FALSE;
    } else if (result == BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT) {
      if (!ProvideOutput(context)) return BROTLI_FALSE;
    } else if (result == BROTLI_DECODER_RESULT_SUCCESS) {
      if (!FlushOutput(context)) return BROTLI_FALSE;
      if (context->available_in != 0 || HasMoreInput(context)) {
        fprintf(stderr, "corrupt input [%s]\n",
                PrintablePath(context->current_input_path));
        return BROTLI_FALSE;
      }
      return BROTLI_TRUE;
    } else {
      fprintf(stderr, "corrupt input [%s]\n",
              PrintablePath(context->current_input_path));
      return BROTLI_FALSE;
    }

    result = BrotliDecoderDecompressStream(s, &context->available_in,
        &context->next_in, &context->available_out, &context->next_out, 0);
  }
}

/* Default brotli_alloc_func */
void* BrotliAllocFunc(void* opaque, size_t size) {
  *(size_t *)opaque = *(size_t *) opaque + size; 
  return malloc(size);
}

/* Default brotli_free_func */
void BrotliFreeFunc(void* opaque, void* address) {
  free(address);
}

size_t scratch_buffer_size = 0;

static BROTLI_BOOL DecompressFiles(Context* context) {
  while (NextFile(context)) {
    BROTLI_BOOL is_ok = BROTLI_TRUE;
    BrotliDecoderState* s = BrotliDecoderCreateInstance(BrotliAllocFunc, BrotliFreeFunc, &scratch_buffer_size);
    if (!s) {
      fprintf(stderr, "out of memory\n");
      return BROTLI_FALSE;
    }
    /* This allows decoding "large-window" streams. Though it creates
       fragmentation (new builds decode streams that old builds don't),
       it is better from used experience perspective. */
    BrotliDecoderSetParameter(s, BROTLI_DECODER_PARAM_LARGE_WINDOW, 1u);
    is_ok = OpenFiles(context);
    if (is_ok && !context->current_input_path &&
        !context->force_overwrite && isatty(STDIN_FILENO)) {
      fprintf(stderr, "Use -h help. Use -f to force input from a terminal.\n");
      is_ok = BROTLI_FALSE;
    }
    if (is_ok) is_ok = DecompressFile(context, s);
    BrotliDecoderDestroyInstance(s);
    if (!CloseFiles(context, is_ok)) is_ok = BROTLI_FALSE;
    if (!is_ok) return BROTLI_FALSE;
  }
  return BROTLI_TRUE;
}

static BROTLI_BOOL CompressFile(Context* context, BrotliEncoderState* s) {
  BROTLI_BOOL is_eof = BROTLI_FALSE;
  InitializeBuffers(context);
  for (;;) {
    if (context->available_in == 0 && !is_eof) {
      if (!ProvideInput(context)) return BROTLI_FALSE;
      is_eof = !HasMoreInput(context);
    }

    if (!BrotliEncoderCompressStream(s,
        is_eof ? BROTLI_OPERATION_FINISH : BROTLI_OPERATION_PROCESS,
        &context->available_in, &context->next_in,
        &context->available_out, &context->next_out, NULL)) {
      /* Should detect OOM? */
      fprintf(stderr, "failed to compress data [%s]\n",
              PrintablePath(context->current_input_path));
      return BROTLI_FALSE;
    }

    if (context->available_out == 0) {
      if (!ProvideOutput(context)) return BROTLI_FALSE;
    }

    if (BrotliEncoderIsFinished(s)) {
      return FlushOutput(context);
    }
  }
}

static BROTLI_BOOL CompressFiles(Context* context) {
  while (NextFile(context)) {
    BROTLI_BOOL is_ok = BROTLI_TRUE;
    BrotliEncoderState* s = BrotliEncoderCreateInstance(NULL, NULL, NULL);
    if (!s) {
      fprintf(stderr, "out of memory\n");
      return BROTLI_FALSE;
    }
    BrotliEncoderSetParameter(s,
        BROTLI_PARAM_QUALITY, (uint32_t)context->quality);
    if (context->lgwin > 0) {
      /* Specified by user. */
      /* Do not enable "large-window" extension, if not required. */
      if (context->lgwin > BROTLI_MAX_WINDOW_BITS) {
        BrotliEncoderSetParameter(s, BROTLI_PARAM_LARGE_WINDOW, 1u);
      }
      BrotliEncoderSetParameter(s,
          BROTLI_PARAM_LGWIN, (uint32_t)context->lgwin);
    } else {
      /* 0, or not specified by user; could be chosen by compressor. */
      uint32_t lgwin = DEFAULT_LGWIN;
      /* Use file size to limit lgwin. */
      if (context->input_file_length >= 0) {
        int32_t size = 1 << BROTLI_MIN_WINDOW_BITS;
        lgwin = BROTLI_MIN_WINDOW_BITS;
        while (size < context->input_file_length) {
          size <<= 1;
          lgwin++;
          if (lgwin == BROTLI_MAX_WINDOW_BITS) break;
        }
      }
      BrotliEncoderSetParameter(s, BROTLI_PARAM_LGWIN, lgwin);
    }
    if (context->input_file_length > 0) {
      uint32_t size_hint = context->input_file_length < (1 << 30) ?
          (uint32_t)context->input_file_length : (1u << 30);
      BrotliEncoderSetParameter(s, BROTLI_PARAM_SIZE_HINT, size_hint);
    }
    is_ok = OpenFiles(context);
    if (is_ok && !context->current_output_path &&
        !context->force_overwrite && isatty(STDOUT_FILENO)) {
      fprintf(stderr, "Use -h help. Use -f to force output to a terminal.\n");
      is_ok = BROTLI_FALSE;
    }
    if (is_ok) is_ok = CompressFile(context, s);
    BrotliEncoderDestroyInstance(s);
    if (!CloseFiles(context, is_ok)) is_ok = BROTLI_FALSE;
    if (!is_ok) return BROTLI_FALSE;
  }
  return BROTLI_TRUE;
}

int main(int argc, char** argv) {
  Command command;
  Context context;
  Context context_dec;
  BROTLI_BOOL is_ok = BROTLI_TRUE;
  int i;

  context.quality = 11;
  context.lgwin = -1;
  context.gmem = 1;
  context.force_overwrite = BROTLI_FALSE;
  context.junk_source = BROTLI_FALSE;
  context.copy_stat = BROTLI_TRUE;
  context.test_integrity = BROTLI_FALSE;
  context.verbose = BROTLI_FALSE;
  context.write_to_stdout = BROTLI_FALSE;
  context.decompress = BROTLI_FALSE;
  context.large_window = BROTLI_FALSE;
  context.output_path = NULL;
  context.suffix = DEFAULT_SUFFIX;
  for (i = 0; i < MAX_OPTIONS; ++i) context.not_input_indices[i] = 0;
  context.longest_path_len = 1;
  context.input_count = 0;

  context.argc = argc;
  context.argv = argv;
  context.modified_path = NULL;
  context.iterator = 0;
  context.ignore = 0;
  context.iterator_error = BROTLI_FALSE;
  context.buffer = NULL;
  context.current_input_path = NULL;
  context.current_output_path = NULL;
  context.fin = NULL;
  context.fout = NULL;

  command = ParseParams(&context);

  if (command == COMMAND_COMPRESS || command == COMMAND_DECOMPRESS ||
      command == COMMAND_TEST_INTEGRITY) {
    if (is_ok) {
      size_t modified_path_len =
          context.longest_path_len + strlen(context.suffix) + 1;
      context.modified_path = (char*)malloc(modified_path_len);
      context.buffer = (uint8_t*)malloc(kFileBufferSize * 2);
      if (!context.modified_path || !context.buffer) {
        fprintf(stderr, "out of memory\n");
        is_ok = BROTLI_FALSE;
      } else {
        context.input = context.buffer;
        context.output = context.buffer + kFileBufferSize;
      }
    }
  }

  if (!is_ok) command = COMMAND_NOOP;

  switch (command) {
    case COMMAND_NOOP:
      break;

    case COMMAND_VERSION:
      PrintVersion();
      break;

    case COMMAND_COMPRESS:
      memcpy (&context_dec, &context, sizeof (context));
      is_ok = CompressFiles(&context);
      if (!is_ok) {
        break;
      }
      context_dec.decompress  = BROTLI_TRUE;
      context_dec.input_count = 1;
      context_dec.current_input_path = context_dec.output_path;
      context_dec.fout = tmpfile ();
      is_ok = DecompressFiles(&context_dec);
      if (!is_ok) {
        break;
      }
      //
      // fill decoder header
      //
      context_dec.fout = fopen(context_dec.output_path, "rb+");  /* open output_path file and add in head info */
      fwrite(&input_file_length, 1, sizeof(int64_t), context_dec.fout);
      scratch_buffer_size += context.gmem * GAP_MEM_BLOCK;  /* there is a memory gap between IA32 and X64 environment*/
      scratch_buffer_size += kFileBufferSize * 2;
      fwrite(&scratch_buffer_size, 1, sizeof(int64_t), context_dec.fout);
      if (fclose(context_dec.fout) != 0) {
        fprintf(stderr, "failed to update ouptut file: %s\n", context_dec.output_path);
        is_ok = 0;
      }
      break;

    case COMMAND_DECOMPRESS:
    case COMMAND_TEST_INTEGRITY:
      is_ok = DecompressFiles(&context);
      break;

    case COMMAND_HELP:
    case COMMAND_INVALID:
    default:
      is_ok = (command == COMMAND_HELP);
      PrintHelp(FileName(argv[0]), is_ok);
      break;
  }

  if (context.iterator_error) is_ok = BROTLI_FALSE;

  free(context.modified_path);
  free(context.buffer);

  if (!is_ok) exit(1);
  return 0;
}
