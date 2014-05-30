/* -------------------------------------------------------------- XIS_EXEC.C */

/* ---------------------------------------------------------- C HEADER FILES */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>
/* ------------------------------------------------------------ HEADER FILES */
#include "../cisis.h"
 //#include "../cib70.h" /* b7_cib7_delete */
#include "citpv.h"
#include "easyfc.h"
#include "easyci.h"
#include "cgilist.h"
#include "xis_comp.h"
#include "freqlist.h"
#include "cimsrt.h"
#include "hlfill.h"
#include "cixml.h"
#include "xis_exec.h"
/* ------------------------------------------------------------------ define */
#if CIFFI
//#define EXE_REC_SIZE       2*1100000L /*1048576L*/ /*4194304L*/ /* defult record size */
//#define EXE_BUFF_SIZE      2*1100000L /*4194304L*/ /*1025000L*/ /* defult buffer size */
#define EXE_REC_SIZE         3*(rec_maxmfrl)/*(rec_maxmfrl)*/ /*1048576L*/ /*4194304L*/ /* defult record size */
#define EXE_BUFF_SIZE        3*(rec_maxmfrl)/*(rec_maxmfrl)*/ /*4194304L*/ /*1025000L*/ /* defult buffer size */
#else
//#define EXE_REC_SIZE       (32L*KBYTE)    /* defult record size */
//#define EXE_BUFF_SIZE      (32L*KBYTE)    /* defult buffer size */
#define EXE_REC_SIZE         3*(rec_maxmfrl) /*(rec_maxmfrl)*/ /* defult record size */
#define EXE_BUFF_SIZE        3*(rec_maxmfrl) /*(rec_maxmfrl)*/ /* defult buffer size */
#endif
#define SEARCH_AUX_SIZE    (64L*KBYTE)    /* defult search record size */
#define SEARCH_PREFIX_SIZE (4L*KBYTE)     /* defult search prefix record size */
/*#define DEFAULT_MAXLK      1024L          /* default maximum link generation */
#define DEFAULT_MAXLK      102400L        /* default maximum link generation - Visalegis set/05 */
#define MINIMUM_MAXLK      100L           /* lower value maximum link generation */
#define FULLINV_TAG_KEY    1              /* extract key field number */
#define DEFAULT_KEYLENGTH  512L           /* default master sort key length */
#define CI_TMPNAM          256            /* cisis temporary file name size */
#if !CIB71
#define b7_CIB7             void
#endif /* CIB71 */

#ifndef MAXNDBX
#define   MAXNDBX    2048L      /* maximum #entries             */
#endif
#ifndef MAXNREC
#define   MAXNREC     256L      /* maximum #entries             */
#endif
#ifndef MAXNTRM
#define   MAXNTRM     256L      /* maximum #entries             */
#endif

#define SRC_TMP_FILE 0                   /* create cisis temporary file */


OUT_BUFFER *outBuffer;

#if ANSI
b7_CIB7 *b7_cib7(b7_CIB7 *cib7p, void *xciapip);
b7_CIB7 *b7_cib7_delete(b7_CIB7 *cib7p);
#else
b7_CIB7 *b7_cib7();
b7_CIB7 *b7_cib7_delete();
#endif

/* ------------------------------------------------------------- enumeration */
typedef enum {
   PARM_0,
   PARM_ACTAB,
   PARM_BUFFERSIZE,
   PARM_CIPAR,
   PARM_COUNT,
   PARM_DB,
   PARM_DECOD,
   PARM_DELIMITER,
   PARM_EXPIRE,
   PARM_EXPRESSION,
   PARM_FILE,
   PARM_FREQSUM,
   PARM_FROM,
   PARM_FST,
   PARM_GIZMO,
   PARM_INDEXLIST,
   PARM_KEY,
   PARM_KEYFIELD,
   PARM_KEYLENGTH,
   PARM_KEYS,
   PARM_KEYSDB,
   PARM_LOCKID,
   PARM_MAXLK,
   PARM_MFN,
   PARM_POSTING,
   PARM_POSTTAG,
   PARM_PREFIX,
   PARM_RESET,
   PARM_REVERSE,
   PARM_SORT,
   PARM_STW,
   PARM_SUFFIX,
   PARM_TASK,
   PARM_TO,
   PARM_TYPE,
   PARM_UCTAB,
   /* 27.Nov.2000 */
   PARM_PREFIX_HTMLPFT,
   PARM_SUFFIX_HTMLPFT,
   /* 28.Nov.2000 */
   PARM_FIRST_LINE,
   /* 22.Feb.2001 */
   PARM_ISISXML_STYLE,
   PARM_ISISXML_TABLE,
   /* 05.Apr.2001 */
   PARM_LOG,

   PARM_LIST_QTT
} PARM_LIST;               /* parameter option */

typedef enum {
   PRECISE_BR,
   PRECISE_CURRENT,
   PRECISE_ERRORINFO,
   PRECISE_FROM,
   PRECISE_GETNEW,
   PRECISE_HLINE,
   PRECISE_KEYS,
   PRECISE_ISO2709,
   PRECISE_ISO2709_CRLF,
   PRECISE_ITEM,
   PRECISE_ITENS,
   PRECISE_ITEMS,
   PRECISE_KEY,
   PRECISE_LOCK,
   PRECISE_MFN,
   PRECISE_NEW,
   PRECISE_NEXT,
   PRECISE_OFF,
   PRECISE_ON,
   PRECISE_POSTING,
   PRECISE_POSTINGS,
   PRECISE_QUIT,
   PRECISE_RECSTAT,
   PRECISE_RLINE,
   PRECISE_STATUS,
   PRECISE_TABLE,
   PRECISE_TO,
   PRECISE_TOTAL,
   PRECISE_MARC21,
   PRECISE_VALUE,
   PRECISE_VLINE,
   PRECISE_WDELETE,
   PRECISE_WLOCK,
   PRECISE_WNOUNLOCK,
   PRECISE_WUNLOCK,

   /* 05.Apr.2001 */
   PRECISE_SETNUMBER,

   PRECISE_LIST_QTT
} PRECISE_LIST;               /* precise option */

typedef enum {
   TRACE_SEPARATOR,
   TRACE_CMD,
   TRACE_TEXT,
   TRACE_LIST_QTT
} TRACE_PART;                 /* trace part to be shown */

typedef enum {
   SEARCH_ERROR_OK,
   SEARCH_ERROR_ALLOC,
   SEARCH_ERROR_SYNTAX,
   SEARCH_ERROR_RUN,
   SEARCH_ERROR_LIST_QTT
} SEARCH_ERROR;               /* search error */

typedef enum {
   LOCK_ERROR_OK,             /* lock operation succeed */
   LOCK_ERROR_DE_DENIED,      /* data entry lock refused */
   LOCK_ERROR_R_DENIED,       /* record lock refused */
   LOCK_ERROR_U_DENIED,       /* record identification update refused */
   LOCK_ERROR_NOT_LOCKED,     /* record not locked */
   LOCK_ERROR_ID_DENIED,      /* lock identification does not match */
   LOCK_ERROR_IU_DENIED,      /* record inverted update refused */
   LOCK_ERROR_NOT_EXPIRED,    /* elapse time was not reached */
   LOCK_ERROR_EW_DENIED,      /* exclusive write lock refused */
   LOCK_ERROR_LIST_QTT        /* quantity of lock return codes */
} LOCK_ERROR;                 /* lock error */
/* --------------------------------------------------------------- structure */
typedef struct {
   FMT_CODE *code;
   CIXML_STRUCT *ciXml;
   char *text;
} EXE_FORMAT;                 /* format */

typedef struct {
   int tag;
   BOOLEAN apply;
   char *text;
   LONGX value;
} SCOPE_DEFINE;               /* scope define */

typedef struct {
   CPL_COMMAND *father;
   LONGX idxPrev;
   LONGX idxRetn;
   LONGX idxCurr;
   char *parmList[PARM_LIST_QTT];
   SCOPE_DEFINE defineList[PRECISE_LIST_QTT];
   PRECISE_LIST skip;
   CPL_COMMAND *jump;
   FILE *export;
   PRECISE_LIST exportType;
   char *stwTable;
} SCOPE_VAR;                  /* scope variables */

typedef struct {
   CPL_STRUCT *program;
   CGI_PARAM *cgiList;
   CPL_ATVALUE trace;
   int scope;
   FREQLIST_VAR freq;
   char *buff;
   FILE *output;
   LONGX buffSize;
} EXE_VAR;                    /* execution variables */

typedef struct {
   LONGX idxHit;
   LONGX idxPfx;
   char tempFile[CI_TMPNAM];
   char *dbLog;
    b7_CIB7 *cib7p;
} SEARCH_VAR;                 /* search variables */
/* ---------------------------------------------------------------- external */
extern char fmterrxy[];                                     /* cifm3.c */
#if !CIB71
int b5_deltmp(int fd, char *tmpnamp);                       /* cib62.c */
#endif /* CIB71 */
extern CPL_IDENTIFIER cplElement[ELEMENT_LIST_QTT];         /* xis_comp.c */
extern CPL_IDENTIFIER cplAttribute[ATTRIBUTE_LIST_QTT];     /* xis_comp.c */
extern CPL_IDENTIFIER cplAtValue[ATVALUE_LIST_QTT];         /* xis_comp.c */
/* --------------------------------------------------------------- recursive */
char *exeText(EXE_VAR *exeVar,SCOPE_VAR *scopeVar,LONGX idxCurr,CPL_COMMAND *cmd);
void  exeGo  (EXE_VAR *exeVar,SCOPE_VAR *scopeVar,CPL_COMMAND *cmd);
/* ------------------------------------------------------------------ global */
char *preciseList[PRECISE_LIST_QTT];   /* precise defines and values list */
PARM_LIST atValueParm[ATVALUE_LIST_QTT];  /* attribute value relate parm value */
LONGX b5msg_idx = -1L;                  /* auxiliary search message index */
int b5msg_tag = 0;                     /* auxiliary search message tag */
unsigned char exeUctab[256] = {        /* upper case character table (ANSI) */
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
    16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
    32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
    48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
    64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
    80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
    96, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
    80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90,123,124,125,126,127,
   128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,
   144, 39, 39,147,148,149,150,151,152,153,154,155,156,157,158,159,
   160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,
   176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,
    65, 65, 65, 65, 65, 65, 65, 67, 69, 69, 69, 69, 73, 73, 73, 73,
   208, 78, 79, 79, 79, 79, 79,215, 48, 85, 85, 85, 85, 89,222,223,
    65, 65, 65, 65, 65, 65,230, 67, 69, 69, 69, 69, 73, 73, 73, 73,
   240, 78, 79, 79, 79, 79, 79,247, 48, 85, 85, 85, 85, 89,254, 89
};
unsigned char exeActab[] = {           /* alphabetic character table (ANSI) */
    48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 65, 66, 67, 68, 69, 70,
    71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86,
    87, 88, 89, 90, 97, 98, 99,100,101,102,103,104,105,106,107,108,
   109,110,111,112,113,114,115,116,117,118,119,120,121,122,192,193,
   194,195,196,197,199,200,201,202,203,204,205,206,207,209,210,211,
   212,213,214,216,217,218,219,220,221,224,225,226,227,228,229,231,
   232,233,234,235,236,237,238,239,241,242,243,244,245,246,248,249,
   250,251,252,253,255,  0
};

/* ================================================================ exeError */
EXE_ERROR exeError(CPL_STRUCT *program,      /* program structure */
                   EXE_ERROR errorCode,      /* error code */
                   char *text)               /* complement message text */
{

   /* Set error code and complement text information */
   program->error.code = errorCode;
   if (text) sprintf(program->error.info,"%-.*s",EFC_ERROR_INFO_MAX,text);

   /* Return error code */
   return errorCode;

} /* exeError */

/* ================================================================== exeNew */
void *exeNew(EXE_VAR *exeVar,       /* execution variables */
             LONGX memSize)          /* buffer size, in bytes */
{
   void *newMem;     /* New memory pointer */

   /* Allocate required memory space, check error */
   newMem = efc_new(memSize);
   if (!newMem) exeError(exeVar->program,EXE_ERROR_ALLOC,"exeNew");

   /* Return allocated pointer */
   return newMem;

} /* exeNew */

/* ============================================================ exeNewRecord */
LONGX exeNewRecord(EXE_VAR *exeVar,     /* execution variables */
                  LONGX memSize)        /* buffer size, in bytes */
{
   LONGX idx;      /* new record index */

   /* Find and allocate new record index */
   idx = eci_rec_new(&(exeVar->program->error),memSize);

   if (idx < 0L) exeError(exeVar->program,EXE_ERROR_MAXNREC,NULL);

   /* Return index, negative number indicates an error */
   return idx;

} /* exeNewRecord */

/* =========================================================== exeWorkRecord */
LONGX exeWorkRecord(EXE_VAR *var) /* execution variables */
{
   LONGX idx;      /* new record index */

   /* Find, allocate and activate new record index */
   idx = exeNewRecord(var,EXE_REC_SIZE);
   if (idx >= 0L) eci_rec_active(idx);

   /* Return index, negative number indicates an error */
   return idx;

} /* exeWorkRecord */

/* =============================================================== exeNumber */
LONGX exeNumber(EXE_VAR *exeVar,           /* execution variables */
               SCOPE_VAR *scopeVar,       /* scope variables */
               PARM_LIST i,               /* identifier index */
               LONGX min,                  /* minimum possible value */
               LONGX max,                  /* maximum possible value */
               LONGX defaultValue)         /* default value */
{
   LONGX val;      /* return value */

   /* Set the default value, get parameter (if present) */
   val = defaultValue;
   if (scopeVar->parmList[i])
      if (*(scopeVar->parmList[i]))
         val = atol(scopeVar->parmList[i]);

   /* Check if value is inside the given range */
   if (val < min || val > max)
      exeError(exeVar->program,EXE_ERROR_INVALID,scopeVar->parmList[i]);

   /* Return value */
   return val;

} /* exeNumber */

/* ============================================================ exeHighLight */
EXE_ERROR exeHighLight(EXE_VAR *exeVar,         /* execution variables */
                       SCOPE_VAR *scopeVar,     /* scope variables */
                       LONGX idx,                /* current record index */
                       int tag,                 /* field number */
                       char *change)            /* change text */
{
   EFC_SPLIT_LINES table;                 /* split line structure */
   LONGX line;                             /* line index */
   char *buff;                            /* auxiliary string buffer pointer */
   char *p;                               /* auxiliary string buffer pointer */
   EXE_ERROR errorCode = EXE_ERROR_OK;    /* error code */

   /* Check mandatory parameters */
   if (!scopeVar->parmList[PARM_PREFIX])
      return exeError(exeVar->program,EXE_ERROR_MISSING,cplAtValue[ATVALUE_PREFIX].text);
   if (!scopeVar->parmList[PARM_SUFFIX])
      return exeError(exeVar->program,EXE_ERROR_MISSING,cplAtValue[ATVALUE_SUFFIX].text);

   /* If no keys, no changed necessary */
   if (!scopeVar->parmList[PARM_KEYS]) {
      if (eci_field_AT(&(exeVar->program->error),idx,tag,change))
         return exeError(exeVar->program,EXE_ERROR_FIELD,NULL);
      return errorCode;
   }
   if (!*scopeVar->parmList[PARM_KEYS]) {
      if (eci_field_AT(&(exeVar->program->error),idx,tag,change))
         return exeError(exeVar->program,EXE_ERROR_FIELD,NULL);
      return errorCode;
   }

   /* Split keys */
   p = strdup(scopeVar->parmList[PARM_KEYS]);
   if (!p) return exeError(exeVar->program,EXE_ERROR_ALLOC,scopeVar->parmList[PARM_KEYS]);
   if (efc_split_lines(&table,p) < 0L)
      errorCode = exeError(exeVar->program,EXE_ERROR_ALLOC,p);
   else {

      /* Copy change buffer */
      buff = strdup(change);
      if (!buff) errorCode = exeError(exeVar->program,EXE_ERROR_ALLOC,change);
      else {

         /* Change all keys */
         for (line = 0L; line < table.qtt; line++)
            if (*table.list[line].text) {
               hl_put(buff,table.list[line].text,scopeVar->parmList[PARM_PREFIX],scopeVar->parmList[PARM_SUFFIX],exeVar->buff);
               buff = efc_strrepl(buff,exeVar->buff);
            }

         /* Field update changed text */
         errorCode = eci_field_AT(&(exeVar->program->error),idx,tag,buff);
      }
   }

   /* Garbage collector */
   efc_free(buff);
   efc_free(p);
   efc_split_lines_free(&table);

   /* Return no error */
   return errorCode;

} /* exeHighLight */

/* ======================================================== exeFieldSplitOcc */
EXE_ERROR exeFieldSplitOcc(EXE_VAR *exeVar,        /* execution variables */
                           SCOPE_VAR *scopeVar,    /* scope variables */
                           LONGX idx,               /* record index */
                           CPL_ATVALUE atValue,    /* field option value */
                           char *text,             /* field split buffer */
                           char charSep,           /* separator character */
                           int tag)                /* update field */
{
   EFC_SPLIT_LINES table;                 /* split line structure */
   LONGX line;                             /* line index */
   EXE_ERROR errorCode = EXE_ERROR_OK;    /* error code */

   /* Split lines */
   if (!*text) return EXE_ERROR_OK;
   if (efc_split_lines_char(&table,text,charSep) < 0L)
      return exeError(exeVar->program,EXE_ERROR_ALLOC,text);

   /* Loop all lines */
   for (line = 0L; line < table.qtt; line++) {

      /* Check error code */
      if (errorCode) break;

      /* Add field */
      if (atValue != ATVALUE_HL) {
         if (eci_field_AT(&(exeVar->program->error),idx,tag,table.list[line].text))
            errorCode = exeError(exeVar->program,EXE_ERROR_FIELD,NULL);
      } else
         errorCode = exeHighLight(exeVar,scopeVar,idx,tag,table.list[line].text);

   } /* for */

   /* Garbage collector */
   efc_split_lines_free(&table);

   /* Return error code */
   return errorCode;

} /* exeFieldSplitOcc */

/* ======================================================== exeStatusControl */
EXE_ERROR exeStatusControl(EXE_VAR *exeVar,        /* execution variables */
                           char *db)               /* data base name */
{
   LONGX idxControl;        /* control record index */

   /* Get control record */
   idxControl = exeNewRecord(exeVar,sizeof(M0STRU));
   if (idxControl < 0L) return exeVar->program->error.code;

   /* Read control record */
   record(idxControl,db,0L);

   /* Set control status string */
   sprintf(exeVar->buff+strlen(exeVar->buff),"^n%"_LD_"^d%"_LD_"^e%"_LD_,
      VMF0nxtmfn(idxControl),VMF0mfcxx2(idxControl),VMF0mfcxx3(idxControl));

   /* Garbage collector */
   eci_rec_free(idxControl);

   /* Return no error */
   return EXE_ERROR_OK;

} /* exeStatusControl */

/* ============================================================ exeFileExist */
BOOLEAN exeFileExist(char *db,            /* data base name */
                     char *extension)     /* file extension */
{
   int handle;       /* auxiliary file handle */

   /* Open file */
   dbxopt_fatal = 0;
   handle = dbxopen(NULL,db,extension);
   if (handle) CLOSE(handle);

   /* Return TRUE if the file exist or FALSE if not */
   return (BOOLEAN)(handle > 0);

} /* exeFileExist */

/* ============================================================== exeExistDB */
EXE_ERROR exeExistDB(EXE_VAR *exeVar,     /* execution variables */
                     LONGX idx,            /* current record index */
                     int tag,             /* update field number */
                     char *buff)          /* Return temporary file name buffer */
{
   EXE_ERROR errorCode = EXE_ERROR_OK;       /* error code */

   /* Set database exist text */
   strcpy(exeVar->buff,"^s");
   if (exeFileExist(buff,".xrf")) strcat(exeVar->buff,"m");
   if (exeFileExist(buff,".cnt")) strcat(exeVar->buff,"i");

   /* If master exist, check control record */
   if (strchr(exeVar->buff,'m')) errorCode = exeStatusControl(exeVar,buff);

   /* Field update exist text information */
   if (!errorCode)
      if (eci_field_MT(&(exeVar->program->error),idx,tag,exeVar->buff))
         errorCode = exeError(exeVar->program,EXE_ERROR_FIELD,NULL);

   /* Return error code */
   return errorCode;

} /* exeExistDB */

/* ========================================================= exeRequiredParm */
char *exeRequiredParm(EXE_VAR *exeVar,          /* execution variables */
                      SCOPE_VAR *scopeVar,      /* scope variables */
                      CPL_ATVALUE i)               /* parameter index */
{
   PARM_LIST parmIndex;    /* parmeter list index */

   /* Get parameter related index */
   parmIndex = atValueParm[i];

   /* If present: return parameter, if absent: set error, return NULL */
   if (scopeVar->parmList[parmIndex])
      if (*(scopeVar->parmList[parmIndex]))
         return scopeVar->parmList[parmIndex];
   exeError(exeVar->program,EXE_ERROR_MISSING,cplAtValue[i].text);
   return NULL;

} /* exeRequiredParm */

/* ======================================================= exeRequiredDefine */
BOOLEAN exeRequiredDefine(EXE_VAR *exeVar,         /* execution variables */
                          SCOPE_VAR *scopeVar,     /* scope variables */
                          PRECISE_LIST i)          /* parameter index */
{
   /* If present: return define, if absent: set error, return NULL */
   if (scopeVar->defineList[i].tag) return TRUE;
   exeError(exeVar->program,EXE_ERROR_MISSING,preciseList[i]);
   return FALSE;

} /* exeRequiredDefine */

/* ============================================================== exePrecise */
PRECISE_LIST exePrecise(char *text)       /* find text */
{
   PRECISE_LIST i;      /* list index */

   /* Check no text */
   if (!text) return PRECISE_LIST_QTT;

   /* Find list index */
   for (i = 0; i < PRECISE_LIST_QTT; i++)
      if (strcmp(text,preciseList[i]) == 0) break;

   /* Return index found */
   return i;

} /* exePrecise */

/* ============================================================= exeFindParm */
CPL_COMMAND *exeFindParm(CPL_COMMAND *cmd,         /* start command */
                         CPL_ATVALUE atValue)      /* name attribute index */
{

   /* loop commands backward to find parameter */
   for ( ; cmd; cmd = cmd->prev) {
      if (cmd->element != ELEMENT_PARM) continue;
      if (cmd->attributeList[ATTRIBUTE_NAME].atValue == atValue) return cmd;
   } /* for */

   /* Return not found: NULL */
   return NULL;

} /* exeFindParm */

/* ================================================================= exeJump */
CPL_COMMAND *exeJump(EXE_VAR *exeVar,        /* execution variables */
                     CPL_COMMAND *cmd,       /* current command */
                     char *target)           /* target string buffer pointer */
{
   CPL_COMMAND *find;         /* auxiliary command pointer */

   /* Find label backward, if found return command label */
   for (find = cmd->prev; find; find = find->prev) {
      if (find->element != ELEMENT_LABEL) continue;
      if (strcmp(target,find->text) == 0) return find;
   } /* for */

   /* Find label forward, if found return command label */
   for (find = cmd; find; find = find->next) {
      if (find->element != ELEMENT_LABEL) continue;
      if (strcmp(target,find->text) == 0) return find;
   } /* for */

   /* Return label not found (same scope) */
   /* [4.0] The caller must test it. Avoid recursive call error set
   exeError(exeVar->program,EXE_ERROR_TARGET,target);
   [4.0] */
   return NULL;

} /* exeJump */

