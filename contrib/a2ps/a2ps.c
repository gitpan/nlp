/************************************************************************/
/*									*/
/* Description: Ascii to PostScript printer program.			*/
/* File: http://www.enst.fr/~demaille/a2ps.html				*/
/* Created: Oct, 04 1996, by  demaille@inf.enst.fr (Demaille Akim)	*/
/* Version: 4.6    							*/
/*									*/
/* Edit history:							*/
/* 1) Derived of shell program written by evan@csli (Evan Kirshenbaum).	*/
/*    Written in C for improve speed execution and portability. Many	*/
/*    improvements have been added.					*/
/* Fixes by Oscar Nierstrasz @ cui.uucp:				*/
/* 2) Fixed incorrect handling of stdin (removed error if no file names)*/
/* 3) Added start_page variable to eliminate blank pages printed for	*/
/*	files that are exactly multiples of 132 lines (e.g., man pages)	*/
/* Modified by santana@imag.fr:						*/
/* 4) Added new options at installation : sheet format (height/width in	*/
/*    inches), page format (number of columns per line and of lines per	*/
/*    page).								*/
/* Modified by santana@imag.fr:						*/
/* 5) Added new option to print n copies of a same document.		*/
/* 6) Cut long filenames if don't fit in the page header.		*/
/* Modified by Tim Clark (T.Clark@warwick.ac.uk):			*/
/* 7) Two additional modes of printing (portrait and wide format modes)	*/
/* 8) Fixed to cope with filenames which contain a character which must	*/
/*    be escaped in a PostScript string.				*/
/* Modified by santana@imag.fr to					*/
/* 9) Added new option to suppress heading printing.			*/
/* 10) Added new option to suppress page surrounding border printing.	*/
/* 11) Added new option to change font size. Lines and columns are	*/
/*     automatically adjusted, depending on font size and printing mode	*/
/* 12) Minor changes (best layout, usage message, etc).			*/
/* Modified by tullemans@apolloway.prl.philips.nl			*/
/* 13) Backspaces (^H) are now handled correctly.			*/
/* Modified by Johan Vromans (jv@mh.nl) to				*/
/* 14) Added new option to give a header title that replaces use of	*/
/*     filename.							*/
/* Modified by craig.r.stevenson@att.com to				*/
/* 15) Print last modification date/time in header                      */
/* 16) Printing current date/time on left side of footer (optional)	*/
/* Modified by erikt@cs.umu.se:						*/
/* 17) Added lpr support for the BSD version				*/
/* 18) Added som output of pages printed.				*/
/* Modified by wstahw@lso.win.tue.nl:					*/
/* 19) Added option to allowing the printing of 2 files in one sheet	*/
/* Modified by mai@wolfen.cc.uow.oz					*/
/* 20) Added an option to set the lines per page to a specified value.	*/
/* 21) Added support for printing nroff manuals				*/
/* Modified by santana@imag.fr						*/
/* 22) Integration of changes.						*/
/* 23) No more standard header file (printed directly by a2ps).		*/
/* 24) New format for command options.					*/
/* 25) Other minor changes.						*/
/* Modified by Johan Garpendahl (garp@isy.liu.se) and santana@imag.fr:	*/
/* 26) Added 8-bit characters printing as ISO-latin 1 chars		*/
/* Modified by John Interrante (interran@uluru.stanford.edu) and	*/
/* santana@imag.fr:							*/
/* 27) Two pages per physical page in portrait mode			*/
/* Modified by santana@imag.fr:						*/
/* 28) New option for two-sided printing				*/
/* 29) Several fixes							*/
/* Modified by Chris Adamo (adamo@ll.mit.edu) and			*/
/*     Larry Barbieri (lbarbieri@ll.mit.edu) 3/12/93			*/
/* 30) Output format enhancements.					*/
/* 31) Added login_id flag (for SYSV and BSD only) for printing user's	*/
/*     login ID at top of page.  Added command line parameter (-nL) to	*/
/*     suppress this feature.						*/
/* 33) Added filename_footer flag for printing file name at bottom	*/
/*     of page.  Added command line parameter (-nu) to suppress this	*/
/*     feature.								*/
/* 34) Added -B (-nB) options to enable (disable) bold font		*/
/* Modified by santana@imag.fr:						*/
/* 35) Adapted to respect Adobe conventions for page independence. A2ps	*/
/*     output can be now used by other Postscript processors.		*/
/* 36) Names of most postscript variables have been coded in order to	*/
/*     reduce the size of the output.					*/
/* 37) Ansi C compilers are now automatically taken into account.	*/
/* 38) Enhanced routine for cutting long filenames			*/ 
/* 39) Added -q option to print files in quiet mode (no summary)	*/
/* 40) Fixed some little bugs (counters, modification time for stdin,	*/
/*     character separator when printing line numbers and cutting a	*/
/*     line).								*/
/* 41) Some minor changes (new preprocessing variables, formatting)	*/
/* Modified by Emmanuel Briot (Emmanuel.Briot@enst-bretagne.fr)         */
/* 42) Made keyword Highligting mode                      		*/
/* Modified by Akim Demaille (demaille@inf.enst.fr)                     */
/* 43) Fixed line numbering, folding, managing of strings and escapes	*/
/* 44) Added alphabet, case sensitiveness, symbol translation (-t, -nt)	*/
/* 45) Added -L, -nk, -u<txt>, and made -k, -nH, -i, -ni positionnnal	*/
/* 46) Sequences now have three fonts (open, in, close)			*/
/* 47) Changed handling of exceptions (e.g. \", \\, etc.)		*/
/* 48) Small fix to allow print without LPR_OPT and with LPR_DEST_OPT	*/
/* 49) Introduced options -V, -a, -o, -W, -G and -k(none)		*/
/* 50) Added a "window" to the buffer to ensure that keywords, even at	*/
/*     the edge of the buffer, are recognized				*/
/* 51) Manage the real length (WX field in afms) of chars        	*/
/* 52) fread is used instead of fgets: \0 no longer ends the line	*/
/* 53) tabs in ps are "absolute": they always correspond to Courier	*/
/* 54) text footer now is `relative' (i.e. it is affected by MARGIN)	*/
/*									*/
/************************************************************************/


