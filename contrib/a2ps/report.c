/************************************************************************/
/* report.c								*/
/* here is everything for building the report on the state of a2ps,	*/
/* with respect to the styles						*/
/************************************************************************/

#include "config.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#ifndef STDC_HEADERS
#  include "ansi.h"
#endif

#include "a2ps.h"
#include "styles.h"
#include "report.h"
#include "routines.h"

#define prescript_font(X)						\
(X) == FONT_BOLD ? "\\textbf" : (X) == FONT_ITALIC ? "\\textit" : 	\
(X) == FONT_EMPH ? "\\textbi" : (X) == FONT_TIMES ? "\\textrm" :	\
(X) == FONT_SYMBOL ? "\\textsy" : "\\texttt"

/************************************************************************/
/*			check consistence of styles 			*/
/************************************************************************/
/*
 * Check that keywords and symbols are in lower case when lang is
 * case insensitive
 */
int check_sensitivity (STYLE lang)
{
  int OK = TRUE;
  
  switch (languages[lang].sensitiveness) {
  case CASE_SENSITIVE:
    break;
  case CASE_INSENSITIVE: 
    {
      const unsigned char **keywords=languages[lang].keywords;
      const unsigned char **symbols=languages[lang].symbols;
      const unsigned char **sequences=languages[lang].sequences;
      
      if (**keywords != NUL)
	while (**keywords != NUL) {
	  if (!is_strlower(*keywords)) {
	    fprintf(stderr, 
		    "In the style %s, the keyword '%s' uses upper chars.\n",
		    languages[lang].name, *keywords);
	    OK = FALSE;
	  }
	  keywords += KEYWORDS_STEP;
	}
      
      if (**symbols != NUL)
	while (**symbols != NUL) {
	  if (!is_strlower(*symbols)) {
	    fprintf(stderr, 
		    "In the style %s, the symbol '%s' uses upper chars.\n",
		    languages[lang].name, *symbols);
	    OK = FALSE;
	  }
	  symbols += SYMBOLS_STEP;
	}

      if (**sequences != NUL)
	while (**sequences != NUL) {
	  if (!is_strlower(*sequences) || !is_strlower(*(sequences+3))) {
	    fprintf(stderr, 
		    "In the style %s, the sequence '%s-%s' uses upper chars.\n",
		    languages[lang].name, *sequences, *(sequences+3));
	    OK = FALSE;
	  }
	  sequences += SEQUENCES_STEP;
	}
    }
  }
  return OK;
}

/*
 * Checks everything, return TRUE iff everything's alright
 */
int check(void)
{
  int OK = TRUE;
  STYLE lang;
 
  for (lang=0 ; lang < NBR_LANGUAGES ; lang++)
    OK = OK && check_sensitivity(lang);

  return OK;
}

/************************************************************************/
/* 				report styles				*/
/************************************************************************/
/*
 *      This is a correspondance between LaTeX chars and font symbol
 *		(extracted by ./getwx)
 */