/* ================================================================= exeFlow */
EXE_ERROR exeFlow(EXE_VAR *exeVar,           /* execution variables */
                  SCOPE_VAR *scopeVar,       /* scope variables */
                  CPL_COMMAND *cmd,          /* current command */
                  char *text)                /* command text */
{

   /* Evaluate action attribute */
   switch (cmd->attributeList[ATTRIBUTE_ACTION].atValue) {

   case ATVALUE_EXIT:
      /* Script exit */
      exeError(exeVar->program,EXE_ERROR_EXIT,text);
      break;

   case ATVALUE_JUMP:
      /* Instruction jump */
      scopeVar->jump = exeJump(exeVar,cmd,text);

        /* [4.0] Return label not found (same scope) */
      if (!scopeVar->jump) {
        exeError(exeVar->program,EXE_ERROR_TARGET,text);
        }

      break;

   case ATVALUE_SKIP:
      /* Group skip */
      scopeVar->skip = exePrecise(text);
      if (scopeVar->skip != PRECISE_NEXT && scopeVar->skip != PRECISE_QUIT)
         return exeError(exeVar->program,EXE_ERROR_INVALID,text);
      break;

   default:
      /* Invalid action attribute */
      return exeError(exeVar->program,EXE_ERROR_UNDEFINED,cmd->attributeList[ATTRIBUTE_ACTION].text);

   } /* switch */

   /* Return no error */
   return EXE_ERROR_OK;

} /* exeFlow */

/* ========================================================= exeCgiDelSubfld */
EXE_ERROR exeCgiDelSubfld(EXE_VAR *exeVar,   /* execution variables */
                          LONGX idx,          /* current record index */
                          char *subfld)      /* subfield buffer */
{
   CGI_PARAM *cgip;  /* cgi list item */
   char *p;          /* auxiliary string buffer pointer */
   int tag;          /* update field */

   /* Loop all cgi couples */
   for (cgip = exeVar->cgiList; cgip; cgip = cgip->next) {

      /* Find subfld, get corresponding tag */
      p = strstr(cgip->name,subfld);
      if (!p) continue;
      tag = atoi(p+2);

      /* Field delete tag */
      if (eci_field_D(&(exeVar->program->error),idx,tag))
         return exeError(exeVar->program,EXE_ERROR_FIELD,p);

   } /* for */

   /* Return no error */
   return EXE_ERROR_OK;

} /* exeCgiDelSubfld */

/* ============================================================ exeCgiSubfld */
EXE_ERROR exeCgiSubfld(EXE_VAR *exeVar,      /* execution variables */
                       LONGX idx,             /* current record index */
                       char *subfld)         /* subfield buffer */
{
   CGI_PARAM *cgip;  /* cgi list item */
   char *p;          /* auxiliary string buffer pointer */
   int tag;          /* update field */
   char charSep;     /* repeatable field separator character */
   char *buff;       /* auxiliary buffer */
   EXE_ERROR errorCode = EXE_ERROR_OK; /* error code */

   /* Loop all cgi couples */
   for (cgip = exeVar->cgiList; cgip; cgip = cgip->next) {

      /* Find subfld, get corresponding tag */
      p = strstr(cgip->name,subfld);
      if (!p) continue;
      tag = atoi(p+2);

      /* Field update cgi value */
      p = strstr(cgip->name,"^r");
      if (p) {
         charSep = (char)atoi(p+2);
         buff = strdup(cgip->value);
         if (!buff)
            return exeError(exeVar->program,EXE_ERROR_ALLOC,cgip->value);
         errorCode = exeFieldSplitOcc(exeVar,NULL,idx,ATVALUE_ADD,buff,charSep,tag);
         efc_free(buff);
         if (errorCode) break;
      } else
         if (*cgip->value)
            if (eci_field_AT(&(exeVar->program->error),idx,tag,cgip->value))
               return exeError(exeVar->program,EXE_ERROR_FIELD,cgip->value);

   } /* for */

   /* Return no error */
   return errorCode;

} /* exeCgiSubfld */

/* ============================================================ exeCgiPrefix */
EXE_ERROR exeCgiPrefix(EXE_VAR *exeVar,      /* execution variables */
                       LONGX idx,             /* current record index */
                       char *prefix)         /* subfield buffer */
{
   CGI_PARAM *cgip;     /* cgi list item */
   int pfxLength;       /* prefix length */
   int tag;             /* update field */

   /* Set prefix length, loop all copules */
   pfxLength = strlen(prefix);
   for (cgip = exeVar->cgiList; cgip; cgip = cgip->next) {

      /* Find prefix, get corresponding tag */
      if (strncmp(cgip->name,prefix,pfxLength) != 0) continue;
      tag = atoi(cgip->name+pfxLength);

      /* Field add cgi value */
      if (eci_field_AT(&(exeVar->program->error),idx,tag,cgip->value))
         return exeError(exeVar->program,EXE_ERROR_FIELD,cgip->value);

   } /* for */

   /* Return no error */
   return EXE_ERROR_OK;

} /* exeCgiPrefix */

/* ============================================================== exeCgiScan */
EXE_ERROR exeCgiScan(EXE_VAR *exeVar,        /* execution variables */
                     int tag,                /* update field */
                     CPL_ATVALUE tagValue,   /* alternate cgi option */
                     BOOLEAN valueOn,        /* value behavior */
                     char *cgiName,          /* subfield buffer */
                     LONGX idx)               /* current record index */
{
   CGI_PARAM *cgip;     /* cgi list item */

   /* Load all cgi couples */
   if (strcmp(cgiName,"^n^v") == 0)
      for (cgip = exeVar->cgiList; cgip; cgip = cgip->next) {
         sprintf(exeVar->buff,"^n%s^v%s",cgip->name,cgip->value);
         if (eci_field_AT(&(exeVar->program->error),idx,tag,exeVar->buff))
            return exeError(exeVar->program,EXE_ERROR_FIELD,exeVar->buff);
      } /* for */

   /* Load cgi couple named with ^t, ^u and ^w and prefixed */
   if (tag == 0) {
      if (tagValue == ATVALUE_CGISFT)
         if (exeCgiSubfld(exeVar,idx,"^t")) return EXE_ERROR_FIELD;
      if (tagValue == ATVALUE_CGISFW)
         if (exeCgiSubfld(exeVar,idx,"^w")) return EXE_ERROR_FIELD;
      if (tagValue == ATVALUE_CGISFU) {
         if (exeCgiDelSubfld(exeVar,idx,"^u")) return EXE_ERROR_FIELD;
         if (exeCgiSubfld(exeVar,idx,"^u")) return EXE_ERROR_FIELD;
      }
      if (tagValue == ATVALUE_PREFIX)
         if (exeCgiPrefix(exeVar,idx,cgiName)) return EXE_ERROR_FIELD;
   }

   /* Find cgi name, field update corresponding value */
   for (cgip = exeVar->cgiList; cgip; cgip = cgip->next) {
      if (strcmp(cgiName,cgip->name) != 0) continue;
      if (!cgip->value) continue;
      if (*cgip->value) {
         if (eci_field_AT(&(exeVar->program->error),idx,tag,cgip->value))
            return exeError(exeVar->program,EXE_ERROR_FIELD,cgip->value);
         continue;
      }
      if (valueOn)
         if (eci_field_AT(&(exeVar->program->error),idx,tag,preciseList[PRECISE_ON]))
            return exeError(exeVar->program,EXE_ERROR_FIELD,preciseList[PRECISE_ON]);
   } /* for */

   /* Return no error */
   return EXE_ERROR_OK;

} /* exeCgiScan */

/* ============================================================ exeTableInfo */
char *exeTableInfo(char *text,      /* text to be scanned */
                   int *number)     /* number data */
{

   /* Get number, skip digit characters */
   *number = atoi(text);
   while (isdigit(*text)) text++;

   /* Return string pointer after separator character */
   if (*text) text++;
   return text;

} /* exeTableInfo */

/* ============================================================= exeCgiTable */
EXE_ERROR exeCgiTable(EXE_VAR *exeVar,          /* execution variables */
                      LONGX idx,                 /* current record index */
                      char *text)               /* cgi table string buffer pointer */
{
   EFC_SPLIT_LINES table;                 /* split line structure */
   LONGX line;                             /* line index */
   int tag;                               /* update field */
   CPL_ATVALUE tagValue;                  /* alternate cgi option */
   BOOLEAN valueOn = FALSE;               /* value behavior */
   EXE_ERROR errorCode = EXE_ERROR_OK;    /* error code */

   /* Split lines */
   if (efc_split_lines(&table,text) < 0L)
      return exeError(exeVar->program,EXE_ERROR_ALLOC,text);

   /* Loop all lines */
   for (line = 0L; line < table.qtt; line++) {

      /* Get table information: field number and content pointer */
      text = exeTableInfo(table.list[line].text,&tag);

      /* Empty value acts like "ON" */
      if (tag == 0) {
         if (strcmp(text,preciseList[PRECISE_ON]) == 0) valueOn = TRUE;
         if (strcmp(text,preciseList[PRECISE_OFF]) == 0) valueOn = FALSE;
      }

      /* Load cgi data */
      tagValue = cplIdentifier(cplAtValue,ATVALUE_LIST_QTT,text);
      /* [0.9] Begin */
      if (tag == 0 && tagValue == ATVALUE_LIST_QTT)
         tagValue = ATVALUE_PREFIX;
      /* [0.9] End */
      errorCode = exeCgiScan(exeVar,tag,tagValue,valueOn,text,idx);
      if (errorCode) break;

   } /* for */

   /* Garbage collector */
   efc_split_lines_free(&table);

   /* Return error code */
   return errorCode;

} /* exeCgiTable */

/* ========================================================== exeFieldDefine */
EXE_ERROR exeFieldDefine(EXE_VAR *exeVar,             /* execution variables */
                         SCOPE_DEFINE defineList[],   /* define field list */
                         int tag,                     /* field number */
                         char *text)                  /* item */
{
   PRECISE_LIST i;      /* auxiliary flow list index */

   /* Find flow option, check invalid option */
   i = exePrecise(text);
   if (i == PRECISE_LIST_QTT)
      return exeError(exeVar->program,EXE_ERROR_INVALID,text);

   /* set field number and apply flag */
   defineList[i].tag = tag;
   defineList[i].apply = TRUE;

   /* Return no error */
   return EXE_ERROR_OK;

} /* exeFieldDefine */

/* =============================================================== exeDefine */
EXE_ERROR exeDefine(EXE_VAR *exeVar,            /* execution variables */
                    SCOPE_VAR *scopeVar,     /* scope variables */
                    char *text)              /* field define list */
{
   EFC_SPLIT_LINES table;                 /* split line structure */
   LONGX line;                             /* line index */
   int tag;                               /* update field */
   EXE_ERROR errorCode = EXE_ERROR_OK;    /* error code */

   /* Split lines */
   if (efc_split_lines(&table,text) < 0L)
      return exeError(exeVar->program,EXE_ERROR_ALLOC,text);

   /* Loop all lines */
   for (line = 0L; line < table.qtt; line++) {

      /* Get field number and text information */
      text = exeTableInfo(table.list[line].text,&tag);

      /* Set table item */
      errorCode = exeFieldDefine(exeVar,scopeVar->defineList,tag,text);
      if (errorCode) break;

   } /* for */

   /* Garbage collector */
   efc_split_lines_free(&table);

   /* Return no error */
   return errorCode;

} /* exeDefine */

/* ============================================================= exeFieldDir */
EXE_ERROR exeFieldDir(EXE_VAR *exeVar,    /* execution variables */
                      CMD_ATTRIBUTE_STRU *attributeType, /* type attribute */
                      LONGX idx,           /* record index */
                      int tag)            /* update field */
{
   char *p;       /* auxiliary string buffer pointer */
   int fldDir;    /* field directory entry */
   int fldQtt;    /* quantity of fields */

   /* Put all fields in the buffer */
   fldQtt = VMFRnvf(idx);
   for (p = exeVar->buff,fldDir = 0; fldDir < fldQtt; fldDir++) {
      sprintf(p,"%05d",VDIRtag(idx,fldDir)); p += 5;
      if (attributeType->atValue != ATVALUE_LIST) {
         *p++ = ' ';
         memmove(p,VFIELDP(idx,fldDir),VDIRlen(idx,fldDir)); p += VDIRlen(idx,fldDir);
      }
      *p++ = '\n';
   } /* for */
   *p = '\0';

   /* Field update the buffer */
   if (attributeType->atValue == ATVALUE_DELETE) {
      eci_field_D(&(exeVar->program->error),idx,0);
   }
   if (eci_field_AT(&(exeVar->program->error),idx,tag,exeVar->buff))
      return exeError(exeVar->program,EXE_ERROR_FIELD,NULL);

   /* Return no error */
   return EXE_ERROR_OK;

} /* exeFieldDir */

/* =========================================================== exeFieldSplit */
EXE_ERROR exeFieldSplit(EXE_VAR *exeVar,        /* execution variables */
                        SCOPE_VAR *scopeVar,    /* scope variables */
                        LONGX idx,               /* record index */
                        CPL_ATVALUE atValue,    /* field option value */
                        CMD_ATTRIBUTE_STRU *attributeSplit, /* field split attribute */
                        CMD_ATTRIBUTE_STRU *attributeType,  /* type attribute */
                        char *text,             /* field split buffer */
                        int tag)                /* update field */
{

   /* Evaluate split options */
   switch (attributeSplit->atValue) {

   case ATVALUE_OCC:
      /* Field update each line into one new occurrences */
      return exeFieldSplitOcc(exeVar,scopeVar,idx,atValue,text,'\n',tag);

   case ATVALUE_FLDDIR:
      /* Field update all the fields into one field */
      return exeFieldDir(exeVar,attributeType,idx,tag);

   default:
      return exeError(exeVar->program,EXE_ERROR_UNDEFINED,attributeSplit->text);

   } /* switch */

} /* exeFieldSplit */

/* ========================================================== exeFieldTransf */
EXE_ERROR exeFieldTransf(EXE_VAR *exeVar,          /* execution variables */
                         LONGX idxSource,           /* source record index */
                         LONGX idxTarget,           /* target record index */
                         CPL_ATVALUE actionValue,  /* action attribute value */
                         CPL_ATVALUE previousValue,/* previous attribute value */
                         char *fieldList)          /* transfer field list */
{
   char *p;                   /* auxiliary string bufer pointer */
   int tag = 0;               /* scaned tag */
   int tagFrom = 0;           /* range first tag */
   BOOLEAN keep = FALSE;      /* keep source tag */
   int tagSource = 0;         /* scanned source tag */
   BOOLEAN delFlag = FALSE;   /* delete flag, used for eci_field_copy */

   /* Previous copy flag: add or replace */
   if (actionValue == ATVALUE_REPLACE) delFlag = TRUE;
   else
      if (actionValue == ATVALUE_IMPORT || actionValue == ATVALUE_EXPORT)
            if (previousValue != ATVALUE_ADD) delFlag = TRUE;

   /* Loop all string buffer */
   for (p = fieldList; *p; p++) {

      /* Skip non digit character, check range specification, check change specification */
      if (!isdigit(*p)) {
         if (*p == '/') tagFrom = tag;
         if (*p == '[') keep = TRUE;
         continue;
      }

      /* Get tag number */
      tag = atoi(p);

      /* Get change source tag */
      if (keep) {
         tagSource = tag;
         keep = FALSE;
         while (isdigit(*(p+1))) p++;
         continue;
      }

      /* Range of tags transfer */
      if (tagFrom) {
         if (tagFrom > tag)
            return exeError(exeVar->program,EXE_ERROR_FIELD,p);
         while (++tagFrom < tag)
            if (actionValue == ATVALUE_DELETE) {
               if (eci_field_D(&(exeVar->program->error),idxTarget,tagFrom))
                  return exeError(exeVar->program,EXE_ERROR_FIELD,p);
            } else
               if (eci_field_copy(&(exeVar->program->error),delFlag,idxTarget,tagFrom,idxSource,tagFrom))
                  return exeError(exeVar->program,EXE_ERROR_FIELD,p);
         tagFrom = 0;
      }

      /* Single transfer */
      if (!tagSource) tagSource = tag;
      if (actionValue == ATVALUE_DELETE) {
         if (eci_field_D(&(exeVar->program->error),idxTarget,tag))
            return exeError(exeVar->program,EXE_ERROR_FIELD,p);
      } else
         if (eci_field_copy(&(exeVar->program->error),delFlag,idxTarget,tag,idxSource,tagSource))
            return exeError(exeVar->program,EXE_ERROR_FIELD,p);
      tagSource = 0;

      /* Skip digit character */
      while (isdigit(*(p+1))) p++;

   } /* for */

   /* Return no error */
   return EXE_ERROR_OK;

} /* exeFieldTransf */

/* ========================================================== exeDefineField */
ECI_ERROR exeDefineField(EXE_VAR *exeVar,             /* execution variables */
                         LONGX idx,                    /* current record index */
                         SCOPE_DEFINE *defineItem)    /* define item */
{

   /* If field number previously set: modify it */
   if (defineItem->tag <= 0) return ECI_ERROR_OK;
   if (defineItem->text)
      return eci_field_MT(&(exeVar->program->error),idx,defineItem->tag,defineItem->text);
   return eci_field_MN(&(exeVar->program->error),idx,defineItem->tag,defineItem->value);

} /* exeDefineField */

/* ============================================================== exeDefines */
EXE_ERROR exeDefines(EXE_VAR *exeVar,           /* execution variables */
                     SCOPE_VAR *scopeVar,       /* scope variables */
                     LONGX idx)                  /* current record index */
{
   PRECISE_LIST i;            /* auxiliary flow option index */
   ECI_ERROR errorCode;       /* error code */

   /* Loop all defines, check apply flag */
   for (i = 0; i < PRECISE_LIST_QTT; i++) {
      if (!scopeVar->defineList[i].apply) continue;
      errorCode = exeDefineField(exeVar,idx,&(scopeVar->defineList[i]));
      if (errorCode)
         return exeError(exeVar->program,EXE_ERROR_FIELD,scopeVar->defineList[i].text);
   }

   /* Return no error */
   return EXE_ERROR_OK;

} /* exeDefines */

/* ============================================================= exeFieldOcc */
EXE_ERROR exeFieldOcc(EXE_VAR *exeVar,          /* execution variables */
                      SCOPE_VAR *scopeVar,      /* scope variables */
                      int tagFrom,              /* from field number */
                      int tagTo,                /* to field number */
                      char *text)               /* field define list */
{
   int occ;       /* from occurrence */

   /* Check occurrence */
   occ = atoi(text);
   if (occ <= 0)
      return exeError(exeVar->program,EXE_ERROR_INVALID,text);

   /* Delete previous content */
   if (eci_field_D(&(exeVar->program->error),scopeVar->idxCurr,tagTo))
      return exeError(exeVar->program,EXE_ERROR_FIELD,NULL);

   /* Get field occurence, field replace */
   if (eci_field(scopeVar->idxCurr,tagFrom,occ,exeVar->buff))
      if (eci_field_AT(&(exeVar->program->error),scopeVar->idxCurr,tagTo,exeVar->buff))
         return exeError(exeVar->program,EXE_ERROR_FIELD,NULL);

   /* Return no error */
   return EXE_ERROR_OK;

} /* exeFieldOcc */

/* ================================================================ exeField */
EXE_ERROR exeField(EXE_VAR *exeVar,          /* execution variables */
                   SCOPE_VAR *scopeVar,      /* scope variables */
                   LONGX idxSource,           /* source record index */
                   LONGX idxTarget,           /* target record index */
                   CPL_COMMAND *cmd,         /* current command */
                   char *text)               /* command text */
{
   CPL_ATVALUE actionValue;               /* action attribute value */
   int tag;                               /* field number */
   BOOLEAN tagList;                       /* tag list flag */
   BOOLEAN tagFirstLine;                  /* first command text line defines the tag number */
   CPL_ATVALUE splitValue;                /* split attribute value */
   int occ;                               /* field occurrence */
   CPL_ATVALUE previousValue;             /* previous action attribute add/replace */
   int parmTag;                           /* parameter field number */
   int tagFrom;                           /* from field number */

   /* Get tag attribute */
   actionValue = cmd->attributeList[ATTRIBUTE_ACTION].atValue;
   if (!actionValue)
      return exeError(exeVar->program,EXE_ERROR_MISSING,cplAttribute[ATTRIBUTE_ACTION].text);
   if (!cmd->attributeList[ATTRIBUTE_TAG].text)
      return exeError(exeVar->program,EXE_ERROR_MISSING,cplAttribute[ATTRIBUTE_TAG].text);
   tag = atoi(cmd->attributeList[ATTRIBUTE_TAG].text);
   if (cmd->attributeList[ATTRIBUTE_TAG].atValue == ATVALUE_LIST_QTT && tag <= 0)
      return exeError(exeVar->program,EXE_ERROR_INVALID,cmd->attributeList[ATTRIBUTE_TAG].text);
   tagList = (BOOLEAN)(cmd->attributeList[ATTRIBUTE_TAG].atValue == ATVALUE_LIST);

   /* 28.Nov.2000 */
   tagFirstLine = (BOOLEAN)(cmd->attributeList[ATTRIBUTE_TAG].atValue == ATVALUE_FIRST_LINE);
   if (tagFirstLine)
   {
      tag = atoi(text);
      if (tag <= 0)
      {
         return exeError(exeVar->program,EXE_ERROR_INVALID,text);
      }
      while (*text)
      {
         if (*text++ == '\n')
         {
            break;
         }
      }
   }

   splitValue = cmd->attributeList[ATTRIBUTE_SPLIT].atValue;
   previousValue = cmd->attributeList[ATTRIBUTE_PREVIOUS].atValue;

   /* Evaluate field action */
   switch (actionValue) {

   case ATVALUE_HL :
   case ATVALUE_REPLACE :
      /* Delete previous field content */
      if (!tagList)
         if (eci_field_D(&(exeVar->program->error),idxTarget,tag))
            return exeError(exeVar->program,EXE_ERROR_FIELD,text);

   case ATVALUE_ADD :
      /* Transfer field list */
      if (tagList)
         return exeFieldTransf(exeVar,idxSource,idxTarget,actionValue,previousValue,text);

      /* Add splited field */
      if (splitValue)
         return exeFieldSplit(exeVar,scopeVar,idxTarget,actionValue,&(cmd->attributeList[ATTRIBUTE_SPLIT]),&(cmd->attributeList[ATTRIBUTE_TYPE]),text,tag);

      /* Add high light field */
      if (actionValue == ATVALUE_HL)
         return exeHighLight(exeVar,scopeVar,idxTarget,tag,text);

      /* Add single field */
      if (eci_field_AT(&(exeVar->program->error),idxTarget,tag,text))
         return exeError(exeVar->program,EXE_ERROR_FIELD,text);

      break;

   case ATVALUE_CGI :
      /* Cgi field */
      return exeCgiScan(exeVar,tag,cmd->attributeList[ATTRIBUTE_TAG].atValue,(BOOLEAN)(cmd->attributeList[ATTRIBUTE_TYPE].atValue == ATVALUE_FLAG),text,scopeVar->idxCurr);

   case ATVALUE_DEFINE :
      /* Define automatic fields */
      return exeFieldDefine(exeVar,scopeVar->defineList,tag,text);

   case ATVALUE_DELETE :
      /* Delete field list */
      if (tagList)
         if (strcmp(text,"ALL") == 0) {
            if (eci_field_D(&(exeVar->program->error),idxTarget,0))
               return exeError(exeVar->program,EXE_ERROR_FIELD,text);
            break;
         } else
            return exeFieldTransf(exeVar,idxSource,idxTarget,actionValue,previousValue,text);

      /* Delete field / occurrence */
      occ = atoi(text);
      if (occ < 0) return exeError(exeVar->program,EXE_ERROR_INVALID,text);
      if (eci_field_DO(&(exeVar->program->error),idxTarget,tag,occ))
         return exeError(exeVar->program,EXE_ERROR_FIELD,text);
      break;

   case ATVALUE_EXPORT :
      /* Field export */
      if (tagList)
         return exeFieldTransf(exeVar,idxSource,scopeVar->idxPrev,actionValue,previousValue,text);
      parmTag = atoi(text);
      if (parmTag <= 0) return exeError(exeVar->program,EXE_ERROR_FIELD,text);
      if (eci_field_copy(&(exeVar->program->error),(BOOLEAN)previousValue != ATVALUE_ADD,scopeVar->idxPrev,tag,idxSource,parmTag))
         return exeError(exeVar->program,EXE_ERROR_FIELD,NULL);
      break;

   case ATVALUE_IMPORT :
      /* Field import */
      if (tagList)
         return exeFieldTransf(exeVar,scopeVar->idxPrev,idxTarget,actionValue,previousValue,text);
      parmTag = atoi(text);
      if (parmTag <= 0) return exeError(exeVar->program,EXE_ERROR_FIELD,text);
      if (eci_field_copy(&(exeVar->program->error),(BOOLEAN)previousValue != ATVALUE_ADD,idxTarget,tag,scopeVar->idxPrev,parmTag))
         return exeError(exeVar->program,EXE_ERROR_FIELD,NULL);
      break;

   case ATVALUE_OCC :
      /* Get specific occurrence */
      if (!cmd->attributeList[ATTRIBUTE_FROM].text)
         return exeError(exeVar->program,EXE_ERROR_MISSING,cplAttribute[ATTRIBUTE_FROM].text);
      tagFrom = atoi(cmd->attributeList[ATTRIBUTE_FROM].text);
      if (tagFrom <= 0)
         return exeError(exeVar->program,EXE_ERROR_INVALID,cmd->attributeList[ATTRIBUTE_FROM].text);
      return exeFieldOcc(exeVar,scopeVar,tagFrom,tag,text);

   case ATVALUE_STATUSDB :
      /* Database status */
      return exeExistDB(exeVar,idxTarget,tag,text);

   case ATVALUE_STATUSFILE :
      /* File status */
      if (eci_field_D(&(exeVar->program->error),idxTarget,tag))
         return exeError(exeVar->program,EXE_ERROR_FIELD,NULL);
      if (exeFileExist(text,NULL))
         if (eci_field_AT(&(exeVar->program->error),idxTarget,tag,"^se"))
            return exeError(exeVar->program,EXE_ERROR_FIELD,NULL);
      break;

   default:
      /* Invalid action attribute */
      return exeError(exeVar->program,EXE_ERROR_UNDEFINED,cmd->attributeList[ATTRIBUTE_ACTION].text);

   } /* switch */

   /* Return error code */
   return EXE_ERROR_OK;

} /* exeField */