/*
 * Copyright (c) 1993, 1994, Miguel Santana, M.Santana@frgu.bull.fr
 *
 * Permission is granted to use, copy, and distribute this software for
 * non commercial use, provided that the above copyright notice appear in
 * all copies and that both that copyright notice and this permission
 * notice appear in supporting documentation.
 *
 * Permission to modify the software is granted, but not the right to
 * distribute the modified code. Please report bugs and changes to
 * briot@enst-bretagne.fr or demaille@inf.enst.fr
 * 
 * This software is provided "as is" without express or implied warranty.
 */


/************************************************************************/
/*                                                                      */
/*			I n c l u d e   f i l e s			*/
/*                                                                      */
/************************************************************************/
#include "config.h"

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#ifndef STDC_HEADERS
#  include "ansi.h"
#endif

#if HAVE_UNISTD_H
#  include <unistd.h>			/* for the pipes */
#endif

#include "a2ps.h"
#include "styles.h"
#include "report.h"
#include "postscript.h"


/************************************************************************/
/*									*/
/*		   G l o b a l   d e f i n i t i o n s			*/
/*									*/
/************************************************************************/
/*
 * Flags related to options.
 */
int numbering = FALSE;          /* Line numbering option 		*/
int folding = TRUE;             /* Line folding option 			*/
int restart = FALSE;            /* Don't restart page number after each file */
int only_printable = FALSE;     /* Replace non printable char by space 	*/
int interpret = TRUE;           /* Interpret TAB, FF and BS chars option */
int print_binaries = FALSE;     /* Force printing for binary files 	*/ 
int guess = FALSE;		/* on TRUE no print, stdout type of files */
int automatic_style = AUTO_DFLT;/* language is guessed from filename ?	*/
int landscape = TRUE;           /* Otherwise portrait format sheets 	*/
int new_landscape = TRUE;       /* To scrute changes of landscape option */
int twinpages = TRUE;           /* 2 pages per sheet if true, 1 otherwise */
int new_twinpages = TRUE;       /* To scrute changes of twinpages option */
int twinfiles = FALSE;          /* Allow 2 files per sheet 		*/
int no_header = FALSE;          /* TRUE if user doesn't want the header */
int no_border = FALSE;          /* Don't print the surrounding border ? */
int printdate = FALSE;          /* Print current date as footnote 	*/
int footer = TRUE;		/* Print file name at bottom of page 	*/
STYLE style = PLAIN;            /* What is the current style ? 		*/
int translate_symbols_req = FALSE; /* User asked for symbol convertion 	*/
int translate_symbols = FALSE;  /* Do we have to convert symbols ? 	*/
int no_summary = FALSE;         /* Quiet mode? 				*/
WEIGHT fontweight = NORMAL;     /* Control font weight 			*/
WEIGHT new_fontweight = NORMAL; /* To scrute changes of bold option 	*/
int login_id = TRUE;            /* Print login ID at top of page */

