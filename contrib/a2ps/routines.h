/*
 *	routines.h
 */

#define ustrcat(x,y) 					\
	(strcat((char *)(x), (const char *)(y)))
#define ustrcpy(x,y) 					\
	(strcpy((char *)(x), (const char *)(y)))
#define ustrncpy(x,y,z) 				\
	(strncpy((char *)(x), (const char *)(y), (z)))
#define ustrcmp(x,y)					\
	(strcmp((const char *)(x), (const char *)(y)))
#define ustrncmp(x,y,z)					\
	(strncmp((const char *)(x), (const char *)(y), (z)))
#define ustrlen(x)					\
	(strlen((const char *)(x)))
#define ustrchr(x,y) 					\
	(strchr((char *)(x), (int)(y)))


void get_user(char **logname);
void get_host(char **host);
int is_strlower(const unsigned char * string);

