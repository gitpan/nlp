/************************************************************************/
/*				buffer.c				*/
/*									*/
/* routines of input, and formatting according to the styles		*/
/************************************************************************/

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifndef STDC_HEADERS
#  include "ansi.h"
#endif

#include "a2ps.h"
#include "buffer.h"
#include "styles.h"

/* result of mygetc when not char */
unsigned char string_result[BUFFER_SIZE+1]; 

/****************************************************************/
/*		pretty printing service routines	       	*/
/****************************************************************/
/*
 * return TRUE if `character' belongs to the alphabet of first chars
 */
int is_a_first_character (unsigned char character, STYLE style)
{
  const unsigned char* list = languages[style].first_characters;
  
  if (isalnum (character))
    return TRUE;
  
  while (*list != '\0') {
      if (*list <= character && character <= *(list+1))
	return TRUE;
      list += 2;
    }
  
  return FALSE;
}

/*
 * return TRUE if `character' belongs to the alphabet of following
 * chars
 */
int is_a_following_character (unsigned char character, STYLE style)
{
  const unsigned char* list = languages[style].following_characters;

  if (isalnum (character))
    return TRUE;
  
  while (*list != '\0') {
      if (*list <= character && character <= *(list+1))
	return TRUE;
      list += 2;
    }
  
  return FALSE;
}

/*
 * return TRUE if string begins with a sub-string in array
 */
int is_in_pattern_array (unsigned char* string, const unsigned char ***array,
			int step, STYLE style)
{
  while (***array != '\0') {
    if (!strncmp (**array, string, strlen (**array)))
      return TRUE;
    *array += step;
  }
  return FALSE;
}

/*
 * return TRUE if string is in array
 */
int is_in_word_array (unsigned char* string, const unsigned char ***array,
		      int step, STYLE style)
{
  while (***array != '\0') {
    /* strings must be equal and string must be followed by a char
     * not bellonging to the alphabet */
    if (!strncmp (**array, string, strlen(**array)) 
	&& !(is_a_following_character(string[strlen(**array)], style)))
      return TRUE;
    *array += step;
  }
  
  return FALSE;
}

/*
 * puts in string_result the longuest word in buffer starting at curr
 */
void read_word (unsigned char* buffer, int* curr, FONT* font, STYLE style)
{
  unsigned char first_word [BUFFER_SIZE+1];
  int i=0;
  char* string = buffer+*curr;

  first_word[i++] = *string++;
  while (is_a_following_character(*string, style))
    first_word[i++] = *string++;
  
  first_word[i] = '\0';
  *curr += i;
  strcpy(string_result, first_word);
}

/*
 * puts in string_result a keyword in buffer starting at curr, returns TRUE
 *	returns FALSE if not a keyword
 */
int is_keyword (unsigned char* buffer, unsigned char* lower_case_buffer,
		int* curr, FONT* font, STYLE style)
{
  const unsigned char** list = languages[style].keywords;
  unsigned char* string;
  
  string=(languages[style].sensitiveness == CASE_SENSITIVE ?
	  buffer : lower_case_buffer) + *curr;
  
  if (is_in_word_array(string, &list, KEYWORDS_STEP, style)) {
    strncpy(string_result, buffer+*curr, strlen(*list));
    string_result[strlen(*list)]='\0';
    *curr += strlen(*list);
    *font = (FONT) *(list+1);
    return TRUE;
  }
  
  return FALSE;
}
	    
/* return TRUE if string begins with a symbol
 * the converted symbol is in string_result
 */
int is_symbol (unsigned char* buffer, unsigned char* lower_case_buffer, 
	       int* curr, FONT* font, STYLE style)
{
  const unsigned char** list = languages[style].symbols;
  unsigned char* string;
  
  string=(languages[style].sensitiveness == CASE_SENSITIVE ?
	  buffer : lower_case_buffer) + *curr;

  if (is_in_word_array(string, &list, SYMBOLS_STEP, style)) {
    strcpy(string_result, *(list+1));
    *curr += strlen(*list);
    *font = (FONT) *(list+2);
    return TRUE;
  }
  
  return FALSE;
}

/* return TRUE if string begins with a special symbol
 * list : types of comment for the current style
 */
int is_special (unsigned char* buffer, int* curr, FONT* font, STYLE style)
{
  const unsigned char** list = languages[style].specials;

  if (is_in_pattern_array(buffer+*curr, &list, SPECIALS_STEP, style)) {
    strcpy(string_result, *(list+1));
    *curr += strlen(*list);
    *font = (FONT) *(list+2);
    return TRUE;
  }
  return FALSE;
}

/*
 * return TRUE if string begins with an "verbatim" sequence
 */
