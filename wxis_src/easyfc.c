/* ---------------------------------------------------------------- EASYFC.C */

/* /////////////////////////////////////////////////////////////////////////

   [ Version 1.0 ]
   16.Oct.1998 - Functions to make things easier.

   ///////////////////////////////////////////////////////////////////////// */

/* ---------------------------------------------------------- C HEADER FILES */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef WIN32
#include <io.h>
#include <sys\stat.h>
#else /* WIN32 */
#include <sys/stat.h>
#include <fcntl.h>
#endif /* WIN32 */
/* ------------------------------------------------------------ HEADER FILES */
/* include "cisis.h" */
#include "easyfc2.h"

/* =========================================== file content buffer variables */
#ifdef XIS_SERVER
const int maxPos = 10;
int first = 1;
int olderPos = 0;
EFC_BUFFER fileContents[10];
#endif

/* =============================================================== efc_error */
int efc_error(EFC_ERROR *err,   /* error structure */
              int error_code,   /* error code */
              char *error_info) {   /* error information */
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    1. Set error values
   2. Return error code

   1.0 - 16.Oct.1998
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */

   /* 1 */
    err->code = error_code;
   *(err->info) = '\0';
   if (error_info) strncpy(err->info,error_info,EFC_ERROR_INFO_MAX);
   /* 2 */
   return error_code;
} /* efc_error */
/* ================================================================= efc_new */
void *efc_new(LONGX mem_size) { /* buffer size, in bytes */
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    1. Return allocated memory pointer, NULL indicates error

   1.0 - 16.Oct.1998
      a) the caller must free returned space
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */

   /* 1 */
   return (void *)malloc((unsigned LONGX)mem_size);
} /* efc_new */
/* ================================================================ efc_free */
void *efc_free(void *buff) { /* original string buffer */
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    1. Free memory space
   2. Return NULL

   1.0 - 16.Oct.1998
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */

    /* 1 */
    if (buff) free(buff);

    /* 2 */
   return NULL;
} /* efc_free */
/* ============================================================= efc_strrepl */
char *efc_strrepl(char *old,    /* current string buffer content */
                  char *from) { /* new string buffer content */
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    1. Free old memory space
    2. Return allocated memory copied, NULL indicates error

   1.0 - 16.Oct.1998
      a) the caller must free returned space
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */

    /* 1 */
   efc_free(old);

    /* 2 */
   return strdup(from);
} /* efc_strrepl */
/* ========================================================== efc_trim_right */
char *efc_trim_right(char *buff) { /* string buffer to be trimed */
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    1. Eliminate trailing spaces
   2. Return trailed buffer

   1.0 - 16.Oct.1998
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
   char *p;  /* auxiliary loop string pointer */
   LONGX len; /* original string lenght */

    /* 1 */
   if (buff)
    if (*buff)
           for (len = strlen(buff),p = buff+(len-1); len && *p == ' '; len--,p--)
           if (*p == ' ') *p = '\0';

    /* 2 */
   return buff;
} /* efc_trim_right */
/* ============================================================ efc_filesize */
LONGX efc_filesize(int handle) { /* file handle */
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    1. Get file status information
   2. Return file size on success, or -1 on error

   1.0 - 16.Oct.1998
    a) fstat isn't an ANSI function
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
   struct stat statbuf; /* stat structure */
   int err; /* stat structure */

    /* 1 */
    err = fstat(handle,&statbuf);

    /* 2 */
   return (LONGX)(err ? err : statbuf.st_size);
} /* efc_filesize */
/* ================================================= efc_filecontents_return */
char *efc_filecontents_return(char *buff,        /* return buffer */
                              LONGX *qttbytes, /* file size in bytes on success, negative code on error */
                              int error_code) { /* error code */
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    1. If error, set qttbytes with the negative error code
   2. Return buffer

   1.0 - 16.Oct.1998
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */

    /* 1 */
    if (error_code) *qttbytes = (LONGX)error_code;

    /* 1 */
    return buff;
} /* efc_filecontents_return */
/* ========================================================= efc_filecontent */
char *efc_filecontent(char *read_file,  /* file name */
                      LONGX *qttbytes) {   /* size of the file in bytes */
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    1. Open the file, on error: qttbytes = -2
   2. Get file size in bytes, on error: qttbytes = -1
   3. Allocate file content buffer, on error: qttbytes = -3
   4. Read file content, set end of string buffer, on error: qttbytes = -4
   5. Close file
   6. Return allocated buffer pointer

   1.0 - 16.Oct.1998
    a) on error -2 or -4, "errno" contains the system error code number
      b) the caller must free returned space
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
   int handle; /* file handle */
   char *buff; /* buffer pointer */

   /* 1 */
   handle = open(read_file,O_TEXT|O_RDONLY|O_DENYNONE);
   if (handle < 0) return efc_filecontents_return(NULL,qttbytes,-FILE_ERROR_OPEN);

   /* 2 */
   *qttbytes = efc_filesize(handle);
   if (*qttbytes <= 0) {
       close(handle);
       return NULL; /* -FILE_ERROR_SIZE */
   }

   /* 3 */
   buff = (char *)efc_new(*qttbytes + 1);
   if (!buff) {
       close(handle);
       return efc_filecontents_return(NULL,qttbytes,-FILE_ERROR_ALLOC);
   }

   /* 4 */
    *qttbytes = read(handle,buff,(unsigned int)*qttbytes);
    if (*qttbytes <= 0) {
        close(handle);
        return efc_filecontents_return(NULL,qttbytes,-FILE_ERROR_READ);
    }
    *(buff+(*qttbytes)) = '\0';

   /* 5 */
   close(handle);

   /* 6 */
   return buff;
} /* efc_filecontent */
/* ===================================================== efc_std_filecontent */
char *efc_std_filecontent(EFC_ERROR *err,    /* error structure */
                          char *file_name) {   /* file name */
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    1. Get file content
    2. Check error
    3. Return allocated buffer

   1.0 - 16.Oct.1998
        a) the caller must free returned space
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
    char *buff;     /* file content buffer pointer */
    LONGX qttbytes; /* file size, in bytes */

   /* 1 */