unsigned char * Symbol_to_LaTeX [256] = {
/* .notdef (/0) */      	UNULL,
/* .notdef (/1) */      	UNULL,
/* .notdef (/2) */      	UNULL,
/* .notdef (/3) */      	UNULL,
/* .notdef (/4) */      	UNULL,
/* .notdef (/5) */      	UNULL,
/* .notdef (/6) */      	UNULL,
/* .notdef (/7) */      	UNULL,
/* .notdef (/8) */      	UNULL,
/* .notdef (/9) */      	UNULL,
/* .notdef (/10) */     	UNULL,
/* .notdef (/11) */     	UNULL,
/* .notdef (/12) */     	UNULL,
/* .notdef (/13) */     	UNULL,
/* .notdef (/14) */     	UNULL,
/* .notdef (/15) */     	UNULL,
/* .notdef (/16) */     	UNULL,
/* .notdef (/17) */     	UNULL,
/* .notdef (/18) */     	UNULL,
/* .notdef (/19) */     	UNULL,
/* .notdef (/20) */     	UNULL,
/* .notdef (/21) */     	UNULL,
/* .notdef (/22) */     	UNULL,
/* .notdef (/23) */     	UNULL,
/* .notdef (/24) */     	UNULL,
/* .notdef (/25) */     	UNULL,
/* .notdef (/26) */     	UNULL,
/* .notdef (/27) */     	UNULL,
/* .notdef (/28) */     	UNULL,
/* .notdef (/29) */     	UNULL,
/* .notdef (/30) */     	UNULL,
/* .notdef (/31) */     	UNULL,
/* space (/32) */       	(ustring) " ",
/* exclam (/33) */      	(ustring) "!",
/* universal (/34) */   	(ustring) "$\\forall$",
/* numbersign (/35) */  	(ustring) "\\#",
/* existential (/36) */ 	(ustring) "$\\exists$",
/* percent (/37) */     	(ustring) "\\%",
/* ampersand (/38) */   	(ustring) "\\&",
/* suchthat (/39) */    	(ustring) "$\\suchthat$",
/* parenleft (/40) */   	(ustring) "(",
/* parenright (/41) */  	(ustring) ")",
/* asteriskmath (/42) */	(ustring) "*",
/* plus (/43) */        	(ustring) "+",
/* comma (/44) */       	(ustring) ",",
/* minus (/45) */       	(ustring) "-",
/* period (/46) */      	(ustring) ".",
/* slash (/47) */       	(ustring) "/",
/* zero (/48) */        	(ustring) "0",
/* one (/49) */ 		(ustring) "1",
/* two (/50) */ 		(ustring) "2",
/* three (/51) */       	(ustring) "3",
/* four (/52) */        	(ustring) "4",
/* five (/53) */        	(ustring) "5",
/* six (/54) */ 		(ustring) "6",
/* seven (/55) */       	(ustring) "7",
/* eight (/56) */       	(ustring) "8",
/* nine (/57) */        	(ustring) "9",
/* colon (/58) */       	(ustring) ":",
/* semicolon (/59) */   	(ustring) ";",
/* less (/60) */        	(ustring) "<",
/* equal (/61) */       	(ustring) "=",
/* greater (/62) */     	(ustring) ">",
/* question (/63) */    	(ustring) "?",
/* congruent (/64) */   	(ustring) "$\\cong$",
/* Alpha (/65) */       	(ustring) "$\\Alpha$",
/* Beta (/66) */        	(ustring) "$\\Beta$",
/* Chi (/67) */ 		(ustring) "$\\Chi$",
/* Delta (/68) */       	(ustring) "$\\Delta$",
/* Epsilon (/69) */     	(ustring) "$\\Epsilon$",
/* Phi (/70) */ 		(ustring) "$\\Phi$",
/* Gamma (/71) */       	(ustring) "$\\Gamma$",
/* Eta (/72) */ 		(ustring) "$\\Eta$",
/* Iota (/73) */        	(ustring) "$\\Iota$",
/* theta1 (/74) */      	(ustring) "$\\vartheta$",
/* Kappa (/75) */       	(ustring) "$\\Kappa$",
/* Lambda (/76) */      	(ustring) "$\\Lambda$",
/* Mu (/77) */  		(ustring) "$\\Mu$",
/* Nu (/78) */  		(ustring) "$\\Nu$",
/* Omicron (/79) */     	(ustring) "$\\Omicron$",
/* Pi (/80) */  		(ustring) "$\\Pi$",
/* Theta (/81) */       	(ustring) "$\\Theta$",
/* Rho (/82) */ 		(ustring) "$\\Rho$",
/* Sigma (/83) */       	(ustring) "$\\Sigma$",
/* Tau (/84) */ 		(ustring) "$\\Tau$",
/* Upsilon (/85) */     	(ustring) "$\\Upsilon$",
/* sigma1 (/86) */      	(ustring) "$\\varsigma$",
/* Omega (/87) */       	(ustring) "$\\Omega$",
/* Xi (/88) */ 		 	(ustring) "$\\Xi$",
/* Psi (/89) */ 		(ustring) "$\\Psi$",
/* Zeta (/90) */        	(ustring) "$\\Zeta$",
/* bracketleft (/91) */ 	(ustring) "[",
/* therefore (/92) */   	(ustring) "$\\therefore$",
/* bracketright (/93) */       	(ustring) "]",
/* perpendicular (/94) */      	(ustring) "$\\perp$",
/* underscore (/95) */  	(ustring) "\\_",
/* radicalex (/96) */   	(ustring) "$\\radicalex$",
/* alpha (/97) */       	(ustring) "$\\alpha$",
/* beta (/98) */        	(ustring) "$\\beta$",
/* chi (/99) */ 		(ustring) "$\\chi$",
/* delta (/100) */      	(ustring) "$\\delta$",
/* epsilon (/101) */    	(ustring) "$\\epsilon$",
/* phi (/102) */        	(ustring) "$\\phi$",
/* gamma (/103) */      	(ustring) "$\\gamma$",
/* eta (/104) */        	(ustring) "$\\eta$",
/* iota (/105) */       	(ustring) "$\\iota$",
/* phi1 (/106) */       	(ustring) "$\\phi$",
/* kappa (/107) */      	(ustring) "$\\kappa$",
/* lambda (/108) */     	(ustring) "$\\lambda$",
/* mu (/109) */ 		(ustring) "$\\mu$",
/* nu (/110) */ 		(ustring) "$\\nu$",
/* omicron (/111) */    	(ustring) "$\\omicron$",
/* pi (/112) */ 		(ustring) "$\\pi$",
/* theta (/113) */      	(ustring) "$\\theta$",
/* rho (/114) */        	(ustring) "$\\rho$",
/* sigma (/115) */      	(ustring) "$\\sigma$",
/* tau (/116) */        	(ustring) "$\\tau$",
/* upsilon (/117) */    	(ustring) "$\\upsilon$",
/* omega1 (/118) */     	(ustring) "$\\varpi$",
/* omega (/119) */      	(ustring) "$\\omega$",
/* xi (/120) */ 		(ustring) "$\\xi$",
/* psi (/121) */        	(ustring) "$\\psi$",
/* zeta (/122) */       	(ustring) "$\\zeta$",
/* braceleft (/123) */  	(ustring) "\\{",
/* bar (/124) */        	(ustring) "$\\|$",
/* braceright (/125) */ 	(ustring) "\\}",
/* similar (/126) */    	(ustring) "$\\sim$",
/* .notdef (/127) */    	UNULL,
/* .notdef (/128) */    	UNULL,
/* .notdef (/129) */    	UNULL,
/* .notdef (/130) */    	UNULL,
/* .notdef (/131) */    	UNULL,
/* .notdef (/132) */    	UNULL,
/* .notdef (/133) */    	UNULL,
/* .notdef (/134) */    	UNULL,
/* .notdef (/135) */    	UNULL,
/* .notdef (/136) */    	UNULL,
/* .notdef (/137) */    	UNULL,
/* .notdef (/138) */    	UNULL,
/* .notdef (/139) */    	UNULL,
/* .notdef (/140) */    	UNULL,
/* .notdef (/141) */    	UNULL,
/* .notdef (/142) */    	UNULL,
/* .notdef (/143) */    	UNULL,
/* .notdef (/144) */    	UNULL,
/* .notdef (/145) */    	UNULL,
/* .notdef (/146) */    	UNULL,
/* .notdef (/147) */    	UNULL,
/* .notdef (/148) */    	UNULL,
/* .notdef (/149) */    	UNULL,
/* .notdef (/150) */    	UNULL,
/* .notdef (/151) */    	UNULL,
/* .notdef (/152) */    	UNULL,
/* .notdef (/153) */    	UNULL,
/* .notdef (/154) */    	UNULL,
/* .notdef (/155) */    	UNULL,
/* .notdef (/156) */    	UNULL,
/* .notdef (/157) */    	UNULL,
/* .notdef (/158) */    	UNULL,
/* .notdef (/159) */    	UNULL,
/* .notdef (/160) */    	UNULL,
/* Upsilon1 (/161) */   	(ustring) "$\\varUpsilon$",/***/
/* minute (/162) */     	(ustring) "$\\prime$",
/* lessequal (/163) */  	(ustring) "$\\leq$",
/* fraction (/164) */   	(ustring) "/",
/* infinity (/165) */   	(ustring) "$\\infty$",
/* florin (/166) */     	(ustring) "$\\florin$",/****/
/* club (/167) */       	(ustring) "$\\clubsuit$",
/* diamond (/168) */    	(ustring) "$\\diamondsuit$",
/* heart (/169) */      	(ustring) "$\\heartsuit$",
/* spade (/170) */      	(ustring) "$\\spadesuit$",
/* arrowboth (/171) */  	(ustring) "$\\leftrightarrow$",
/* arrowleft (/172) */  	(ustring) "$\\leftarrow$",
/* arrowup (/173) */    	(ustring) "$\\uparrow$",
/* arrowright (/174) */ 	(ustring) "$\\rightarrow$",
/* arrowdown (/175) */  	(ustring) "$\\downarrow$",
/* degree (/176) */     	(ustring) "$\\circ$",/***/
/* plusminus (/177) */  	(ustring) "$\\pm$",
/* second (/178) */     	(ustring) "''",
/* greaterequal (/179) */       (ustring) "$\\geq$",
/* multiply (/180) */   	(ustring) "$\\times$",
/* proportional (/181) */       (ustring) "$\\propto$",
/* partialdiff (/182) */        (ustring) "$\\partial$",
/* bullet (/183) */    		(ustring) "$\\bullet$",
/* divide (/184) */     	(ustring) "$\\div$",
/* notequal (/185) */   	(ustring) "$\\neq$",
/* equivalence (/186) */        (ustring) "$\\equiv$",
/* approxequal (/187) */        (ustring) "$\\approx$",
/* ellipsis (/188) */   	(ustring) "$\\ldots$",
/* arrowvertex (/189) */        (ustring) "$\\|$",
/* arrowhorizex (/190) */       UNULL,
/* carriagereturn (/191) */     (ustring) "\\carriagereturn",
/* aleph (/192) */      	(ustring) "$\\aleph$",
/* Ifraktur (/193) */   	(ustring) "$\\Im$",
/* Rfraktur (/194) */   	(ustring) "$\\Re$",
/* weierstrass (/195) */        (ustring) "$\\wp$",
/* circlemultiply (/196) */     (ustring) "$\\otimes$",
/* circleplus (/197) */ 	(ustring) "$\\oplus$",
/* emptyset (/198) */ 		(ustring) "$\\emptyset$",
/* intersection (/199) */       (ustring) "$\\cap$",
/* union (/200) */     		(ustring) "$\\cup$",
/* propersuperset (/201) */     (ustring) "$\\supset$",
/* reflexsuperset (/202) */     (ustring) "$\\supseteq$",
/* notsubset (/203) */  	(ustring) "$\\not\\subset$",
/* propersubset (/204) */       (ustring) "$\\subset$",
/* reflexsubset (/205) */       (ustring) "$\\subseteq$",
/* element (/206) */    	(ustring) "$\\in$",
/* notelement (/207) */ 	(ustring) "$\\not\\in$",
/* angle (/208) */      	(ustring) "$\\angle$",
/* gradient (/209) */   	(ustring) "$\\nabla$",
/* registerserif (/210) */      (ustring) "$\\register$", /****/
/* copyrightserif (/211) */     (ustring) "\\copyright",/****/
/* trademarkserif (/212) */     (ustring) "$\\trademark$",/****/
/* product (/213) */    	(ustring) "$\\prod$",
/* radical (/214) */   	 	(ustring) "$\\surd$",
/* dotmath (/215) */    	(ustring) "$\\cdot$",
/* logicalnot (/216) */ 	(ustring) "$\\not$",
/* logicaland (/217) */ 	(ustring) "$\\wedge$",
/* logicalor (/218) */  	(ustring) "$\\vee$",
/* arrowdblboth (/219) */       (ustring) "$\\Leftrightarrow$",
/* arrowdblleft (/220) */       (ustring) "$\\Leftarrow$",
/* arrowdblup (/221) */ 	(ustring) "$\\Uparrow$",
/* arrowdblright (/222) */      (ustring) "$\\Rightarrow$",
/* arrowdbldown (/223) */       (ustring) "$\\Downarrow$",
/* lozenge (/224) */    	(ustring) "$\\diamondsuit$",	/****/
/* angleleft (/225) */  	(ustring) "$\\langle$",
/* registersans (/226) */       (ustring) "$\\register$",/***/
/* copyrightsans (/227) */      (ustring) "\\copyright",/***/
/* trademarksans (/228) */      (ustring) "$\\trademark$",/****/
/* summation (/229) */  	(ustring) "$\\sum$",
/* parenlefttp (/230) */        UNULL,
/* parenleftex (/231) */        UNULL,
/* parenleftbt (/232) */        UNULL,
/* bracketlefttp (/233) */      (ustring) "$\\lceil$",
/* bracketleftex (/234) */      UNULL,
/* bracketleftbt (/235) */      (ustring) "$\\lfloor$",
/* bracelefttp (/236) */        UNULL,
/* braceleftmid (/237) */       UNULL,
/* braceleftbt (/238) */        UNULL,
/* braceex (/239) */    	UNULL,
/* .notdef (/240) */    	(ustring) "\\apple",
/* angleright (/241) */ 	(ustring) "$\\rangle$",
/* integral (/242) */   	(ustring) "$\\int$",
/* integraltp (/243) */ 	UNULL,
/* integralex (/244) */		UNULL,
/* integralbt (/245) */		UNULL,
/* parenrighttp (/246) */       UNULL,
/* parenrightex (/247) */       UNULL,
/* parenrightbt (/248) */       UNULL,
/* bracketrighttp (/249) */     (ustring) "$\\rceil$",
/* bracketrightex (/250) */     UNULL,
/* bracketrightbt (/251) */     (ustring) "$\\rfloor$",
/* bracerighttp (/252) */       UNULL,
/* bracerightmid (/253) */      UNULL,
/* bracerightbt (/254) */       UNULL,
};