int is_verbatim (unsigned char* buffer, int* curr, STYLE style)
{
  /* list : "verbatim" sequences, should be written straightly when
   * outside a sequence (e.g. in ada ''' must be written such as) */
  const unsigned char** list = languages[style].verbatims;

  if (is_in_pattern_array(buffer+*curr, &list, VERBATIMS_STEP, style)) {
    strcpy(string_result, *(list));
    *curr += strlen(*list);
    return TRUE;
  }
  return FALSE;
}

/*
 * return TRUE if string begins with an "escape" sequence
 */
int is_escape (unsigned char* buffer, int* curr, STYLE style)
{
  /* list : "escape" sequences, should be written straightly when in a
     sequence (e.g. \" in a C string)  */
  const unsigned char** list = languages[style].escapes;

  if (is_in_pattern_array(buffer+*curr, &list, ESCAPES_STEP, style)) {
    strcpy(string_result, *(list));
    *curr += strlen(*list);
    return TRUE;
  }
  return FALSE;
}

/* 
 * return TRUE if buffer+*curr begins with a sequence
 */
int is_sequence_begin(unsigned char* buffer, unsigned char* lower_case_buffer,
		      int* curr, 
		      FONT* begin_font, FONT* font, FONT* end_font, 
		      unsigned char* end_sequence_pattern, STYLE style)
{
  const unsigned char** list = languages[style].sequences;
  unsigned char* string;
  
  string=(languages[style].sensitiveness == CASE_SENSITIVE ?
	  buffer : lower_case_buffer) + *curr;

  if (is_in_pattern_array(string, &list, SEQUENCES_STEP, style)) {
    /* printed opening string 	*/
    strncpy(string_result, buffer+*curr, strlen(*list));
    string_result[strlen(*list)]='\0';
    *curr += strlen(*list);		/* opening string in the source */
    *begin_font = (FONT) *(list+1);	/* font for the opening string 	*/
    *font = (FONT) *(list+2);		/* font for the inside 		*/
    strcpy(end_sequence_pattern, *(list+3)); /* source closing string	*/
    *end_font = (FONT) *(list+4);	/* font for the closing string	*/
    return TRUE;
  }
  return FALSE;
}

/* 
 * return TRUE if buffer+*curr begins with with end sequence pattern
 */
int is_sequence_end (unsigned char* buffer, unsigned char* lower_case_buffer,
		     int* curr, 
		     FONT* font, FONT* end_font,
		     unsigned char* end_sequence_pattern, STYLE style)
{
  unsigned char* string;
  
  string=(languages[style].sensitiveness == CASE_SENSITIVE ?
	  buffer : lower_case_buffer) + *curr;

  if (!strncmp(string, end_sequence_pattern, strlen (end_sequence_pattern))) {
    /* printed closing string 	*/
    strncpy(string_result, buffer+*curr, strlen(end_sequence_pattern));
    string_result[strlen(end_sequence_pattern) ] = '\0';
    *curr += strlen(end_sequence_pattern);
    *font = *end_font;
    return TRUE;
  }
  return FALSE;
}



/****************************************************************/
/*		 	Buffer Service routines	       		*/
/****************************************************************/
/* mygetbuf
 * reads input with "sliding" buffering:
 * - to ensure that keywords are correctly recognized, the
 *   buffer is completed to its complete size whenever
 *   there are too few chars left to read (less than BUFFER_OVERLAY)
 */
int mygetbuf(unsigned char buffer[],
	     int *curr)
{
  static int size=0;
  
  if (size == 0)	/* First buffer of a new file */
    size = fread((char *)buffer, sizeof(unsigned char), BUFFER_SIZE, stdin);
  else {
    /* The buffer was full, and there are less than BUFFER_OVERLAY
       chars to read. Fill up the buffer to BUFFER_SIZE */
    int copy;
    for (copy = 0 ; *curr + copy < size ; copy ++)
      buffer[copy] = buffer[*curr + copy];
    size = copy + fread((char *)buffer + copy, sizeof(unsigned char), 
		 BUFFER_SIZE - copy, stdin);
  }
  
  /* If the buffer is not full, there might remain char from the
   * previous buffer */
  if (size < BUFFER_SIZE)
    buffer[size]='\0';

  *curr = 0;
  return size;
}

/* - returns a char if no keyword are recocognized, or EOF, or
 *   STRING if returns a string. Then the result is in the global
 * string_result */