/* ============================================================= exeUnlockDB */
EXE_ERROR exeUnlockDB(EXE_VAR *exeVar,    /* execution variables */
                      char *db)           /* data base name */
{
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
   - Copied from retag.c
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
   LONGX idxControl;        /* control record index */

   /* Get control record index */
   idxControl = exeNewRecord(exeVar,sizeof(M0STRU));
   if (idxControl < 0L) return exeVar->program->error.code;

   /* Read control record, reopen for write */
   record(idxControl,db,0L);
   dbxopenw("",VRDBname(idxControl),mx1extp,&VRDBmsopn(idxControl),&VRDBmsopw(idxControl),"msopn/w"); /* recwmast() */

   /* Write with no lock information */
   if (VMF0mfcxx2(idxControl) || VMF0mfcxx3(idxControl)) {
      VMF0mfcxx2(idxControl) = VMF0mfcxx3(idxControl) = 0L;
      recwmast(vrecp[idxControl],NULL,0L,0,0,sizeof(M0STRU));
   }

   /* Garbage collector */
   eci_rec_free(idxControl);

   /* Return no error */
   return EXE_ERROR_OK;

} /* exeUnlockDB */

/* ============================================================= exeFileTemp */
EXE_ERROR exeFileTemp(EXE_VAR *exeVar,    /* execution variables */
                      char *buff)         /* Return temporary file name buffer */
{
   FILE *temp;       /* file pointer */

   /* Get a temporary file name, if OK, save it, else return error */
   if (!dbxtmpnm("CI_TEMPDIR", 0, buff))
      return exeError(exeVar->program,EXE_ERROR_FILE,"tmpnam");
   temp = fopen(buff,"w");
   if (!temp) return exeError(exeVar->program,EXE_ERROR_FILE,buff);
   fclose(temp);

   /* Return no error */
   return EXE_ERROR_OK;

} /* exeFileTemp */

/* =============================================================== exeOutput */
EXE_ERROR exeOutput(EXE_VAR *exeVar,         /* execution variables */
                    CPL_ATVALUE mode,        /* output open mode */
                    char *fileName)          /* output file name */
{
   char *p;       /* auxiliary string buffer pointer */

   /* Close previously opended output file, open a new one */
   if (exeVar->output != stdout) fclose(exeVar->output);
   p = dbxcipar(NULL,fileName,'=');
   exeVar->output = fopen(p,mode == ATVALUE_APPEND ? "a+" : "w+");
   if (exeVar->output) return EXE_ERROR_OK;

   /* Error: go back to the standard output, return error code */
   exeVar->output = stdout;
   return exeError(exeVar->program,EXE_ERROR_FILE,p);

} /* exeOutput */

/* ============================================================= exeCopyFile */
EXE_ERROR exeCopyFile(EXE_VAR *exeVar,    /* execution variables */
                      char *text)         /* file names text */
{
   EFC_SPLIT_LINES table;                 /* split line structure */
   char *pIn;                             /* auxiliary string buffer pointer */
   char *pOut;                            /* auxiliary string buffer pointer */
   EXE_ERROR errorCode = EXE_ERROR_OK;    /* error code */
   COPYFILE_ERROR copyFileError;          /* copy file error code */

   /* Split lines */
   if (efc_split_lines(&table,text) < 0L)
      return exeError(exeVar->program,EXE_ERROR_ALLOC,text);

   /* Check file name presence */
   if (table.qtt < 1)
      return exeError(exeVar->program,EXE_ERROR_MISSING,"source copy file");
   if (table.qtt < 2)
      return exeError(exeVar->program,EXE_ERROR_MISSING,"target copy file");

   /* Get cipar file names */
   pIn = strdup(dbxcipar(NULL,table.list[0].text,'='));
   pOut = strdup(dbxcipar(NULL,table.list[1].text,'='));

   /* copy file */
   copyFileError = efc_copyFile(pIn,pOut);
   if (copyFileError)
   {
      errorCode = exeError(exeVar->program,EXE_ERROR_FILE,copyFileError == COPYFILE_ERROR_IN_OPEN ? pIn : pOut);
   }

   /* Garbage collector */
   efc_free(pIn);
   efc_free(pOut);
   efc_split_lines_free(&table);

   /* Return error code */
   return errorCode;

} /* exeCopyFile */

/* ================================================================= exeFile */
EXE_ERROR exeFile(EXE_VAR *exeVar,        /* execution variables */
                  SCOPE_VAR *scopeVar,    /* scope variables */
                  CPL_COMMAND *cmd,       /* attribute list */
                  char *text)             /* command text */
{
   int tag;                   /* field number */
   EXE_ERROR errorCode;       /* error code */
   char *ciparText;           /* cipar translated file name */

   /* Evaluate action attribute */
   switch (cmd->attributeList[ATTRIBUTE_ACTION].atValue) {

   case ATVALUE_APPEND:
      /* Evaluate type attribute */
      switch (cmd->attributeList[ATTRIBUTE_TYPE].atValue) {

      case ATVALUE_OUTPUT:
         /* Open output file for append */
         return exeOutput(exeVar,ATVALUE_APPEND,text);

      default:
         return exeError(exeVar->program,EXE_ERROR_INVALID,cplAttribute[ATTRIBUTE_TYPE].text);

      } /* switch */

   case ATVALUE_CREATE:
      /* Evaluate type attribute */
      switch (cmd->attributeList[ATTRIBUTE_TYPE].atValue) {

      case ATVALUE_DATABASE:
      case ATVALUE_MASTER:
         /* Create master file */
         recisis0(text);
         if (cmd->attributeList[ATTRIBUTE_TYPE].atValue == ATVALUE_MASTER) break;

      case ATVALUE_INVERTED:
         /* Create inverted file */
         trmisis0(text);
         break;

      case ATVALUE_OUTPUT:
         /* Open output file for create */
         return exeOutput(exeVar,ATVALUE_OUTPUT,text);

      case ATVALUE_TEMPFILE:
         /* Create an unique temporary file */
         tag = atoi(text);
         if (tag <= 0) return exeError(exeVar->program,EXE_ERROR_INVALID,text);
         errorCode = exeFileTemp(exeVar,exeVar->buff);
         if (errorCode) return errorCode;
         if (eci_field_MT(&(exeVar->program->error),scopeVar->idxCurr,tag,exeVar->buff))
            return exeError(exeVar->program,EXE_ERROR_FIELD,exeVar->buff);
         break;

      default:
         /* Invalid type */
         return exeError(exeVar->program,EXE_ERROR_INVALID,cplAttribute[ATTRIBUTE_TYPE].text);

      } /* switch */

      break;

   case ATVALUE_CLOSE:
      /* Evaluate type attribute */
      switch (cmd->attributeList[ATTRIBUTE_TYPE].atValue) {

      case ATVALUE_DATABASE:
         /* Database close */
         dbxflush(text);
         break;

      case ATVALUE_OUTPUT:
         /* Output close */
         if (exeVar->output != stdout) {
            fclose(exeVar->output);
            exeVar->output = stdout;
         }
         break;

      default:
         /* Invalid type */
         return exeError(exeVar->program,EXE_ERROR_INVALID,cplAttribute[ATTRIBUTE_TYPE].text);

      } /* switch */

      break;

   case ATVALUE_COPY:
      /* Evaluate type attribute */
      switch (cmd->attributeList[ATTRIBUTE_TYPE].atValue) {

      case ATVALUE_FILE:
         /* file copy */
         return exeCopyFile(exeVar,text);

      default:
         return exeError(exeVar->program,EXE_ERROR_INVALID,cplAttribute[ATTRIBUTE_TYPE].text);

      } /* switch */

   case ATVALUE_DELETE:
      /* Delete file */

      /* 27.Nov.2000 */
      ciparText = dbxcipar(NULL,text,'=');

      if (remove(ciparText) == -1) {
         sprintf(exeVar->buff,"%s (%d)",ciparText,errno);
         return exeError(exeVar->program,EXE_ERROR_FILE,exeVar->buff);
      }
      break;

   case ATVALUE_UNLOCK:
      /* Evaluate type attribute */
      switch (cmd->attributeList[ATTRIBUTE_TYPE].atValue) {

      case ATVALUE_DATABASE:
         /* Database unlock */
         return exeUnlockDB(exeVar,text);

      default:
         /* Invalid type */
         return exeError(exeVar->program,EXE_ERROR_INVALID,cplAttribute[ATTRIBUTE_TYPE].text);

      } /* switch */

   default:
      /* Invalid type */
      return exeError(exeVar->program,EXE_ERROR_MISSING,cplAttribute[ATTRIBUTE_ACTION].text);

   } /* switch */

   /* Return no error */
   return EXE_ERROR_OK;

} /* exeFile */

/* =============================================================== exeInvert */
LOCK_ERROR exeInvert(EXE_VAR *exeVar,           /* execution variables */
                     SCOPE_VAR *scopeVar,       /* scope variables */
                     CPL_COMMAND *cmd,          /* current command */
                     char *db,                  /* data base name */
                     LONGX idx)                  /* record index */
{
   LONGX maxLinks;                            /* maximum inverted links */
   FST_CODE *fstCode;                        /* FST code */
   LOCK_ERROR lockError = LOCK_ERROR_OK;     /* Lock error code */

   /* Check if the invertion was required */
   if (!scopeVar->parmList[PARM_FST]) return LOCK_ERROR_OK;
   if (!*(scopeVar->parmList[PARM_FST])) return LOCK_ERROR_OK;

   /* Set maximum links */
   maxLinks = exeNumber(exeVar,scopeVar,PARM_MAXLK,MINIMUM_MAXLK,LONGX_MAX,DEFAULT_MAXLK);
   if (exeVar->program->error.code) return LOCK_ERROR_LIST_QTT;

   /* FST generation */
   if (!cmd->func) cmd->func = exeFindParm(scopeVar->father->prev,ATVALUE_FST);
   fstCode = (FST_CODE *)cmd->func->altn;
   if (!fstCode) return exeError(exeVar->program,EXE_ERROR_FST,cplElement[cmd->element].text);

   /* Avoiding cnt buffer */
   invflush(db);

   /* Update inverted file */
   if (exePrecise(scopeVar->parmList[PARM_RESET]) == PRECISE_OFF)
      ifupd_reset = 0;
   if (ifupdat(db,VMFRmfn(idx),VMFRmfn(idx),db,fstCode,
      scopeVar->stwTable,maxLinks,maxLinks,IFUPISIS,IFUPWRIT))
      lockError = LOCK_ERROR_IU_DENIED;
   ifupd_reset = 2; /* ciifu.c */

   /* Return lock error code */
   return lockError;

} /* exeInvert */

/* ============================================================= exeLockDate */
char *exeLockDate(char *buff,           /* return buffer */
                  time_t *secsNow)      /* return seconds */
{
   struct tm *tp;    /* time structure pointer */

   /* Get current time */
   time(secsNow);
   tp = localtime(secsNow);

   /* Set joint date buffer */
   sprintf(buff,"%04d%02d%02d%02d%02d",1900+tp->tm_year,tp->tm_mon+1,
      tp->tm_mday,tp->tm_hour,tp->tm_min);

   /* Return buffer */
   return buff;

} /* exeLockDate */

/* =============================================================== exeExpire */
BOOLEAN exeExpire(EXE_VAR *exeVar,           /* execution variables */
                  SCOPE_VAR *scopeVar,       /* scope variables */
                  LONGX idxLock,              /* lock record index */
                  LOCK_ERROR *lockError)     /* lock error */
{
   LONGX expireValue;       /* expire value in seconds */
   time_t secsNow;         /* time seconds */
   LONGX lockTime;          /* recorded lock time */
   LONGX gapTime;           /* current time less recorded time */

   /* Check if expire was informed */
   if (!scopeVar->parmList[PARM_EXPIRE]) return FALSE;
   if (!*(scopeVar->parmList[PARM_EXPIRE])) return FALSE;
   expireValue = atol(scopeVar->parmList[PARM_EXPIRE]);

   /* Get recorded lock time */
   if (!eci_sub_field('x',eci_field(idxLock,scopeVar->defineList[PRECISE_LOCK].tag,1,exeVar->buff))) {
      /* Missing recorded lock time */
      *lockError = LOCK_ERROR_ID_DENIED;

   } else {

      /* Check difference between recorded lock time and expire limit */
      lockTime = atol(exeVar->buff);
      exeLockDate(exeVar->buff,&secsNow);
      gapTime = (LONGX)difftime(secsNow,(time_t)lockTime);
      if (gapTime <= expireValue) {
         *lockError = LOCK_ERROR_NOT_EXPIRED;
         return FALSE; /* [0.9g] */
      }

   }

   /* Return expire checked */
   return TRUE;

} /* exeExpire */

/* ================================================================= exeLock */
EXE_ERROR exeLock(EXE_VAR *exeVar,           /* execution variables */
                  SCOPE_VAR *scopeVar,       /* scope variables */
                  LONGX idxControl,           /* control record index */
                  LONGX mfn,                  /* MFN */
                  char *lockId,              /* Lock identification */
                  LOCK_ERROR *lockError)     /* lock error */
{
   LONGX idxLock;           /* lock record index */
   time_t secsNow;         /* time seconds */
   char *p;                /* auxiliary string buffer pointer */
   BOOLEAN updtLock = FALSE;  /* update lock identification field [0.9g] */

   /* Get lock record index */
   idxLock = exeWorkRecord(exeVar);
   if (idxLock < 0L) return exeVar->program->error.code;

   /* Try to lock the record */
   eci_lock(idxLock,VRDBname(idxControl),mfn,RLOCK);

   /* Get recorded lock identification */
   if (eci_sub_field('i',eci_field(idxLock,scopeVar->defineList[PRECISE_LOCK].tag,1,exeVar->buff))) {

      /* Compare lock identification: recorded X current, if different: lock error */
      if (strcmp(exeVar->buff,lockId) != 0) {
         if (!exeExpire(exeVar,scopeVar,idxLock,lockError)) {
            /* locked and not expired: denied */
            *lockError = LOCK_ERROR_ID_DENIED;
         } else {
            /* locked, but expired: update lock identification field [0.9g] */
            updtLock = TRUE;
         }
      } else {
         /* lock again for this user: update lock identification field [0.9g] */
         updtLock = TRUE;
      }

      /* avoid fatal record not locked, simulate record locked [0.9g] */
      if (updtLock) {
         VRECgdbl(idxLock) = VRECgdbw(idxLock) = (FFI)0-VMFRmfrl(idxLock);
      }

   } else

      /* Check if it is locked for another user */
      if (VRECrc(idxLock) != RCNORMAL && VRECrc(idxLock) != RCLDEL)
         /* Locked for another user */
         *lockError = LOCK_ERROR_R_DENIED;
      else
         /* Not locked: update lock identification field */
         updtLock = TRUE;

   /* Update lock identification field */
   if (updtLock) {

      /* Prepare lock data */
      exeLockDate(exeVar->buff,&secsNow);
      p = strdup(exeVar->buff);
      sprintf(exeVar->buff,"^i%s^t%s^x%"_LD_,lockId,p,secsNow);
      efc_free(p);
      eci_field_MT(&(exeVar->program->error),idxLock,scopeVar->defineList[PRECISE_LOCK].tag,exeVar->buff);

      /* Record update the lock identification */
      VRECwlock(idxLock) = WLOCK;
      if (recupdat(idxControl,idxLock) == RCLOCK) {
         eci_unlock(idxLock,mfn,RLOCK);
         *lockError = LOCK_ERROR_U_DENIED;
      }

   }

   /* Garbage collector */
   eci_rec_free(idxLock);

   /* Return no error */
   return EXE_ERROR_OK;

} /* exeLock */

/* ================================================================ exeWrite */
EXE_ERROR exeWrite(EXE_VAR *exeVar,          /* execution variables */
                   SCOPE_VAR *scopeVar,      /* scope variables */
                   CPL_COMMAND *cmd,         /* current command */
                   char *text)               /* command text */
{
   char *db;                                 /* data base name */
   CPL_ATVALUE mfnValue;                     /* MFN option value */
   LONGX mfn = 0L;                            /* MFN */
   char *lockId = NULL;                      /* lock identification */
   CPL_ATVALUE writeValue;                   /* write option value */
   LONGX idxControl;                          /* control record index */
   LOCK_ERROR lockError = LOCK_ERROR_OK;     /* lock error code */
   EXE_ERROR errorCode = EXE_ERROR_OK;       /* error code */

   /* Check hierarchic */
   if (scopeVar->father->element != ELEMENT_UPDATE)
      return exeError(exeVar->program,EXE_ERROR_MISSING,cplElement[ELEMENT_UPDATE].text);

   /* Get database name, MFN and lock identification, if missing: error */
   db = scopeVar->parmList[PARM_DB];
   mfnValue = exePrecise(scopeVar->parmList[PARM_MFN]);
   if (mfnValue != PRECISE_NEW && mfnValue != PRECISE_GETNEW) {
      mfn = atol(scopeVar->parmList[PARM_MFN]);
      if (mfn <= 0L)
         exeError(exeVar->program,EXE_ERROR_INVALID,cplAtValue[ATVALUE_MFN].text);
      lockId = exeRequiredParm(exeVar,scopeVar,ATVALUE_LOCKID);
      if (!lockId) return exeVar->program->error.code;
      if (!exeRequiredDefine(exeVar,scopeVar,PRECISE_LOCK))
         return exeVar->program->error.code;
   }
   writeValue = exePrecise(text);

   /* Check lock and status defines, if missing: error */
   if (!exeRequiredDefine(exeVar,scopeVar,PRECISE_STATUS))
      return exeVar->program->error.code;

   /* Get control record index */
   idxControl = exeNewRecord(exeVar,sizeof(M0STRU));
   if (idxControl < 0L) return exeVar->program->error.code;

   /* Delete lock control and lock status fields */
   if (scopeVar->defineList[PRECISE_LOCK].tag)
      if (writeValue != PRECISE_WNOUNLOCK)
         eci_field_D(&(exeVar->program->error),scopeVar->idxCurr,scopeVar->defineList[PRECISE_LOCK].tag);
   eci_field_D(&(exeVar->program->error),scopeVar->idxCurr,scopeVar->defineList[PRECISE_STATUS].tag);

   /* Data entry lock */
   if (eci_lock(idxControl,db,0L,DELOCK)) lockError = LOCK_ERROR_DE_DENIED;
   else {

      if (mfnValue == PRECISE_NEW || mfnValue == PRECISE_GETNEW) {

         /* Append new record */
         VMFRmfn(scopeVar->idxCurr) = 0L;
         VRECwlock(scopeVar->idxCurr) = WUNLOCK|NEWREC;
         if (recupdat(idxControl,scopeVar->idxCurr) == RCLOCK) lockError = LOCK_ERROR_R_DENIED;
         VMFRmfn(scopeVar->idxCurr) = VMF0nxtmfn(idxControl)-1L;

         /* Update inverted file */
         if (lockError == LOCK_ERROR_OK)
            lockError = exeInvert(exeVar,scopeVar,cmd,db,scopeVar->idxCurr);

      } else {

         if (writeValue == PRECISE_WLOCK) {

            /* Lock current record */
            errorCode = exeLock(exeVar,scopeVar,idxControl,VMFRmfn(scopeVar->idxCurr),lockId,&lockError);

         } else {

            /* Lock specified mfn record */
            errorCode = exeLock(exeVar,scopeVar,idxControl,mfn,lockId,&lockError);
            if (!errorCode && !lockError) {

               /* Delete lock field and lock status field */
               if (writeValue != PRECISE_WNOUNLOCK)
                  eci_field_D(&(exeVar->program->error),scopeVar->idxCurr,scopeVar->defineList[PRECISE_LOCK].tag);
               eci_field_D(&(exeVar->program->error),scopeVar->idxCurr,scopeVar->defineList[PRECISE_STATUS].tag);

               /* Forcing RLOCK: record is locked */
               VRECgdbl(scopeVar->idxCurr) = VRECgdbw(scopeVar->idxCurr) = (FFI)0-VMFRmfrl(scopeVar->idxCurr);

               /* Record update */
               VRECwlock(scopeVar->idxCurr) = writeValue == PRECISE_WNOUNLOCK ? WLOCK : WUNLOCK;
               VMFRmfn(scopeVar->idxCurr) = mfn;
               VMFRstatus(scopeVar->idxCurr) = (writeValue == PRECISE_WDELETE) ? DELETED : ACTIVE;
               if (recupdat(idxControl,scopeVar->idxCurr) == RCLOCK) lockError = LOCK_ERROR_R_DENIED;
               VRECrc(scopeVar->idxCurr) = (writeValue == PRECISE_WDELETE) ? RCLDEL : RCNORMAL;

               /* Update inverted file */
               if (lockError == LOCK_ERROR_OK)
                  lockError = exeInvert(exeVar,scopeVar,cmd,db,scopeVar->idxCurr);

            }

         }

      }

      /* Data entry unlock */
      eci_unlock(idxControl,0L,DELOCK);

   }

   /* Garbage collector */
   eci_rec_free(idxControl);

   /* Field update error code */
   if (!errorCode) {
      scopeVar->defineList[PRECISE_STATUS].value = (LONGX)lockError;
      errorCode = exeDefineField(exeVar,scopeVar->idxCurr,&(scopeVar->defineList[PRECISE_STATUS]));
   }
   if (errorCode) return errorCode;

   /* Return no error */
   scopeVar->defineList[PRECISE_RECSTAT].value = (LONGX)VRECrc(scopeVar->idxCurr);
   errorCode = exeDefineField(exeVar,scopeVar->idxCurr,&(scopeVar->defineList[PRECISE_RECSTAT]));
   return errorCode;

} /* exeWrite */

