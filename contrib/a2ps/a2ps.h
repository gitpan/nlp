/*
 *		a2ps.h
 * shared header with the whole package
 */

/************************************************************************/
/*									*/
/*	     P r e p r o c e s s i n g   d e f i n i t i o n s		*/
/*									*/
/************************************************************************/
/*
 * Common definitions
 */
#define	FALSE		0
#define	TRUE		1
#ifndef NULL
#  define NULL		0
#endif
#define UNULL		(unsigned char *) "\0"
#ifndef NUL
#  define NUL		'\0'
#endif
#ifndef EXIT_SUCCESS
#  define EXIT_SUCCESS	0
#endif
#ifndef EXIT_FAILURE
#  define EXIT_FAILURE	1
#endif
#define EXIT_BADARG	2
#define EXIT_UKNLANG	3


/*
 * Configuration values
 */
#define	PORTRAIT_HEADER		0.29
#define	LANDSCAPE_HEADER	0.22
#define	PIXELS_INCH		72
#define MAXFILENAME		32
#define MAX_LINES               160	/* max. lines per page */
#define MAN_LINES               66	/* no lines for a man */
#define PLAIN 			-1	/* Mode for no language */
#define STRING			-123	/* When my-getc returns a string */
#define BUFFER_SIZE		1024	/* size of the buffer */

/*
 * Global types
 */
typedef unsigned char * ustring;

/* fonts used in language modes */
typedef enum { 
  INVISIBLE, COURIER, BOLD, ITALIC, EMPH, TIMES, SYMBOL
} FONT;
/* languages style sheets are numbered */
typedef int STYLE;                  

/* font weights */
typedef enum { 
  BLACK, NORMAL 
} WEIGHT;		


/* convertion from a font to the postscript function */
#define PS_FONT(X) X == COURIER ? "st" 	: X == BOLD ? "b" : 		\
		   X == ITALIC ? "i"	: X == EMPH ? "bi" :		\
		   X == SYMBOL ? "sy" : "t"
/* return TRUE if the font is fix sized */
#define IS_FIXED_SIZE(font) ((font == COURIER) 				\
			    || (font == BOLD) 				\
			    || (font == ITALIC) 			\
			    || (font == EMPH))

/* return the max of to >-comparable stuff */
#define MAX(X,Y)	(((X) > (Y)) ? (X) : (Y))

/*
 * Flags related to options.
 */
extern int numbering;		/* Line numbering option 			*/
extern int folding;		/* Line folding option 				*/
extern int restart;		/* Don't restart page number after each file	*/
extern int only_printable;	/* Replace non printable char by space		*/
extern int interpret;           /* Interpret TAB, FF and BS chars option 	*/
extern int print_binaries;	/* Force printing for binary files 		*/ 
extern int landscape;		/* Otherwise portrait format sheets 		*/
extern int new_landscape;	/* To scrute changes of landscape option 	*/
extern int twinpages;           /* 2 pages per sheet if true, 1 otherwise 	*/
extern int new_twinpages;       /* To scrute changes of twinpages option 	*/
extern int twinfiles;		/* Allow 2 files per sheet 			*/
extern int no_header;		/* TRUE if user doesn't want the header 	*/
extern int no_border;		/* Don't print the surrounding border ? 	*/
extern int printdate;		/* Print current date as footnote 		*/
extern int footer;		/* Print file name at bottom of page 		*/
extern char *footer_text;	/* Content of the footer			*/
extern STYLE mode;		/* Highlight keywords ? 			*/
extern int translate_symbols;	/* Convert some keywords to symbols 		*/
extern int no_summary;		/* Quiet mode? 					*/
extern WEIGHT fontweight;	/* Control font weight 				*/
extern WEIGHT new_fontweight;	/* To scrute changes of bold option 		*/
extern int login_id;		/* Print login ID at top of page 		*/
#ifdef LPR_PRINT                   /* Fork a lpr process to do the printing 	*/
extern int lpr_print; 		/* Print by default ? 				*/
#  ifdef RECTO_VERSO_PRINTING
extern int rectoverso; 		/* Two-side printing 				*/
#  endif
#endif
extern int ISOlatin1;		/* Print 8-bit characters?			*/


/*
 * Counters of different kinds
 */
extern int copies_number;	/* Number of copies to print 			*/
extern int pages;			/* Number of logical pages printed 	*/
extern int sheets;			/* Number of physical pages printed 	*/
extern int old_pages, old_sheets;	/* Value before printing current file	*/
extern int sheetside;		/* Side of the sheet currently printing 	*/

/*
 * Sheet dimensions
 */
extern double paper_height; 	/* Paper dimensions */
extern double paper_width;
extern double page_height;	/* Page dimensions */
extern double page_width;

/*
 * Other global variables.
 */
extern char *prologue;		/* postscript header file 			*/
#ifdef LPR_PRINT
extern char *lpr_opt;		/* Options to lpr 				*/
#endif
extern float new_fontsize;	/* To scrute new values for fontsize 		*/
extern char *header_text;	/* Allow for different header text 		*/
extern int column_width;	/* Default column tab width (8) 		*/
extern int new_linesrequest;	/* To scrute new values for lines_requested 	*/
extern char *command;		/* name under which a2ps has been called 	*/