#ifndef XIS_SERVER
    buff = efc_filecontent(file_name,&qttbytes);
#else
   buff = efc_load_buffer(file_name, &qttbytes);
#endif

   /* 2 */
   if (!buff) efc_error(err,qttbytes,file_name);

   /* 3 */
   return buff;
}
/* ==================================================== efc_split_lines_char */
LONGX efc_split_lines_char(EFC_SPLIT_LINES *split_lines,  /* split lines structure */
                          char *text,                    /* text to be splited */
                          char char_sep) {                 /* separator character */
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    1. Count number of lines
   2. Allocate line pointers
   3. Set line begin pointers
   4. Return number of lines

   1.0 - 24.Nov.1998
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
    char *p;                /* auxiliary string buffer pointer */
   char char_pair = '\0';
   BOOLEAN newline; /* new line flag */
   LONGX i;              /* line index */

   /* 1 */
    for (split_lines->qtt = 0L,newline = TRUE,p = text; *p; p++) {
      if (newline) {
        split_lines->qtt++;
         newline = FALSE;
      }
        if (*p == char_sep) {
        newline = TRUE;
         continue;
      }
   } /* for */

   /* 2 */
   split_lines->list = NULL;    /* 16.Apr.1999 for empty buffer */
   if (split_lines->qtt) {
       split_lines->list = (EFC_LINE *)efc_new((size_t)sizeof(EFC_LINE)*split_lines->qtt);
      if (!split_lines->list) return -1L;
   }

   /* 3 */
    if (char_sep == '\r') char_pair  = '\n';
    if (char_sep == '\n') char_pair  = '\r';
    for (newline = TRUE,i = 0L,p = text; *p; p++) {
      if (newline) {
            if (*p == char_pair) p++;
        split_lines->list[i++].text = p;
         newline = FALSE;
      }
        if (*p == char_pair && *(p+1) == char_sep) *p = '\0';
        if (*p == char_sep) {
         *p = '\0';
        newline = TRUE;
      }
   } /* for */

   /* 4 */
   return split_lines->qtt;
}
/* ========================================================= efc_split_lines */
LONGX efc_split_lines(EFC_SPLIT_LINES *split_lines,  /* split lines structure */
                     char *text) {                    /* text to be splited */
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    1. Split lines

   1.0 - 24.Nov.1998
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */

   /* 1 */
    return efc_split_lines_char(split_lines,text,'\n');
}
/* ==================================================== efc_split_lines_free */
void efc_split_lines_free(EFC_SPLIT_LINES *split_lines) { /* split lines structure */
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    1. No lines, garbage collector

   1.0 - 24.Nov.1998
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */

    /* 1 */
    split_lines->qtt = 0L;
   split_lines->list = (EFC_LINE *)efc_free(split_lines->list);
}