/* =============================================================== exeExport */
EXE_ERROR exeExport(EXE_VAR *exeVar,            /* execution variables */
                   SCOPE_VAR *scopeVar)         /* scope variables */
{
   char *fileName;            /* export file name */
   char *p;                   /* auxiliary string buffer pointer */

   /* If not opened yet, get file name parameter and open export file */
   if (!scopeVar->export) {
      fileName = exeRequiredParm(exeVar,scopeVar,ATVALUE_FILE);
      if (!fileName) return exeVar->program->error.code;
      p = dbxcipar(NULL,fileName,'=');

      /* Export to the standard output. iAH 26.Jun.2000
      scopeVar->export = fopen(p,"w");
      */
      if (strcmp(p,"STDOUT")==0) scopeVar->export = stdout;
      else scopeVar->export = fopen(p,"w");

      if (scopeVar->export == NULL)
         return exeError(exeVar->program,EXE_ERROR_FILE,p);
      scopeVar->exportType = exePrecise(scopeVar->parmList[PARM_TYPE]);
   }

   /* If required: add MFN in a field */
   if (scopeVar->defineList[PARM_MFN].tag)
      eci_field_MN(&(exeVar->program->error),scopeVar->idxCurr,scopeVar->defineList[PARM_MFN].tag,VMFRmfn(scopeVar->idxCurr));

   /* Select export type */
   switch (scopeVar->exportType) {

   case PRECISE_HLINE :
      /* HLine export */
      eci_export_hline(scopeVar->export,scopeVar->idxCurr);
      break;

   case PRECISE_VLINE :
      /* VLine export */
      eci_export_vline(scopeVar->export,scopeVar->idxCurr);
      break;

   case PRECISE_ISO2709_CRLF :
      /* ISO 2709 forcing CRLF export */
      if (!eci_export_iso2709(scopeVar->export,scopeVar->idxCurr,TRUE))
         return exeError(exeVar->program,EXE_ERROR_ALLOC,NULL);
      break;

   case PRECISE_MARC21:
      if (!eci_export_iso2709_marc(scopeVar->export,scopeVar->idxCurr))
         return exeError(exeVar->program,EXE_ERROR_ALLOC,NULL);
      break;

   default :
      /* ISO 2709 export */
      if (!eci_export_iso2709(scopeVar->export,scopeVar->idxCurr,FALSE))
         return exeError(exeVar->program,EXE_ERROR_ALLOC,NULL);
      break;

   } /* switch */

   /* Return no error */
   return EXE_ERROR_OK;
}

/* =============================================================== exeImport */
ECI_ERROR exeImport(EXE_VAR *exeVar,         /* execution variables */
                    SCOPE_VAR *scopeVar,     /* scope variables */
                    FILE *importFile,        /* import file type value */
                    PRECISE_LIST typeValue,  /* import file type value */
                    LONGX mfn)                /* current import MFN */
{
   ECI_ERROR errorCode;       /* error code */
   char delimiter = '|';

   /* Set current MFN */
   VMFRmfn(scopeVar->idxCurr) = mfn;

   /* Check import type */
   switch (typeValue) {

   case PRECISE_HLINE:
   case PRECISE_VLINE:
      /* HLINE and VLINE import */
      errorCode = eci_import_hvline(&(exeVar->program->error),importFile,scopeVar->idxCurr,exeVar->buff);
      break;

   case PRECISE_RLINE:
      /* RLINE import */
      if (scopeVar->parmList[PARM_DELIMITER])
         if (*(scopeVar->parmList[PARM_DELIMITER]))
            delimiter = *(scopeVar->parmList[PARM_DELIMITER]);
      errorCode = eci_import_rline(&(exeVar->program->error),importFile,delimiter,scopeVar->idxCurr,exeVar->buff);
      break;

   case PRECISE_MARC21:
      /* ISO2709/MARC21 import */
      errorCode = eci_import_iso2709_marc(&(exeVar->program->error),importFile,scopeVar->idxCurr,exeVar->buff);
      break;

   default:
      /* ISO2709 import */
      errorCode = eci_import_iso2709(&(exeVar->program->error),importFile,scopeVar->idxCurr,exeVar->buff);
      break;

   } /* switch */

   /* Check error code */
   if (errorCode) return errorCode;

   /* Check field with MFN option */
   if (!scopeVar->defineList[PRECISE_MFN].tag) return errorCode;

   /* Get field with import MFN */
   *exeVar->buff = '\0';
   eci_field(scopeVar->idxCurr,scopeVar->defineList[PRECISE_MFN].tag,1,exeVar->buff);
   scopeVar->defineList[PRECISE_MFN].value = atol(exeVar->buff);
   if (scopeVar->defineList[PRECISE_MFN].value < 1L)
      return efc_error(&(exeVar->program->error),ECI_ERROR_IMPORT,cplAtValue[ATVALUE_MFN].text);
   VMFRmfn(scopeVar->idxCurr) = mfn;

   /* Return error code */
   return errorCode;

} /* exeImport */

/* ================================================================= exeList */
EXE_ERROR exeList(EXE_VAR *exeVar,        /* execution variables */
                  SCOPE_VAR *scopeVar,    /* scope variables */
                  CPL_COMMAND *cmd,       /* first command */
                  char *text)             /* identifier text */
{
   LONGX freqValue;         /* frequency load value */

   /* Check action attribute */
   if (!cmd->attributeList[ATTRIBUTE_ACTION].text)
      return exeError(exeVar->program,EXE_ERROR_MISSING,cplAttribute[ATTRIBUTE_ACTION].text);

   /* Evaluate action attribute */
   switch (cmd->attributeList[ATTRIBUTE_ACTION].atValue) {

   case ATVALUE_DELETE:
      /* Delete list */
      freq_free(exeVar->freq.head);
      memset(&(exeVar->freq),0x00,(size_t)sizeof(FREQLIST_VAR));
      break;

   case ATVALUE_LOAD:
      /* Get load type attribute, list load parameter */
      if (!cmd->attributeList[ATTRIBUTE_ACTION].text)
         return exeError(exeVar->program,EXE_ERROR_MISSING,cplAttribute[ATTRIBUTE_TYPE].text);

      /* Get frequency sum value */
      freqValue = 1L;
      if (scopeVar->parmList[PARM_FREQSUM])
         freqValue = atol(scopeVar->parmList[PARM_FREQSUM]);
      if (freqValue <= 0L)
         return exeError(exeVar->program,EXE_ERROR_INVALID,cplAtValue[ATVALUE_FREQSUM].text);

      /* Load item */
      if (!freq_load(&(exeVar->freq),text,freqValue,
         (BOOLEAN)(cmd->attributeList[ATTRIBUTE_TYPE].atValue == ATVALUE_LIST),
         (BOOLEAN)(cmd->attributeList[ATTRIBUTE_TYPE].atValue == ATVALUE_SORT)))
         return exeError(exeVar->program,EXE_ERROR_ALLOC,text);

      break;

   default :
      /* Invalid action attribute */
      return exeError(exeVar->program,EXE_ERROR_INVALID,cmd->attributeList[ATTRIBUTE_ACTION].text);

   } /* switch */

   /* Return no error */
   return EXE_ERROR_OK;

} /* exeList */

/* ========================================================== exeExtractSave */
EXE_ERROR exeExtractSave(EXE_VAR *exeVar,          /* execution variables */
                         SCOPE_VAR *scopeVar,      /* scope variables */
                         char *text)               /* extract list */
{
   EFC_SPLIT_LINES table;                 /* split line structure */
   LONGX line;                             /* line index */
   char *db;                              /* database name */
   LONGX idxControl = -1L;                 /* control record index */
   LONGX idx = -1L;                        /* auxiliary record index */
   LONGX len;                              /* auxiliary string lenght */
   char *p;                               /* auxiliary string pointer */

   LONGX mfn;
   LONGX tag;
   LONGX occ;
   LONGX count;

   EXE_ERROR errorCode = EXE_ERROR_OK;    /* error code */

   /* Check mandatory parameters */

   /*
   db = exeRequiredParm(exeVar,scopeVar,ATVALUE_KEYSDB);
   if (!db) return exeVar->program->error.code;
   */

   /* if ATVALUE_KEYSDB is present: generate extraction data base
      else: store all fields generated by the fst extraction (like mx)
   */
   db = scopeVar->parmList[PARM_KEYSDB];

   if (db)
   {
      /* need Isis_Key define */
      if (!exeRequiredDefine(exeVar,scopeVar,PRECISE_KEY))
      {
         return exeError(exeVar->program,EXE_ERROR_MISSING,preciseList[PRECISE_KEY]);
      }
   }


   /* Split lines */
   if (efc_split_lines(&table,text) < 0L)
      return exeError(exeVar->program,EXE_ERROR_ALLOC,text);

   if (db)
   {
      /* Get control record index */
      idxControl = exeNewRecord(exeVar,sizeof(M0STRU));
      if (idxControl < 0L) return EXE_ERROR_ALLOC;
      record(idxControl,db,0L);

      /* Get update record index */
      idx = exeWorkRecord(exeVar);
      if (idx < 0L) return EXE_ERROR_ALLOC;
   }

   /* Loop all lines */
   for (line = 0L; line < table.qtt; line++) {

      /* Check error code */
      if (errorCode) break;

      /* Check FST line */
      if (!*table.list[line].text) {
         errorCode = exeError(exeVar->program,EXE_ERROR_EXTRACT,NULL);
         break;
      }

      /* Reverse find subfield m */
      len = strlen(table.list[line].text)-1;
      for (p = table.list[line].text+len; len; len--,p--)
         if (*p == '^')
            if (*(p+1) == 'm') break;

      /* Extract posting information */
      sscanf(p,"^m%"_LD_"^t%"_LD_"^o%"_LD_"^c%"_LD_,&mfn,&tag,&occ,&count);

      /* Check if the extraction is to placed at the scope index */
      if (!db)
      {
         /* Add field to the current scope */
         if (eci_field_AT(&(exeVar->program->error),scopeVar->idxCurr,tag,table.list[line].text))
         {
            errorCode = exeError(exeVar->program,EXE_ERROR_FIELD,table.list[line].text);
            break;
         }
         continue;
      }

      /* Truncate key */
      if (len > LE2) len = LE2;
      *(table.list[line].text+len) = '\0';

      /* Format fixed key */
      sprintf(exeVar->buff,"%d|%-*s|%7"_LD_"|%5"_LD_"|%4"_LD_"|%4"_LD_,
         len <= LE1 ? 1 : 2,
         len <= LE1 ? LE1 : LE2,eci_uctab(table.list[line].text),
         mfn,tag,occ,count);

      /* Add field */
      if (eci_field_MT(&(exeVar->program->error),idx,scopeVar->defineList[PRECISE_KEY].tag,exeVar->buff)) {
         errorCode = exeError(exeVar->program,EXE_ERROR_FIELD,table.list[line].text);
         break;
      }

      /* Add record */
      VMFRmfn(idx) = VMF0nxtmfn(idxControl);
      if (recupdat(idxControl,idx) != RCNORMAL) {
         errorCode = exeError(exeVar->program,EXE_ERROR_EXTRACT,table.list[line].text);
         break;
      }

   } /* for */

   /* Garbage collector */
   efc_split_lines_free(&table);
   if (idxControl >= 0L) eci_rec_free(idxControl);
   if (idx >= 0L) eci_rec_free(idx);

   /* Return error code */
   return errorCode;

} /* exeExtractSave */

/* ============================================================== exeExtract */
EXE_ERROR exeExtract(EXE_VAR *exeVar,        /* execution variables */
                     SCOPE_VAR *scopeVar,    /* scope variables */
                     CPL_COMMAND *cmd)       /* current command */
{
   char *fst;           /* FST parameter */
   LONGX maxLinks;       /* maximum inverted links */
   FST_CODE *fstCode;   /* FST code */
   char *text;          /* text list */
   char *lnk1p = NULL;  /* FST generation parameter */
   char *lnk2p = NULL;  /* FST generation parameter */
   LONGX qtylk1;         /* FST generation parameter */
   LONGX qtylk2;         /* FST generation parameter */

   /* Check mandatory parameters */
   fst = exeRequiredParm(exeVar,scopeVar,ATVALUE_FST);
   if (!fst) return exeVar->program->error.code;

   /* Set maximum links */
   maxLinks = exeNumber(exeVar,scopeVar,PARM_MAXLK,MINIMUM_MAXLK,LONGX_MAX,DEFAULT_MAXLK);
   if (exeVar->program->error.code) return exeVar->program->error.code;

   /* FST generation */
   if (!cmd->func)
      cmd->func = exeFindParm(scopeVar->father->prev,ATVALUE_FST);
   fstCode = (FST_CODE *)cmd->func->altn;
   if (!fstCode) return exeError(exeVar->program,EXE_ERROR_FST,cplElement[cmd->element].text);

   /* FST generation */
   *exeVar->buff = '\0';
   fst_hdrp = exeVar->buff; fst_hdrx = EXE_REC_SIZE-1; fst_hdrt = TRUE;
   fst_inter(fstCode,scopeVar->idxCurr,scopeVar->stwTable,
      &lnk1p,maxLinks,&lnk2p,maxLinks,&qtylk1,&qtylk2);
   efc_free(lnk1p);  /* 04.Aug.1999 fst_inter allocated and doesn't deallocated */
   efc_free(lnk2p);  /* 04.Aug.1999 */
   fst_hdrp = NULL; fst_hdrx = 0; fst_hdrt = FALSE;
   if (fst_error)
      return exeError(exeVar->program,EXE_ERROR_FST,fst_errp);

   /* Extraction save */
   text = strdup(exeVar->buff);
   if (exeExtractSave(exeVar,scopeVar,text))
      return exeVar->program->error.code;

   /* Garbage collector */
   efc_free(text);

   /* Return no error */
   return EXE_ERROR_OK;

} /* exeExtract */

/* =========================================================== exeStartScope */
void exeStartScope(SCOPE_VAR *scopeVar)         /* scope variables */
{

   /* Reset all variables */
   memset(scopeVar,0x00,sizeof(SCOPE_VAR));

   /* No record index indicator */
   scopeVar->idxPrev = -1L;
   scopeVar->idxRetn = -1L;
   scopeVar->idxCurr = -1L;

} /* exeStartScope */

/* =========================================================== exeScopeClose */
void exeScopeClose(SCOPE_VAR *scopeVar)      /* scope variables */
{

   /* Close previously opened files */
   if (scopeVar->export) fclose(scopeVar->export);

} /* exeScopeClose */

/* ============================================================ exeScopeFree */
void exeScopeFree(SCOPE_VAR *scopeVar)    /* scope variables */
{
   PRECISE_LIST i;      /* auxiliary loop index */

   /* Close scope */
   exeScopeClose(scopeVar);

   /* Garbage collector */
   for (i = 0; i < PRECISE_LIST_QTT; i++)
      efc_free(scopeVar->defineList[i].text);

   /* stop word free */
   efc_free(scopeVar->stwTable);

   /* Code Guard*/
   efc_free(scopeVar->parmList[PARM_KEYFIELD]);
   efc_free(scopeVar->parmList[PARM_KEYLENGTH]);
   efc_free(scopeVar->parmList[PARM_KEYSDB]);

} /* exeScopeFree */

/* ========================================================= exeFindFunction */
CPL_COMMAND *exeFindFunction(EXE_VAR *exeVar,         /* execution variables */
                             CPL_COMMAND *cmdFrom,    /* from command */
                             char *callName)          /* function call name */
{
   CPL_COMMAND *cmd;       /* auxiliary command finder */
   CPL_COMMAND *cmdFind;   /* auxiliary command finder for sub-command */

   /* No commands */
   if (!cmdFrom) return NULL;

   /* Loop IsisScript commands, if function element and same attribute name: return cmd */
   for (cmd = cmdFrom; cmd; cmd = cmd->next) {

      /* Find in sub-commands */
      if (cmd->element == ELEMENT_ISISSCRIPT) {
         cmdFind = exeFindFunction(exeVar,cmd->sub,callName);
         if (cmdFind) return cmdFind;
      }

      /* Find element function with the exact name */
      if (cmd->element != ELEMENT_FUNCTION) continue;
      if (strcmp(cmd->attributeList[ATTRIBUTE_NAME].text,callName) == 0)
         return cmd;

   }

   /* Function not found */
   return NULL;
}

/* ================================================================= exeCall */
EXE_ERROR exeCall(EXE_VAR *exeVar,           /* execution variables */
                   SCOPE_VAR *scopeVar,      /* scope variables */
                   CPL_COMMAND *cmd,         /* current command */
                   char *text)               /* text parameter */
{
   SCOPE_VAR localScopeVar;               /* local variables for sub-command */
   EXE_ERROR errorCode = EXE_ERROR_OK;    /* error code */

   /* Find function */
   if (!cmd->func) {
    if (!cmd->attributeList[ATTRIBUTE_NAME].text)
        {
         return exeError(exeVar->program,EXE_ERROR_MISSING,cplAttribute[ATTRIBUTE_NAME].text);
      }
      cmd->func = exeFindFunction(exeVar,exeVar->program->cmd,cmd->attributeList[ATTRIBUTE_NAME].text);
      if (!cmd->func)
      {
         return exeError(exeVar->program,EXE_ERROR_MISSING,cmd->attributeList[ATTRIBUTE_NAME].text);
      }
   }

   /* Start scope variables */
   exeStartScope(&localScopeVar);

   /* Set local variables */
   localScopeVar.idxPrev = scopeVar->idxCurr; /* [0.9a] idxPrev */
   localScopeVar.idxRetn = scopeVar->idxCurr;
   localScopeVar.idxCurr = exeWorkRecord(exeVar);
   if (localScopeVar.idxCurr < 0L)
      return exeError(exeVar->program,EXE_ERROR_ALLOC,cplElement[cmd->element].text);
   localScopeVar.father = cmd;

   /* Get function parameter */
   if (cmd->func->attributeList[ATTRIBUTE_ACTION].text ||
      cmd->func->attributeList[ATTRIBUTE_TAG].text) {
      errorCode = exeField(exeVar,scopeVar,scopeVar->idxCurr,localScopeVar.idxCurr,cmd->func,text);
   }

   /* Call function command */
   if (!exeVar->program->error.code) exeGo(exeVar,&localScopeVar,cmd->func->sub);

   /* Garbage collector */
   exeScopeFree(&localScopeVar);

   /* Garbage collector */
   eci_rec_free(localScopeVar.idxCurr);

   /* Return error code */
   return errorCode;

} /* exeCall */

/* =============================================================== exeReturn */
EXE_ERROR exeReturn(EXE_VAR *exeVar,            /* execution variables */
                    SCOPE_VAR *scopeVar,        /* scope variables */
                    CPL_COMMAND *cmd,           /* current command */
                    char *text)                 /* return parameter */
{

   /* Check scope */
   if (scopeVar->idxRetn < 0L)
      return exeError(exeVar->program,EXE_ERROR_RETURN,text);

   /* Set return parameter */
   if (cmd->attributeList[ATTRIBUTE_ACTION].text ||
      cmd->attributeList[ATTRIBUTE_TAG].text) {
      return exeField(exeVar,scopeVar,scopeVar->idxCurr,scopeVar->idxRetn,cmd,text);
   }

   /* Return nothing to do */
   return EXE_ERROR_OK;

} /* exeReturn */

/* ============================================================== exeHtmlPft */
char *exeHtmlPft(EXE_VAR *exeVar,         /* execution variables */
                 SCOPE_VAR *scopeVar,     /* scope variables */
                 char *text)              /* Html text */
{
   char *prefix = "[pft]";       /* format begin */
   char *suffix = "[/pft]";      /* format end */
   int pfxLength;                /* prefix lenght */
   int sfxLength;                /* suffix lenght */
   char *p;                      /* auxiliary string pointer */
   char *buff;                   /* auxiliary string pointer */

   /* Get prefix and suffix parameters */
   if (scopeVar->parmList[PARM_PREFIX_HTMLPFT])
      if (*(scopeVar->parmList[PARM_PREFIX_HTMLPFT]))
         prefix = scopeVar->parmList[PARM_PREFIX_HTMLPFT];
   if (scopeVar->parmList[PARM_SUFFIX_HTMLPFT])
      if (*(scopeVar->parmList[PARM_SUFFIX_HTMLPFT]))
         suffix = scopeVar->parmList[PARM_SUFFIX_HTMLPFT];

   /* Set auxiliary variables */
   pfxLength = strlen(prefix);
   sfxLength = strlen(suffix);
   buff = exeVar->buff;

   /* Open format literal, loop all characters */
   *buff++ = '\'';
   for (p = text; *p; p++) {

      /* Check if it is the prefix */
      if (*p == *prefix)
         if (strncmp(p,prefix,pfxLength) == 0) {

            /* Close format literal */
            strcpy(buff,"\',"); buff += 2;

            /* Loop all characters until suffix */
            for (p += pfxLength; *p; p++) {

               /* Check if it is the suffix, close format literal */
               if (*p == *suffix)
                  if (strncmp(p,suffix,sfxLength) == 0) {
                     strcpy(buff,",\'"); buff += 2;
                     p += sfxLength-1;
                  break;
               }

               /* Substitute the corresponding HTML specificattion to format specificattion */
               /* This was used to correct the Front Page behaviour
               if (*p == '&') {
                  if (strncmp(p+1,"lt;",3) == 0)
                     { *buff++ = '<'; p += 3; continue; }
                  if (strncmp(p+1,"gt;",3) == 0)
                     { *buff++ = '>'; p += 3; continue; }
                  if (strncmp(p+1,"quot;",5) == 0)
                     { *buff++ = '\"'; p += 5; continue; }
               }
                  ... it mustn't be done by the WXIS */

               /* Copy character */
               *buff++ = *p;

            } /* for */

            /* Format substitution end */
            continue;

         }

      /* Substitute format literal specification to the corresponding HTML specification */
      if (*p == '\'') {
         strcpy(buff,"&#39"); buff += 4;
         continue;
      }

      /* Copy character */
      if (*p != '\r') *buff++ = *p;

   } /* for */

   /* Close format literal, set string buffer end, show buffer */
   *buff++ = '\'';
   *buff = '\0';

   /* Return new buffer copy */
   return strdup(exeVar->buff);
}

