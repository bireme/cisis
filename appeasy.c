/* --------------------------------------------------------------- APPEASY.C */

/* -------------------------------------------------------------- C includes */
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
/* ---------------------------------------------------------------- includes */
#include "cisis.h"
#include "globdef.h"
#include "appeasy.h"

#if UNIX
/* ----------------------------------------------------------------- strlwr */
char *strlwr(p)
char *p;
{
   while (*p) {
      if (isupper(*p)) *p = tolower(*p);
      p++;
   }
}
#endif /* UNIX */
/* ------------------------------------------------------------------ strall */
UCHAR *strall(p)
UCHAR *p;
{
   UCHAR *new_p;

   if (!p) return (UCHAR *)NULL;
   new_p = (UCHAR *)ALLOC((ALLOPARM)strlen(p)+1);
   if (!new_p) fatal("strall");
   return strcpy(new_p,p);
}
/* ---------------------------------------------------------------- strdeall */
UCHAR *strdeall(p)
UCHAR *p;
{
   if (p) FREE(p);
   return (UCHAR *)NULL;
}
/* ---------------------------------------------------------------- strreall */
UCHAR *strreall(old,from)
UCHAR *old,*from;
{
   if (old) FREE(old);
   return strall(from);
}
/* ----------------------------------------------------------- easy_trimleft */
UCHAR *easy_trimleft(s)
UCHAR *s;
{
   UCHAR *dup,
         *p;

   if (!s) return (UCHAR *)NULL;
   dup = strall(s);
   for (p = dup; *p == ' '; p++);
   if (*p) strcpy(s,p);
   strdeall(dup);
   return s;
}
/* ---------------------------------------------------------- easy_trimright */
UCHAR *easy_trimright(s)
UCHAR *s;
{
   UCHAR *p;
   int    len;

   if (!s) return (UCHAR *)NULL;
   if (!*s) return s;
   for (len = strlen(s),p = s+(len-1); len && *p == ' '; len--,p--)
      if (*p == ' ') *p = '\0';
   return s;
}
/* ------------------------------------------------------------ easy_getfile */
BOOLEAN easy_getfile(file_name,buff,max,ignore)
UCHAR *file_name,
      *buff;
long   max;
UCHAR *ignore;
{
   int handle,
       i,
       qtt;

   handle = OPEN(file_name,O_BINARY|O_RDONLY|O_DENYNONE);
   if (handle < 0) fatal(file_name);
   for (i = 0; i < max; ) {
      qtt = CIREAD(handle,buff,1);
      if (qtt < 0) fatal(file_name);
      if (qtt == 0) break;
      if (ignore)
         if (strchr(ignore,*buff)) continue;
      if (*buff == CONTROL_Z) continue;
      buff++;
      i++;
   }
   *buff = '\0';
   CLOSE(handle);
   return (BOOLEAN)(i < max);
}
