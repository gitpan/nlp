/************************************************************************/
/*				ansi.h					*/
/*									*/
/* 	supplies prototypes of the function used by a2ps, when ansi	*/
/*	headers are not available					*/
/************************************************************************/

/* stdio.h */
int printf(const char *format, /* args */ ... );
int fprintf(FILE *strm, const char *format, /* args */ ... );
int sscanf(const char *s, const char *format, /* args */ ...);
size_t fread(void *ptr, size_t size, size_t nitems, FILE *stream);
int _filbuf();
int _flsbuf();

/* ctype.h */
int tolower(int c);

/* sys/types.h & time.h */
time_t time(time_t *tloc);