/* ================================================================== exePft */
EXE_ERROR exePft(EXE_VAR *exeVar,         /* execution variables */
                 SCOPE_VAR *scopeVar,     /* scope variables */
                 LONGX idxCurr,            /* current index */
                 CPL_COMMAND *cmd)        /* current command */
{
   EXE_FORMAT *fmt;        /* format structure pointer */
   char *text;             /* auxiliary text pointer */
   char *htmlpft = NULL;   /* auxiliary text pointer */
   LONGX fmtError;          /* format error code */

   /* If first time, allocate and reset format structure */
   if (!cmd->altn) {
      cmd->altn = (EXE_FORMAT *)exeNew(exeVar,sizeof(EXE_FORMAT));
      if (!cmd->altn)
         return exeError(exeVar->program,EXE_ERROR_ALLOC,cplElement[cmd->element].text);
      memset(cmd->altn,0x00,sizeof(EXE_FORMAT));
   }

   /* Cast alternate pointer to format pointer, reset formated text */
   fmt = (EXE_FORMAT *)(cmd->altn);
   if (cmd->attributeList[ATTRIBUTE_ACTION].atValue != ATVALUE_CONVERT) /* [0.9b] */
      fmt->text = efc_free(fmt->text);

   /* Reload format if required */
   if (cmd->attributeList[ATTRIBUTE_TYPE].atValue == ATVALUE_RELOAD) {
      if (fmt->code) fmt_free(fmt->code);
      fmt->code = NULL;
      if ( fmt->ciXml )
      {
            cixmlFree(fmt->ciXml);
         efc_free(fmt->ciXml);
            fmt->ciXml = NULL;
      }
   }

   /* If first time or reload, get format specification */
   if (!fmt->code && !fmt->ciXml) {
      text = exeText(exeVar,scopeVar,idxCurr,cmd);
      if (!text) return EXE_ERROR_OK;
      if (cmd->element == ELEMENT_HTMLPFT)
      {
         text = htmlpft = exeHtmlPft(exeVar,scopeVar,text);
      }
       if (cmd->element == ELEMENT_ISISXML)
    {
        text = "";
        if ( scopeVar->parmList[PARM_ISISXML_TABLE] )
         {
                text = strdup(scopeVar->parmList[PARM_ISISXML_TABLE]);
             if ( !text )
          {
                    return exeError(exeVar->program,EXE_ERROR_ALLOC,"PARM_ISISXML_TABLE");
            }
         }
            if ( !fmt->ciXml )
         {
            fmt->ciXml = (CIXML_STRUCT *)exeNew(exeVar,sizeof(CIXML_STRUCT));
            fmt->ciXml->style = CIXML_STYLE_ALL;
         }
           if ( scopeVar->parmList[PARM_ISISXML_STYLE] )
           {
                fmt->ciXml->style = atoi(scopeVar->parmList[PARM_ISISXML_STYLE]);
            }
           if ( cixmlLoad(text,fmt->ciXml,fmt->ciXml->style) != CIXML_ERROR_OK )
        {
                return exeError(exeVar->program,EXE_ERROR_ALLOC,"cixmlLoad");
           }
       }
   }

   /* If first time or reload, compile format specification */
   if (!fmt->code && !fmt->ciXml)
   {
      if (strcmp(text,"ALL") != 0) {
         fmtError = fmt_gener(&(fmt->code),text);

         /* Check syntax error */
         if (fmtError) {

            fmt->code = NULL; /* on error: fmt_gener calls fmt_free */
            efc_free(htmlpft);

            /* If is check return error code and place else execution error */
            if (cmd->attributeList[ATTRIBUTE_TYPE].atValue == ATVALUE_CHECK) {
               sprintf(exeVar->buff,"%05"_LD_" %.256s",fmtError,text+fmt_errof);
               fmt->text = strdup(exeVar->buff);
               return EXE_ERROR_OK;
            } else {
               return exeError(exeVar->program,EXE_ERROR_FORMAT,text+fmt_errof);
            }

         }

         /* If is check return no error buffer */
         if (cmd->attributeList[ATTRIBUTE_TYPE].atValue == ATVALUE_CHECK) {
            sprintf(exeVar->buff,"%05"_LD_,fmtError);
            fmt->text = strdup(exeVar->buff);
            return EXE_ERROR_OK;
         }

      }
   }

   /* [0.9b] Check action convert attribute */
   if (cmd->attributeList[ATTRIBUTE_ACTION].atValue == ATVALUE_CONVERT) {
      if (htmlpft) {
         efc_free(fmt->text);
         fmt->text = htmlpft;
      }
      return EXE_ERROR_OK;
   }

   /* Gabage collector */
   efc_free(htmlpft);
   /* If code absent is ALL specification else interpret format code */
   if (!fmt->code && !fmt->ciXml)
   {
    eci_fmt_all(idxCurr,exeVar->buff);
   }
   else
   {
    /* isisxml element */
    if (cmd->element == ELEMENT_ISISXML)
      {
        cixmlRun(vrecp[idxCurr],fmt->ciXml,exeVar->buff);
      }
      else
      {
        /* pft element */
        if (fmt_inter(fmt->code,idxCurr,exeVar->buffSize,exeVar->buff,exeVar->buffSize) < 0L)
         {
            return exeError(exeVar->program,EXE_ERROR_FORMAT,fmterrxy);
         }
      }
    }

   /* Set format result */
   fmt->text = strdup(exeVar->buff);

   /* Return no error */
   return EXE_ERROR_OK;

} /* exePft */

/* ================================================================ exeText */
char *exeText(EXE_VAR *exeVar,         /* execution variables */
              SCOPE_VAR *scopeVar,     /* scope variables */
              LONGX idxCurr,            /* current index */
              CPL_COMMAND *cmd)        /* current command */
{
   EXE_FORMAT *fmt;     /* format structure pointer */

   /* Check sub-command, if absent return statment */
   if (!cmd->sub) return cmd->text;

   /* Format to get the parameter */
   exePft(exeVar,scopeVar,idxCurr,cmd->sub);

   /* Check format result */
   fmt = (EXE_FORMAT *)(cmd->sub->altn);
   if (fmt->text) {
       if (*(fmt->text)) {
           return fmt->text;
       }
   }

   /* Return no text supplied */
   return NULL;

} /* exeText */

/* ================================================================== exeCmd */
CPL_COMMAND *exeCmd(EXE_VAR *exeVar,      /* execution variables */
                    SCOPE_VAR *scopeVar,        /* scope variables */
                    CPL_COMMAND *cmd)           /* first command */
{
   char *text;       /* text parameter */

   /* Get command parameter */
   text = exeText(exeVar,scopeVar,scopeVar->idxCurr,cmd);
   if (!text) return cmd;

   /* Evaluate element */
   switch (cmd->element) {

   case ELEMENT_CALL:
      /* Call function */
      if (exeCall(exeVar,scopeVar,cmd,text)) return cmd;
      break;

   case ELEMENT_CGITABLE:
      /* CGI table list */
      if (exeCgiTable(exeVar,scopeVar->idxCurr,text)) return cmd;
      break;

   case ELEMENT_DEFINE:
      /* Define list */
      if (exeDefine(exeVar,scopeVar,text)) return cmd;
      break;

   case ELEMENT_EXPORT:
      /* Export record */
      exeExport(exeVar,scopeVar);
      break;

   case ELEMENT_EXTRACT:
      /* Extract record keys */
      if (exeExtract(exeVar,scopeVar,cmd)) return cmd;
      break;

   case ELEMENT_FIELD:
      /* Field action */
      exeField(exeVar,scopeVar,scopeVar->idxCurr,scopeVar->idxCurr,cmd,text);
      break;

   case ELEMENT_FILE:
      /* File action */
      exeFile(exeVar,scopeVar,cmd,text);
      break;

   case ELEMENT_FLOW:
      /* Flow shotcut */
      exeFlow(exeVar,scopeVar,cmd,text);
      break;

   case ELEMENT_LIST:
      /* List item */
      exeList(exeVar,scopeVar,cmd,text);
      break;

   case ELEMENT_PROC:
      /* Field update specification format */
      if (eci_field_update(&(exeVar->program->error),scopeVar->idxCurr,text)) {
         exeError(exeVar->program,EXE_ERROR_PROC,NULL);
         return cmd;
      }
      break;

   case ELEMENT_RETURN:
      /* Return from function */
      exeReturn(exeVar,scopeVar,cmd,text);
      return NULL; /* [0.9d] */

   case ELEMENT_TRACE:
      /* Trace commands */
      exeVar->trace = exePrecise(text);
      if (exeVar->trace != PRECISE_ON && exeVar->trace != PRECISE_BR &&
          exeVar->trace != PRECISE_TABLE) exeVar->trace = PRECISE_OFF;

      if (exeVar->trace == PRECISE_OFF) {

         if (strcmp(text,"dbxtrace") == 0) dbxtrace = 1; /* dbxopen/dbxcipar trace */
         if (strcmp(text,"rectrace") == 0) rectrace = 1; /* rec RESTRACE runtime switch */
         if (strcmp(text,"dectrace") == 0) dectrace = 1; /* decoread()/recdeco() runtime switch  */
         if (strcmp(text,"trmtrace") == 0) trmtrace = 1; /* trm TRSTRACE runtime switch */
#if !CIB71
         if (strcmp(text,"b50trace") == 0) b50trace = 1; /* b50 RUXTRACE runtime switch */
#else /* CIB71 */
         if (strcmp(text,"b70trace") == 0) b70trace = 1; /* b50 RUXTRACE runtime switch */
#endif /* CIB71 */
         if (strcmp(text,"fmttrace") == 0) fmttrace = 1; /* fmt runtime switch */
         if (strcmp(text,"fsttrace") == 0) fsttrace = 1; /* fst FSSTRACE runtime switch */
         if (strcmp(text,"multrace") == 0) multrace = 1; /* upd MULTRACE runtime switch */

         if (strcmp(text,"-dbxtrace") == 0) dbxtrace = 0;   /* dbxopen/dbxcipar trace */
         if (strcmp(text,"-rectrace") == 0) rectrace = 0;   /* rec RESTRACE runtime switch */
         if (strcmp(text,"-dectrace") == 0) dectrace = 0;   /* decoread()/recdeco() runtime switch  */
         if (strcmp(text,"-trmtrace") == 0) trmtrace = 0;   /* trm TRSTRACE runtime switch */
#if !CIB71
         if (strcmp(text,"-b50trace") == 0) b50trace = 0;   /* b50 RUXTRACE runtime switch */
#else /* CIB71 */
         if (strcmp(text,"-b70trace") == 0) b70trace = 0;   /* b50 RUXTRACE runtime switch */
#endif /* CIB71 */
         if (strcmp(text,"-fmttrace") == 0) fmttrace = 0;   /* fmt runtime switch */
         if (strcmp(text,"-fsttrace") == 0) fsttrace = 0;   /* fst FSSTRACE runtime switch */
         if (strcmp(text,"-multrace") == 0) multrace = 0;   /* upd MULTRACE runtime switch */

      }

      break;

   case ELEMENT_WRITE:
      /* Write record */
      exeWrite(exeVar,scopeVar,cmd,text);
      break;

   default :
      /* Invalid option */
      exeError(exeVar->program,EXE_ERROR_UNDEFINED,cplElement[cmd->element].text);
      return cmd;

   } /* switch */

   /* return command */
   return cmd;

} /* exeCmd */

/* ================================================================= exeParm */
void exeParm(EXE_VAR *exeVar,             /* execution variables */
             SCOPE_VAR *scopeVar,         /* scope variables */
             CPL_COMMAND *cmd)            /* current command */
{
   PARM_LIST parmIndex;             /* parameter index */
   FST_CODE *fstCode;               /* FST code */
   char *p;                         /* auxiliary string pointer */
   int tag;                         /* field number */

   /* Get parameter text */
   parmIndex = atValueParm[cmd->attributeList[ATTRIBUTE_NAME].atValue];
   scopeVar->parmList[parmIndex] = exeText(exeVar,scopeVar,scopeVar->idxCurr,cmd);
   if (parmIndex == PARM_SORT) scopeVar->parmList[parmIndex] = "void";
   if (!scopeVar->parmList[parmIndex]) return;

   /* Evaluate parameter option */
   switch (parmIndex) {

   case PARM_ACTAB:
      /* Alphabetic character table */
      if (eci_set_actab(&(exeVar->program->error),scopeVar->parmList[parmIndex])) {
         exeError(exeVar->program,EXE_ERROR_ACTAB,NULL);
         return;
      }
      break;

   case PARM_BUFFERSIZE:
      /* Reallocate common string buffer area, if error return */
      exeVar->buffSize = atol(scopeVar->parmList[parmIndex]);
      efc_free(exeVar->buff);
      exeVar->buff = exeNew(exeVar,exeVar->buffSize);
      if (!exeVar->buff) {
         exeError(exeVar->program,EXE_ERROR_ALLOC,scopeVar->parmList[parmIndex]);
         return;
      }
      break;

   case PARM_CIPAR:
      /* CISIS file assignment table */
      dbxcdcip = efc_strrepl(dbxcdcip,scopeVar->parmList[parmIndex]);
      break;

   case PARM_FST:
      /* FST generation */
      fstCode = (FST_CODE *)cmd->altn;
      if (fstCode) fst_free(fstCode);
      fst_gener(&fstCode,scopeVar->parmList[PARM_FST]);

      if (fst_error) {

         if (cmd->attributeList[ATTRIBUTE_TYPE].atValue == ATVALUE_CHECK) {
            sprintf(exeVar->buff,"%05"_LD_" %.256s",fst_error,fst_errp);
            for (p = exeVar->buff; *p; p++) {
               if (*p == '\r' || *p == '\n') {
                  *p = '\0';
                  break;
               }
            }
            tag = atoi(cmd->attributeList[ATTRIBUTE_TAG].text);
            if (tag <= 0) {
               exeError(exeVar->program,EXE_ERROR_INVALID,cplAttribute[ATTRIBUTE_TAG].text);
               return;
            }
            if (eci_field_MT(&(exeVar->program->error),scopeVar->idxCurr,tag,exeVar->buff))
               exeError(exeVar->program,EXE_ERROR_FIELD,NULL);

         } else {

            exeError(exeVar->program,EXE_ERROR_FST,fst_errp);

         }
         return;

      } else {

         if (cmd->attributeList[ATTRIBUTE_TYPE].atValue == ATVALUE_CHECK) {
            sprintf(exeVar->buff,"%05"_LD_,fst_error);
            tag = atoi(cmd->attributeList[ATTRIBUTE_TAG].text);
            if (tag <= 0) {
               exeError(exeVar->program,EXE_ERROR_INVALID,cplAttribute[ATTRIBUTE_TAG].text);
               return;
            }
            if (eci_field_MT(&(exeVar->program->error),scopeVar->idxCurr,tag,exeVar->buff))
               exeError(exeVar->program,EXE_ERROR_FIELD,NULL);
         }

      }
      cmd->altn = fstCode;
      break;

   case PARM_STW:
      /* Load stop word, if required */
      if (scopeVar->parmList[PARM_STW]) {
         int wordsLoaded;
         efc_free(scopeVar->stwTable);
         scopeVar->stwTable = loadstw(NULL,scopeVar->parmList[PARM_STW],NULL,0L,&wordsLoaded);
         if (!scopeVar->stwTable) {
            exeError(exeVar->program,EXE_ERROR_STW,scopeVar->parmList[PARM_STW]);
            return;
         }
      }
      break;

   case PARM_UCTAB:
      /* Upper case character table */
      if (eci_set_uctab(&(exeVar->program->error),scopeVar->parmList[parmIndex])) {
         exeError(exeVar->program,EXE_ERROR_UCTAB,NULL);
         return;
      }
      break;

   } /* switch */

} /* exeParm */

/* ============================================================== exeDBDecod */
EXE_ERROR exeDBDecod(EXE_VAR *exeVar,     /* execution variables */
                     char *db,            /* target database */
                     char *p)             /* decod database */
{
   LONGX idx;                              /* auxiliary record index */
   LONGX idxDecod;                         /* auxiliary decod record index */
   EXE_ERROR errorCode = EXE_ERROR_OK;    /* error code */

   /* Allocate auxiliary record indexes */
   idx = exeWorkRecord(exeVar);
   if (idx < 0L) errorCode = exeVar->program->error.code;
   idxDecod = exeWorkRecord(exeVar);
   if (idxDecod < 0L) errorCode = exeVar->program->error.code;

   /* Set decod database table */
   if (!errorCode) {
      record(idx,db,1L);
      recdecod(idx,p,idxDecod);
   }

   /* Garbage collector */
   eci_rec_free(idx);
   eci_rec_free(idxDecod);

   /* Return error code */
   return errorCode;

} /* exeDBDecod */

/* ============================================================== exeDBGizmo */
EXE_ERROR exeDBGizmo(EXE_VAR *exeVar,     /* execution variables */
                     char *db,            /* target database */
                     char *p)             /* gizmo list */
{
   EFC_SPLIT_LINES table;                 /* split line structure */
   LONGX line;                             /* line index */
   DBXSTRU *dbxp;                         /* dbx structure pointer */
   VGIZPSTRU *gizmap;                     /* gizmo map structure pointer */
   LONGX idx;                              /* auxiliary record index */
   EXE_ERROR errorCode = EXE_ERROR_OK;    /* error code */

   /* Split lines */
   if (efc_split_lines(&table,p) < 0L)
      return exeError(exeVar->program,EXE_ERROR_ALLOC,p);

   /* Get auxiliary record */
   idx = exeWorkRecord(exeVar);
   if (idx < 0L) errorCode = exeVar->program->error.code;

   /* Loop all lines */
   for (line = 0L; line < table.qtt; line++) {

      /* Check error code */
      if (errorCode) break;

      /* Set gizmo reference */
      dbxp = dbxstorp(db);
      if (DBXvgzrp) {
         for (gizmap = DBXvgzrp; gizmap->nextp; gizmap = gizmap->nextp);
         gizmread(table.list[line].text,&(gizmap->nextp),idx);
      } else gizmread(table.list[line].text,&DBXvgzrp,idx);

   } /* for */

   /* Garbage collector */
   efc_split_lines_free(&table);
   eci_rec_free(idx);

   /* Return error code */
   return errorCode;

} /* exeDBGizmo */

/* =================================================================== exeDB */
char *exeDB(EXE_VAR *exeVar,           /* execution variables */
            SCOPE_VAR *scopeVar)       /* scope variables */
{
   char *db;      /* auxiliary database parameter string pointer */

   /* Check if db paramete is present */
   db = scopeVar->parmList[PARM_DB];
   if (!db) return NULL;

   /* Check gizmo parameter */
   if (scopeVar->parmList[PARM_GIZMO])
      if (*(scopeVar->parmList[PARM_GIZMO]))
         if (exeDBGizmo(exeVar,db,scopeVar->parmList[PARM_GIZMO]))
            return NULL;

   /* Check decod parameter */
   if (scopeVar->parmList[PARM_DECOD])
      if (*(scopeVar->parmList[PARM_DECOD]))
         exeDBDecod(exeVar,db,scopeVar->parmList[PARM_DECOD]);

   /* Return database name pointer */
   return db;

} /* exeDB */

/* ======================================================= exeTaskMasterSort */
void exeTaskMasterSort(EXE_VAR *exeVar,         /* execution variables */
                       SCOPE_VAR *scopeVar,     /* scope variables */
                       EXE_FORMAT *fmt)         /* sort format */
{
   char *db;               /* data base name */
   LONGX keyField;          /* sort key field */
   LONGX keyLength;         /* sort key length */
   LONGX idxControl;        /* control record index */
   CPL_COMMAND *func;      /* format reference */

   /* Get database name, if missing: error */
   db = exeDB(exeVar,scopeVar);
   if (!db) return;

   /* Get required sort parameters */
   keyField = exeNumber(exeVar,scopeVar,PARM_KEYFIELD,0L,LONGX_MAX,0L);
   if (exeVar->program->error.code) return;
   if (!keyField)
      if (!exeRequiredParm(exeVar,scopeVar,ATVALUE_KEY)) return;
   if (!exeRequiredParm(exeVar,scopeVar,ATVALUE_KEYLENGTH)) return;
   keyLength = exeNumber(exeVar,scopeVar,PARM_KEYLENGTH,1L,EXE_REC_SIZE,DEFAULT_KEYLENGTH);
   if (exeVar->program->error.code) return;
   if (!exeRequiredDefine(exeVar,scopeVar,PRECISE_STATUS)) return;

   /* Find key parameter backward */
   if (!keyField) {
      if (!fmt) {
         func = exeFindParm(scopeVar->father->prev,ATVALUE_KEY);
         if (func)
            if (func->sub)
               fmt = (EXE_FORMAT *)func->sub->altn;
      }
      if (!fmt) {
         exeError(exeVar->program,EXE_ERROR_MSRT,cplAtValue[ATVALUE_KEY].text);
         return;
      }
   }

   /* Get control record index */
   idxControl = exeNewRecord(exeVar,sizeof(M0STRU));
   if (idxControl < 0L) return;

   /* Exclusive write lock */
   if (eci_lock(idxControl,db,0L,EWLOCK))
      scopeVar->defineList[PRECISE_STATUS].value = LOCK_ERROR_EW_DENIED;
   else {
      scopeVar->defineList[PRECISE_STATUS].value = LOCK_ERROR_OK;

      /* Sort master file */
      if (!cms_sort(db,EXE_REC_SIZE,keyLength,fmt ? fmt->code : NULL,FALSE/*fmtgen*/,TRUE/*parmmfn*/,keyField)) {
         exeError(exeVar->program,EXE_ERROR_MSRT,NULL);
         return;
      }
   }

   /* Exclusive write unlock */
   if (scopeVar->defineList[PRECISE_STATUS].value == LOCK_ERROR_OK)
      eci_unlock(idxControl,0L,EWLOCK);

   /* Garbage collector */
   eci_rec_free(idxControl);

} /* exeTaskMasterSort */

#if LIND
/* =============================================================== svdifload */
LONGX svdifload(char *dbnp,char *filekeys_1,char *filekeys_2,
                  int pstflag,LONGX tell)
{
   fatal("svdifload X svdiflind");
   return 0L;
}
#endif /* LIND */

/* ===================================================== exeTaskInvertedLoad */
extern int ifl_balan;   /* ciifl.c */

