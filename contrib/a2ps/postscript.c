/************************************************************************/
/*				postscript.c				*/
/* routines for the postscript output					*/
/************************************************************************/

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifndef STDC_HEADERS
#  include "ansi.h"
#endif

#if TIME_WITH_SYS_TIME
#  include <time.h>
#  include <sys/time.h>
#else 
#  if HAVE_SYS_TIME_H
#    include <sys/time.h>
#  else
#    include <time.h>
#  endif
#endif

#include "a2ps.h"		/* most global variables 		*/
#include "afm.h"		/* width of postscript fonts 		*/
#include "buffer.h"		/* routines of input and formating 	*/
#include "routines.h"		/* general interest routines		*/
#include "postscript.h"


/*
 * Counters of different kinds.
 */
unsigned long WX = 0;		/* Column number in pts (in current line) */
int column = 0;			/* Column number in chars 		*/
int columnsperline;		/* Characters per output line 		*/
unsigned long WXperline;	/* Points per output line		*/
int line;			/* Line number (in current page) 	*/
int line_number;		/* Source line number 			*/
int linesperpage;		/* Lines per page 			*/
int lines_requested = 0;	/* Lines per page requested by the user */
int nonprinting_chars, chars;	/* Number of nonprinting and total chars */

/*
 * Other global variables.
 */
int first_page;			/* First page for a file		*/
int prefix_size;		/* Width in characters for line prefix	*/
unsigned long prefix_WX;	/* width for line prefix in points	*/
float header_size;		/* Size of the page header		*/
extern float fontsize;		/* Size of a char for body font		*/
unsigned char output_string[BUFFER_SIZE+1]; /* formated output		*/
char * curr_filename;		/* Full name of the current file	*/
char trunc_filename[MAXFILENAME+1];/* Truncated name of the file being printed */
char file_date[18];		/* Last modification time for current file */
char current_date[18];		/* Date for today */

/***********************************/

/*
 * Print the prologue necessary for printing each physical page.
 * Adobe convention for page independence is enforced through this routine.
 */
void print_page_prologue(int side)
/* side: Logical page to print (left/right) */
{
  /* General format */
  printf("/twp %s def\n", twinpages ? "true" : "false");
  printf("/fnfs %d def\n", landscape ? 11 : twinpages ? 10 : 15);
  printf("/dfs fnfs 0.8 mul def\n");
  printf("/df /Helvetica dfs getfont def\n");
  printf("/dw df setfont td stringwidth pop def\n");
  printf("/sfnf filenmfontname fnfs getfont def\n");
  printf("/hm fnfs 0.25 mul def\n");
  /* Header size */
  if (header_size == 0.0)
    printf("/hs 0.0 def\n");
  else
    printf("/hs %g inch def\n",
	   landscape || twinpages ? LANDSCAPE_HEADER : PORTRAIT_HEADER);
  /* Font sizes */
  printf("/bfs %g def\n", fontsize);
  printf("/bdf /Courier-Bold bfs getfont def\n"); 
  printf("/bm bfs 0.7 mul def\n");
  printf("/bf %s bfs getfont def\n",
	 fontweight == NORMAL ? "/CourierBack" : "/Courier-Bold");
  printf("/cw bf setfont (0) stringwidth pop def \n");
  printf("/itf /Courier-Oblique bfs getfont def\n");  
  printf("/bif /Courier-BoldOblique bfs getfont def\n");  
  printf("/tf /Times-Roman bfs getfont def\n");     
  printf("/sf /Symbol bfs getfont def\n");     
  /* Page attributes */
  printf("/l %d def\n", linesperpage);
  printf("/c %d def\n", columnsperline);
  printf("/pw\n");
  printf("   bf setfont (0) stringwidth pop c mul bm dup add add\n");
  printf("   def\n");
  printf("/ph\n");
  printf("   bfs l mul bm dup add add hs add\n");
  printf("   def\n");
  printf("/fns\n");
  printf("      pw\n");
  printf("      fnfs 4 mul dw add (Page 999) stringwidth pop add\n");
  printf("    sub\n");
  printf("  def\n");
  printf("/tm margin twp {3} {2} ifelse div def\n");
  printf("/sd %d def\n", side);
  if (landscape) {
    printf("/y [ rm ph add bm add\n");
    printf("          dup ] def\n");
/* This is experimental (Akim) 
    printf("/sny dfs dfs add def\n"); */
    printf("/sny tm dfs sub def\n");

    printf("/snx sh tm dfs add sub def\n");
    printf("/dy sny def\n");
    printf("/dx tm dfs add def\n");
    if (twinpages) {
      printf("/x [ tm			%% left page\n");
      printf("          dup 2 mul pw add	%% right page\n");
      printf("        ] def\n");
    }
    else {
      printf("/x [ tm dup ] def\n");
    }
    printf("/scx sh 2 div def\n");
  }
  else {
    printf("/x [ lm dup ] def\n");
    printf("/sny tm dfs 2 mul sub def\n");
    printf("/snx sw rm sub dfs sub def\n");
    printf("/dy sny def\n");
    printf("/dx lm def\n");
    if (twinpages) {
      printf("/y [ tm ph add 2 mul %% up\n");
      printf("          tm ph add	 %% down\n");
      printf("        ] def\n");
    }
    else {
      printf("\n%% Only one logical page\n");
      printf("/y [ sh tm sub dup ] def\n");
    }
    printf("/scx sw 2 div def\n");
  }
  printf("/fny dy def\n");
  printf("/fnx scx def\n");
  printf("/ly fnfs 2 div y sd get add def\n");
  printf("/lx snx def\n");
  printf("/sx %d def\n", prefix_size);
  printf("/d (%s) def\n", file_date);
  printf("( %s ) fn\n", trunc_filename);
  /* If a footer text was given, then use it, else if the truncated
   * name is not the same as the filename, use it, else, truncated
   * filename (less postscript text) */
  if (footer_text)
    printf("/footer ( %s ) def\n", footer_text);
  else if (strcmp(trunc_filename, curr_filename))
    printf("/footer ( %s ) def \n", curr_filename);
  else
    printf("/footer filenm def\n");
}