/*
 * sends to stdout the string quoted for LaTeX
 * math mode is never in a \text??{}, and special chars are backslashed
 */
void print_quoted_latex(const unsigned char *font, const unsigned char* string)
{
  int font_given=FALSE;

  switch ((int) font) {
  case INVISIBLE:
    return;
  case SYMBOL:
    for (/*skip */; *string; string++)
      if (*Symbol_to_LaTeX[*string])
	printf("%s", Symbol_to_LaTeX[*string]);
      else
	printf("%c", *string);
    break;
  default:
    for (/* skip */; *string; string++)
      switch (*string) {
      case '#':
      case '&':
      case '_':
      case '$':
      case '%':
	if (font_given) {
	  printf("}");
	  font_given=FALSE;
	}
	printf("\\%c", *string);
	break;
      case '\\':
	if (font_given) {
	  printf("}");
	  font_given=FALSE;
	}
	printf("$backslash$");
	break;
      case '\n':
	if (font_given) {
	  printf("}");
	  font_given=FALSE;
	}
	printf("$\\backslash$n");
	break;
      default:
	if (!font_given) {
	  printf("%s{", prescript_font(font));
	  font_given=TRUE;
	}
	printf("%c", *string);
      }
    if (font_given)
      printf("}");
  }
}

/*
 * printf a string in the \verb+ + command
 */