void exeTaskInvertedLoad(EXE_VAR *exeVar,             /* execution variables */
                         SCOPE_VAR *scopeVar,         /* scope variables */
                         BOOLEAN needLock)            /* need lock */
{
   char *db;               /* data base name */
   char *keysDB;           /* keys data base name */
   LONGX idxControl;        /* control record index */
   LONGX idxAux;            /* auxiliary record index */
   LOCK_ERROR lockErr;     /* lock error */

   /*  */
#if LIND
   exeError(exeVar->program,EXE_ERROR_INVALID,"LIND - Inverted load");
   if (exeVar->program->error.code) return;
#endif /* LIND */

   /* Get database name, if missing: error */
   db = exeDB(exeVar,scopeVar);
   if (!db) return;

   /* Check mandatory parameters */
   keysDB = exeRequiredParm(exeVar,scopeVar,ATVALUE_KEYSDB);
   if (!keysDB) return;
   if (!exeRequiredDefine(exeVar,scopeVar,PRECISE_KEY)) {
      exeError(exeVar->program,EXE_ERROR_MISSING,preciseList[PRECISE_KEY]);
      return;
   }

   /* Check if exclusive write lock was required */
   lockErr = -1L;
   if (needLock) {

      /* Get lock parameter */
      if (!exeRequiredDefine(exeVar,scopeVar,PRECISE_STATUS)) {
         exeError(exeVar->program,EXE_ERROR_MISSING,preciseList[PRECISE_STATUS]);
         return;
      }

      /* Get control record index */
      idxControl = exeNewRecord(exeVar,sizeof(M0STRU));
      if (idxControl < 0L) return;

      /* Exclusive write lock */
      if (eci_lock(idxControl,db,0L,EWLOCK)) lockErr = LOCK_ERROR_EW_DENIED;
      else lockErr = LOCK_ERROR_OK;

   }

   /* If allowed, load inverted and reset master */
   if (lockErr != LOCK_ERROR_EW_DENIED) {

      /* Inverted load */
      sprintf(exeVar->buff,"%d",scopeVar->defineList[PRECISE_KEY].tag);

#if SUN
      ifl_balan = 0;
#endif /* SUN */

      svdifload(db,keysDB,exeVar->buff,IFUPISIS/* ? IFUPDICT */,-1L);

#if SUN
      ifl_balan = 1;
#endif /* SUN */

      if (exePrecise(scopeVar->parmList[PARM_RESET]) != PRECISE_OFF) {

         /* Get auxiliary record index */
         idxAux = exeNewRecord(exeVar,sizeof(M0STRU));
         if (idxAux < 0L) return;

         /* Reset master */
         recreset(db,1L,MAXUPDMFN,idxAux,0L/*parmtell*/);

         /* Garbage collector */
         eci_rec_free(idxAux);

      }

   }

   /* Check if exclusive write lock was set */
   if (needLock) {
      scopeVar->defineList[PRECISE_STATUS].value = lockErr;
      if (lockErr == LOCK_ERROR_OK) eci_unlock(idxControl,0L,EWLOCK);
      eci_rec_free(idxControl);
   }

} /* exeTaskInvertedLoad */

/* ============================================== exeTaskFullInvertionReturn */
int exeTaskFullInvertionReturn(SCOPE_VAR *scopeVar,      /* scope variables */
                               LONGX idxControl)          /* control record index */
{

   /* If locked, exclusive write unlock */
   if (scopeVar->defineList[PRECISE_STATUS].value == LOCK_ERROR_OK)
      eci_unlock(idxControl,0L,EWLOCK);

   /* Garbage collector */
   if (idxControl > -1L) eci_rec_free(idxControl);

   /* Return */
   return 0;

} /* exeTaskFullInvertionReturn */

/* ==================================================== exeTaskFullInvertion */
int exeTaskFullInvertion(EXE_VAR *exeVar,          /* execution variables */
                         SCOPE_VAR *scopeVar)      /* scope variables */
{
   char *db;               /* data base name */
   char *keysDB;           /* keys data base name */
   LONGX idxControl;        /* control record index */
   LONGX pos;               /* current position */
   CPL_COMMAND cmd;        /* extract FST host */
   char *sortLength = "80";   /* sort lenght */

   /* Get database name, if missing: error */
   db = exeDB(exeVar,scopeVar);
   if (!db) return 0;

   /* Check mandatory parameters */
   if (!scopeVar->parmList[PARM_KEYSDB]) {
      if (exeFileTemp(exeVar,exeVar->buff)) return 0;
      scopeVar->parmList[PARM_KEYSDB] = strdup(exeVar->buff);
   }
   keysDB = exeRequiredParm(exeVar,scopeVar,ATVALUE_KEYSDB);
   if (!keysDB) return 0;
   if (!exeRequiredDefine(exeVar,scopeVar,PRECISE_STATUS)) {
      exeError(exeVar->program,EXE_ERROR_MISSING,preciseList[PRECISE_STATUS]);
      return 0;
   }

   /* Set define tags and FST host */
   if (!scopeVar->defineList[PRECISE_KEY].tag)
      scopeVar->defineList[PRECISE_KEY].tag = FULLINV_TAG_KEY;
   memset(&cmd,0x00,sizeof(CPL_COMMAND));
   cmd.element = ELEMENT_EXTRACT;

   /* Set define value */
   scopeVar->defineList[PRECISE_STATUS].value = -1L;

   /* Get control record index */
   idxControl = exeNewRecord(exeVar,sizeof(M0STRU));
   if (idxControl < 0L) return exeTaskFullInvertionReturn(scopeVar,idxControl);

   /* Exclusive write lock */
   if (eci_lock(idxControl,db,0L,EWLOCK)) {
      scopeVar->defineList[PRECISE_STATUS].value = LOCK_ERROR_EW_DENIED;
      return exeTaskFullInvertionReturn(scopeVar,idxControl);
   }
   scopeVar->defineList[PRECISE_STATUS].value = LOCK_ERROR_OK;

   /* Reinitialize links extraction database */
   recisis0(keysDB);

   /* Loop all records */
   for (pos = 1L; ; pos++) {

      /* Read record */
      record(scopeVar->idxCurr,db,pos);
      if (VRECrc(scopeVar->idxCurr) == RCEOF) break;
      if (VRECrc(scopeVar->idxCurr) != RCNORMAL) continue;

      /* Extract keys */
      if (exeExtract(exeVar,scopeVar,&cmd))
         return exeTaskFullInvertionReturn(scopeVar,idxControl);

   } /* for */

   /* Get sort parameters */

   scopeVar->parmList[PARM_DB] = scopeVar->parmList[PARM_KEYSDB];
   sprintf(exeVar->buff,"%d",scopeVar->defineList[PRECISE_KEY].tag);
   scopeVar->parmList[PARM_KEYFIELD] = strdup(exeVar->buff);
   scopeVar->parmList[PARM_KEYLENGTH] = strdup(sortLength);

   /* Sort extracted keys database */
   exeTaskMasterSort(exeVar,scopeVar,NULL);
   if (exeVar->program->error.code) {
      scopeVar->parmList[PARM_DB] = db;
      return exeTaskFullInvertionReturn(scopeVar,idxControl);
   }

   /* Restore original database parameter */
   scopeVar->parmList[PARM_DB] = db;

   /* Inverted load from sorted extracted keys database */
   exeTaskInvertedLoad(exeVar,scopeVar,FALSE);
   if (exeVar->program->error.code)
      return exeTaskFullInvertionReturn(scopeVar,idxControl);

   /* Remove temporary files */
   remove(keysDB);
   dbxflush(keysDB);
   sprintf(exeVar->buff,"%s.xrf",keysDB);
   remove(exeVar->buff);
   sprintf(exeVar->buff,"%s.mst",keysDB);
   remove(exeVar->buff);

   /* Return */
   return exeTaskFullInvertionReturn(scopeVar,idxControl);

} /* exeTaskFullInvertion */

/* ============================================================= exeListSort */
EXE_ERROR exeListSort(EXE_VAR *exeVar,          /* execution variables */
                      SCOPE_VAR *scopeVar,      /* scope variables */
                      CPL_COMMAND *cmd,         /* sort command */
                      LONGX idx)                 /* record index */
{
   STRUCT_FREQLIST *freqPos;     /* frequency list current position */
   EXE_FORMAT *fmt;              /* format parameter structure */

   /* Get sort format */
   if (!cmd->sub) return exeError(exeVar->program,EXE_ERROR_SORT,cplElement[ELEMENT_PFT].text);
   fmt = (EXE_FORMAT *)(cmd->sub->altn);
   if (!fmt) return exeError(exeVar->program,EXE_ERROR_SORT,cplElement[ELEMENT_PFT].text);

   /* Loop all loaded itens */
   for (freqPos = exeVar->freq.head; freqPos; freqPos = freqPos->next) {

      /* Clear sort record */
      eci_field_D(&(exeVar->program->error),idx,0);

      /* Set define values */
      scopeVar->defineList[PRECISE_ITEM].text = freqPos->item;
      scopeVar->defineList[PRECISE_VALUE].value = freqPos->qtt;

      /* Field update defines */
      exeDefines(exeVar,scopeVar,idx);

      /* Format sort key */
      if (fmt_inter(fmt->code,idx,exeVar->buffSize,exeVar->buff,exeVar->buffSize) < 0L)
         return exeError(exeVar->program,EXE_ERROR_FORMAT,fmterrxy);

      /* Store sort key */
      freqPos->sort = strdup(exeVar->buff);
      if (!freqPos->sort)
         return exeError(exeVar->program,EXE_ERROR_ALLOC,exeVar->buff);

   } /* for */

   /* Sort frequency list */
   freq_sort(&(exeVar->freq));

   /* Return no error */
   return EXE_ERROR_OK;

} /* exeListSort */

/* ============================================================= exeTaskList */
void exeTaskList(EXE_VAR *exeVar,         /* execution variables */
                 SCOPE_VAR *scopeVar,     /* scope variables */
                 CPL_COMMAND *cmd)        /* first command */
{
   STRUCT_FREQLIST *freqPos;  /* frequency current position */
   LONGX from;                 /* first item */
   LONGX to;                   /* last item */
   LONGX count;                /* quantity limit */
   LONGX pos;                  /* current position */
   LONGX relPos = 0L;          /* relative current position */
   BOOLEAN reverse;           /* reverse flag */

   /* Check if any freq was previously loaded */
   if (!exeVar->freq.head) return;

   /* Check mandatory parameters */
   if (!scopeVar->defineList[PRECISE_ITEM].tag) {
      exeError(exeVar->program,EXE_ERROR_MISSING,preciseList[PRECISE_ITEM]);
      return;
   }
   if (scopeVar->defineList[PRECISE_ITEM].tag < 0) {
      exeError(exeVar->program,EXE_ERROR_INVALID,preciseList[PRECISE_ITEM]);
      return;
   }

   /* Get range limit */
   from = exeNumber(exeVar,scopeVar,PARM_FROM,1L,LONGX_MAX,1L);
   to = exeNumber(exeVar,scopeVar,PARM_TO,from,LONGX_MAX,LONGX_MAX);
   count = exeNumber(exeVar,scopeVar,PARM_COUNT,1L,LONGX_MAX,LONGX_MAX);
   if (exeVar->program->error.code) return;
   reverse = (BOOLEAN)(exePrecise(scopeVar->parmList[PARM_REVERSE]) == PRECISE_ON);

   /* Set define values */
   scopeVar->defineList[PRECISE_FROM].value = from;
   scopeVar->defineList[PRECISE_TO].value = to;
   scopeVar->defineList[PRECISE_ITENS].value = exeVar->freq.item_tot;
   scopeVar->defineList[PRECISE_ITEMS].value = exeVar->freq.item_tot;
   scopeVar->defineList[PRECISE_TOTAL].value = exeVar->freq.freq_tot;

   /* Sort frequency, if required */
   if (scopeVar->parmList[PARM_SORT]) {

      /* Find sort parameter backward */
      if (!cmd->func) cmd->func = exeFindParm(scopeVar->father->prev,ATVALUE_SORT);

      /* Call list sort */
      exeListSort(exeVar,scopeVar,cmd->func,scopeVar->idxCurr);

   }

   /* Loop range of itens */
   for (pos = 1L,freqPos = reverse ? exeVar->freq.tail : exeVar->freq.head; relPos < count && freqPos; pos++,freqPos = reverse ? freqPos->prev : freqPos->next) {

      /* Check limits */
      if (pos < from) continue;
      if (pos > to) break;
      relPos++;

      /* Clear frequency record */
      eci_field_D(&(exeVar->program->error),scopeVar->idxCurr,0);

      /* Set define values */
      scopeVar->defineList[PRECISE_ITEM].text = freqPos->item;
      scopeVar->defineList[PRECISE_VALUE].value = freqPos->qtt;
      scopeVar->defineList[PRECISE_CURRENT].value = pos;

      /* Call record execution */
      exeGo(exeVar,scopeVar,cmd);
      if (scopeVar->skip == PRECISE_QUIT) break;

   } /* for */

   /* Reset define values */
   scopeVar->defineList[PRECISE_ITEM].text = NULL;

} /* exeTaskList */

/* =========================================================== exeTaskImport */
void exeTaskImport(EXE_VAR *exeVar,          /* execution variables */
                   SCOPE_VAR *scopeVar,      /* scope variables */
                   CPL_COMMAND *cmd)         /* first command */
{
   char *fileName;            /* export file name */
   char *p;                   /* auxiliary string buffer pointer */
   FILE *importFile;          /* import file pointer */
   PRECISE_LIST typeValue;    /* import file type value */
   LONGX count;                /* quantity limit */
   LONGX pos;                  /* current position */
   ECI_ERROR errorCode;

   /* Get range limit */
   count = exeNumber(exeVar,scopeVar,PARM_COUNT,1L,LONGX_MAX,LONGX_MAX);
   if (exeVar->program->error.code) return;

   /* Open import file */
   fileName = exeRequiredParm(exeVar,scopeVar,ATVALUE_FILE);
   if (!fileName) return;
   p = dbxcipar(NULL,fileName,'=');
   importFile = fopen(p,"r");
   if (importFile == NULL) {
      exeError(exeVar->program,EXE_ERROR_FILE,p);
      return;
   }
   typeValue = exePrecise(scopeVar->parmList[PARM_TYPE]);

   /* Loop all import file */
   for (pos = 1L; pos <= count; pos++) {

      /* Get next import record */
      errorCode = exeImport(exeVar,scopeVar,importFile,typeValue,pos);
      if (errorCode) {
         if (errorCode == ECI_ERROR_EOF) break;
         exeError(exeVar->program,EXE_ERROR_IMPORT,NULL);
         break;
      }

      /* Set define current position */
      scopeVar->defineList[PRECISE_CURRENT].value = pos;

      /* Call record execution */
      exeGo(exeVar,scopeVar,cmd);
      if (scopeVar->skip == PRECISE_QUIT) break;

   } /* for */

   /* Close import file */
   fclose(importFile);

} /* exeTaskImport */

/* =========================================================== exeTaskUpdate */
void exeTaskUpdate(EXE_VAR *exeVar,             /* execution variables */
                   SCOPE_VAR *scopeVar,         /* scope variables */
                   CPL_COMMAND *cmd)            /* first command */
{
   char *db;                                 /* data base name */
   char *mfnText;                   /* MFN text */
   CPL_ATVALUE mfnValue;            /* MFN option value */
   LONGX mfn;                        /* MFN */

   /* Get required parameters */
   db = exeDB(exeVar,scopeVar);
   if (!db) return;
   mfnText = exeRequiredParm(exeVar,scopeVar,ATVALUE_MFN);
   if (!mfnText) return;
   mfnValue = exePrecise(mfnText);
   mfn = atol(mfnText);

   /* If not new record, read original record */
   if (mfnValue == PRECISE_GETNEW)
      eci_rec_copy(scopeVar->idxCurr,scopeVar->idxPrev);
   else
      if (mfnValue != PRECISE_NEW) record(scopeVar->idxCurr,db,mfn);

   /* Valid only at the update scope [0.9g] */
   scopeVar->defineList[PRECISE_LOCK].apply = FALSE;
   scopeVar->defineList[PRECISE_STATUS].apply = FALSE;

   /* Call record execution */
   exeGo(exeVar,scopeVar,cmd);

} /* exeTaskUpdate */

/* ================================================================== exePos */
LONGX exePos(BOOLEAN reverse,     /* reverse flag */
            LONGX last,           /* last position */
            LONGX pos)            /* orignal position */
{

   /* If reverse: return complement postition, else: return current position */
   return reverse ? (last - pos) + 1L : pos;

} /* exePos */

#if !CIB71
/* ================================================================== b5_msg */
int b5_msg(int tline,
           char *msg,
           LONGX docs,
           int tmsg)
{
   extern char b5_pfxmsg[];   /* prefix message */
   char buff[LE2+LE2+1];      /* auxiliary string buffer */
   EFC_ERROR error;           /* error structure */

   /* If no target message index and field: quit */
   if (b5msg_idx < 0L || b5msg_tag < 1) return 0;

   /* Store search message for highlight */
   if (tmsg == 1 && docs) {
      sprintf(buff,"%-.*s%-.*s",LE2,b5_pfxmsg,LE2,msg);
      eci_field_AT(&error,b5msg_idx,b5msg_tag,buff);
   }

   /* Return OK */
   return 0;

}

#else /* CIB71 */

/* ================================================================== b7_msg */
int b7_msg(b7_CIB7 *cib7p,
              int tline,
           char *msg,
           LONGX psts,
           LONGX docs,
           int tmsg)
{
   char buff[LE2+LE2+1];      /* auxiliary string buffer */
   EFC_ERROR error;           /* error structure */

   /* If no target message index and field: quit */
   if (b5msg_idx < 0L || b5msg_tag < 1) return 0;

   /* Store search message for highlight */
   if (tmsg == 1 && docs)
   {
       if (cib7p->nqualts) /* tagged key: plants/(24,70) */
    {
        for (cib7p->qualtp=cib7p->vqualt; *cib7p->qualtp; cib7p->qualtp++)
          {
        sprintf(buff,"%d %-.*s",*cib7p->qualtp,LE2,msg);
            eci_field_AT(&error,b5msg_idx,b5msg_tag,buff);
          }
    }
      else /* normal key: PLANTS or prefixed key: TI PLANTS */
      {
        sprintf(buff,"%-.*s%-.*s",LE2,cib7p->b7_pfxmsg,LE2,msg);
        eci_field_AT(&error,b5msg_idx,b5msg_tag,buff);
      }
   }

   /* Return OK */
   return 0;

}
#endif /* CIB71 */

/* ================================================================== b5_kbh */
int b5_kbh(void)
{

   /* Return no interruption */
   return 0;

}

/* ================================================================== exeHit */
LONGX exeHit
(
    b7_CIB7 *cib7p,
    LONGX idx,       /* search hit record index */
    LONGX hit            /* request hit */
)
{

   /* Return MFN of requested hit */
#if !CIB71
   return b6_hit(idx,hit,VMFRmfn(idx),b6bufferp);
#else /* !CIB71 */
   return b7_hit(cib7p,idx,hit,VMFRmfn(idx));
#endif /* !CIB71 */

} /* exeHit */

/* ==================================================== exeSearchCisisReturn */
SEARCH_ERROR exeSearchCisisReturn(EFC_ERROR *err,           /* error structure */
                                  LONGX idxControl,          /* record index to be freed */
                                  char *buff,               /* buffer to be freed */
                                  SEARCH_ERROR errorCode,   /* error code */
                                  char *msg)                /* error message */
{

   /* Garbage collector */
   if (idxControl >= -1L) eci_rec_free(idxControl);
   efc_free(buff);

   /* Return error code */
   return efc_error(err,errorCode,msg);

} /* exeSearchCisisReturn */

/* ======================================== exeSearchCisisPropagateQualifier */
void exeSearchCisisPropagateQualifier
(
    EFC_ERROR *err,         /* error structure */
    LONGX idx,
   char *buff
)
{
    char *expr;
    char *e;
    char *p;
    char qlf[LE2+1] = "";
   char *q;

    eci_field(idx,MFQTOBJ0,1,buff);
   expr = strdup(buff);
/* printf("<!-- antes : %s -->\n",expr); */

    for (e = expr,p = buff; *e; e++)
   {
        if ( *e == '[' )
        {
            if ( *(e+1) == '/' )
         {
            *p++ = '[';
               for (q = qlf; *++e; q++)
            {
            *p++ = *q = *e;
             if ( *e == TOKDELIM )
                {
                    break;
                }
            }
                *q = '\0';
            continue;
         }
         else
         {
            qlf[0] = '\0';
         }
        }
        if ( *e == '_' && qlf[0] != '\0' )
        {
            if ( *(p-1) != TOKDELIM )
         {
                *p++ = TOKDELIM;
            }
            strcpy(p,qlf);
         p += strlen(qlf);
         *p++ = TOKDELIM;
        }
        *p++ = *e;
    }
    *p='\0';

    eci_field_MT(err,idx,MFQTOBJ0,buff);
/* printf("<!-- depois: %s -->\n",buff); */

} /* exeSearchCisisPropagateQualifier */

/* ========================================================== exeSearchCisis */
SEARCH_ERROR exeSearchCisis
(
    EFC_ERROR *err,         /* error structure */
    b7_CIB7 *cib7p,
    LONGX idxHit,            /* search hit record index */
    char *db,               /* database name */
    char *expression,       /* search expression */
   char *dbLog,         /* search log database */
   LONGX setNumber,      /* log set number */
    char *tempFile,         /* temporary file name */
    char *pfxList,          /* prefix list */
    LONGX idxPfx,            /* search prefix record index */
    int tag                 /* message tag */
)
{
   char *searchErrorText;        /* search error point */
   char *searchPrefix;           /* search prefix auxiliary buffer */
   char *defSearchPrefix = "^p*^y"; /* default search prefix */
   char *buff = NULL;            /* auxiliary buffer */
   LONGX idxControl;              /* auxiliary search control record index */
   int searchErrorCode;          /* search error code */
#if !CIB71
   LONGX nbytes1;                 /* bit string space */
#endif /* CIB71 */

   /* Get auxiliary record index: prefix and control */
   idxControl = eci_rec_new(err,EXE_REC_SIZE/*sizeof(M0STRU)*/);
   if (idxControl < 0L) return SEARCH_ERROR_ALLOC;

   /* Set search prefix index list */
   if (pfxList)
      if (!*pfxList) pfxList = NULL;
   if (pfxList) eci_field_ML(err,idxPfx,MFQTXTAG,pfxList);
   else {
      searchPrefix = efc_new(strlen(defSearchPrefix)+strlen(db)+1);
      if (!searchPrefix)
         return exeSearchCisisReturn(err,idxControl,buff,SEARCH_ERROR_ALLOC,NULL);
      sprintf(searchPrefix,"%s%s",defSearchPrefix,db);
      eci_field_MT(err,idxPfx,MFQTXTAG,searchPrefix);
      efc_free(searchPrefix);
   }

   /* Search needed parameters */
   VMFRmfn(idxHit) = 0L;
   if ( dbLog )
   {
    record(idxControl,dbLog,0L);
   }
    record(idxControl,db,0L);
#if !CIB71
    nbytes1 = (VMF0nxtmfn(idxControl) / 8) + 1L;
    if (nbytes1 > ALLOMAXV) nbytes1 = ALLOMAXV;
#endif /* !CIB71 */
   buff = efc_new(EXE_REC_SIZE);
   if (!buff) return exeSearchCisisReturn(err,idxControl,buff,SEARCH_ERROR_ALLOC,NULL);

   /* Call search expression */
#if !CIB71
   searchErrorText = b5_exp(idxHit,db,expression,buff,NULL,idxControl,&searchErrorCode);
#else /* !CIB71 */
   searchErrorText = b7_exp(cib7p,idxHit,db,expression,buff,dbLog,idxControl,&searchErrorCode);
#endif /* !CIB71 */

   /* Check search expression error code */
   if (searchErrorCode)
      return exeSearchCisisReturn(err,idxControl,buff,SEARCH_ERROR_SYNTAX,searchErrorText);

   /* Propagating field qualifier */
//      exeSearchCisisPropagateQualifier(err,idxHit,buff);

   /* Add search temporary file name */
   if ( dbLog )
   {
    sprintf(tempFile,"%s.%03d",dbLog,VMF0nxtmfn(idxControl)-1L);
    eci_field_MT(err,idxHit,MFQTHMEM,tempFile);
      *tempFile = '\0';
   }
   else if (SRC_TMP_FILE)
   {
    eci_field_MT(err,idxHit,MFQTHMEM,tempFile);
   } else {           // HB 20070320
    char *hitfnamp="null";
    char bup[512];
    sprintf(bup,"D%dA%d|%s|",MFQTHMEM,MFQTHMEM,hitfnamp);
    if (fldupdat(idxHit,bup) != NULL) fatal("mxbol/fldupdat");
   }

   /* Set global parameters, call search run, reset global parameters */
   b5msg_idx = idxPfx;
   b5msg_tag = tag;
#if !CIB71
   searchErrorText = b6_run(idxHit,db,buff,EXE_REC_SIZE,nbytes1,2L,isisuctab,"",idxPfx,idxControl,&searchErrorCode);
#else /* CIB71 */
   searchErrorText = b7_run(cib7p,idxHit,db,buff,EXE_REC_SIZE,2L,isisuctab,"",idxPfx,idxControl,&searchErrorCode);
#endif /* CIB71 */
   b5msg_idx = -1L;
   b5msg_tag = 0;

   /* Check search run error code */
   if (searchErrorCode)
      return exeSearchCisisReturn(err,idxControl,buff,SEARCH_ERROR_RUN,searchErrorText);

   /* Return error code */
   return exeSearchCisisReturn(err,idxControl,buff,SEARCH_ERROR_OK,NULL);

} /* exeSearchCisis */