#ifdef LPR_PRINT                /* Fork a lpr process to do the printing */
int lpr_print = LPR_PRINT_DFLT; /* Print by default ? */
#  ifdef RECTO_VERSO_PRINTING
int rectoverso = TWOSIDED_DFLT; /* Two-side printing */
#  endif
#endif
int ISOlatin1 = ISOLATIN1_DFLT; /* Print 8-bit characters? */

/*
 * Counters of different kinds
 */
int copies_number = 1;		/* Number of copies to print */
int pages = 0;			/* Number of logical pages printed */
int sheets = 0;			/* Number of physical pages printed */
int old_pages, old_sheets;	/* Value before printing current file */
int sheetside = 0;		/* Side of the sheet currently printing */

/*
 * Sheet dimensions
 */
double paper_height = HEIGHT; 	/* Paper dimensions */
double paper_width = WIDTH;
double page_height = HEIGHT;	/* Page dimensions */
double page_width = WIDTH;

/*
 * Other global variables.
 */
char *prologue = NULL;		/* postscript header file */
#ifdef LPR_PRINT
char *lpr_opt = NULL;		/* Options to lpr */
#endif
float new_fontsize = 0.0;	/* To scrute new values for fontsize */
char *header_text = NULL;	/* Allow for different header text */
char *footer_text = NULL;	/* Allow for different header text */
int column_width = 8;	        /* Default column tab width (8) */
int new_linesrequest = 0;	/* To scrute new values for lines_requested */

/*
 * Other global variables.
 */
int no_files = TRUE;		/* No file until now */
float fontsize = 0.0;		/* Size of a char for body font */
char *command;			/* Name of a2ps program */
int stdout_redirected = FALSE;	/* Has stdout been redirected ? */


/************************************************************************/
/*				style selection				*/
/************************************************************************/
/*
 * Return the index of the language which has string has name or abbrev.
 * Return PLAIN if none
 */
STYLE select_style (char *string)
{
  STYLE lang;
  const char ** abbr;

  for (lang=0 ; lang < NBR_LANGUAGES ; lang++) {
    if (!strcmp(string, languages[lang].name))
      return lang;
    for (abbr = languages[lang].abbreviations ; **abbr ; abbr++)
      if (!strcmp(string, *abbr))
	return lang;
  }
  return PLAIN;
}
  
/*
 * Given a filename, guess its languages from the prefix
 */
STYLE guess_language (char *filename)
{
#define FILEBUFSIZE	128
  char * last_dot=NULL;

#ifdef FILE_COMMAND
  char command[FILEBUFSIZE];
  char res[FILEBUFSIZE];
  char field1[FILEBUFSIZE], field2[FILEBUFSIZE];
  FILE *ptr;
  STYLE lang;
  
  /* first, try to guess using file */
  strcpy(command, FILE_COMMAND" ");
  strcat(command, filename);
  if ((ptr = popen(command, "r")) != NULL) {
    fgets(res, FILEBUFSIZE, ptr);
    /* the two first fields are of interest */
    sscanf(res, "%*[^:]: %s%s", field1, field2);
    /* first, if it says it's an executable, the name is in the 2nd field */
    if (!strcmp(field1, "executable"))
      if (strrchr(field2, '/')) {
	if ((lang = select_style(strrchr(field2, '/')+1)) != PLAIN)
	  return lang;
      } else if ((lang = select_style(field2)) != PLAIN)
	return lang;
    
    if (strcmp(field1, "c")) {
      /* never trust file when it says it's a c program */
      /* but try for the others */
      if ((lang = select_style(field1)) != PLAIN)
	return lang;
    }
  }
#endif FILE_COMMAND  


  /* them, trust the prefix */
  if ((last_dot=strrchr(filename, '.')) != NULL)
    return select_style(last_dot+1);
  return PLAIN;
}

/************************************************************************/
/*				Interface				*/
/************************************************************************/
/*
 * Print the version of a2ps, and information depending on the installation.
 * Then exit.
 */