/*
 * Print the standard prologue.
 */
void print_standard_prologue(char *datestring)
{
  printf("%%!PS-Adobe-3.0\n");
  printf("%%%%Creator: a2ps version %s\n", VERSION);
  printf("%%%%CreationDate: %.24s\n", datestring);
  printf("%%%%Pages: (atend)\n");
  printf("%%%%DocumentFonts: Courier Courier-Oblique Courier-BoldOblique Times-Roman Courier-Bold Helvetica Helvetica-Bold Symbol\n");
  printf("%%%%EndComments\n");
  printf("%% Copyright (c) 1993, 1994, Miguel Santana, M.Santana@frgu.bull.fr\n");
  printf("\n/$a2psdict 100 dict def\n");
  printf("$a2psdict begin\n");
  printf("\n%% General macros.\n");
  printf("/xdef {exch def} bind def\n");
  printf("/getfont {exch findfont exch scalefont} bind def\n");

  if (ISOlatin1) {
    printf("\n%% Set up ISO Latin 1 character encoding\n");
    printf("/reencodeISO {\n");
    printf("	dup dup findfont dup length dict begin\n");
    printf("	{ 1 index /FID ne { def }{ pop pop } ifelse\n");
    printf("	} forall\n");
    printf("	/Encoding ISOLatin1Encoding def\n");
    printf("	currentdict end definefont\n");
    printf("} def\n");
    printf("/Helvetica-Bold reencodeISO def\n");
    printf("/Helvetica reencodeISO def\n");
    printf("/Courier reencodeISO def\n");
    printf("/Courier-Bold reencodeISO def\n");
    printf("/Courier-Oblique reencodeISO def\n");
    printf("/Courier-BoldOblique reencodeISO def\n");
    printf("/Times-Roman reencodeISO def\n");
  }

  printf("\n%% Create Courier backspace font\n");
  printf("/backspacefont {\n");
  printf("    /Courier findfont dup length dict begin\n");
  printf("	{ %% forall\n");
  printf("	    1 index /FID eq { pop pop } { def } ifelse\n");
  printf("	} forall\n");
  printf("	currentdict /UniqueID known { %% if\n");
  printf("	    /UniqueID UniqueID 16#800000 xor def\n");
  printf("	} if\n");
  printf("	CharStrings length 1 add dict begin\n");
  printf("	    CharStrings { def } forall\n");
  printf("	    /backspace { -600 0 0 0 0 0 setcachedevice } bind def\n");
  printf("	    currentdict\n");
  printf("	end\n");
  printf("	/CharStrings exch def\n");
  printf("	/Encoding Encoding 256 array copy def\n");
  printf("	Encoding 8 /backspace put\n");
  printf("	currentdict\n");
  printf("    end\n");
  printf("    definefont pop\n");
  printf("} bind def\n");

  printf("\n%% FUNCTIONS\n");
  printf("\n%% Function filename: Initialize file printing.\n");
  printf("/fn\n");
  printf("{ /filenm xdef\n");
  printf("  /filenmwidth filenm stringwidth pop def\n");
  printf("  /filenmfont\n");
  printf("       filenmwidth fns gt\n");
  printf("       {\n");
  printf("	       filenmfontname\n");
  printf("	       fnfs fns mul filenmwidth div\n");
  printf("	     getfont\n");
  printf("       }\n");
  printf("       { sfnf }\n");
  printf("     ifelse\n");
  printf("  def\n");
  printf("} bind def\n");
  printf("\n%% Function header: prints page header. no page\n");
  printf("%% is passed as argument.\n");
  printf("/hp\n");
  printf("  { x sd get  y sd get hs sub 1 add  moveto\n");
  printf("    df setfont\n");
  printf("    gsave\n");
  printf("      x sd get y sd get moveto\n");
  printf("      0 hs 2 div neg rmoveto \n");
  printf("      hs setlinewidth\n");
  printf("      0.95 setgray\n");
  printf("      pw 0 rlineto stroke\n");
  printf("    grestore\n");
  printf("    gsave\n");
  printf("      dfs hm rmoveto\n");
  printf("      d show				%% date/hour\n");
  printf("    grestore\n");
  printf("    gsave\n");
  printf("      pnum cvs pop				%% page pop up\n");
  printf("        pw (Page 999) stringwidth pop sub\n");
  printf("        hm\n");
  printf("	  rmoveto\n");
  printf("      (Page ) show pnum show		%% page number\n");
  printf("    grestore\n");
  printf("    empty pnum copy pop\n");
  printf("    gsave\n");
  printf("      filenmfont setfont\n");
  printf("         fns filenm stringwidth pop sub 2 div dw add\n");
  printf("          bm 2 mul \n");
  printf("        add \n");
  printf("        hm\n");
  printf("      rmoveto\n");
  printf("        filenm show			%% file name\n");
  printf("      grestore\n");
  printf("    } bind def\n");
  printf("\n%% Function border: prints border page\n");
  printf("/border \n");
  printf("{ x sd get y sd get moveto\n");
  printf("  gsave				%% print four sides\n");
  printf("    0.7 setlinewidth		%% of the square\n");
  printf("    pw 0 rlineto\n");
  printf("    0 ph neg rlineto\n");
  printf("    pw neg 0 rlineto\n");
  printf("    closepath stroke\n");
  printf("  grestore\n");
  printf("} bind def\n");
  printf("\n%% Function hborder: completes border of the header.\n");
  printf("/hborder \n");
  printf("{ gsave\n");
  printf("	0.7 setlinewidth\n");
  printf("	0 hs neg rmoveto\n");
  printf("	pw 0 rlineto\n");
  printf("	stroke\n");
  printf("  grestore\n");
  printf("} bind def\n");
  printf("\n%% Function sheetnumber: prints the sheet number.\n");
  printf("/sn\n");
  printf("    { snx sny moveto\n");
  printf("      df setfont\n");
  printf("      pnum cvs\n");
  printf("	  dup stringwidth pop (0) stringwidth pop sub neg 0 rmoveto show\n");
  printf("      empty pnum copy pop\n");
  printf("    } bind def\n");
  printf("\n%% Function loginprint: prints the login id of the requestor.\n");
  printf("/lgp\n");
  printf("    { lx ly moveto\n");
  printf("      df setfont\n");
  printf("      dup stringwidth pop neg 0 rmoveto show\n");
  printf("    } bind def\n");
  printf("\n%% Function current_date: prints the current date.\n");
  printf("/cd\n");
  printf("    { dx dy moveto\n");
  printf("      df setfont\n");
  printf("      (Printed: ) show\n");
  printf("      td show\n");
  printf("    } bind def\n");
  printf("\n%% Function filename_footer: prints the file name at bottom of page.\n");
  printf("/fnf\n");
  printf("    { fnx fny moveto\n");
  printf("      df setfont\n");
  printf("      footer center show\n");
  printf("    } bind def\n");
  printf("\n%% Function center: centers text.\n");
  printf("/center\n");
  printf("    { dup stringwidth pop\n");
  printf("      2 div neg 0 rmoveto\n");
  printf("    } bind def\n");
  printf("\n%% Function s: print a source line\n");
  printf("/s  { show\n");
  printf("      /y0 y0 bfs sub def\n");
  printf("      x0 y0 moveto\n");
  printf("    } bind def\n");
  printf("\n%% Functions b, st, i, bi, t, sy: change to bold, standard, italic, bold-italic, times and symbol fonts\n");
  printf("/b  { show\n");
  printf("      bdf setfont\n");
  printf("    } bind def\n");
  printf("/st { show\n");
  printf("      bf setfont\n");
  printf("    } bind def\n");
  printf("/i  { show\n");
  printf("      itf setfont\n");
  printf("    } bind def\n");
  printf("/bi  { show\n");
  printf("      bif setfont\n");
  printf("    } bind def\n");
  printf("/t  { show\n");
  printf("      tf setfont\n");
  printf("    } bind def\n");
  printf("/sy { show\n");
  printf("      sf setfont\n");
  printf("    } bind def\n");

  printf("\n %% Function T(ab), jumps to the n-th tabulation in the current line\n");
  printf("/T { exch show\n");
  printf("       cw %d mul mul sx cw mul add x0 add y0 moveto",
	 column_width);
  printf("     } bind def\n");
    
  printf("\n%% Strings used to make easy printing numbers\n");
  printf("/pnum 12 string def\n");
  printf("/empty 12 string def\n");
  printf("\n%% Global initializations\n");
  printf("\n/CourierBack backspacefont\n");
  printf("/filenmfontname /Helvetica-Bold def\n");
  printf("/inch {72 mul} bind def\n");

  printf("\n%%\n");
  printf("%%%% Meaning of some variables and functions (coded names)\n");
  printf("%%\n");
  printf("%% Variables from 'outside'\n");
  printf("%%	twp:		twinpages?\n");
  printf("%%	sd:		sheet side\n");
  printf("%%	l:		line counter\n");
  printf("%%	c:		column counter\n");
  printf("%%	d:		date\n");
  printf("%%	td:		current date (for today)\n");
  printf("%%	lg:		login name\n");
  printf("%% Functions\n");
  printf("%%	fn:		filename printing function\n");
  printf("%%	sn:		sheetnumber printing function\n");
  printf("%%	cd:		current date printing function\n");
  printf("%%	fnf:		filename footer printing function\n");
  printf("%%	lgp:		login printing function\n");
  printf("%%	hp:		header printing function\n");
  printf("%% Coordinates\n");
  printf("%%	x, y:		the logical page\n");
  printf("%%	snx, sny:	the sheet number\n");
  printf("%%	dx, dy:		the date\n");
  printf("%%	lx, ly:		the login\n");
  printf("%%	fnx, fny:	the filename (footer)\n");
  printf("%%	scx:		x coordinate for the sheet center\n");
  printf("%% Font sizes\n");
  printf("%%	fnfs:		filename\n");
  printf("%%	bfs:		body\n");
  printf("%%	dfs:		date\n");
  printf("%% Fonts\n");
  printf("%%	df:		date\n");
  printf("%%	bf:		body\n");
  printf("%%	bdf:		bold\n");
  printf("%%	itf:		italic\n");
  printf("%%	tf:		times\n");
  printf("%%	sf:		symbol\n");
  printf("%%	sfnf:		standard filename\n");
  printf("%% Widths and heigths\n");
  printf("%%	cw:		a Courier char\n");
  printf("%%	dw:		date\n");
  printf("%%	pw, ph:		page\n");
  printf("%%	sw, sh:		sheet\n");
  printf("%% Margins\n");
  printf("%%	hm:		header\n");
  printf("%%	tm:		top\n");
  printf("%%	bm:		body\n");
  printf("%%	rm:		right\n");
  printf("%%	lm:		left\n");
  printf("%%	hs:		header size\n");
  printf("%%	fns:		filename size\n");
}