/* ========================================================== efc_numberList */
LONGX efc_numberList(EFC_ERROR *err,                 /* error structure */
                    EFC_NUMBER_LIST *numberList,    /* store number list */
                    char *text) {                   /* text to be splited */
   char *textDup;                                       /* text string copy */
   EFC_SPLIT_LINES splitLines;                  /* split lines structure */
    int line;                                 /* line index */

    /* initialization */
   splitLines.qtt = 0L;
   splitLines.list = NULL;

    /* split text string into lines */
   textDup = strdup(text);
    if (!textDup)
   {
    return -efc_error(err,NUMBERLIST_ERROR_ALLOC,text);
   }
   if (efc_split_lines(&splitLines,textDup) < 0L)
    {
    return -efc_error(err,NUMBERLIST_ERROR_ALLOC,textDup);
    }

    /* store numbers from each text line */
    numberList->qtt = splitLines.qtt;
    numberList->list = (LONGX *)efc_new( (size_t)sizeof(LONGX *) * numberList->qtt );
    for (line = 0L; line < numberList->qtt; line++)
    {
        numberList->list[line] = atol(splitLines.list[line].text);
    }

    /* garbage collector */
    efc_free(textDup);
    efc_split_lines_free(&splitLines);

    /* return quantity of numbers */
    return numberList->qtt;

} /* efc_numberList */

/* ====================================================== efc_freeNumberList */
void efc_numberListFree(EFC_NUMBER_LIST *numberList) {      /* number list */
    numberList->qtt = 0L;
   numberList->list = (LONGX *)efc_free(numberList->list);
} /* efc_numberListFree */

/* ------------------------------------------------------------ efc_copyFile */
COPYFILE_ERROR efc_copyFile(unsigned char *fileNameIn,
                            unsigned char *fileNameOut) {
   FILE *copyIn;
   FILE *copyOut;
   int theChar;

   copyIn = fopen((const char *)fileNameIn,"rb");
   if (!copyIn)
   {
    return COPYFILE_ERROR_IN_OPEN;
   }

   copyOut = fopen((const char *)fileNameOut,"wb");
   if (!copyOut)
   {
    fclose(copyIn);
       return COPYFILE_ERROR_OUT_OPEN;
   }

   theChar = fgetc(copyIn);
   while (theChar != EOF)
   {
        fputc(theChar,copyOut);
    theChar = fgetc(copyIn);
   }

   fclose(copyIn);
   fclose(copyOut);

   return COPYFILE_ERROR_OK;

} /* efc_copyFile */

#ifdef XIS_SERVER
/* ========================================================= efc_load_buffer */
char * efc_load_buffer(char *fileName,
                       LONGX *qttbytes) {
    char * ret = NULL;
    LONGX size = 0;
    int newPos = 0;
    int pos = 0;

    if (first == 1) {
        for (pos = 0; pos < maxPos; pos++) {
            fileContents[pos].fname[0] = '\0';
        }
        //memset(fileContents, 0x00, sizeof(fileContents));
        first = 0;
    }

    for (pos = 0; pos < maxPos; pos++) {
        if (fileContents[pos].fname[0] == '\0') {
            fileContents[pos].buffer = efc_filecontent(fileName, &size);
            if (size > 0) {
                strcpy(fileContents[pos].fname, fileName);
                ret = fileContents[pos].buffer;
            }
            break;
        } else if (strcmp(fileName, fileContents[pos].fname) == 0) {
            ret = fileContents[pos].buffer;
            size = ((ret != NULL) ? strlen(ret) : 0);
            break;
        }
    }

    if (ret == NULL) {

        newPos = olderPos;
        olderPos = ((olderPos < (maxPos - 1)) ? (++olderPos) : 0);
        ret = efc_filecontent(fileName, &size);
        if (size > 0) {
            efc_free(fileContents[newPos].buffer);
            fileContents[newPos].buffer = ret;
            strcpy(fileContents[newPos].fname, fileName);
        } else {
            olderPos = newPos;
        }
    }

    *qttbytes = size;

    return ret;
}

/* ======================================================== efc_delete_buffer */
void efc_delete_buffer() {
    int pos = 0;

    for (pos = 0; pos < maxPos; pos++) {
        efc_free(fileContents[pos].buffer);
        fileContents[pos].buffer = NULL;
        fileContents[pos].fname[0] = '\0';
    }
}
#endif /* XIS_SERVER */