void version(void)
{
#define B2YN(B)	((B) ? "yes" : "no")

  int i, tab;

  fprintf(stderr, "%s\n", command);
  fprintf(stderr, "\tversion               = %s\n", VERSION);
  fprintf(stderr, "\tpaper size            = %.2fin x %.2fin\n", HEIGHT, WIDTH);
  fprintf(stderr, "\talternative size      = %.2fin x %.2fin\n", ALT_HEIGHT, ALT_WIDTH);
  fprintf(stderr, "\tmargin                = %.2fin\n", MARGIN);
#ifdef LPR_PRINT
  fprintf(stderr, "\tprint capabilities    = yes\n");
  fprintf(stderr, "\t  print by default    = %s\n", B2YN(LPR_PRINT_DFLT));
  fprintf(stderr, "\t  lpr command         = %s\n", LPR_COMMAND); 
  fprintf(stderr, "\t  option              = %s\n", LPR_OPT); 
  fprintf(stderr, "\t  option for printer  = %s\n", LPR_DEST_OPT); 
#  ifdef RECTO_VERSO_PRINTING
#    ifdef ONESIDED
  fprintf(stderr, "\t  1 sided option      = %s\n", ONESIDED);
#    endif
  fprintf(stderr, "\t  2 sided option      = %s\n", TWOSIDED);
  fprintf(stderr, "\t  default             = %d-sided\n", 
	  TWOSIDED_DFLT ? 2 : 1);
#  else 
  fprintf(stderr, "\t  recto-verso         = no\n");
#  endif RECTO_VERSO_PRINTING
#else 
  fprintf(stderr, "\tprint capabilities    = no\n");
#endif
  fprintf(stderr, "\tISOLatin1 default     = %s\n", B2YN(ISOLATIN1_DFLT));
  fprintf(stderr, "\tdirectory separator   = %c\n", DIR_SEP);
  fprintf(stderr, "\tpretty print default  = %s\n", B2YN(AUTO_DFLT));
  fprintf(stderr, "\tknown languages (%d)  = ", NBR_LANGUAGES);
  tab = 0;
  for (i=0 ; i < NBR_LANGUAGES ; i++) {
    if (tab >= 5) {
      fprintf(stderr, "\n\t\t\t    \t");
      tab = 0;
    }
    tab += (strlen(languages[i].name) < 8) ? 1 : 2;
    fprintf(stderr, "%s\t", languages[i].name);
  }
  fprintf (stderr, "\n");  
  exit(EXIT_SUCCESS);
}

/*
 * Print a usage message.
 */
void usage(void)
{
  int tab;
  STYLE lang;

  fprintf(stderr,"a2ps v%s \n"
	  "usage: %s [global] [pos.] [file 1 [[pos.] file 2 ...]]\n\n",
	  VERSION, command);

  /* Global options */
  fprintf(stderr,"global =  -?\t-h\tprint this information\n");
  fprintf(stderr,"          -V\t\tversion and installation information\n");
  fprintf(stderr,"          -8\t-n8\t%s 8-bit chars\n",
	  ISOLATIN1_DFLT ? "DISPLAY (don't)" : "display (DON'T)");
  fprintf(stderr,"          -a\t-na\tuse (DON'T) alternate paper format\n");
  fprintf(stderr,"          -b\t-nb\tforce (DON'T) binary printing\n");
  fprintf(stderr,"          -c\t-nc\tallow (DON'T) two files on the same sheet\n");
  fprintf(stderr,"          -f\t-nf\tFOLD (don't) lines\n");
  fprintf(stderr,"          -G\t\tdon't print but write guessed languages\n");
  fprintf(stderr,"          -Ifile\tinclude this file as a2ps prologue\n");
  fprintf(stderr,"          -o\t-ofile\tsave into a2ps.ps or <file>\n");
#ifdef LPR_PRINT
  fprintf(stderr,"          -P -Ppr -nP\t%s directly to the default printer/pr\n",
	  LPR_PRINT_DFLT ? "SEND (don't)" : "send (DON'T)");
#endif
  fprintf(stderr,"          -q\t\tprint in quiet mode (no summary)\n");
  fprintf(stderr,"          -r\t-nr\tRESTART (don't) page number after each file\n");
#ifdef RECTO_VERSO_PRINTING
  fprintf(stderr,"          -s1\t-s2\t%s printing\n",
	  TWOSIDED_DFLT ? "one-sided (TWO-SIDED)" : "ONE-SIDED (two-sided)");
#endif
  fprintf(stderr,"          -tnum\t\tset tab size to n\n");
  fprintf(stderr,"          -v\t-nv\tVISIBLE (blank) display of unprintable chars\n");
  fprintf(stderr,"          -W\t-Wlang\tgenerate prescript report on all/lang style\n");
  fprintf(stderr, "\n");

  /* Positionnal options */
  fprintf(stderr,"pos.   =  -#num\t\tnumber of copies to print\n");
  fprintf(stderr,"          -1\t-2\tone/TWIN page(s) per sheet\n");
  fprintf(stderr,"          -B\t-nB\tprint (DON'T) in bold font\n");
  fprintf(stderr,"          -d\t-nd\tprint (DON'T) current date at the bottom\n");
  fprintf(stderr,"          -Fnum\t\tfont size, num is a float number\n");
  fprintf(stderr,"          -H -Hstr -nH\tuse FILENAME/str/nothing as header title\n");
  fprintf(stderr,"          -i\t-ni\tINTERPRET (don't) tab, bs and ff chars\n");
  fprintf(stderr,"          -k -klang -nk\tGUESS/use lang/don't use styles\n");
  fprintf(stderr,"          -L\t-nL\tPRINT (don't) login ID on top of page\n");
  fprintf(stderr,"          -l\t-p\tprint in LANDSCAPE/portrait mode\n");
  fprintf(stderr,"          -lnum\t\tuse num lines per page\n");
  fprintf(stderr,"          -m\t\tprocess the file as a man\n");
  fprintf(stderr,"          -n\t-nn\tNUMBER (don't) line files\n");
  fprintf(stderr,"          -s\t-ns\tPRINT (don't) surrounding borders\n");
  fprintf(stderr,"          -t\t-nt\ttranslate (DON'T) in symbols\n");
  fprintf(stderr,"          -u -ustr -nu\tuse FILENAME/str/nothing as footer title\n");
  fprintf(stderr, "\n");

  /* Known languages */
  fprintf(stderr,"lang. =\t");
  tab = 0;
  for (lang=0 ; lang < NBR_LANGUAGES ; lang++) {
    if (tab >= 8) {
      fprintf(stderr, "\n\t");
      tab = 0;
    }
    tab += (strlen(languages[lang].name) < 8) ? 1 : 2;
    fprintf(stderr, "%s\t", languages[lang].name);
  }
  fprintf (stderr, "\n");
  exit(EXIT_SUCCESS);
}

