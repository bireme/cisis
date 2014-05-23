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
#define EFC_ERROR_INFO_MAX		60
#endif
/* ------------------------------------------------------------- enumaration */
typedef enum {
    FILE_ERROR_EMPTY,
    FILE_ERROR_SIZE,
    FILE_ERROR_OPEN,
    FILE_ERROR_ALLOC,
    FILE_ERROR_READ,
    FILE_ERROR_LIST_QTT
} EFC_FILE_ERROR_LIST;		/* file error */

typedef enum {
    COPYFILE_ERROR_OK,
    COPYFILE_ERROR_IN_OPEN,
    COPYFILE_ERROR_OUT_OPEN,
    COPYFILE_ERROR_LIST_QTT
} COPYFILE_ERROR;				/* file copy error */

typedef enum {
    NUMBERLIST_ERROR_OK,
    NUMBERLIST_ERROR_ALLOC,
    NUMBERLIST_ERROR_LIST_QTT
} EFC_NUMBERLIST_ERROR_LIST;	/* number list error */
/* --------------------------------------------------------------- structure */
typedef struct {
    int code;
   char info[EFC_ERROR_INFO_MAX+1];
} EFC_ERROR;					/* error */

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
/* --------------------------------------------------------------- prototype */
int efc_error               (EFC_ERROR *err,
                             int error_code,
                             char *error_info);
LONGX  efc_filesize			(int handle);
char *efc_filecontent		(char *read_file,
                             LONGX *qttbytes);
void *efc_free				(void *buff);
void *efc_new				(LONGX mem_size);
LONGX efc_split_lines		(EFC_SPLIT_LINES *split_lines,
                             char *text);
LONGX efc_split_lines_char	(EFC_SPLIT_LINES *split_lines,
                             char *text,
                             char char_sep);
void efc_split_lines_free	(EFC_SPLIT_LINES *split_lines);
LONGX efc_numberList			(EFC_ERROR *err,
                             EFC_NUMBER_LIST *numberList,
                             char *text);
void efc_numberListFree		(EFC_NUMBER_LIST *numberList);
char *efc_std_filecontent	(EFC_ERROR *err,
                             char *file_name);
char *efc_strrepl			(char *old,
                             char *from);
char *efc_trim_right        (char *buff);
COPYFILE_ERROR efc_copyFile(unsigned char *fileNameIn,
                            unsigned char *fileNameOut);