/****************************************************************/
/*		Printing a file					*/
/****************************************************************/

/*
 *
 */
void strccat(char *string, char c)
{
  int len = strlen(string);
  *(string+len) = c;
  *(string+len+1) = '\0';
}

#define STRCCAT(s, c)					\
  do { int len = strlen(s);				\
       *(s+len) = c;					\
       *(s+len+1) = '\0'; 				\
  } while (FALSE)


/*
 * Print a char in a form accepted by postscript printers.
 * Returns number of columns used (on the output) to print the char.
 */
int format_char(unsigned char c, FONT font)
{
  /* The number of columns used must be calculated here because of the
   * \ before non-ascii chars, and (, ), and \ itself */

  int len=0;

  /* Regular characters */
  if (' ' <= c && c < 0177) {
    if (c == '(' || c == ')' || c == '\\')
      STRCCAT(output_string, '\\');
    STRCCAT(output_string, c);
    return 1;
  }

  if ((ISOlatin1 || (font == SYMBOL)) && (0177 < c)) {
    sprintf(output_string, "%s\\%o", output_string, c);
    return 1;
  }

  /* We are dealing with a non-printing character */
  nonprinting_chars++;

  if (only_printable) {
    STRCCAT(output_string, ' ');
    return 1;
  }
    
  if (0177 < c) {
    strcat(output_string, "M-");
    len += 2; 
    c &= 0177;
  }

  if (c < ' ') {
    STRCCAT(output_string, '^');
    len += 2; 
    if ((c = c + '@') == '(' || c == ')' || c == '\\')
      STRCCAT(output_string, '\\');
    STRCCAT(output_string, c);
  } else if (c == 0177) {
    strcat(output_string, "^?");
    len += 2;
  } else {
    if (c == '(' || c == ')' || c == '\\')
      STRCCAT(output_string, '\\');
    STRCCAT(output_string, c);
    len++;
  }

  return len;
}