void print_verb_latex(const unsigned char *string)
{
  unsigned char* verb_delims=(unsigned char *)"!#+|";

  /* search for a delimiter (for \verb) which is not in the string */
  for (/* skip */; *verb_delims; verb_delims++)
    if (!ustrchr(string, *verb_delims))
      break;

  if (*verb_delims) {
    printf("\\verb%c", *verb_delims);
    for (/* skip*/ ; *string; string++)
      switch (*string) {
      case '\n':
	printf("\\n");
	break;
      default:
	printf("%c", *string);
      }
    printf("%c", *verb_delims);
  } else {
    fprintf(stderr, "I need another \\verb delimiter than (%s) to print %s\n", 
	    verb_delims, string);
    exit(EXIT_FAILURE);
  }
}

/*
 *
 */
void print_char_verb_latex(const unsigned char c)
{
  switch (c) {
  case '|':
    printf("\\verb+%c+", c);
    break;
  default:
    printf("\\verb|%c|", c);
    break;
  }
}

/*
 * Give the content of an alphabet
 */
void report_alphabet(const unsigned char *alphabet)
{
  unsigned char letter;

  while (*alphabet) {
    if (*alphabet == *(alphabet+1)) {
      printf("'");
      print_char_verb_latex(*alphabet);
      printf("'(%d)%s",
	     *alphabet,
	     *(alphabet+2) != '\0' ? ", " : ".\n");
    } else {
      printf("'");
      for (letter = *alphabet; letter <= *(alphabet+1); letter++)
	print_char_verb_latex(letter);
      printf("'(%d-%d)%s", *alphabet, *(alphabet+1),
	     *(alphabet+2) != '\0' ? ", " : ".\n");
    }
    alphabet += 2;
  }
}

