#ifndef PBLACKBOX_H
#define PBLACKBOX_H

/*
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

/* Completely rewritten by Chris Uzdavinis (chris@atdesk.com) for MR23 */

#include "pcctscfg.h"

#include "pccts_iostream.h"

PCCTS_NAMESPACE_STD

//  MR20 Added #include for "DLexerBase.h"

#include "DLexerBase.h"

//
//  The default buffer size of the lexer is given by the
//   second argument of the lexer's ctor.  It is optional
//   and defaults to 2000
//

template<class Lexer, class Parser, class Token>
class DllExportPCCTS ParserBlackBox {
private:
  // no copy construction allowed
  ParserBlackBox(ParserBlackBox const &);

  // no copy assignment allowed
  ParserBlackBox & operator=(ParserBlackBox const &);
  
protected:
  DLGFileInput *in;
  Lexer *scan;
  _ANTLRTokenPtr tok;
  ANTLRTokenBuffer *pipe;
  Parser *_parser;
  FILE *file;
  int openByBlackBox;    /* MR21 Don't close what we haven't opened */
public:
	
  ParserBlackBox(FILE *f)
    : in(0)
    , scan(0)
    , tok(0)
    , pipe(0)
    , _parser(0)
    , file(0)
    , openByBlackBox(0)
  {
    if (f == NULL)
    {
      cerr << "invalid file pointer\n"; 
    }
    else
    {
      openByBlackBox = 0;     /* MR21a */
      file = f;
      in = new DLGFileInput(f);
      scan = new Lexer(in);
      pipe = new ANTLRTokenBuffer(scan);
      tok = new Token;
      scan->setToken(tok);
      _parser = new Parser(pipe);
      _parser->init();
    }
  }
  ParserBlackBox(char *fname)
    : in(0)
    , scan(0)
    , tok(0)
    , pipe(0)
    , _parser(0)
    , file(0)
    , openByBlackBox(0)
  {
    FILE *f = fopen(fname, "r");
    if ( f==NULL ) {
      openByBlackBox = 0;
      cerr << "cannot open " << fname << "\n"; return;
    }
    else {
      openByBlackBox = 1;
      file = f;
      in = new DLGFileInput(f);
      scan = new Lexer(in);
      pipe = new ANTLRTokenBuffer(scan);
      tok = new Token;
      scan->setToken(tok);
      _parser = new Parser(pipe);
      _parser->init();
    }
  }
	
  ~ParserBlackBox()
  {
    delete in; delete scan; delete pipe; delete _parser; delete tok;
    if (1 == openByBlackBox) {
      fclose(file);
    }
  }

  Parser *parser()	   { return _parser; }
  Lexer  *getLexer()     { return scan; }
};

#endif