/************************************************************************/
/*				arguments				*/
/************************************************************************/
/*
 * Exit because of a bad argument
 */
void bad_arg(char *arg)
{
  fprintf(stderr, "%s: Bad argument given: '%s'. Try -h for help.\n",
	  command, arg);
  exit(EXIT_BADARG);
}

/*
 * Set an option only if it's global.
 */
void set_global_option(char *arg)
{
  switch (arg[1]) {
  case '?':				/* help */
  case 'h':
    usage();
  case 'a':                 		/* alternative paper sizes */
    if (arg[2] != NUL)
      bad_arg(arg);
    paper_height = ALT_HEIGHT;
    paper_width = ALT_WIDTH;
    break;
  case 'b':				/* print binary files */
    if (arg[2] != NUL)
      bad_arg(arg);
    print_binaries = TRUE;
    break;
  case 'c': /* allow two files per sheet */
    if (arg[2] != NUL)
      bad_arg(arg);
    twinfiles = TRUE;
    break;
  case 'p':
    if (arg[2] != NUL)
      bad_arg(arg);
    new_landscape = FALSE;  		/* portrait format */
    break;
  case 'f':				/* fold lines too large */
    if (arg[2] != NUL)
      bad_arg(arg);
    folding = TRUE;
    break;
  case 'G':				/* Don't print, but guess file type */
    if (arg[2] != NUL)
      bad_arg(arg);
    guess = TRUE;
    break;
  case 'I':				/* include this file as ps prologue */
    if (arg[2] == NUL)
      bad_arg(arg);
    prologue = arg+2;
    break;
  case 'n':
    if (arg[2] == NUL)
      return;
    if (arg[3] != NUL)
      bad_arg(arg);
    switch (arg[2]) {
    case '8':			/* don't print 8-bit chars */
      ISOlatin1 = FALSE;
      break;
    case 'a':                 	/* regular paper sizes */
      paper_height = HEIGHT;
      paper_width = WIDTH;
      break;
    case 'b':			/* don't print binaries */
      print_binaries = FALSE;
      break;
    case 'c':			/* don't allow 2 files/sheet */
      twinfiles = FALSE;
      break;
    case 'f':			/* cut lines too long */
      folding = FALSE;
      break;
    case 'P':		
#ifdef LPR_PRINT		/* even if it is not compiled for  */
      lpr_print = FALSE;	/* direct printing, we do not want */
#endif				/* complains for the option -nP    */
      break;
    case 'r':			/* don't restart sheet number */
      restart = FALSE;
      break;
    case 'v':			/* only printable chars */
      only_printable = TRUE;
      break;
    case 'B':			/* pos. options */
    case 'd':
    case 'H':
    case 'i':
    case 'k':
    case 'L':
    case 'm':
    case 'n':
    case 's':
    case 't':
    case 'u':
      if (arg[3] != NUL)
	bad_arg(arg);
      return;
    default:
      bad_arg(arg);
    }
    break;
  case 'o':			/* output goes into a file */
    if (arg[2] != NUL) {
      if (freopen(arg+2, "w", stdout) == NULL) {
	fprintf(stderr, "%s: Unable to create output file %s.\n",
		command, arg+2);
	exit(EXIT_FAILURE);
      }
    } else {
      if (freopen("a2ps.ps", "w", stdout) == NULL) {
	fprintf(stderr, "%s: Unable to create output file a2ps.ps.\n",
		command);
	exit(EXIT_FAILURE);      
      }
    }
#ifdef LPR_PRINT
    lpr_print = FALSE;
#endif
    stdout_redirected = TRUE;
    break;
#ifdef LPR_PRINT
  case 'P':					/* fork a process to print */ 
    if (stdout_redirected) {
      fprintf(stderr, "%s: -P can't be used after -o.\n", command);
      exit(EXIT_FAILURE);
    }
    lpr_print = TRUE;
    if (arg[2] != NUL) {
      lpr_opt = (char *)malloc(strlen(arg)+strlen(LPR_DEST_OPT));
      strcpy(lpr_opt, LPR_DEST_OPT);
      strcat(lpr_opt, arg+2);
    }
    break;
#endif
  case 'q':					/* don't print a summary */
    no_summary = TRUE;
    break;
  case 'r':					/* restart sheet number */
    if (arg[2] != NUL)
      bad_arg(arg);
    restart = TRUE;
    break;
  case 's':					/* surrounding border */
    if (arg[2] == NUL)
      return;
    if (arg[3] == NUL) {
      if (arg[2] == '1') {			/* one-sided printing */
#ifdef RECTO_VERSO_PRINTING
	rectoverso = FALSE;
#endif
	break;
      }
      if (arg[2] == '2') {			/* two-sided printing */
#ifdef RECTO_VERSO_PRINTING
	rectoverso = TRUE;
#endif
	break;
      }
    }
    bad_arg(arg);
    break;
  case 't':
    if (arg[2] == NUL) return;			/* translate symbols */
    else if ((column_width = atoi(arg+2)) <= 0)    /* set tab size */
      bad_arg(arg);
    break;
  case 'v':				/* print control chars */
    if (arg[2] != NUL)
      bad_arg(arg);
    only_printable = FALSE;
    break;
  case 'V':				/* version and configuration info */
    if (arg[2] != NUL)
      bad_arg(arg);
    version();
  case 'W':				/* give a full report */
    if (arg[2] == NUL)
      report();
    else {
      STYLE language;
      language = select_style(arg+2);
      if (language != PLAIN) 
	report_one_language(language);
      else 
	fprintf (stderr, "%s: Language %s unknown. Try -h for help.\n",
		 command, arg+2);
      exit(EXIT_UKNLANG);
    }
  case '8':				/* print 8-bit chars */
    if (arg[2] != NUL)
      bad_arg(arg);
    ISOlatin1 = TRUE;
    break;
  case '1':	/* these never have arguments */
  case '2':
  case 'B':
  case 'd':
  case 'i':
  case 'L':
  case 'm':
    if (arg[2] != NUL)
      bad_arg(arg);
  case 'u':	/* these may have arguments */
  case '#':
  case 'F':
  case 'H':
  case 'l':
    return;
  case 'k':	/* for this, it's easy to see if the arg is correct */
    if ((arg[2] != NUL) && (select_style(arg+2) == PLAIN)) {
      fprintf (stderr, "%s: Language %s unknown. Try -h for help.\n",
	       command, arg+2);
      exit(EXIT_UKNLANG);
    }
    return;
  default:
    bad_arg(arg);
  }
  arg[0] = NUL;
}

