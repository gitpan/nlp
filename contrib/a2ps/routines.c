/*
 *	routines.c
 * general use routines
 */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>

#if HAVE_UNISTD_H
#  include <unistd.h>			/* for getlogin */
#endif

#if HAVE_UNAME
#  include <sys/utsname.h>
#endif

#if HAVE_UNAME || HAVE_GETHOSTNAME
#  define MAX_HOSTNAME		40
#endif

#include "a2ps.h"
#include "routines.h"


/*
 * Get user name and machine name
 */
void get_user(char **logname)
{
  char *user_name, *separator;
  struct passwd *user_info;
  /* Retrieve user's login name */
  *logname = getlogin();
  if (!*logname)
    *logname=getenv("LOGNAME");
  if (!*logname)
    *logname=getenv("USER");
  if (*logname) {
    user_info = getpwnam(*logname);
    user_name = user_info->pw_gecos;
    if ((separator = strchr(user_name, ',')) != NULL)
      *separator = '\0';
    if (user_name && *user_name != '\0')
      *logname = user_name;
  }
}

void get_host(char **host)
{
#if HAVE_UNAME	/* todo: use sysinfo */
  struct utsname snames;
#endif

  /* Retrieve machine name */
  *host = (char *)malloc(MAX_HOSTNAME);
  if (*host != NULL) {
#if HAVE_UNAME
    if (uname(&snames) == -1 || snames.nodename[0] == 0) {
      free(*host);
      *host = NULL;
    } else
      strcpy(*host, snames.nodename);
#elif HAVE_GETHOSTNAME
    if (gethostname(*host, MAX_HOSTNAME) == -1 || *host[0] == NUL) {
      free(*host);
      *host = NULL;
    }
#endif
  }
}

/*
 * return TRUE iff there are no upper case chars
 */
int is_strlower(const unsigned char * string)
{
  for (/* skip */; *string != NUL; string++)
    if (isupper(*string))
      return FALSE;
  return TRUE;
}


