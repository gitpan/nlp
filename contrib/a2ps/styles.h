/* styles.h */

typedef enum { CASE_SENSITIVE, CASE_INSENSITIVE } CASE_SENSITIVENESS; 

struct struct_for_highlight {
  /* name of the language (compare with command-line option) */
  const char* name; 

  /* What are the abbreviationsadmited for the language (eg modula-3
     -> m3 */
  const char ** abbreviations;

  /* Note describing the mode or the language */
  const unsigned char * description;
  
  /* case sensitiveness for keywords and symbols */
  const CASE_SENSITIVENESS sensitiveness;

  /* definition of the "words" (keywords and symbols):
   * a char belonging to a first alphabet (first_characters),
   * and any number of chars belonging to the second (following_characters).
   * The definition of those alphabet is alphanumerical plus 
   * ascii intervals */

  /* First characters alphabet extension:
     begin_char_1, end_char_1,	ie ascii code between char 1 & char 2
     begin_char_2, end_char_2, ...
     \0 */
  const unsigned char *first_characters;

  /* the other than first letter may be : */
  const unsigned char *following_characters;

  /* list of keywords for this language
     keyword_1, font_1,
     keyword_2, font_2, ...*/
  const unsigned char ** keywords;

  /* keywords to be converted into symbols in another font
     keyword_1, symbol_1, font_1, 
     keyword_2, symbol_2, font_2, ... */
  const unsigned char ** symbols;

  /* same as symbols BUT there is no need to be preceded and followed
     by not in extended alphabet characters. In other words, these are
     not "words" of the alphabet, but any sequence of chars */
  const unsigned char ** specials;

  /* type of sequences (strings, comments, etc.) :
     begin_string_1, font_for_begin_string_1, font_for_inside_sequence_1, 
     	end_string_1, font_for_end_string_1, 
     begin_string_2, font_for_begin_string_2, font_for_inside_sequence_2, 
     	end_string_2, font_for_end_string_2, ...
     \0 */
  const unsigned char ** sequences;

  /* type of verbatims: strings that are written "verbatim" */
  const unsigned char ** verbatims;

  /* type of escapes: strings that are written "verbatim" when in a
     sequence */
  const unsigned char ** escapes;
};

/*	Notes
 *
 * - verbatims is made for languages such as tcl where, whereever the
 * symbol appears, it must not be "understood", e.g. \" and \\ are
 * written directly.
 *
 * - escapes is made for аз§и! languages such as ada where "" is the
 * empty string, but appearing in a string, "" denotes ".
 */

#define KEYWORDS_STEP	2
#define SYMBOLS_STEP	3
#define SPECIALS_STEP	3
#define SEQUENCES_STEP	5
#define VERBATIMS_STEP	1
#define ESCAPES_STEP	1


#define FONT_INVISIBLE 	(unsigned char*) INVISIBLE
#define FONT_COURIER 	(unsigned char*) COURIER
#define FONT_BOLD 	(unsigned char*) BOLD
#define FONT_ITALIC 	(unsigned char*) ITALIC
#define FONT_EMPH 	(unsigned char*) EMPH
#define FONT_TIMES 	(unsigned char*) TIMES
#define FONT_SYMBOL 	(unsigned char*) SYMBOL


extern struct struct_for_highlight languages[];
extern int NBR_LANGUAGES;