/* ===================================================== exeTaskSearchReturn */
int exeTaskSearchReturn(SCOPE_VAR *scopeVar,       /* scope variables */
                        SEARCH_VAR *searchVar)     /* search variables */
{

   /* Previous call will be able to see the search result */
   scopeVar->defineList[PRECISE_STATUS].apply = TRUE;
   scopeVar->defineList[PRECISE_ERRORINFO].apply = TRUE;

   /* Garbage collector */
   /* <R> */
   if( searchVar->dbLog )
   {
    dbxflush( searchVar->dbLog );
   }
   /* </R> */
#if CIB71
    if (searchVar->cib7p) b7_cib7_delete(searchVar->cib7p);
#endif /* CIB71 */
    if (*searchVar->tempFile) remove(searchVar->tempFile);
   if (searchVar->idxHit > -1L) eci_rec_free(searchVar->idxHit);
   if (searchVar->idxPfx > -1L) eci_rec_free(searchVar->idxPfx);

   /* Return */
   return 0;

} /* exeTaskSearchReturn */

/* =============================================================== exeTaskSearch */
int exeTaskSearch(EXE_VAR *exeVar,           /* execution variables */
                  SCOPE_VAR *scopeVar,       /* scope variables */
                  CPL_COMMAND *cmd)          /* first command */
{
   char *db;               /* data base name */
   char *expression;       /* expression */
   LONGX setNumber = 0L;     /* log set number */
   SEARCH_VAR searchVar;   /* search variables */
   LONGX from;              /* first hit */
   LONGX to;                /* last hit */
   LONGX count;             /* quantity limit */
   BOOLEAN reverse;        /* reverse flag */
   LONGX pos;               /* current position */
   LONGX relPos = 0L;       /* relative position */
   int hlTag;              /* highlight term list field number */
   EFC_ERROR err;          /* error structure */

   /* Get data base name, if missing: error */
   db = exeDB(exeVar,scopeVar);
   if (!db) return 0;

   /* search parameters */
   expression = exeRequiredParm(exeVar,scopeVar,ATVALUE_EXPRESSION);
   if (!expression) return 0;

   /* Setup search variables */
   searchVar.dbLog = NULL;
   if (scopeVar->parmList[PARM_LOG])
      if (*(scopeVar->parmList[PARM_LOG]))
         searchVar.dbLog = scopeVar->parmList[PARM_LOG];
   *(searchVar.tempFile) = '\0';
   searchVar.idxHit = exeWorkRecord(exeVar);
   searchVar.idxPfx = exeNewRecord(exeVar,SEARCH_PREFIX_SIZE);
   if (searchVar.idxHit < 0L || searchVar.idxPfx < 0L) {
      exeError(exeVar->program,EXE_ERROR_ALLOC,NULL);
      return exeTaskSearchReturn(scopeVar,&searchVar);
   }
   eci_rec_active(searchVar.idxPfx);
   /* Allocate work area */
#if !CIB71
    if (!b6bufferp) b6_hballoc(SEARCH_AUX_SIZE,&b6bufferp);
#else /* !CIB71 */
    searchVar.cib7p = NULL;
//  searchVar.cib7p = (b7_CIB7 *)b7_cib7(searchVar.cib7p);
    searchVar.cib7p = b7_cib7(searchVar.cib7p,NULL);
//  b7_hballoc(searchVar.cib7p,SEARCH_AUX_SIZE,&(searchVar.cib7p->b7bufferp));
#endif /* !CIB71 */

   /* get temporary file */
   if ( !searchVar.dbLog && SRC_TMP_FILE)
   {
       if (!dbxtmpnm("CI_TEMPDIR", 0, searchVar.tempFile))
    {
        exeTaskSearchReturn(scopeVar,&searchVar);
          return exeError(exeVar->program,EXE_ERROR_SEARCH,"temporary file");
    }
   }

   /* Set defines parameters */
   hlTag = scopeVar->defineList[PRECISE_KEYS].tag;
   scopeVar->defineList[PRECISE_KEYS].apply = FALSE;
   scopeVar->defineList[PRECISE_STATUS].apply = FALSE;
   scopeVar->defineList[PRECISE_ERRORINFO].apply = FALSE;

   /* Call CISIS search */
   if (exeSearchCisis(&err,searchVar.cib7p,searchVar.idxHit,db,expression,searchVar.dbLog,setNumber,searchVar.tempFile,
      scopeVar->parmList[PARM_INDEXLIST],searchVar.idxPfx,hlTag)) {
      if (err.code == SEARCH_ERROR_SYNTAX || err.code == SEARCH_ERROR_RUN) {
         scopeVar->defineList[PRECISE_STATUS].value = err.code;
         scopeVar->defineList[PRECISE_ERRORINFO].text = efc_strrepl(scopeVar->defineList[PRECISE_ERRORINFO].text,err.info);
         return exeTaskSearchReturn(scopeVar,&searchVar);
      }
      exeError(exeVar->program,err.code,err.info);
      return exeTaskSearchReturn(scopeVar,&searchVar);
   }

   /* Get range limit */
   from = exeNumber(exeVar,scopeVar,PARM_FROM,1L,LONGX_MAX,1L);
   to = exeNumber(exeVar,scopeVar,PARM_TO,from,LONGX_MAX,LONGX_MAX);
   count = exeNumber(exeVar,scopeVar,PARM_COUNT,1L,LONGX_MAX,LONGX_MAX);
   if (exeVar->program->error.code)
      return exeTaskSearchReturn(scopeVar,&searchVar);
   reverse = (BOOLEAN)(exePrecise(scopeVar->parmList[PARM_REVERSE]) == PRECISE_ON);

   /* Set define values */
#if CIB71
   scopeVar->defineList[PRECISE_SETNUMBER].value = searchVar.cib7p->b7setno;
#endif /* CIB71 */
   scopeVar->defineList[PRECISE_TOTAL].value = exeHit(searchVar.cib7p,searchVar.idxHit,0L);
   scopeVar->defineList[PRECISE_FROM].value = from;
   scopeVar->defineList[PRECISE_TO].value = to;

   /* Check search quantity of hits retrieved */
   if (scopeVar->defineList[PRECISE_TOTAL].value <= 0)
      return exeTaskSearchReturn(scopeVar,&searchVar);

   /* Loop range of hits */
   for (pos = from; pos <= to; pos++) {

      /* Check count and total limit */
      if (relPos++ >= count) break;
      if (pos > scopeVar->defineList[PRECISE_TOTAL].value) break;

      /* Read record, check record status */
      record(scopeVar->idxCurr,db,exeHit(searchVar.cib7p,searchVar.idxHit,exePos(reverse,scopeVar->defineList[PRECISE_TOTAL].value,pos)));
      if (VRECrc(scopeVar->idxCurr) != RCNORMAL) continue;

      /* Set define current position, copy highlight fields */
      scopeVar->defineList[PRECISE_CURRENT].value = pos;
      if (hlTag)
         eci_field_copy(&(exeVar->program->error),TRUE,scopeVar->idxCurr,hlTag,searchVar.idxPfx,hlTag);

      /* Call record execution */
      exeGo(exeVar,scopeVar,cmd);
      if (scopeVar->skip == PRECISE_QUIT) break;

   } /* for */

   /* Return */
   return exeTaskSearchReturn(scopeVar,&searchVar);

} /* exeTaskSearch */

/* ================================================================== exeKey */
void exeKey(char *target,        /* target key buffer */
            char *parameter)     /* key parameter */
{

   /* Reset target content, copy parameter, convert with current upper case table */
   memset(target,0x00,LE2+1);
   if (parameter) strncpy(target,parameter,LE2);
   eci_uctab(target);

} /* exeKey */

/* ============================================================== exeNextKey */
void exeNextKey(BOOLEAN reverse,             /* reverse flag */
                LONGX idx,                    /* current term index */
                STRUCT_PREVTERM *prevTrm)    /* previous term structure */
{

   /* If reverse, return previous term, else return next term */
   if (reverse)
      if (TermPrevious(prevTrm,idx,"",VTRMkey(idx)) < 0L) VTRMrc(idx) = RCEOF;
      else VTRMrc(idx) = RCNORMAL;
   else nxterm(idx);

} /* exeNextKey */

/* =================================================== exeTaskKeyrangeReturn */
int exeTaskKeyrangeReturn
(
    LONGX idxTerm,                               /* term index */
   EFC_NUMBER_LIST *listPostTagValue    /* splited line structure */
)
{
   /* Garbage collector */
   if (idxTerm > -1L) eci_trm_free(idxTerm);
    efc_numberListFree(listPostTagValue);

   /* Return */
   return 0;

} /* exeTaskKeyrangeReturn */

/* ========================================================= exeTaskKeyrange */
int exeTaskKeyrange(EXE_VAR *exeVar,            /* execution variables */
                    SCOPE_VAR *scopeVar,        /* scope variables */
                    CPL_COMMAND *cmd)           /* first command */
{
   char *db;               /* data base name */
   LONGX idxTerm = -1L;     /* term index */
   char from[LE2+1];       /* first key */
   char to[LE2+1];         /* last key */
   LONGX count;             /* quantity limit */
   BOOLEAN reverse;        /* reverse flag */
   LONGX postingValue;      /* parameter posting value */
   EFC_NUMBER_LIST listPostTagValue = { 0, NULL };  /* parameter posting tag value list */
    EFC_ERROR efcError = { 0, "" };                     /* efc module error locator */
    int line;                   /* text lit line count */
   LONGX post;              /* posting index */
   LONGX postTagCount;      /* posting tag index */
   char *postFlag;         /* posting required flag */
   STRUCT_PREVTERM prevTrm;   /* previous term structure */
   char key[LE2+1];        /* current key */
   LONGX relPos = 0L;       /* relative position */

   /* Get data base name, if missing: error */
   db = exeRequiredParm(exeVar,scopeVar,ATVALUE_DB);
   if (!db) return 0;

   /* Get range limit */
   exeKey(from,scopeVar->parmList[PARM_FROM]);
   exeKey(to,scopeVar->parmList[PARM_TO]);
   count = exeNumber(exeVar,scopeVar,PARM_COUNT,1L,LONGX_MAX,LONGX_MAX);
   postingValue = exeNumber(exeVar,scopeVar,PARM_POSTING,0L,LONGX_MAX,0L);
   if (scopeVar->parmList[PARM_POSTTAG])
   {
        if (efc_numberList(&efcError,&listPostTagValue,scopeVar->parmList[PARM_POSTTAG]) < 0L)
      {
        exeError(exeVar->program,EXE_ERROR_ALLOC,efcError.info);
        return exeTaskKeyrangeReturn(idxTerm,&listPostTagValue);
      }
   }
   reverse = (BOOLEAN)(exePrecise(scopeVar->parmList[PARM_REVERSE]) == PRECISE_ON);

   /* Get local term index */
   idxTerm = eci_trm_new(&(exeVar->program->error));
   if (idxTerm < 0L) {
      exeError(exeVar->program,EXE_ERROR_MAXNTRM,NULL);
      return exeTaskKeyrangeReturn(idxTerm,&listPostTagValue);
   }

   /* Set parameters */
   scopeVar->defineList[PRECISE_FROM].text = efc_strrepl(scopeVar->defineList[PRECISE_FROM].text,from);
   scopeVar->defineList[PRECISE_TO].text = efc_strrepl(scopeVar->defineList[PRECISE_TO].text,to);
   postFlag = scopeVar->parmList[PARM_POSTING];
   if (postFlag)
      if (!*postFlag) postFlag = NULL;
   if (listPostTagValue.qtt && !postFlag) postFlag = scopeVar->parmList[PARM_POSTTAG];

   /* Start previous term sctructure */
   IniPreviousTerm(&prevTrm);

   /* Call term, check end of inverted */
   term(idxTerm,db,from);
   if (reverse)
      if (VTRMrc(idxTerm) != RCNORMAL) exeNextKey(reverse,idxTerm,&prevTrm);
   if (VTRMrc(idxTerm) == RCEOF) return exeTaskKeyrangeReturn(idxTerm,&listPostTagValue);

   /* Loop range of terms */
   for ( ; ; exeNextKey(reverse,idxTerm,&prevTrm)) {

      /* Check end of inverted, check quantity of itens limit */
      if (VTRMrc(idxTerm) == RCEOF) break;
      if (relPos >= count) break;

      /* Check end of range */
      if (*to)
         if (reverse) {
            if (strcmp(VTRMkey(idxTerm),to) < 0) break;
         } else
            if (strcmp(VTRMkey(idxTerm),to) > 0) break;

      /* Get total of postings */
      scopeVar->defineList[PRECISE_POSTINGS].value = posting(idxTerm,0L);
      if (scopeVar->defineList[PRECISE_POSTINGS].value <= 0L) continue;

      /* Trim right current key */
      efc_trim_right(strcpy(key,VTRMkey(idxTerm)));
      scopeVar->defineList[PRECISE_KEY].text = efc_strrepl(scopeVar->defineList[PRECISE_KEY].text,key);

      /* Loop all postings */
      for (post = 1L,postTagCount = 1L; /* no error */; post++) {

         /* Posting information required */
         if (postFlag) {

            /* Get current posting information */
            if (posting(idxTerm,post) < 1L) break;

            /* Check posting tag value list */
                if (listPostTagValue.qtt > 0)
            {
                 for (line = 0L; line < listPostTagValue.qtt; line++)
               {
                if (VTRMptag(idxTerm) == listPostTagValue.list[line]) break;
               }
               if (line >= listPostTagValue.qtt) continue;
            }

            /* Check posting limit */
            if (postingValue)
               if (listPostTagValue.qtt > 0) {
                  if (postTagCount > postingValue) break;
                  else postTagCount++;
               } else
                  if (post > postingValue) break;

            /* Make posting information buffer */
            if (scopeVar->defineList[PRECISE_POSTING].tag) {
               sprintf(exeVar->buff,"^i%"_LD_"^m%"_LD_"^t%d^o%d^c%d",post,VTRMpmfn(idxTerm),
                  VTRMptag(idxTerm),VTRMpocc(idxTerm),VTRMpcnt(idxTerm));
               scopeVar->defineList[PRECISE_POSTING].text = efc_strrepl(scopeVar->defineList[PRECISE_POSTING].text,exeVar->buff);
            }

         }

         /* Check quantity of itens limit */
         if (relPos++ >= count) break;

         /* Set define current position */
         scopeVar->defineList[PRECISE_CURRENT].value = relPos;

         /* Call record execution */
         exeGo(exeVar,scopeVar,cmd);
         if (scopeVar->skip == PRECISE_QUIT) break;

         /* Check posting loop */
         if (!postFlag) break;

      } /* for */

      /* Check skip flag */
      if (scopeVar->skip == PRECISE_QUIT) break;

   } /* for */

   /* Return */
   return exeTaskKeyrangeReturn(idxTerm,&listPostTagValue);

} /* exeTaskKeyrange */

/* =============================================================== exeTaskDo */
void exeTaskDo(EXE_VAR *exeVar,           /* execution variables */
               SCOPE_VAR *scopeVar,       /* scope variables */
               CPL_COMMAND *cmd)          /* first command */
{
   LONGX from;              /* first MFN */
   LONGX to;                /* last MFN */
   LONGX count;             /* quantity limit */
   BOOLEAN reverse;        /* reverse flag */
   LONGX pos;               /* current position */
   LONGX relPos = 0L;       /* relative position */

   /* Get range limit */
   from = exeNumber(exeVar,scopeVar,PARM_FROM,1L,LONGX_MAX,1L);
   to = exeNumber(exeVar,scopeVar,PARM_TO,from,LONGX_MAX,LONGX_MAX);
   count = exeNumber(exeVar,scopeVar,PARM_COUNT,1L,LONGX_MAX,LONGX_MAX);
   if (exeVar->program->error.code) return;
   reverse = (BOOLEAN)(exePrecise(scopeVar->parmList[PARM_REVERSE]) == PRECISE_ON);

   /* Set define variables */
   scopeVar->defineList[PRECISE_FROM].value = from;
   scopeVar->defineList[PRECISE_TO].value = to;
   scopeVar->defineList[PRECISE_TOTAL].value = to;

   /* Loop range of limits */
   for (pos = from; pos <= to; pos++) {

      /* Check quantity of records limit */
      if (relPos++ >= count) break;

      /* Set define current position */
      /* scopeVar->defineList[PRECISE_CURRENT].value = pos */
      scopeVar->defineList[PRECISE_CURRENT].value = exePos(reverse,to,pos); /* 0.9g */

      /* Clear record */
      if (eci_field_D(&(exeVar->program->error),scopeVar->idxCurr,0)) {
         exeError(exeVar->program,EXE_ERROR_FIELD,NULL);
         return;
      }

      /* Call record execution */
      exeGo(exeVar,scopeVar,cmd);
      if (scopeVar->skip == PRECISE_QUIT) break;

   } /* for */

} /* exeTaskDo */

/* ========================================================= exeTaskMfnrange */
void exeTaskMfnrange(EXE_VAR *exeVar,           /* execution variables */
                     SCOPE_VAR *scopeVar,       /* scope variables */
                     CPL_COMMAND *cmd)          /* first command */
{
   char *db;               /* data base name */
   LONGX from;              /* first MFN */
   LONGX to;                /* last MFN */
   LONGX count;             /* quantity limit */
   BOOLEAN reverse;        /* reverse flag */
   LONGX pos;               /* current position */
   LONGX relPos = 0L;       /* relative position */

   /* Get data base name, if missing: error */
   db = exeDB(exeVar,scopeVar);
   if (!db) return;

   /* Get range limit */
   from = exeNumber(exeVar,scopeVar,PARM_FROM,1L,LONGX_MAX,1L);
   to = exeNumber(exeVar,scopeVar,PARM_TO,from,LONGX_MAX,LONGX_MAX);
   count = exeNumber(exeVar,scopeVar,PARM_COUNT,1L,LONGX_MAX,LONGX_MAX);
   if (exeVar->program->error.code) return;
   reverse = (BOOLEAN)(exePrecise(scopeVar->parmList[PARM_REVERSE]) == PRECISE_ON);

   /* Set define variables */
   scopeVar->defineList[PRECISE_TOTAL].value = eci_last_mfn(&(exeVar->program->error),db);
   scopeVar->defineList[PRECISE_FROM].value = from;
   scopeVar->defineList[PRECISE_TO].value = to;

   /* 14.Aug.2000 Avoid Isis_Key field update for the <extract> */
   scopeVar->defineList[PRECISE_KEY].apply = FALSE;

   /* Loop range of MFNs */
   for (pos = from; pos <= to; pos++) {

      /* Check quantity of records limit */
      if (relPos++ >= count) break;

      /* Read record, check record status */
      if (pos > scopeVar->defineList[PRECISE_TOTAL].value) break;
      record(scopeVar->idxCurr,db,exePos(reverse,scopeVar->defineList[PRECISE_TOTAL].value,pos));
      if (VRECrc(scopeVar->idxCurr) == RCEOF) break;
      if (scopeVar->defineList[PRECISE_STATUS].tag)
      {
         scopeVar->defineList[PRECISE_STATUS].value = VRECrc(scopeVar->idxCurr);
         if (VRECgdbw(scopeVar->idxCurr) != 0) scopeVar->defineList[PRECISE_STATUS].value = RCLOCK;
         /* 04.Nov.1999 - The record is locked, but the RECrc contains RCNORMAL */
      }
      else
         if (VRECrc(scopeVar->idxCurr) != RCNORMAL) continue;

      /* Set define current position */
      scopeVar->defineList[PRECISE_CURRENT].value = pos;

      /* Call record execution */
      exeGo(exeVar,scopeVar,cmd);
      if (scopeVar->skip == PRECISE_QUIT) break;
   } /* for */

} /* exeTaskMfnrange */

/* ================================================================= exeLoop */
void exeLoop(EXE_VAR *exeVar,          /* execution variables */
             SCOPE_VAR *scopeVar,      /* scope variables */
             CPL_COMMAND *cmd)         /* current command */
{
   SCOPE_VAR localScopeVar;      /* local variables for sub-command */
   CPL_ATVALUE task;             /* task to be executed */
   char *taskText;               /* task text message */

   /* Copy scope variables */
   memmove(&localScopeVar,scopeVar,sizeof(SCOPE_VAR));

   /* Set local variables */
   localScopeVar.idxPrev = scopeVar->idxCurr;
   localScopeVar.idxCurr = exeWorkRecord(exeVar);
   if (localScopeVar.idxCurr < 0L) {
      exeError(exeVar->program,EXE_ERROR_ALLOC,cplElement[cmd->element].text);
      return;
   }
   localScopeVar.father = cmd;

   /* Get task attribute, if absent get task parameter */
   task = scopeVar->father->attributeList[ATTRIBUTE_TASK].atValue;
   taskText = scopeVar->father->attributeList[ATTRIBUTE_TASK].text;
   if (!task || task >= ATVALUE_LIST_QTT)
      if (scopeVar->parmList[PARM_TASK]) {
         task = cplIdentifier(cplAtValue,ATVALUE_LIST_QTT,scopeVar->parmList[PARM_TASK]);
         taskText = scopeVar->parmList[PARM_TASK];
      }

   /* Check default loop */
   if (!task)
      exeTaskDo(exeVar,&localScopeVar,cmd->sub);
   else {
      /* Evaluate task attribute */
      switch (task) {

      case ATVALUE_FULLINV :
         /* Database Full invertion */
         exeTaskFullInvertion(exeVar,&localScopeVar);
         break;

      case ATVALUE_IMPORT :
         /* Import records */
         exeTaskImport(exeVar,&localScopeVar,cmd->sub);
         break;

      case ATVALUE_INVLOAD :
         /* Inverted load */
         exeTaskInvertedLoad(exeVar,&localScopeVar,TRUE);
         break;

      case ATVALUE_KEYRANGE :
         /* Range of keys */
         exeTaskKeyrange(exeVar,&localScopeVar,cmd->sub);
         break;

      case ATVALUE_LIST :
         /* Loaded list sequence */
         exeTaskList(exeVar,&localScopeVar,cmd->sub);
         break;

      case ATVALUE_MFNRANGE :
         /* Range of records */
         exeTaskMfnrange(exeVar,&localScopeVar,cmd->sub);
         break;

      case ATVALUE_MSTSORT :
         /* Master sort */
         exeTaskMasterSort(exeVar,&localScopeVar,NULL);
         break;

      case ATVALUE_SEARCH :
         /* Search */
         exeTaskSearch(exeVar,&localScopeVar,cmd->sub);
         break;

      case ATVALUE_UPDATE :
         /* Update record */
         exeTaskUpdate(exeVar,&localScopeVar,cmd->sub);
         break;

      default :
         /* Invalid option */
         exeError(exeVar->program,EXE_ERROR_INVALID,taskText);
         break;

      } /* switch */
   }

   /* Transfer defines */
   exeDefines(exeVar,&localScopeVar,scopeVar->idxCurr);

   /* Close scope */
   exeScopeClose(&localScopeVar);

   /* Garbage collector */
   eci_rec_free(localScopeVar.idxCurr);

} /* exeLoop */