/*
 * Give the alphabets of lang
 */
void report_alphabets(STYLE lang)
{
  /* are alphabets equal ? */
  if (!ustrcmp(languages[lang].first_characters, 
	      languages[lang].following_characters)) {
    printf("\\item \\textrm{first and second alphabet extensions are:}\n");
    report_alphabet(languages[lang].first_characters);
  } else {
    printf("\\item \\textrm{first alphabet extension is:}\n");
    report_alphabet(languages[lang].first_characters);
    printf("\\item \\textrm{second alphabet extension is:}\n");
    report_alphabet(languages[lang].following_characters);
  }
}
/*
 * Give the description note of lang
 */
void report_description(STYLE lang)
{
  const unsigned char * desc = languages[lang].description;

  if (*desc != NUL) {
    printf("\\item %s\n", desc);
    /*print_quoted_latex((unsigned char *)TIMES, desc);*/
  }
}

/*
 * Give the abbreviations admited of lang
 */
void report_abbreviations(STYLE lang)
{
  const char ** abbr=languages[lang].abbreviations;

  if (**abbr == NUL)
    printf("\\item \\textrm{there are no abbreviation.} \n");
  else {
    printf("\\item \\textrm{the abbreviation are:} ");
    for (abbr = languages[lang].abbreviations ; **abbr ; abbr++) {
      printf("%s%s", *abbr, **(abbr+1)==NUL ? ".\n" : ", ");
    }
  }
}

/*
 * Give the case sensitivity of lang
 */