int mygetstring (FONT *statusp, STYLE style)
{
  static unsigned char buffer[BUFFER_SIZE+1];
  static unsigned char lower_case_buffer[BUFFER_SIZE+1];
  static int curr = 0;
  static int size = 0;
  /* are we currently in a sequence ? */
  static int in_sequence = FALSE; 
  static int return_to_courier = FALSE;
  /* font in a sequence, and closing symbol's font */
  static FONT in_font, end_font;

  /* when will the sequence end ? */
  /* I must get rid of that. */
  static unsigned char end_sequence_pattern[50]; 
  int c;
  
  if (curr >= size 
      || ((size == BUFFER_SIZE) && (BUFFER_SIZE - curr <= BUFFER_OVERLAY))) {
    size = mygetbuf(buffer, &curr);
    if (size == 0) {
      /* end of file: reset values */
      /* If this is a new file, it must not depend on the trailling
       * parameters of the previous file */
      in_sequence = FALSE;
      return_to_courier = FALSE;
      return EOF;
    }
    /* if necessary, take a lower case buffer */
    if (style != PLAIN 
	&& languages[style].sensitiveness == CASE_INSENSITIVE) {
      int i;
      for (i=0 ; i <= size ; i++)
	lower_case_buffer[i] = tolower(buffer[i]);
    }
  }

  if (style != PLAIN) {
    if (return_to_courier) {
      return_to_courier = FALSE;
      *statusp = COURIER;
    }
    if (in_sequence) {
      *statusp = in_font;
      /* verbatim: not converted _anywhere_ */
      if (is_verbatim (buffer, &curr, style))
	return STRING;
      /* escape: not converted when in a sequence */
      if (is_escape (buffer, &curr, style))
	return STRING;
      /* end of sequence ? */
      if (is_sequence_end (buffer, lower_case_buffer, &curr, 
			   statusp, &end_font, end_sequence_pattern, style))
	{
	  in_sequence = FALSE;
	  return_to_courier = TRUE;
	  return STRING;
	}
    } else { /* (not in sequence) */
      /* whether we are in a string or not, verbatims are copied */
      if (is_verbatim (buffer, &curr, style))
	return STRING;
      if (is_sequence_begin (buffer, lower_case_buffer, &curr, 
			     statusp, &in_font, &end_font,
			     end_sequence_pattern, style))
	{
	  in_sequence = TRUE;
	  return STRING;
	}
      else if (is_a_first_character (buffer[curr], style)) {  
	/* we are in a word since this was a char belonging to the
	 * first alphabet */
	if (translate_symbols
	    && is_symbol (buffer, lower_case_buffer, &curr, statusp, style))
	  return STRING;
	else if (is_keyword (buffer, lower_case_buffer, &curr, statusp, style))
	  return STRING;
	else if (translate_symbols 
		 && is_special (buffer, &curr, statusp, style)) {
	  return_to_courier = TRUE;
	  return STRING;
	} else {
	  /* since some characters may be used inside an identifier
	   * (eg, x' = x in claire) but can also be used to open
	   * a special sequence (eg, 'x' in claire), then we must read
	   * the whole word, and print in.
	   */
	  read_word (buffer, &curr, statusp, style);
	  return STRING;
	}
      } else if (translate_symbols 
		 && is_special (buffer, &curr, statusp, style)) {
	return_to_courier = TRUE;
	return STRING;
      } else
	*statusp = COURIER;
    }

    return buffer[curr++];
  } else {
    /* This is not a known mode */
    *statusp = COURIER;
    c = buffer[curr++];
    
    /* Check if it is a bold sequence (man pages style) */
    if (buffer[curr] == '\b') {
      if (c		== buffer[curr+1] &&
	  buffer[curr]	== buffer[curr+2] &&
	  c		== buffer[curr+3] &&
	  buffer[curr]	== buffer[curr+4] &&
	  c		== buffer[curr+5])
	{
	  *statusp = BOLD;
	  curr += 6;
	}
      /* Return the first character of the sequence */
    }
    return c;
  }
}

/*
 * read a new char of input. 
 * If font is INVISIBLE, discard it
 */
int mygetc(FONT *statusp, STYLE style)
{
  static int curr=0; 	/* curr in the string returned by mygetstring */
  static int size=0;	/* size of that string */
  int c=0;

  for (;;) {
    if (size != 0) {
      c = string_result[curr++];
      if (curr == size) 
	size = curr = 0;
      return c;
    } else {
      do
	c = mygetstring(statusp, style);
      while /* if the font is INVISIBLE, forget everything */
	((*statusp == INVISIBLE) && (c != EOF));
      if (c != STRING)
	return c;
      else
	size = strlen(string_result);
    }
  }
}

/*
 * Cut a textline too long to the size of a page line.
 */
int cut_line(void)
{
  int c;
  FONT status;

  while ((c = mygetc(&status, PLAIN)) != EOF 
	 && c != '\n' && c != '\f');
  return c;
}