/* =================================================================== exeHL */
void exeHL(EXE_VAR *exeVar,         /* execution variables */
           SCOPE_VAR *scopeVar,     /* indexes */
           CPL_COMMAND *cmd)        /* current command */
{
   SCOPE_VAR localScopeVar;      /* local variables for sub-command */

   /* Copy scope variables */
   memmove(&localScopeVar,scopeVar,sizeof(SCOPE_VAR));

   /* Set local variables */
   localScopeVar.idxPrev = scopeVar->idxCurr;
   localScopeVar.idxCurr = exeWorkRecord(exeVar);
   if (localScopeVar.idxCurr < 0L) {
      exeError(exeVar->program,EXE_ERROR_ALLOC,cplElement[cmd->element].text);
      return;
   }
   eci_rec_copy(localScopeVar.idxCurr,scopeVar->idxCurr);
   localScopeVar.father = cmd;

   /* Call sub-command */
   exeGo(exeVar,&localScopeVar,cmd->sub);

   /* Close scope */
   exeScopeClose(&localScopeVar);

   /* Garbage collector */
   eci_rec_free(localScopeVar.idxCurr);

} /* exeHL */

/* =================================================================== exeDo */
void exeDo(EXE_VAR *exeVar,         /* execution variables */
           SCOPE_VAR *scopeVar,     /* indexes */
           CPL_COMMAND *cmd)        /* current command */
{
   SCOPE_VAR localScopeVar;      /* local variables for sub-command */

   /* Start scope variables */
   exeStartScope(&localScopeVar);

   /* Set local variables */
   localScopeVar.idxPrev = scopeVar->idxPrev;
   localScopeVar.idxRetn = scopeVar->idxRetn;
   localScopeVar.idxCurr = scopeVar->idxCurr;
   localScopeVar.father = cmd;

   /* Call sub-command */
   exeGo(exeVar,&localScopeVar,cmd->sub);

   /* Close scope */
   exeScopeClose(&localScopeVar);

   /* Garbage collector */
   exeScopeFree(&localScopeVar);

} /* exeDo */

/* ========================================================== exeStartGlobal */
void exeStartGlobal(void)
{
   int i;      /* auxiliary index */

   /* Return cipar flag to no cipar */
   dbxcdcip = efc_free(dbxcdcip);

   /* Return isisuctab and isisactab to the standard */
   for (i = 0; i < 256; i++) isisuctab[i] = exeUctab[i];
   memset(isiswctab,0x00,256);
   for (i = 0; exeActab[i]; i++) isiswctab[exeActab[i]] = 1;
}

/* ================================================================ exeGroup */
void exeSection(EXE_VAR *exeVar,          /* execution variables */
                SCOPE_VAR *scopeVar,      /* indexes */
                CPL_COMMAND *cmd)         /* current command */
{
   SCOPE_VAR localScopeVar;      /* local variables for sub-command */

   /* Start scope variables */
   exeStartScope(&localScopeVar);

   /* Set local variables */
   localScopeVar.idxPrev = scopeVar->idxCurr;
   localScopeVar.idxRetn = scopeVar->idxRetn;
   localScopeVar.idxCurr = exeWorkRecord(exeVar);
   if (localScopeVar.idxCurr < 0L) {
      exeError(exeVar->program,EXE_ERROR_ALLOC,cplElement[cmd->element].text);
      return;
   }
   localScopeVar.father = cmd;

   /* Call sub-command */
   exeGo(exeVar,&localScopeVar,cmd->sub);

   /* Garbage collector */
   eci_rec_free(localScopeVar.idxCurr);

   /* Close scope */
   exeScopeClose(&localScopeVar);

   /* Garbage collector */
   eci_rec_free(localScopeVar.idxCurr);

   /* Garbage collector */
   exeScopeFree(&localScopeVar);

   /* Restore IsisScript default */
   exeStartGlobal();

} /* exeSection */

/* ================================================================= exeNext */
CPL_COMMAND *exeNext(CPL_COMMAND *cmd)    /* current command */
{

   /* Check command list */
   if (!cmd) return NULL;

   /* Return next command */
   return cmd->next;

} /* exeNext */

/* ================================================================ exePrint */
void exePrint(EXE_VAR *exeVar,            /* execution variables */
              CPL_COMMAND *cmd,           /* current command */
              TRACE_PART tracePart)       /* print trace part */
{
   /* [0.8] Check command parameter */
   if (!cmd) return;

   /* [4.0] Trace over compiled command is forbidden */
   if (cmd->isCompiled) return;

   /* Avoid obvious print */
   if (cmd->element == ELEMENT_DISPLAY ||
       cmd->element == ELEMENT_DISPL) return;

   /* Evaluate trace option */
   switch (exeVar->trace) {

   case PRECISE_ON:
      /* Normal trace */
      switch (tracePart) {

      case TRACE_SEPARATOR:
         printf("\n");
         break;

      case TRACE_CMD:
         cplPrintScope(exeVar->scope);
         cplPrintCommand(cmd,FALSE);
         break;

      case TRACE_TEXT:
         if (cmd->sub)
            if (cmd->sub->altn) printf("%s",((EXE_FORMAT *)cmd->sub->altn)->text);
            else
               if (!cmd->sub->altn) cplPrintScope(exeVar->scope);
         cplPrintCommandClose(cmd,FALSE);
         break;

      } /* switch */

      break;

   case PRECISE_BR:
      /* HTML style */
      switch (tracePart) {

      case TRACE_SEPARATOR:
         break;

      case TRACE_CMD:
         printf("<br>");
         cplPrintCommand(cmd,TRUE);
         break;

      case TRACE_TEXT:
         if (cmd->sub)
            if (cmd->sub->altn) printf("%s",((EXE_FORMAT *)cmd->sub->altn)->text);
         cplPrintCommandClose(cmd,TRUE);
         break;

      } /* switch */

      break;

   case PRECISE_TABLE:
      /* HTML table style */
      switch (tracePart) {

      case TRACE_SEPARATOR:
         break;

      case TRACE_CMD:
         printf("<table border=\"1\"><tr><td bgcolor=\"#C0C0C0\">");
         cplPrintCommand(cmd,TRUE);
         printf("</td></tr></table>");
         break;

      case TRACE_TEXT:
         printf("<table border=\"1\"><tr><td bgcolor=\"#C0C0C0\">");
         if (cmd->sub)
            if (cmd->sub->altn) printf("%s",((EXE_FORMAT *)cmd->sub->altn)->text);
         cplPrintCommandClose(cmd,TRUE);
         printf("</td></tr></table>");
         break;

      } /* switch */

      break;

   } /* switch */

} /* exePrint */

/* =================================================================== exeGo */
void exeGo(EXE_VAR *exeVar,       /* execution variables */
           SCOPE_VAR *scopeVar,   /* indexes */
           CPL_COMMAND *cmd)      /* current command */
{
   char *text;          /* auxiliary text buffer */

   /* Increase scope, reset skip option, field update define variables */
   exeVar->scope++;
   scopeVar->skip = PRECISE_OFF;
   exeDefines(exeVar,scopeVar,scopeVar->idxCurr);
   if (exeVar->program->error.code) return;

   /* Print trace */
   if (exeVar->trace != PRECISE_OFF) exePrint(exeVar,cmd,TRACE_SEPARATOR);

   /* Loop all commands until error or no more commands */
   while (cmd) {

      /* Print trace */
      if (exeVar->trace != PRECISE_OFF) exePrint(exeVar,cmd,TRACE_CMD);

      /* Evaluate element type */
      switch (cmd->element) {

      case ELEMENT_CALL:
      case ELEMENT_CGITABLE:
      case ELEMENT_DEFINE:
      case ELEMENT_EXPORT:
      case ELEMENT_EXTRACT:
      case ELEMENT_FIELD:
      case ELEMENT_FILE:
      case ELEMENT_FLOW:
      case ELEMENT_LIST:
      case ELEMENT_PROC:
      case ELEMENT_RETURN:
      case ELEMENT_TRACE:
      case ELEMENT_WRITE:
         /* Command element */
         cmd = exeCmd(exeVar,scopeVar,cmd);
         break;

      case ELEMENT_DISPLAY:
      case ELEMENT_DISPL:
         /* Display text */
         text = exeText(exeVar,scopeVar,scopeVar->idxCurr,cmd);

#ifdef XIS_SERVER
       if (text) {
             writeBuff(text, outBuffer, 1);
         }
#else
         if (text) fprintf(exeVar->output,"%s",text);
#endif /* XIS_SERVER */
         break;

      case ELEMENT_DO:
         /* Do a task */
         if (cmd->sub) exeDo(exeVar,scopeVar,cmd);
         break;

      case ELEMENT_HL:
         /* Highlight group */
         exeHL(exeVar,scopeVar,cmd);
         break;

      case ELEMENT_INCLUDE:
         break;

      case ELEMENT_ISISSCRIPT:
         /* Start isis script */
         exeGo(exeVar,scopeVar,cmd->sub);
         break;

      case ELEMENT_LOOP:
      case ELEMENT_UPDATE:
         /* Repeat commands */
         exeLoop(exeVar,scopeVar,cmd);
         break;

      case ELEMENT_PARM:
         /* Get parameter */
         exeParm(exeVar,scopeVar,cmd);
         break;

      case ELEMENT_SECTION:
         /* Section group */
         if (cmd->sub) exeSection(exeVar,scopeVar,cmd);
         break;

      } /* switch */

      /* Check error code */
      if (exeVar->program->error.code) break;

      /* Print trace */
      if (exeVar->trace != PRECISE_OFF) exePrint(exeVar,cmd,TRACE_TEXT);

      /* Check flow option */
      if (scopeVar->skip == PRECISE_NEXT || scopeVar->skip == PRECISE_QUIT) break;
      if (scopeVar->jump) {
         cmd = scopeVar->jump;
         scopeVar->jump = NULL;
         continue;
      }

      /* Get next command */
      cmd = exeNext(cmd);

   } /* while */

   /* Decrease scope */
   exeVar->scope--;

} /* exeGo */

/* ========================================================= exeStartOptions */
void exeStartOptions(void)       /* execution variables */
{

   /* Set attribute value to parm index table */
   memset(&atValueParm,0x00,sizeof(atValueParm));
   atValueParm[ATVALUE_ACTAB]       = PARM_ACTAB;
   atValueParm[ATVALUE_BUFFERSIZE]  = PARM_BUFFERSIZE;
   atValueParm[ATVALUE_CIPAR]       = PARM_CIPAR;
   atValueParm[ATVALUE_COUNT]       = PARM_COUNT;
   atValueParm[ATVALUE_DB]          = PARM_DB;
   atValueParm[ATVALUE_DECOD]       = PARM_DECOD;
   atValueParm[ATVALUE_DELIMITER]   = PARM_DELIMITER;
   atValueParm[ATVALUE_EXPIRE]      = PARM_EXPIRE;
   atValueParm[ATVALUE_EXPRESSION]  = PARM_EXPRESSION;
   atValueParm[ATVALUE_FILE]        = PARM_FILE;
   atValueParm[ATVALUE_FREQSUM]     = PARM_FREQSUM;
   atValueParm[ATVALUE_FROM]        = PARM_FROM;
   atValueParm[ATVALUE_FST]         = PARM_FST;
   atValueParm[ATVALUE_GIZMO]       = PARM_GIZMO;
   atValueParm[ATVALUE_INDEXLIST]   = PARM_INDEXLIST;
   atValueParm[ATVALUE_KEY]         = PARM_KEY;
   atValueParm[ATVALUE_KEYFIELD]    = PARM_KEYFIELD;
   atValueParm[ATVALUE_KEYLENGTH]   = PARM_KEYLENGTH;
   atValueParm[ATVALUE_KEYS]        = PARM_KEYS;
   atValueParm[ATVALUE_KEYSDB]      = PARM_KEYSDB;
   atValueParm[ATVALUE_LOCKID]      = PARM_LOCKID;
   atValueParm[ATVALUE_MAXLK]       = PARM_MAXLK;
   atValueParm[ATVALUE_MFN]         = PARM_MFN;
   atValueParm[ATVALUE_POSTING]     = PARM_POSTING;
   atValueParm[ATVALUE_POSTTAG]     = PARM_POSTTAG;
   atValueParm[ATVALUE_PREFIX]      = PARM_PREFIX;
   atValueParm[ATVALUE_RESET]       = PARM_RESET;
   atValueParm[ATVALUE_REVERSE]     = PARM_REVERSE;
   atValueParm[ATVALUE_SORT]        = PARM_SORT;
   atValueParm[ATVALUE_STW]         = PARM_STW;
   atValueParm[ATVALUE_SUFFIX]      = PARM_SUFFIX;
   atValueParm[ATVALUE_TASK]        = PARM_TASK;
   atValueParm[ATVALUE_TO]          = PARM_TO;
   atValueParm[ATVALUE_TYPE]        = PARM_TYPE;
   atValueParm[ATVALUE_UCTAB]       = PARM_UCTAB;
   /* 27.Nov.2000 */
   atValueParm[ATVALUE_PREFIX_HTMLPFT] = PARM_PREFIX_HTMLPFT;
   atValueParm[ATVALUE_SUFFIX_HTMLPFT] = PARM_SUFFIX_HTMLPFT;
   /* 28.Nov.2000 */
   atValueParm[ATVALUE_FIRST_LINE]  = PARM_FIRST_LINE;
   /* 22.Feb.2001 */
   atValueParm[ATVALUE_ISISXML_STYLE] = PARM_ISISXML_STYLE;
   atValueParm[ATVALUE_ISISXML_TABLE] = PARM_ISISXML_TABLE;
   /* 05.Apr.2001 */
   atValueParm[ATVALUE_LOG] = PARM_LOG;

   /* Set precise option list */
   preciseList[PRECISE_CURRENT] =   "Isis_Current";
   preciseList[PRECISE_ERRORINFO] = "Isis_ErrorInfo";
   preciseList[PRECISE_FROM] =      "Isis_From";
   preciseList[PRECISE_GETNEW] =    "GetNew";
   preciseList[PRECISE_HLINE] =     "HLine";
   preciseList[PRECISE_KEYS] =      "Isis_Keys";
   preciseList[PRECISE_BR] =        "BR";
   preciseList[PRECISE_ISO2709] =   "ISO2709";
   preciseList[PRECISE_ISO2709_CRLF] = "ISO2709_CRLF";
   preciseList[PRECISE_ITEM] =      "Isis_Item";
   preciseList[PRECISE_ITENS] =     "Isis_Itens";
   preciseList[PRECISE_ITEMS] =     "Isis_Items";
   preciseList[PRECISE_KEY] =       "Isis_Key";
   preciseList[PRECISE_LOCK] =      "Isis_Lock";
   preciseList[PRECISE_MFN] =       "Isis_MFN";
   preciseList[PRECISE_NEW] =       "New";
   preciseList[PRECISE_NEXT] =      "Next";
   preciseList[PRECISE_OFF] =       "Off";
   preciseList[PRECISE_ON] =        "On";
   preciseList[PRECISE_POSTING] =   "Isis_Posting";
   preciseList[PRECISE_POSTINGS] =  "Isis_Postings";
   preciseList[PRECISE_QUIT] =      "Quit";
   preciseList[PRECISE_RECSTAT] =   "Isis_RecordStatus";
   preciseList[PRECISE_RLINE] =     "RLine";
   preciseList[PRECISE_STATUS] =    "Isis_Status";
   preciseList[PRECISE_TABLE] =     "Table";
   preciseList[PRECISE_TO] =        "Isis_To";
   preciseList[PRECISE_TOTAL] =     "Isis_Total";
   preciseList[PRECISE_MARC21] =    "MARC21";
   preciseList[PRECISE_VALUE] =     "Isis_Value";
   preciseList[PRECISE_VLINE] =     "VLine";
   preciseList[PRECISE_WDELETE] =   "Delete";
   preciseList[PRECISE_WLOCK] =     "Lock";
   preciseList[PRECISE_WNOUNLOCK] = "NoUnlock";
   preciseList[PRECISE_WUNLOCK] =   "Unlock";

    /* 05.Abr.2001 */
   preciseList[PRECISE_SETNUMBER] =   "Isis_SetNumber";

} /* exeStartOptions */

/* ================================================================ exeStart */
EXE_ERROR exeStart(EXE_VAR *exeVar,       /* execution variables */
                   CPL_STRUCT *program,   /* program structure */
                   CGI_PARAM *cgiList)    /* CGI argument list */
{
    LONGX irec;
    char *p;

    /* Reset all variables */
    memset(exeVar,0x00,sizeof(EXE_VAR));

    /* Set cisis global tables */
    exeStartGlobal();

    /* Set some cipar variables */
    irec=nrecs;
    p=dbxcipar(NULL,"maxmfrl",'='); 
    if (dbxcipok) {
   	    sscanf(p,"%"_LD_,&rec_maxmfrl);
   	    if (rec_maxmfrl < (LONGX)MSNVSPLT || (RECHSIZE+rec_maxmfrl) > ALLOMAXV) rec_maxmfrl=MAXMFRL;
   	    recallok(irec,rec_maxmfrl);
    }
    p=dbxcipar(NULL,"mstxl",'=');
    if (dbxcipok) {
        if (p[strlen(p)-1] == 'G') {
            int num=atoi(p);
            int cap = 1;
            cipmstxl = 1;
            while (num != cap) {
                if (num < cap) return EXE_ERROR_INVALID;
                cipmstxl++;
                cap *= 2;
            }
        } else {
            if (sscanf(p,"%d",&cipmstxl) != 1) return EXE_ERROR_INVALID;
            if (cipmstxl < 0) return EXE_ERROR_INVALID;
            if (cipmstxl == 4) cipmstxl=3;
        }
    }

   /* Set current program variables */
   exeVar->program = program;
   exeVar->cgiList = cgiList;
   exeVar->output = stdout;
   exeVar->scope = -1;
   exeVar->trace = PRECISE_OFF;
   exeVar->buffSize = EXE_BUFF_SIZE;

   /* Set precise option identifiers */
   exeStartOptions();

   /* Allocate common string buffer area, if error return */
   exeVar->buff = exeNew(exeVar,exeVar->buffSize);
   if (!exeVar->buff) return EXE_ERROR_ALLOC;

   /* Return no error */
   return EXE_ERROR_OK;

} /* exeStart */

/* ============================================================== exeCplFree */
void exeCplFree(CPL_COMMAND *first)    /* first command */
{
   CPL_COMMAND *cmd;       /* current command */
   EXE_FORMAT *fmt;        /* format cast */
   FST_CODE *fstCode;      /* FST code cast */

   /* Garbage collector */
   for (cmd = first; cmd; cmd = cmd->next) {
      if (cmd->sub) exeCplFree(cmd->sub);
      if (cmd->element == ELEMENT_PARM && cmd->attributeList[ATTRIBUTE_NAME].atValue == ATVALUE_FST) {
         fstCode = (FST_CODE *)(cmd->altn);
         if (fstCode) fst_free(fstCode);
      }
      if (cmd->element == ELEMENT_PFT || cmd->element == ELEMENT_ISISXML) {
         fmt = (EXE_FORMAT *)(cmd->altn);
         if (fmt) {
            if (fmt->code) fmt_free(fmt->code);
            if (fmt->ciXml)
            {
                cixmlFree(fmt->ciXml);
               efc_free(fmt->ciXml);
            }
            if (fmt->text) efc_free(fmt->text);
            efc_free(fmt);
         }
      }
   } /* for */

} /* cplFree */

/* ================================================================= exeFree */
void exeFree(EXE_VAR *exeVar) /* execution variables */
{

   /* Garbage collection */
   efc_free(exeVar->buff);
   freq_free(exeVar->freq.head);
   if (exeVar->output != stdout) fclose(exeVar->output);
} /* exeFree */

/* ============================================================= cleanShelves */
void cleanShelves() {
    int idx;

    if (nrecs >= 10) {
        for(idx = 0; idx < MAXNREC; idx++) {
            if (vrecp[idx]) {
                FREE(vrecp[idx]);
                vrecp[idx] = NULL;
            }
        }
        nrecs = 0;
    }

    if (ntrms >= 10) {
        for(idx = 0; idx < MAXNTRM; idx++) {
            if (vtrmp[idx]) {
                FREE(vtrmp[idx]);
                vtrmp[idx] = NULL;
            }
        }
        ntrms = 0;
    }

    if (ndbxs > 10) {
        for(idx = 0; idx < MAXNDBX; idx++) {
            if (vdbxp[idx]) {
                dbxflush(vdbxp[idx]->dbxname);
                //mstclose(vdbxp[idx]->dbxname);
            }
        }
        ndbxs = 0;
    }
}

/* ================================================================== exeRun */
EXE_ERROR exeRun(CPL_STRUCT *program,     /* program structure */
                 CGI_PARAM *cgiList,      /* cgi couple parameter list */
                 OUT_BUFFER *out)
{
   EXE_VAR exeVar;         /* execution variables */
   SCOPE_VAR scopeVar;     /* scope variables */

   outBuffer = out;

   //chrono_start();

   /* Start execution variables, if OK execute program instructions */
   if (exeStart(&exeVar,program,cgiList) == EXE_ERROR_OK) {
      /* Start scope variables */
      exeStartScope(&scopeVar);
      /* Allocate a record index */
      scopeVar.idxCurr = exeWorkRecord(&exeVar);
      if (scopeVar.idxCurr >= 0L) {
         /* Call program execution, free record index */
         exeGo(&exeVar,&scopeVar,program->cmd);
         eci_rec_free(scopeVar.idxCurr);
      }
   }

   /* Garbage collector */
   /*eci_rec_free(scopeVar.idxCurr); - freed in cleanShelves */
   exeScopeFree(&scopeVar);
   exeCplFree(program->cmd);
   exeFree(&exeVar);

   cleanShelves();

   //chrono_stop("exeRun");

   /* Return error code */
   return program->error.code;

} /* exeRun */