/*
 * Print a string in a form accepted by postscript printers.
 */
int format_string(unsigned char*string, FONT font)
{
  int i, delta_column=0;

  for (i = 0 ; i < strlen(string) ; i++) {
    delta_column += format_char(string[i], font);
  }
  return delta_column;
}

/*
 * Print the a2ps prologue.
 */
void print_prologue(void)
{
  register int c;
  FILE *f;
  char *datestring;
/*#if defined(SYSV) || defined(BSD)*/
  char *logname, *host;
/*#endif*/

  /* Retrieve date and hour */
  time_t date;

  if (time(&date) == -1) {
    fprintf(stderr, "Error calculating time\n");
    exit(EXIT_FAILURE);
  }
  datestring = ctime(&date);

  /* Retrieve user name and machine name */
  get_user(&logname);
  get_host(&host);
    
  /* Print a general prologue */
  if (prologue == NULL)
    print_standard_prologue(datestring);
  else if ((f = fopen(prologue, "r")) != NULL) {
    /* Header file printing */
    while ((c = getc(f)) != EOF)
      putchar(c);
  } else {
    fprintf(stderr, "Postscript header missing: %s\n", prologue);
    exit(EXIT_FAILURE);
  }

  /* Completes the prologue with a2ps static variables */
  printf("\n%% Initialize page description variables.\n");
  printf("/x0 0 def\n");
  printf("/y0 0 def\n");
  printf("/sh %g inch def\n", (double)paper_height);
  printf("/sw %g inch def\n", (double)paper_width);
  printf("/margin %g inch def\n", (double)MARGIN);
  printf("/rm margin 3 div def\n");
  printf("/lm margin 2 mul 3 div def\n");
  printf("/d () def\n");

  /* And print them */
  sprintf(current_date, "%.6s %.4s %.5s",
	  datestring+4, datestring+20, datestring+11);
  printf("/td (%s) def\n", current_date);

  /* Add the user's login name string to the Postscript output */
  if (logname != NULL || host != NULL) {
    if (logname != NULL && host != NULL)
      printf("/lg (Printed by %s from %s) def\n", logname, host);
    else if (logname != NULL)
      printf("/lg (Printed by %s) def\n", logname);
    else
      printf("/lg (Printed from %s) def\n", host);
  }

  /* If the host string was allocated via malloc, release the memory */
  if (host != NULL)
    free(host);

  /* Close prolog */
  printf("%%%%EndProlog\n\n");

  /* Go on */
  printf("/docsave save def\n");
}
/*
 * Print a physical page.
 */