void report_sensitivity(STYLE lang)
{
  printf("\\item \\textrm{keywords, symbols and sequences recognition "
	 "is case %s.}\n",
	 languages[lang].sensitiveness == CASE_SENSITIVE ?
	 "sensitive" : "insensitive");
}

/*
 * Give the keywords of lang
 */
void report_keywords(STYLE lang)
{
  const unsigned char **array=languages[lang].keywords;
  int count=0;

  if (**array == NUL)
    printf("\\item \\textrm{there are no keywords.}\n");
  else {
    printf("\\item \\textrm{the keywords are:}\n");
    while (**array != NUL) {
            print_quoted_latex(*(array+1),*array);
      printf("%s", **(array+2) == NUL ? ".\n" : (count % 4) ? ", " : ", %\n");
      array += KEYWORDS_STEP;
      count++;
    }
  }
}

/*
 * Give the symbols of lang
 */
void report_symbols(STYLE lang)
{
  const unsigned char **array=languages[lang].symbols;
  int count=0;

  if (**array == NUL)
    printf("\\item \\textrm{there are no regular symbols.}\n");
  else {
    printf("\\item \\textrm{the regular symbols are:}\n");
    while (**array != NUL) {
      print_verb_latex(*array);		/* the original word */
      printf(" $\\to$ ");		
      print_quoted_latex(*(array+2),*(array+1));/* the written string & font */
      printf("%s", **(array+3) == NUL ? ".\n" : (count % 2) ? ", " : ", %\n");
      array += SYMBOLS_STEP;
      count ++;
    }
  }
}

/*
 * Give the specials of lang
 */
void report_specials(STYLE lang)
{
  const unsigned char **array=languages[lang].specials;
  int count=0;

  if (**array == NUL)
    printf("\\item \\textrm{there are no special symbols.}\n");
  else {
    printf("\\item \\textrm{the special symbols are:}\n");
    while (**array != NUL) {
      print_verb_latex(*array);		/* the original word */
      printf(" $\\to$ ");		
      print_quoted_latex(*(array+2),*(array+1));/* the written font & string */
      printf("%s", **(array+SPECIALS_STEP) == NUL ? ".\n" : (count % 2) ? ", " : ", %\n");
      array += SPECIALS_STEP;
      count++;
    }
  }
}

/*
 * Give the sequences of lang
 */
void report_sequences(STYLE lang)
{
  const unsigned char **array=languages[lang].sequences;
  unsigned char string[256];

  if (**array == NUL)
    printf("\\item \\textrm{there are no sequences.}\n");
  else {
    printf("\\item \\textrm{the sequences are:}\n");
    while (**array != NUL) {
      sprintf((char *) string, "%sinside%s", *array, *(array+3));
      print_verb_latex(string);
      printf(" $\\to$ "); 
      print_quoted_latex(*(array+1), *array);/* written opening string */
      print_quoted_latex(*(array+2), (ustring) "inside");/* the inside font */
      print_quoted_latex(*(array+4),*(array+3));/* written closing string */
      printf("%s", **(array+SEQUENCES_STEP) != NUL ? ", %\n" : ".\n");
      array += SEQUENCES_STEP;
    }
  }
}

/*
 * Give the verbatims of lang
 */
void report_verbatims(STYLE lang)
{
  const unsigned char **array=languages[lang].verbatims;

  if (**array == NUL)
    printf("\\item \\textrm{there are no verbatims.}\n");
  else {
    printf("\\item \\textrm{the verbatims are:}\n");
    while (**array != NUL) {
      printf("\"");
      print_verb_latex(*array);
      printf("\"%s",
	     **(array+1) != NUL ? ", " : ".\n");
      array += VERBATIMS_STEP;
    }
  }
}

/*
 * Give the escapes of lang
 */
void report_escapes(STYLE lang)
{
  const unsigned char **array=languages[lang].escapes;

  if (**array == NUL)
    printf("\\item \\textrm{there are no escapes.}\n");
  else {
    printf("\\item \\textrm{the escapes are:}\n");
    while (**array != NUL) {
      printf("\"");
      print_verb_latex(*array);
      printf("\"%s",
	     **(array+1) != NUL ? ", " : ".\n");
      array += ESCAPES_STEP;
    }
  }
}

/*
 * Report about a language 
 */
void report_language(STYLE lang)
{
  printf("\\subsection{%s style}\n", languages[lang].name);
  printf("\\begin{itemize}\n");
  report_description(lang);
  report_abbreviations(lang);
  report_alphabets(lang);
  report_sensitivity(lang);
  report_keywords(lang);
  report_symbols(lang);
  report_specials(lang);
  report_sequences(lang);
  report_verbatims(lang);
  report_escapes(lang);
  printf("\\end{itemize}\n");
}

