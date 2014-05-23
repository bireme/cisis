#ifndef EASYFC2_H
#define EASYFC2_H

/* ---------------------------------------------------------------- EASYFC.H */
#include <limits.h>
/* ----------------------------------------------------------------- defines */

#if SIXTY_FOUR
#ifndef LONGX
#define LONGX int
#endif /* LONGX */
#ifndef LONGX_MAX
#define LONGX_MAX INT_MAX
#endif /* LONGX_MAX */
#else /* SIXTY_FOUR */
#ifndef LONGX
#define LONGX long
#endif /* LONGX */
#ifndef LONGX_MAX
#define LONGX_MAX LONG_MAX
#endif /* LONGX_MAX */
#endif /* SIXTY_FOUR */


#ifndef BOOLEAN
#define BOOLEAN    int
#endif

#ifndef TRUE
#define TRUE       1
#endif

#ifndef FALSE
#define FALSE      0
#endif

#ifndef O_TEXT
#define O_TEXT     0
#endif

#ifndef O_BINARY
#define O_BINARY   0
#endif

/* File access modes used with open() and fcntl() */
/* Copy of fcntl.h */
#ifndef O_RDONLY
#define O_RDONLY      0000000   /* Open for reading only */
#endif
#ifndef O_WRONLY
#define O_WRONLY      0000001   /* Open for writing only */
#endif
#ifndef O_RDWR
#define O_RDWR        0000002   /* Open for reading or writing */
#endif
#ifndef O_ACCMODE
#define O_ACCMODE     0000003   /* Mask for file access modes */
#endif

#ifndef O_DENYNONE
#define O_DENYNONE 0
#endif

#ifndef KBYTE
#define KBYTE      1024L
#endif

#ifndef EFC_ERROR_INFO_MAX
#define EFC_ERROR_INFO_MAX      60
#endif
/* ------------------------------------------------------------- enumaration */
typedef enum {
    FILE_ERROR_EMPTY,
    FILE_ERROR_SIZE,
    FILE_ERROR_OPEN,
    FILE_ERROR_ALLOC,
    FILE_ERROR_READ,
    FILE_ERROR_LIST_QTT
} EFC_FILE_ERROR_LIST;      /* file error */

typedef enum {
    COPYFILE_ERROR_OK,
    COPYFILE_ERROR_IN_OPEN,
    COPYFILE_ERROR_OUT_OPEN,
    COPYFILE_ERROR_LIST_QTT
} COPYFILE_ERROR;               /* file copy error */

typedef enum {
    NUMBERLIST_ERROR_OK,
    NUMBERLIST_ERROR_ALLOC,
    NUMBERLIST_ERROR_LIST_QTT
} EFC_NUMBERLIST_ERROR_LIST;    /* number list error */
/* --------------------------------------------------------------- structure */
typedef struct {
    int code;
   char info[EFC_ERROR_INFO_MAX+1];
} EFC_ERROR;                    /* error */

typedef struct {
   char *text;
} EFC_LINE;

typedef struct {
    LONGX qtt;
   EFC_LINE *list;
} EFC_SPLIT_LINES;

typedef struct {
    LONGX qtt;
    LONGX *list;
} EFC_NUMBER_LIST;

typedef struct {
    char fname[160];
    char *buffer;
} EFC_BUFFER;

/* --------------------------------------------------------------- prototype */

LONGX efc_filesize(int handle);

char *efc_filecontents_return(char *buff,      /* return buffer */
                              LONGX *qttbytes,  /* file size in bytes on success, negative code on error */
                              int error_code); /* error code */

void *efc_free(void *buff); /* original string buffer */

void *efc_new(LONGX mem_size); /* buffer size, in bytes */

LONGX efc_split_lines_char(EFC_SPLIT_LINES *split_lines,  /* split lines structure */
                          char *text,                    /* text to be splited */
                          char char_sep);                /* separator character */


LONGX efc_split_lines_char(EFC_SPLIT_LINES *split_lines,  /* split lines structure */
                          char *text,                    /* text to be splited */
                          char char_sep);                /* separator character */

void efc_split_lines_free(EFC_SPLIT_LINES *split_lines); /* split lines structure */

LONGX efc_numberList(EFC_ERROR *err,                 /* error structure */
                    EFC_NUMBER_LIST *numberList,    /* store number list */
                    char *text);                    /* text to be splited */

void efc_numberListFree(EFC_NUMBER_LIST *numberList);       /* number list */

char *efc_std_filecontent(EFC_ERROR *err,    /* error structure */
                          char *file_name);  /* file name */

char *efc_strrepl(char *old,    /* current string buffer content */
                  char *from);  /* new string buffer content */

char *efc_trim_right       ();
COPYFILE_ERROR efc_copyFile();

char *efc_trim_right(char *buff); /* string buffer to be trimed */

COPYFILE_ERROR efc_copyFile(unsigned char *fileNameIn,
                            unsigned char *fileNameOut);

LONGX efc_split_lines(EFC_SPLIT_LINES *split_lines,  /* split lines structure */
                     char *text);                   /* text to be splited */

#ifdef XIS_SERVER
char * efc_load_buffer(char *fileName,
                       LONGX *qttbytes);

void efc_delete_buffer();
#endif

#endif /* EASYFC2_H */