/*
 * Set an option of the command line. This option will be applied
 * to all files that will be found in the rest of the command line.
 * The -H option is the only exception: it is applied only to the
 * file.
 */
void set_positional_option(char *arg)
{
  int copies;
  int lines;
  float size;

  switch (arg[1]) {
  case NUL:				/* global option */
    break;
  case '#':				/* n copies */
    if (sscanf(&arg[2], "%d", &copies) != 1 || copies <= 0)
      fprintf(stderr, "Bad number of copies: '%s'. Ignored\n", &arg[2]);
    else
      copies_number = copies;
    printf("/#copies %d def\n", copies_number);
    break;
  case '1':				/* 1 logical page per sheet */
    if (arg[2] != NUL)
      bad_arg(arg);
    new_twinpages = FALSE;
    break;
  case '2':				/* twin pages */
    if (arg[2] != NUL)
      bad_arg(arg);
    new_twinpages = TRUE;
    break;
  case 'B':
    new_fontweight = BLACK;		/* use bold font */
    break;
  case 'd':				/* print current date/time */
    printdate = TRUE;
    break;
  case 'F':				/* change font size */
    if (arg[2] == NUL 
	|| sscanf(&arg[2], "%f", &size) != 1 
	|| size == 0.0) {
      fprintf(stderr, "Wrong value for option -F: '%s'. Ignored\n",
	      &arg[2]);
      break;
    }
    new_fontsize = size;
    break;
  case 'H':				/* header text */
    header_text = ((arg[2]) != NUL) ? arg+2 : NULL;
    no_header=FALSE;
    break;
  case 'i':				/* interpret control chars */
    if (arg[2] != NUL)
      bad_arg(arg);
    interpret = TRUE;
    break;
  case 'k':           			
    if (arg[2] != NUL)			/* select language */
      style = select_style (arg+2);
    else 
      automatic_style = TRUE;		/* auto pretty-print */
    break;
  case 'l':
    if (arg[2] == NUL) {		/* landscape format */
      new_landscape = TRUE;
      break;
    }
    /* set lines per page */
    /* Useful with preformatted files. Scaling is automatically	*/
    /* done when necessary.						*/
    if (sscanf(&arg[2], "%d", &lines) != 1
	|| lines < 0 || lines > MAX_LINES) {
      fprintf(stderr, "Wrong value for option -l: '%s'. Ignored\n",
	      &arg[2]);
      break;
    }
    new_linesrequest = lines;
    break;
  case 'L':			/* no login name in footer */
    login_id = TRUE;
    break;
  case 'm':				/* Process file as a man */
    new_linesrequest = MAN_LINES;
    numbering = FALSE;
    break;
  case 'n':				/* number file lines */
    if (arg[2] == NUL) {
      numbering = TRUE;
      break;
    }
    switch (arg[2]) {
    case 'B':			/* disable bold text */
      new_fontweight = NORMAL;
      break;
    case 'd':			/* don't print date/time */
      printdate = FALSE;
      break;
    case 'H':			/* don't print header */
      no_header = TRUE;
      break;
    case 'i':			/* don't interpret ctrl chars */
      interpret = FALSE;
      break;
    case 'k':			/* don't use keyword highlight	*/
      automatic_style = FALSE;
      style = PLAIN;
      break;
    case 'L':			/* no login name in footer */
      login_id = FALSE;
      break;
    case 'l':			/* portrait format */
      new_landscape = FALSE;
      break;
    case 'm':			/* stop processing as a man */
      new_linesrequest = 0;
      break;
    case 'n':			/* don't number lines */
      numbering = FALSE;
      break;
    case 'p':			/* landscape format */
      new_landscape = TRUE;
      break;
    case 's':			/* no surrounding border */
      no_border = TRUE;
      break;
    case 't':			/* don't translate symbols */
      translate_symbols_req = FALSE;
      break;
    case 'u':			/* no filename nor text in footer */
      footer = FALSE;
      break;
    default:
      bad_arg(arg);
    }
    break;
  case 's':				/* surrounding border */
    if (arg[2] != NUL)
      bad_arg(arg);
    no_border = FALSE;
    break;
  case 't':				/* translate symbols */
    if (arg[2] != NUL)
      bad_arg(arg);
    translate_symbols_req = TRUE;
    break;
  case 'u':			/* print filename in footer */
    footer_text = ((arg[2]) != NUL) ? arg+2 : NULL;
    footer = TRUE;
    break;
  default:
    bad_arg(arg);
  }
}

