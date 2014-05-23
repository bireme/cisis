/* --------------------------------------------------------------- ARGLIST.C */

/* /////////////////////////////////////////////////////////////////////////

   [ Version 1.0 ]
   16.Oct.1998 - Argument list.

   ///////////////////////////////////////////////////////////////////////// */

/* ---------------------------------------------------------- C HEADER FILES */
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
/* ------------------------------------------------------------ HEADER FILES */
/* include "cisis.h" */
#include "easyfc.h"
#include "arglist.h"

/* ================================================================ arg_load */
void arg_load(STRUCT_ARGUMENT *item,	/* argument list item */
                  char *id,						/* argument identification */
                  ARG_TYPE id_type)			/* argument type */
{
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    1. Reset argument parameter
    2. Set argument parameter values

   1.0 - 30.Oct.1998
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */

    /* 1 */
   memset(item,0x00,sizeof(STRUCT_ARGUMENT));

    /* 2 */
   item->id = id;
   item->id_size = (int)strlen(id);
   item->id_type = id_type;
} /* arg_load */
/* =============================================================== arg_lines */
int arg_lines(char *argv[],	/* argment vector */
                  int arg_from,	/* first argument vector index */
              int arg_max,		/* argument vector index limit */
                  char *buff)		/* buffer to be splited */
{
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    1. Avoid empty buffer
   2. Loop for all lines
   3. Find line end, replace line end with end of string character
   4. Check limit of arguments, if reached return -1
   5. Set line argument begin
   6. Return next index argument available

   1.0 - 16.Oct.1998
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
    int argi; /* argument count */
    char *p;	 /* auxiliary buffer pointer */

   /* 1 */
    if (!buff) return arg_from;
    if (!*buff) return arg_from;

   /* 2 */
   for (p = argv[arg_from] = buff,argi = arg_from/*+1*/; *p; )
   {
        /* 3 */
    if (*p != '\n' && *p != '\r')
      {
        p++;
        continue;
      }

        do
      {
        *p++ = '\0';
      }
      while (*p == '\n' || *p == '\r');

        /* 4 */
        if (argi >= arg_max) return -1;

        /* 5 */
      if (*p)
      {
        argv[++argi] = p;
      }

   } /* for */

    /* 6 */
    return ++argi;
} /* arg_lines */
/* ================================================================== arg_in */
int arg_in(char *argv[],   	/* argment vector */
              int arg_from,      /* first argument vector index */
           int arg_max,       /* argument vector index limit */
           char *filename)		/* in file name */
{
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    1. Get file content, on error return negative EFC_FILE_ERROR_LIST
   2. Split lines trought out the argment vector, on error return -5

   1.0 - 16.Oct.1998
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
    char *buff;		/* buffer with file contents */
   LONGX qttbytes;	/* file size, in bytes */

    /* 1 */
    buff = efc_filecontent(filename,&qttbytes);
   if (qttbytes < 0L) return (int)qttbytes;

    /* 2 */
    arg_from = arg_lines(argv,arg_from,arg_max,buff);
    return arg_from > 0 ? arg_from : -ARG_ERROR_MAX;
} /* arg_in */
/* ================================================================ arg_read */
void arg_read(STRUCT_ARGUMENT argl[],	/* available argument list */
                  int argl_max,		  		/* quantity of available arguments */
                  char *argv[],		  		/* arrray of arguments */
                  int arg_from,     	  		/* first argument */
                  int arg_max)     	  		/* quantity of arguments */
{
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    1. Loop for all arguments received
   2. Scan the list of valid arguments
   3. Check argument identifier
   4. BOOLEAN argument, presence means TRUE
   5. NUMBER/TEXT argument requires the equal signal

   1.0 - 16.Oct.1998
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
   int argi;	/* arguments loop index */
   int argli;	/* arguments list loop index */
   char *argp;	/* auxiliary argument pointer */

    /* 1 */
   for (argi = arg_from; argi < arg_max; argi++) {

        argp = argv[argi];
      if (!argp) continue;

        /* 2 */
      for (argli = 0; argli < argl_max; argli++) {

            /* 3 */
        if (strncmp(argp,argl[argli].id,argl[argli].id_size) != 0) continue;

         switch (argl[argli].id_type) {

            /* 4 */
         case ARG_TYPE_BOOLEAN :
            if (*(argp+argl[argli].id_size) == '\0') {
                argl[argli].num = (LONGX)TRUE;
                    argl[argli].text = argv[argi];
                    argl[argli].status = 1;
            }
            break;

            /* 5 */
         case ARG_TYPE_NUMBER :
         case ARG_TYPE_TEXT :
            if (*(argp+argl[argli].id_size) == '=') {
                argl[argli].num = atol(argp+argl[argli].id_size+1);
                    argl[argli].text = argp+argl[argli].id_size+1;
                    argl[argli].status = 1;
            }
            break;

         } /* switch */

      } /* for */

   } /* for */
} /* arg_read */

