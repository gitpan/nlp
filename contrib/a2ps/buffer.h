/* buffer.h */

int cut_line(void);
int mygetc (FONT *statusp, STYLE mode);

/* size of the buffer "window" (should be bigger than longuest words
 * in a2ps.h) */
#define BUFFER_OVERLAY 64

