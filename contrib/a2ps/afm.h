/************************************************************************
 *			Definition of the postscript fonts used		*
 ************************************************************************/


#define Courier_WX	600	/* Any char in courier has WX = 600 */
#define UKN		400 	/* size when size is unknown */
/* Compute the WX of a char, given its font */
#define CHAR_WX(c,f)							\
 (((f == COURIER) || (f == ITALIC) || (f == BOLD) || (f == EMPH)) ? 	\
  Courier_WX :								\
  (f == TIMES) ? Times_WX[(int) c] : /* f == Symbol */ Symbol_WX[(int) c])


extern unsigned long Times_WX [];
extern unsigned long Symbol_WX [];

/*
 * Prototypes
 */
unsigned long char_WX(unsigned char c, FONT font);
unsigned long string_WX(unsigned char *string, FONT font);
