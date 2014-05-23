/* --------------------------------------------------------------- CGILIST.C */

/* ---------------------------------------------------------  C HEADER FILES */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
/* ------------------------------------------------------------ HEADER FILES */
#include "../cisis.h"
#include "easyfc.h"
#include "cgilist.h"
/* ----------------------------------------------------------------- globals */
char *cgi_empty = "";	/* empty string */

/* ============================================================== cgi_getenv */
char *cgi_getenv(char *env_name)		/* environment name */
{
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    1. Get environment content, return string pointer

   1.0 - 28.Oct.1998
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
   char *envp;                  /* environment content pointer */

   /* 1 */
   envp = getenv(env_name);
   return envp ? envp : cgi_empty;
}
/* ======================================================== cgi_0xHH_to_char */
static char cgi_0xHH_to_char(char *what)
{
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    1. Copied from a magazine (original name: x2h)

   1.0 - 28.Oct.1998
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
   register char digit;

   /* 1 */
   digit = (what[0] >= 'A' ? ((what[0] & 0xdf) - 'A')+10 : (what[0] - '0'));
   digit *= 16;
   digit += (what[1] >= 'A' ? ((what[1] & 0xdf) - 'A')+10 : (what[1] - '0'));
   return(digit);
}
/* ======================================================== cgi_unescape_url */
static void cgi_unescape_url(char *url)
{
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    1. Copied from a magazine (original name: unescape_url)

   1.0 - 28.Oct.1998
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
   register int x,y;

   /* 1 */
   for (x=0,y=0; url[y]; ++x,++y) {
      if ((url[x] = url[y]) == '%') {
         url[x] = cgi_0xHH_to_char(&url[y+1]);
         y+=2;
      }
   }
   url[x] = '\0';
}
/* ======================================================= cgi_plus_to_space */
static void cgi_plus_to_space(char *str)
{
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    1. Copied from a magazine (original name: plustospace)

   1.0 - 28.Oct.1998
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
   register int x;

   /* 1 */
   for (x=0; str[x]; x++) if (str[x] == '+') str[x] = ' ';
}
/* ========================================================== cgi_method_get */
char *cgi_method_get(void)
{
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    1. Get CGI query string of method GET
   2. Return the content copy, NULL indicates no query string

   1.0 - 28.Oct.1998
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
   char *buff;		/* auxiliary string buffer pointer */

   /* 1 */
   buff = getenv("QUERY_STRING");
   return buff ? strdup(buff) : NULL;
}
/* ========================================================= cgi_method_post */
char *cgi_method_post(EFC_ERROR *err)	/* error structure */
{
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    1. Check content type
   2. Get content lenght
   3, Allocate buffer for the CGI data
   4. Read CGI data from stdin
   5. Return allocated buffer with CGI data

   1.0 - 28.Oct.1998
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
   char *buff;					/* auxiliary string buffer pointer */
   LONGX content_lenght;		/* content lenght */
   LONGX bytes_read;			/* bytes read from stdin */

   /* 1 */
   if (strcmp(cgi_getenv("CONTENT_TYPE"),
              "application/x-www-form-urlencoded") != 0) return NULL;

   /* 2 */
   content_lenght = atol(cgi_getenv("CONTENT_LENGTH"));
   if (content_lenght < 1L) return NULL;

   /* 3 */
   buff = efc_new(content_lenght+1);
   if (!buff) {
    efc_error(err,CGI_ERROR_POST,"buff");
      return NULL;
   }

   /* 4 */
   bytes_read = (LONGX)fread(buff,1,content_lenght,stdin);
   if (bytes_read != content_lenght) {
    efc_error(err,CGI_ERROR_POST,"bytes_read");
      return NULL;
   }
   *(buff+content_lenght) = '\0';

   /* 5 */
   return buff;
}
/* ============================================================ cgi_newparam */
CGI_PARAM *cgi_newparam(EFC_ERROR *err,		/* error structure */
                               CGI_PARAM *cgi_pos,	/* last cgiparam item */
                        char *cgicouple)     /* cgi couple name=value */
{
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    1.	Allocate cgi parameter structure, reset it
   2. Set cgi parameter name
   3. Set cgi parameter value
   4. Set list pointer
   5. Return allocated pointer

   1.0 - 16.Nov.1998
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
   CGI_PARAM *cgi_aux;	   /* auxiliary cgiparam structure */
   char *sepp;					/* separator pointer */

   /* 1 */
   cgi_aux = efc_new(sizeof(CGI_PARAM));
   if (!cgi_aux) {
    efc_error(err,CGI_ERROR_ALLOC,"newcgiparam");
    return NULL;
   }
   memset(cgi_aux,0x00,sizeof(CGI_PARAM));

   /* 2 */
   cgi_aux->name = cgicouple;

    /* 3 */
   sepp = strchr(cgicouple,'=');
   if (sepp) {
      *sepp = '\0';
      cgi_aux->value = ++sepp;
   } else cgi_aux->value = cgi_empty;

    /* 4 */
   if (cgi_pos) cgi_pos->next = cgi_aux;

   /* 5 */
   return cgi_aux;
}
/* =============================================================== cgi_split */
CGI_PARAM *cgi_split(EFC_ERROR *err,		/* error structure */
                            char *query_string)	/* query string buffer */
{
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    1. Loop all query string
   2. Check string end
   3. Find end of item, set end of string for that item
   4. Copy item, apply convertion
   5. Allocate item, append on the list
   6. Return first item

   1.0 - 16.Nov.1998
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
   CGI_PARAM *cgi_head = NULL;	/* first cgi parameter */
   CGI_PARAM *cgi_pos = NULL;		/* current cgi parameter */
   char *p;								/* auxiliary string pointer */
   char *endp;							/* end of item pointer */
   char *cgicouple;

   /* 1 */
   for (p = query_string; p; p = endp) {

    /* 2 */
      if (!*p) break;

    /* 3 */
      endp = strchr(p,'&');
      if (endp) { *endp = '\0'; endp++; }

    /* 4 */
      cgicouple = strdup(p);
      if (!cgicouple) {
        efc_error(err,CGI_ERROR_ALLOC,p);
        return NULL;
      }
      cgi_plus_to_space(cgicouple);
      cgi_unescape_url(cgicouple);

    /* 5 */
      cgi_pos = cgi_newparam(err,cgi_pos,cgicouple);
      if (!cgi_pos) return NULL;
      if (!cgi_head) cgi_head = cgi_pos;

   } /* for */

   /* 6 */
   return cgi_head;
}
/* ================================================================= cgi_env */
char *cgi_env(EFC_ERROR *err)		/* error structure */
{
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    1. Get method, get query string
   2. Return query string

   1.0 - 28.Oct.1998
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
   char *req_method;				/* CGI request method */
   char *query_string = NULL;	/* CGI query string */

   /* 1 */
   req_method = cgi_getenv("REQUEST_METHOD");
   if (strcmp(req_method,"GET") == 0) query_string = cgi_method_get();
   else
      if (strcmp(req_method,"POST") == 0) query_string = cgi_method_post(err);

   /* 2 */
   return query_string;
}
/* ================================================================ cgi_read */
CGI_PARAM *cgi_read(EFC_ERROR *err)		/* error structure */
{
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    1. Get CGI query string
   2. Allocate cgiparam list
   3. Garbage collector
   4. Return cgiparam first item

   1.0 - 28.Oct.1998
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
   CGI_PARAM *cgi_head;		/* first cgi parameter */
   char *cgibuff;				/* cgi query string buffer pointer */

   /* 1 */
   cgibuff = cgi_env(err);
   if (!cgibuff) return NULL;

   /* 2 */
   cgi_head = cgi_split(err,cgibuff);
   if (!cgi_head) return NULL;

   /* 3 */
   efc_free(cgibuff);

   /* 4 */
   return cgi_head;
}
/* ================================================================ cgi_free */
void cgi_free(CGI_PARAM *cgiparam)	/* first cgi parameter */
{
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    1. Garbage collector

   1.0 - 28.Oct.1998
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
    CGI_PARAM *next;   /* next cgi paramater pointer */

   /* 1 */
   for ( ; cgiparam; cgiparam = next) {
    next = cgiparam->next;
      cgiparam->name = efc_free(cgiparam->name);
      /* cgiparam->value = efc_strfree(cgiparam->value); */
      efc_free(cgiparam);
   }
}

