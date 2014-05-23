/* -------------------------------------------------------------- XIS_EXEC.H */

#include "xis_server.h"

/* ------------------------------------------------------------- enumeration */
typedef enum {
   EXE_ERROR_OK,
   EXE_ERROR_ACTAB,
   EXE_ERROR_ALLOC,
   EXE_ERROR_EXIT,
   EXE_ERROR_EXTRACT,
   EXE_ERROR_FIELD,
   EXE_ERROR_FILE,
   EXE_ERROR_FORMAT,
   EXE_ERROR_FST,
   EXE_ERROR_IMPORT,
   EXE_ERROR_INVALID,
   EXE_ERROR_MAXNREC,
   EXE_ERROR_MAXNTRM,
   EXE_ERROR_MISSING,
   EXE_ERROR_MSRT,
   EXE_ERROR_PROC,
   EXE_ERROR_RETURN,
   EXE_ERROR_SEARCH,
   EXE_ERROR_SORT,
   EXE_ERROR_STW,
   EXE_ERROR_TARGET,
   EXE_ERROR_UCTAB,
   EXE_ERROR_UNDEFINED,
   EXE_ERROR_QTT
} EXE_ERROR;            /* errors */

/* -------------------------------------------------------------- prototypes */
EXE_ERROR   exeRun   (CPL_STRUCT *program,CGI_PARAM *cgiList,OUT_BUFFER *outBuff);
#if ANSI
void cleanShelves(void);
#else
void cleanShelves();
#endif