void print_page(void)
{
    sheetside = 0;
    sheets++;
    printf("/sd 0 def\n");
    if (no_border == FALSE)
	printf("%d sn\n", sheets - (restart ? old_sheets : 0));
    if (printdate)
	printf("cd\n");
    if (login_id)
	printf("lg lgp\n");
    printf("pagesave restore\n");
    printf("showpage\n");
}

/*
 * Prints page header and page border and
 * initializes printing of the file lines.
 */
void start_page(void)
{
  if (sheetside == 0) {
#ifdef RECTO_VERSO_PRINTING
    if (rectoverso && (sheets & 0x1)) {
      /* Shift to left backside pages.  */
      printf("rm neg 0 translate\n");
    }
#endif
    if (landscape) {
      printf("sw 0 translate\n");
      printf("90 rotate\n");
    }
  }
  pages++;
  if (no_header == FALSE)
    printf("%d hp\n", pages - old_pages);
  if (no_border == FALSE) {
    printf("border\n");
    if (no_header == FALSE)
      printf("hborder\n");
  }
  if (footer && landscape && sheetside == 0)
    printf("fnf\n");
  printf("/x0 x %d get bm add def\n", sheetside);
  printf("/y0 y %d get bm bfs add %s add sub def\n",
	 sheetside, no_header ? "0" : "hs");
  printf("x0 y0 moveto\n");
  printf("bf setfont\n");
}

/*
 * Terminates printing, flushing last page.
 */
void clean_up(void)
{
    if (twinpages && sheetside == 1)
	print_page();
#ifdef RECTO_VERSO_PRINTING
    if (!twinfiles && rectoverso && (sheets & 0x1) != 0) {
	sheetside = 0;
	sheets++;
	printf("%%%%Page: %d %d\n", sheets, sheets);
	printf("showpage\n");
    }
#endif
}