/************************************************************************/
/*			Main routine for a2ps.				*/
/************************************************************************/
int main(int argc, char *argv[])
{
  register int narg;
  register char *arg;
  int total;
  STYLE lang;		/* style used to print current file */

#ifdef LPR_PRINT
  int fd[2];
  char *lpr_args[10];
#endif
  
  /* Process global options  */
  command = argv[0];
  arg = argv[narg = 1];
  while (narg < argc) {
    if (arg[0] == '-')
      set_global_option(arg);
    arg = argv[++narg];
  }

  /* do we have not to print, but return the type of the files */
  if (guess) {
    STYLE lang;
    arg = argv[narg = 1];
    while (narg < argc) {
      if (arg[0] != NUL) {
	if (arg[0] == '-')
	  set_positional_option(arg);
	else {
	  lang = guess_language(arg);
	  printf("[%s(%s)]\n", arg, 
		 lang == PLAIN ? "unknown" : languages[lang].name);
	}
      }
      arg = argv[++narg];
    }
    exit(EXIT_SUCCESS);
  }
  
#ifdef LPR_PRINT
  /* Start lpr process */
  if (lpr_print) {
    if (pipe(fd) == -1) {
      fprintf(stderr, "Could not open a pipe.\n");
      exit (1);
    };
    if (fork() == 0) {
      dup2(fd[0], 0);
      close(fd[0]); close(fd[1]);
      narg = 0;
      lpr_args[narg++] = LPR_COMMAND;
#  ifdef LPR_OPT
      if (LPR_OPT[0]!=NUL)
	lpr_args[narg++] = LPR_OPT;
#  endif
      if (lpr_opt && lpr_opt[0]!=NUL)
	lpr_args[narg++] = lpr_opt;
#  ifdef RECTO_VERSO_PRINTING
      if (rectoverso)
	lpr_args[narg++] = TWOSIDED;
#    ifdef ONESIDED
      else
	lpr_args[narg++] = ONESIDED;
#    endif
#  endif
      lpr_args[narg] = (char *)0;
      execvp(LPR_COMMAND, lpr_args);
      fprintf(stderr, "Error starting lpr process \n");
      exit(EXIT_FAILURE);
    }
    dup2(fd[1],1);
    close(fd[0]);
    close(fd[1]);
  }
#endif

  /* Initialize variables not depending of positional options */
  landscape = twinpages = -1;	        /* To force format switching */
  fontsize = -1.0;			/* To force fontsize switching */
  page_height = (double)(paper_height - MARGIN) * PIXELS_INCH;
  page_width  = (double)(paper_width  - MARGIN) * PIXELS_INCH;
    
  /* Postcript prologue printing */
  print_prologue();
    
  /* Print files designated or standard input */
  arg = argv[narg = 1];
  while (narg < argc) {
    if (arg[0] != NUL) {
      if (arg[0] == '-')
	set_positional_option(arg);
      else {
	if (freopen(arg, "r", stdin) == NULL) {
	  fprintf(stderr, "Error opening %s\n", arg);
	  clean_up();
	  printf("\n%%%%Trailer\ndocsave restore end\n\4");
	  exit(EXIT_FAILURE);
	}
	no_files = FALSE;

	/* Save counters values */
	old_pages = pages;
	if (twinfiles && twinpages)
	  old_sheets = sheets;
	else
	  old_sheets = sheets + sheetside;

	/* Print the file */
	if (style != PLAIN)
	  lang = style;
	else if (automatic_style)
	  lang = guess_language(arg);
	else
	  lang = PLAIN;

	/* prescript should always be in -t (convert symbol) */
	translate_symbols = translate_symbols_req 
	  || ((lang != PLAIN) && !strcmp(languages[lang].name, "prescript"));

	print_file(arg, header_text, lang);

	/* Print the number of pages and sheets printed */
	if (no_summary == FALSE) {
	  total = pages - old_pages;
	  fprintf(stderr, "[%s(%s): %d page%s on ", arg,
		  lang == PLAIN ? "plain" : languages[lang].name,
		  total, total == 1 ? "" : "s");
	  total = sheets - old_sheets + sheetside;
#ifdef RECTO_VERSO_PRINTING
	  if (rectoverso)
	    total = (total+1) / 2;
#endif
	  fprintf(stderr, "%d sheet%s]\n", total, total == 1 ? "" : "s");
	}

	/* Reinitialize header title */
	header_text = NULL;
      }
    }
    arg = argv[++narg];
  }
  if (no_files) {
    /* prescript should always be in -t (convert symbol) */
    translate_symbols = translate_symbols_req 
      || ((style != PLAIN) && !strcmp(languages[style].name, "prescript"));
    print_file("stdin", header_text, style);
  }

  /* Print the total number of pages printed */
  if (no_summary == FALSE && pages != old_pages) {
    fprintf(stderr, "[Total: %d page%s on ", pages, pages == 1 ? "" : "s");
    total = sheets + sheetside;
#ifdef RECTO_VERSO_PRINTING
    if (rectoverso)
      total = (total+1) / 2;
#endif
    fprintf(stderr, "%d sheet%s]\n", total, total == 1 ? "" : "s");
  }

  /* And stop */
  clean_up();
  printf("\n%%%%Trailer\n");
  printf("%%%%Pages: %d\n", sheets + sheetside);
  printf("docsave restore end\n");

  return(EXIT_SUCCESS);
}