/*
 * Print the prescript banner
 */
void report_styles(void)
{
  printf("\\section{Style definitions}\n");
  printf("\\textrm{The reader should first note that a2ps is _not_ a");
  printf("powerful syntactical pretty-printer: it just handles lexical");
  printf("structures, i.e., if in your language IF IF == THEN THEN");
  printf("ELSE := IF ELSE ELSE := THEN, then a2ps is not the tool you");
  printf("need. It is for the same reason that you can't expect a2ps");
  printf("to highlight the function definitions in C.");
  printf("\n");
  printf("In the following is described how the style sheets are");
  printf("defined. You can skip this section if you don't care how");
  printf("a2ps does this, and if you don't expect to implement new");
  printf("styles.}");
  printf("\n\n");
  printf("\\subsection{Name and abbreviations}\n");
  printf("\\textrm{Every style has a unique name which is the one");
  printf("written in verbose mode. Any of the name or abbreviations");
  printf("can be used with option -k.  When automatic pretty-printing");
  printf("is enabled, first a2ps calls} file \\textrm{to see whether");
  printf("the language is recognizable. Otherwise, the suffix of the");
  printf("file is compared with every name and abbreviation. On");
  printf("failure, plain style is used.}");
  printf("\n\n");
  printf("\\subsection{Alphabets}\n");
  printf("\\textrm{a2ps needs to know the beginning and the end of a");
  printf("word, especially keywords. Alpha-numerical characters are");
  printf("always supposed to be in the alphabet. Hence it needs two");
  printf("alphabet extensions (lists of intervals): the first");
  printf("specifying by which letters an identifier can begin, and the");
  printf("second for the rest of the word.}");
  printf("\\subsection{Keywords, regular symbols}\n");
  printf("\\textrm{A keyword if recognized, is just written in a");
  printf("special font.  A symbol, when recognized, is replaced by the");
  printf("corresponding character in the symbol font. To be");
  printf("recognized, both need to start by a character in the first");
  printf("alphabet and to be immediately followed by a character which");
  printf("does not belong to the second alphabet.}");
  printf("\n\n");
  printf("\\subsection{Special symbols}\n");
  printf("\\textrm{They need not to be preceded and followed by");
  printf("characters belonging to the alphabet. For instance in caml,");
  printf("`not' is a regular symbol written} \\not\\textrm{; it is not");
  printf("the case of `nota' or `notnot'. But <> always means }");
  printf("\\neq\\textrm{, even if there are no spaces around.  \n Both");
  printf("regular and special symbols are transformed only if option");
  printf("-t was given.}");
  printf("\n\n");
  printf("\\subsection{Sequences}\n");
  printf("\\textrm{They start with one special sequence of characters");
  printf("and finish with another one, such as strings, comments");
  printf("etc. Three fonts are used: one for the initial marker, one");
  printf("for the core of the sequence, and a last one for the final");
  printf("maker.}");
  printf("\n\n");
  printf("\\subsection{Escapes and verbatims}\n");
  printf("\\textrm{Escapes are immediately copied when found in a");
  printf("sequence. Their main use is to avoid a sequence from being");
  printf("terminated too soon, e.g., the string} \\verb+\"\\\"\"+");
  printf("\\textrm{is legal in C, hence it is necessary to specify in");
  printf("a2ps that} \\verb+\\\\+ \\textrm{and} \\verb+\\\"+");
  printf("\\textrm{have to be written has such when in a sequence:");
  printf("they are escapes.");
  printf("\n");
  printf("Verbatims are immediately copied wherever they are met. It");
  printf("has been implemented for ada in which ''' is the character");
  printf("'. a2ps must not understand it as opening of a sequence");
  printf("(first quote), closing of the sequence (second quote),");
  printf("opening of a sequence (third quote). Nor is it possible to");
  printf("specify that ' is an escape...}");
  printf("\n\n");
  printf("\\subsection{Case sensitivity}\n");
  printf("\\textrm{If the style is case insensitive, the case does not");
  printf("matter for the keyword, symbol and sequence");
  printf("recognition. Other categories are not concerned by case");
  printf("sensitivity.}");
  printf("\n\n\n\n");
}

/*
 * Print the prescript banner
 */