/*
 * Adds a sheet number to the page (footnote) and prints the formatted
 * page (physical impression). Activated at the end of each source page.
*/
void end_page(void)
{
    if (twinpages && sheetside == 0) {
	sheetside = 1;
	printf("/sd 1 def\n");
    }
    else
	print_page();
}

/*
 * Begins a new logical page.
 */
void skip_page(void)
{
  if (twinpages == FALSE || sheetside == 0) {
    printf("%%%%Page: %d %d\n", sheets+1, sheets+1);
    printf("/pagesave save def\n");
    /* Reinitialize state variables for each new sheet */
    print_page_prologue(0);
  }
  start_page();
}

/*
 * Cut long filenames.
 */
void cut_filename(char *old_name, char *new_name)
{
  register char *p;
  register int i;
  char *separator;

  if ((i = strlen(old_name)) <= MAXFILENAME) {
    strcpy(new_name, old_name);
    return;
  }
  p = old_name + (i-1);
  separator = NULL;
  i = 1;
  while (p >= old_name && i < MAXFILENAME) {
    if (*p == DIR_SEP)
      separator = p;
    p--;
    i++;
  }
  if (separator != NULL)
    p = separator;
  else if (p >= old_name)
    while (p >= old_name && *p != DIR_SEP) p--;

  for (i = 0, p++; *p != NUL; i++)
    *new_name++ = *p++;
  *new_name = NUL;
}

/*
 * Test if we have a binary file.
 */
int is_binary_file(char *name)
{
  if (chars > 120 || pages > 1) {
    first_page = FALSE;
    if (chars && !print_binaries && (nonprinting_chars*100 / chars) >= 60) {
      fprintf(stderr, "%s is a binary file: printing aborted\n", name);
      return TRUE;
    }
  }
  return FALSE;
}

/*
 * Fold a line too long. Return false if was binary file.
 */
int fold_line(char *name, FONT status)
{
  /* A line has to begin in COURIER */
  if (status != COURIER)
    printf(") \n st (");
  printf(") s\n");
  if (++line >= linesperpage) {
    end_page();
    skip_page();
    if (first_page && is_binary_file(name))
      return FALSE;
    line = 0;
  }
  if (numbering)
    printf("(    +");
  else
    printf("( ");

  /* Return to the current font */
  if (status != COURIER)
    printf(")\n%s (", PS_FONT(status));
    
  return TRUE;
}



/*
 * Print the file prologue.
 */
void init_file_printing(char *name, char *title)
{
  int new_format, new_font;
  char *string;
  int lines;
  struct stat statbuf;
  float char_width;

  /* Print last page of previous file, if necessary */
  if (pages > 0 && !twinfiles)
    clean_up();

  /* Initialize variables related to the format */
  new_format = FALSE;
  if (new_landscape != landscape || new_twinpages != twinpages) {
    landscape = new_landscape;
    twinpages = new_twinpages;
    new_format = TRUE;
  }

  /* Initialize variables related to the header */
  if (no_header && name == title)
    header_size = 0.0;
  else {
    if (landscape || twinpages)
      header_size = LANDSCAPE_HEADER * PIXELS_INCH;
    else
      header_size = PORTRAIT_HEADER * PIXELS_INCH;
    cut_filename(title, trunc_filename);
  }

  /* Initialize variables related to the footer */
  curr_filename = title;

  /* Initialize variables related to the font size */
  new_font = FALSE;
  if (fontsize != new_fontsize || new_format ||
      lines_requested != new_linesrequest || fontweight != new_fontweight)
    {
      if (new_fontsize == 0.0 || (fontsize == new_fontsize && new_format))
	new_fontsize = landscape ? 6.8 : twinpages ? 6.4 : 9.0;
      if (lines_requested != new_linesrequest) {
	if ((lines_requested = new_linesrequest) != 0) {
	  /* Scale fontsize */
	  if (landscape)
	    lines = (int)((page_width-header_size) / new_fontsize) - 1;
	  else if (twinpages)
	    lines = (int)(((page_height - 2*header_size) / 2) / new_fontsize)-2;
	  else
	    lines = (int)((page_height-header_size) / new_fontsize) - 1;
	  new_fontsize *= (float)lines / (float)lines_requested;
	}
      }
      fontsize = new_fontsize;
      fontweight = new_fontweight;
      new_font = TRUE;
    }

  /* Initialize file printing, if there is any change */
  if (new_format || new_font) {
    char_width = 0.6 * fontsize;
    if (landscape) {
      linesperpage = (int)((page_width - header_size) / fontsize) - 1;
      if (! twinpages) {
	columnsperline = (int)(page_height / char_width) - 1;
      } else {
	columnsperline = (int)((page_height / 2) / char_width) - 1;
      }
    } else {
      if (!twinpages)
	linesperpage = (int)((page_height - header_size) / fontsize) - 1;
      else
	linesperpage = (int)(((page_height - 2*header_size) / 2) / fontsize)
	  - 2;
      columnsperline = (int)(page_width / char_width) - 1;
    }
    WXperline = columnsperline * Courier_WX;
    if (lines_requested > 0)
      linesperpage = lines_requested;
    if (linesperpage <= 0 || columnsperline <= 0) {
      fprintf(stderr, "Font %g too big !!\n", fontsize);
      exit(EXIT_FAILURE);
    }
  }

  /* Retrieve file modification date and hour */
  /* AD: was:  if (fstat(fileno(stdin), &statbuf) == -1) { 
   * but fileno is not ansi, and fileno(stdin) should always be 0 */
  if (fstat(0, &statbuf) == -1) {
    fprintf(stderr, "Error getting file modification time\n");
    exit(EXIT_FAILURE);
  }
  /* Do we have a pipe? */
  if (S_ISFIFO(statbuf.st_mode))
    strcpy(file_date, current_date);
  else {
    string = ctime(&statbuf.st_mtime);
    sprintf(file_date, "%.6s %.4s %.5s", string+4, string+20, string+11);
  }
}

