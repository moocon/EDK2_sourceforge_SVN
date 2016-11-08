/* DLGLexerBase.h
 *
 * SOFTWARE RIGHTS
 *
 * We reserve no LEGAL rights to the Purdue Compiler Construction Tool
 * Set (PCCTS) -- PCCTS is in the public domain.  An individual or
 * company may do whatever they wish with source code distributed with
 * PCCTS or the code generated by PCCTS, including the incorporation of
 * PCCTS, or its output, into commerical software.
 *
 * We encourage users to develop software with PCCTS.  However, we do ask
 * that credit is given to us for developing PCCTS.  By "credit",
 * we mean that if you incorporate our source code into one of your
 * programs (commercial product, research project, or otherwise) that you
 * acknowledge this fact somewhere in the documentation, research report,
 * etc...  If you like PCCTS and have developed a nice tool with the
 * output, please mention that you developed it using PCCTS.  In
 * addition, we ask that this header remain intact in our source code.
 * As long as these guidelines are kept, we expect to continue enhancing
 * this system and expect to make other tools available as they are
 * completed.
 *
 * ANTLR 1.33
 * Terence Parr
 * Parr Research Corporation
 * with Purdue University and AHPCRC, University of Minnesota
 * 1989-2000
 */

#ifndef DLGX_H
#define DLGX_H

#include "pcctscfg.h"
#include "pccts_stdio.h"

PCCTS_NAMESPACE_STD

#include ATOKEN_H
#include ATOKENSTREAM_H

class ANTLRParser;							// MR1

/* must define what a char looks like; can make this a class too */
typedef char DLGChar;

/*  Can have it as a class too: (ack this looks weird; is it right?)
class DllExportPCCTS DLGChar {
private:
	int c;
public:
	DLGChar(int ch) { c = ch; }
	int atom() { return c; }
};
*/

/* user must subclass this */
class DllExportPCCTS DLGInputStream {
public:
	virtual int nextChar() = 0;
    virtual ~DLGInputStream() {};
};

/* Predefined char stream: Input from FILE */
class DllExportPCCTS DLGFileInput : public DLGInputStream {
private:
	int found_eof;
	FILE *input;
public:
	DLGFileInput(FILE *f) { input = f; found_eof = 0; }
	int nextChar() {
			int c;
			if ( found_eof ) return EOF;
			else {
				c=getc(input);
				if ( c==EOF ) found_eof = 1;
				return c;
			}
	}
    void DLGFileReset(FILE *f) {input=f; found_eof = 0; };              // MR11
};

// MR9  Suggested by Bruce Guenter (bruceg@qcc.sk.ca)
// MR9  Make DLGStringInput const correct

/* Predefined char stream: Input from string */
class DllExportPCCTS DLGStringInput : public DLGInputStream {
private:
	const DLGChar *input;                                           // MR9
	const DLGChar *p;                                               // MR9
public:
	DLGStringInput(const DLGChar *s) { input = s; p = &input[0];}   // MR9
	int nextChar()
		{
			if (*p) return (int) (unsigned char) *p++;              // MR14
			else return EOF;
		}

    void DLGStringReset(const DLGChar *s) {input=s; p= &input[0]; }; // MR11 // MR16
};

class DllExportPCCTS DLGState {
public:
	DLGInputStream *input;
	int interactive;
	int track_columns;
	int auto_num;
	int add_erase;
	int lookc;
	int char_full;
	int begcol, endcol;
	int line;
	DLGChar *lextext, *begexpr, *endexpr;
	int bufsize;
	int bufovf;
	DLGChar *nextpos;
	int	class_num;
	int	debugLexerFlag;						// MR1
	ANTLRParser *parser;						// MR1
};

/* user must subclass this */
class DllExportPCCTS DLGLexerBase : public ANTLRTokenStream {
private:
    DLGLexerBase(const DLGLexerBase&);             // Prevent copy-construction
    DLGLexerBase& operator=(const DLGLexerBase&);  // Prevent assignment
public:
	virtual ANTLRTokenType erraction();

protected:
	DLGInputStream *input;
	int interactive;
	int track_columns;
	DLGChar	*_lextext;	/* text of most recently matched token */
	DLGChar	*_begexpr;	/* beginning of last reg expr recogn. */
	DLGChar	*_endexpr;	/* beginning of last reg expr recogn. */
	int	_bufsize;		/* number of characters in lextext */
	int	_begcol;		/* column that first character of token is in*/
	int	_endcol;		/* column that last character of token is in */
	int	_line;			/* line current token is on */
	int	ch;				/* character to determine next state */
	int	bufovf;			/* indicates that buffer too small for text */
	int	charfull;
	DLGChar	*nextpos;	/* points to next available position in lextext*/
	int	cl;
	int automaton;
	int	add_erase;
	DLGChar ebuf[70];
	_ANTLRTokenPtr token_to_fill;

	int	debugLexerFlag;						// MR1
	ANTLRParser	*parser;					// MR1
public:
	virtual _ANTLRTokenPtr getToken();      // MR12 public
	virtual void advance(void) = 0;
	void	skip(void);		/* erase lextext, look for antoher token */
	void	more(void);		/* keep lextext, look for another token */
	void	mode(int k);	/* switch to automaton 'k' */
	void	saveState(DLGState *);
	void	restoreState(DLGState *);
	virtual ANTLRTokenType nextTokenType(void)=0;/* get next token */
	void	replchar(DLGChar c);	/* replace last recognized reg. expr. with
									 a character */
	void	replstr(const DLGChar *s);	/* replace last recognized reg. expr. with
    									 a string */ /* MR20 const */
        virtual int err_in();						// MR1
	virtual void errstd(const char *);				// MR1  MR20 const
	int		line()		{ return _line; }
	void	set_line(int newValue)	{ _line=newValue; };		// MR1
	virtual void newline()	{ _line++; }
	DLGChar	*lextext()	{ return _lextext; }

	int		begcol()	{ return _begcol; }
	int		endcol()	{ return _endcol; }
	void	set_begcol(int a)	{ _begcol=a; }
	void	set_endcol(int a)	{ _endcol=a; }
	DLGChar	*begexpr()	{ return _begexpr; }
	DLGChar	*endexpr()	{ return _endexpr; }
	int		bufsize()	{ return _bufsize; }

	void	setToken(ANTLRAbstractToken *t)	{ token_to_fill = t; }

	void	setInputStream(DLGInputStream *);
	DLGLexerBase(DLGInputStream *in,
				 unsigned bufsize=2000,
				 int interactive=0,
				 int track_columns=0);
	void reset();									// MR19
	virtual ~DLGLexerBase() { delete [] _lextext; }
	virtual void panic(const char *msg);			// MR1  MR20 const
	void	trackColumns() {
				track_columns = 1;
				this->_begcol = 0;
				this->_endcol = 0;
			};
	virtual ANTLRParser *setParser(ANTLRParser *p);			// MR1
	virtual ANTLRParser *getParser();				// MR1
	virtual int debugLexer(int value);				// MR1
    int     lexErrCount;                            // MR12
	virtual int printMessage(FILE* pFile, const char* pFormat, ...); // MR23
};

#endif