void report_prologue(void)
{
  printf("%% This has been generated by %s version %s\n", command, VERSION);
  printf("%% It should be processed by either LaTeX or\n");
  printf("%% a2ps with options -kpre -t to be printable.\n");
  printf("%%\n\n");

  printf("%%%%prescript:skip\n");
  printf("%% a2ps skips everything between this line, and the one with `piks'\n");

  printf("\\documentclass{article}\n");

  printf("\n");
  printf("%%%% These are LaTeX commands for prescript primitives.\n");
  printf("\\newcommand{\\textbi}[1]{\\textbf{\\it #1}}\n");
  printf("\\newcommand{\\textsy}[1]{#1}\n");

  printf("\n");
  printf("%%%% These are made to be forgotten by LaTeX but seen by a2ps\n");
  printf("%%%% To get a better LaTeX file, remove any occurrence.\n");
  printf("\\newcommand{\\magicbf}[1]{}\n");
  printf("\\newcommand{\\magicit}[1]{}\n");
  printf("\\newcommand{\\magicbi}[1]{}\n");
  printf("\\newcommand{\\magicrm}[1]{}\n");
  printf("\\newcommand{\\magicsy}[1]{}\n");
  printf("\\newcommand{\\magictt}[1]{}\n");

  printf("\n");
  printf("%%%% These symbols are available in ps but not in LaTeX.\n");
  printf("\\newcommand{\\carriagereturn}{$\\backslash$n}\n");
  printf("\\newcommand{\\suchthat}{``suchthat''}\n");
  printf("\\newcommand{\\therefore}{``therefore''}\n");
  printf("\\newcommand{\\radicalex}{``radicalex''}\n");
  printf("\\newcommand{\\florin}{``florin''}\n");
  printf("\\newcommand{\\register}{``register''}\n");
  printf("\\newcommand{\\trademark}{ ^{TM}}\n");
  printf("\\newcommand{\\apple}{``apple''}\n");
  printf("\\newcommand{\\Alpha}{A}\n");
  printf("\\newcommand{\\Beta}{B}\n");
  printf("\\newcommand{\\Chi}{X}\n");
  printf("\\newcommand{\\Epsilon}{E}\n");
  printf("\\newcommand{\\Eta}{H}\n");
  printf("\\newcommand{\\Iota}{I}\n");
  printf("\\newcommand{\\Kappa}{K}\n");
  printf("\\newcommand{\\Mu}{M}\n");
  printf("\\newcommand{\\Nu}{N}\n");
  printf("\\newcommand{\\Omicron}{O}\n");
  printf("\\newcommand{\\Rho}{P}\n");
  printf("\\newcommand{\\Tau}{T}\n");
  printf("\\newcommand{\\Zeta}{Z}\n");
  printf("\\newcommand{\\varUpsilon}{\\Upsilon}\n");
  printf("\\newcommand{\\omicron}{o}\n");
  printf("%%%%prescript:piks\n");
}

/*
 * Report just about one language
 */
void report_one_language(STYLE lang)
{
  /* try to be as close as possible of LaTeX */
  report_prologue();
  printf("\\begin{document}\n");

  printf("\\textbi{This is the automatic report about %s-style of "
	 "%s, version %s}\n\n", languages[lang].name, command, VERSION);
  printf("\n\n\n");
  
  /* Language by language */
  report_language(lang);

  printf("\\end{document}\n");
  exit(EXIT_SUCCESS);
}

/*
 * The whole report about a2ps
 */
void report(void)
{
  STYLE lang;

  /* try to be as close as possible of LaTeX */
  report_prologue();
  printf("\\begin{document}\n");

  printf("\\textbi{This is the automatic report about styles of "
	 "%s, version %s}\n\n", command, VERSION);
  printf("\n\n\n");

  /* Explanation about the style definition */
  report_styles();

  /* Check the consistancy of styles */
  printf("\\section{Consistancy}\n");
  if (check())
    printf("\\textrm{There are no found problems in the styles.}\n");
  else
    printf("\\textrm{Problems were found in the file styles.c}\n");
  printf("\n\n\n");

  /* list of the known languages */
  printf("\\section{Known languages}\n");
  printf("\\begin{enumerate}\n");
  for (lang=0 ; lang < NBR_LANGUAGES ; lang++)
    printf("\\magictt{%d}\\item %s\n",
	   lang + 1, languages[lang].name);
  printf("\\end{enumerate}\n");
  printf("\n\n\n");

  /* Description of the language styles */
  printf("\\section{Description of the language styles}\n");
  /* Language by language */
  for (lang=0 ; lang < NBR_LANGUAGES ; lang++) {
    printf("\\magicbf{%d} ", lang + 1);
    report_language(lang);
    if (lang < NBR_LANGUAGES - 1)
      printf("\n\n\n");
  }
  printf("\\end{document}\n");

  exit(EXIT_SUCCESS);
}