/*
 * Print one file.
 */
void print_file(char *name, char *header, STYLE mode)
{
  register int c;
  int nchars;	/* Counts the number of chars printed including M- and ^ */
  int start_line, start_page;
  int continue_exit;
  int nextcol;
  FONT status, new_status;

  /* Reinitialize postscript variables depending on positional options */
  init_file_printing(name, header == NULL ? name : header);

  /* If we are in compact mode and the file beginning is to be printed */
  /* in the middle of a twinpage, we have to print a new page prologue */
  if (twinfiles && sheetside == 1)
    print_page_prologue(1);

  /*
   * Boolean to indicates that previous char is \n (or interpreted \f)
   * and a new page would be started, if more text follows
   */
  start_page = FALSE;
    
  /*
   * Printing binary files is not very useful. We stop printing
   * if we detect one of these files. Our heuristic to detect them:
   * if 75% characters of first page are non-printing characters,
   * the file is a binary file.
   * Option -b force binary files impression.
   */
  nonprinting_chars = chars = 0;
    
  /* Initialize printing variables */
  column = 0;
  nextcol=0;
  WX = 0;
  line = line_number = 0;
  first_page = TRUE;
  start_line = TRUE;
  prefix_size = numbering ? 6 : 1;
  prefix_WX = prefix_size * Courier_WX;
  status = COURIER;
  new_status = COURIER;

  /* Start printing */
  skip_page();

  /* Read the first char */
  c = mygetc(&new_status, mode);
  
  /* Process each character of the file */
  while (c != EOF) {
    /*
     * Preprocessing (before printing):
     * - TABs expansion (see interpret option)
     * - FF and BS interpretation
     * - replace non printable characters by a space or a char sequence
     *   like:
     *     ^X for ascii codes < 0x20 (X = [@, A, B, ...])
     *     ^? for del char
     *     M-c for ascii codes > 0x3f
     * - prefix parents and backslash ['(', ')', '\'] by backslash
     *   (escape character in postcript)
     */
    /* Form feed */
    if (c == '\f' && interpret) {
      /* Close current line */
      if (!start_line) {
	printf(") s\n");
	start_line = TRUE;
      }
      /* start a new page ? */
      if (start_page)
	skip_page();
      /* Close current page and begin another */
      end_page();
      start_page = TRUE;
      /* Verification for binary files */
      if (first_page && is_binary_file(name))
	return;
      line = 0;
      column = 0;
      WX = 0;
      if ((c = mygetc(&new_status, mode)) == EOF)
	break;
    }
	
    /* Start a new line ? */
    if (start_line) {
      if (start_page) {
	/* only if there is something to print! */
	skip_page();
	start_page = FALSE ;
	/* 
	 * if this is the first line a of new page, hence the current
	 * font is COURIER
	 */
	if (numbering) {
	  printf("(%4d|", ++line_number);
	} else 
	  printf("( ");
	/* go back to the current font */
	if (status != COURIER)
	  printf(")\n%s (", PS_FONT(status));
	start_line = FALSE;
      } else {
	/* This is not the first line in the page.
	 * A line always finishes in COURIER if numbering,
	 * or if status was not fix sized (case \n).
	 */
	if (numbering) {
	  printf("(%4d|", ++line_number);
	} else 
	  printf("( ");
	/* Therefore, go back to the current font */
	if (!IS_FIXED_SIZE(status) || (numbering && status != COURIER))
	  printf(")\n%s (", PS_FONT(status));
	start_line = FALSE;
      }
    }
    
    /* Is a new font ? This feature is used in connexion with mygetc.	*/
    if (status != new_status) {
      status = new_status;
      printf(")\n%s (", PS_FONT(status));
    }

    /* Interpret each character */
    switch (c) {
    case '\b':
      if (!interpret)
	goto print;
      /* A backspace is converted to 2 chars ('\b'). These chars	*/
      /* with the Courier backspace font produce correct under-		*/
      /* lined strings.	*/
      if (column) {
	column--;
	WX -= Courier_WX;
      }
      putchar('\\');
      putchar('b');
      break;
    case '\n':
      column = 0;
      WX = 0;
      start_line = TRUE;
      /* a line has to begin in in a fixed size font 
       * (because of the prefix which is never empty)
       */
      if (!IS_FIXED_SIZE(status) || (numbering && status != COURIER))
	printf(") \n st (");
	    
      printf(") s\n");
      if (++line >= linesperpage) {
	end_page();
	start_page = TRUE ;
	if (first_page && is_binary_file(name))
	  return;
	line = 0;
      }
      break;
    case '\t':
      if (interpret) {
	continue_exit = FALSE;
	/* Tabs are interpreted. In the case of Courier fonts,
	 * white spaces are enough, and are better understandable
	 * in the postscript generated 
	 */
	if (IS_FIXED_SIZE(status) && (WX == Courier_WX * column)) {
	  do {
	    /*	  if (++column + prefix_size > columnsperline) {	*/
	    column++;
	    WX += Courier_WX;
	    if (WX + prefix_WX > WXperline) {
	      if (folding) {
		if (fold_line(name, status) == FALSE)
		  return;
		column = 0;
		WX = 0;
	      } else {
		c = cut_line();
		continue_exit = TRUE;
		break;
	      }
	    } else
	      putchar(' ');
	  } while (column % column_width);	/* todo	*/
	} else { 
	  /* but in the other fonts, we want to go to the same
	   * column as if the font were Courier
	   */
	  nextcol = MAX(WX / Courier_WX, column) / column_width + 1;
	  WX = Courier_WX * nextcol * column_width;
	  if (WX + prefix_WX > WXperline) {
	    if (folding) {
	      if (fold_line(name, status) == FALSE)
		return;
	      column = 0;
	      WX = 0;
	      nextcol = 0;
	    } else {
	      c = cut_line();
	      continue_exit = TRUE;
	      break;
	    }
	  }
	  printf(") %d T (", nextcol);
	  column=nextcol * column_width;
	  WX = column * Courier_WX;
	}
	if (continue_exit)
	  continue;
	break;
      }
#if 0 /* pieces of this can be useful for word wrap */
    case STRING:
      if (*string_result == '\n') {
	column = 0;
	WX = 0;
	start_line = TRUE;
	/* a line has to begin in fixed size font */
	if (!IS_FIXED_SIZE(status) || (numbering && status != COURIER))
	  printf(") \n st (");
	
	printf(") s\n");
	if (++line >= linesperpage) {
	  end_page();
	  start_page = TRUE ;
	  if (first_page && is_binary_file(name))
	    return;
	  line = 0;
	}
	break;
      }
      /* Not containing a \n */
      chars += strlen(string_result);
      *output_string = '\0';
      nchars = format_string(string_result, status);
      column += nchars;
      WX += string_WX(string_result, status);
      if (prefix_WX + WX > WXperline) {
	if (folding) {
	  if (fold_line(name, status) == FALSE)
	    return;
	  column = nchars;
	  WX = string_WX(string_result, status);
	} else {
	  c = cut_line();
	  new_status = COURIER;
	  continue;
	}
      }
      /* not printf(output_string) because the % would be catched */
      printf("%s", output_string); 
      break;
#endif
    print:
    default:
      *output_string = '\0';
      nchars = format_char(c, status);
      WX += char_WX(c, status);
      column += nchars;
      if (prefix_WX + WX > WXperline) {
	if (folding) {
	  if (fold_line(name, status) == FALSE)
	    return;
	  column = nchars;
	  WX = char_WX(c, status);
	} else {
	  c = cut_line();
	  new_status = COURIER;
	  continue;
	}
      }
      printf("%s", output_string);
      chars++;
      break;
    }
    /* Read next char */
    c = mygetc(&new_status, mode);
  }
  
  if (!start_line)
    printf(") s\n");
  if (!start_page)
    end_page();
}
