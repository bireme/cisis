/* -------------------------------------------------------------- XIS_COMP.H */

/* ------------------------------------------------------------------ define */
#define PATH_NAME_LEN		512			/* path name lenght */
/* ------------------------------------------------------------- enumeration */
typedef enum {
    CPL_ERROR_OK,
    CPL_ERROR_ALLOC,
    CPL_ERROR_INVALID,
    CPL_ERROR_MISSING,
    CPL_ERROR_PATHNAMELEN,
    CPL_ERROR_NOTFOUND,
    CPL_ERROR_FILE,
    CPL_ERROR_WRITE,
    CPL_ERROR_READ,
   CPL_ERROR_LIST_QTT
} CPL_ERROR;						/* error code */

typedef enum {
    TOKEN_ASSIGN,
    TOKEN_ATTRIBUTE,
    TOKEN_ELCLOSE,
    TOKEN_ELEMENT,
    TOKEN_ELEND,
    TOKEN_END,
    TOKEN_SEPARATOR,
   TOKEN_LIST_QTT
} CPL_TOKEN;						/* token */

typedef enum {
    ELEMENT_0,
    ELEMENT_CALL,
    ELEMENT_CGITABLE,
    ELEMENT_DEFINE,
    ELEMENT_DISPLAY,
    ELEMENT_DISPL,
    ELEMENT_DO,
    ELEMENT_EXPORT,
    ELEMENT_EXTRACT,
    ELEMENT_FIELD,
    ELEMENT_FILE,
    ELEMENT_FLOW,
    ELEMENT_FUNCTION,
    ELEMENT_HL,
    ELEMENT_HTMLPFT,
    ELEMENT_INCLUDE,
    ELEMENT_ISISSCRIPT,
    ELEMENT_LABEL,
    ELEMENT_LIST,
    ELEMENT_LOOP,
    ELEMENT_PARM,
    ELEMENT_PFT,
    ELEMENT_PROC,
    ELEMENT_RETURN,
    ELEMENT_SECTION,
    ELEMENT_TRACE,
    ELEMENT_UPDATE,
    ELEMENT_WRITE,

    ELEMENT_ISISXML,				/* 22.Feb.2001 */

   ELEMENT_LIST_QTT
} CPL_ELEMENT;						/* element */

typedef enum {
    ATTRIBUTE_0,
    ATTRIBUTE_ACTION,
    ATTRIBUTE_FROM,
    ATTRIBUTE_NAME,
    ATTRIBUTE_PREVIOUS,
    ATTRIBUTE_SPLIT,
    ATTRIBUTE_TAG,
    ATTRIBUTE_TASK,
    ATTRIBUTE_TYPE,
    ATTRIBUTE_AUX1,
    ATTRIBUTE_AUX2,
   ATTRIBUTE_LIST_QTT
} CPL_ATTRIBUTE;					/* attribute */

typedef enum {
    ATVALUE_0,
    ATVALUE_ACTAB,
    ATVALUE_ADD,
    ATVALUE_APPEND,
    ATVALUE_BUFFERSIZE,
    ATVALUE_CGI,
    ATVALUE_CGISFT,
    ATVALUE_CGISFU,
    ATVALUE_CGISFW,
    ATVALUE_CHECK,
    ATVALUE_CIPAR,
    ATVALUE_CLOSE,
    ATVALUE_CONVERT,
    ATVALUE_COPY,
    ATVALUE_COUNT,
    ATVALUE_CREATE,
    ATVALUE_DATABASE,
    ATVALUE_DB,
    ATVALUE_DECOD,
    ATVALUE_DEFINE,
    ATVALUE_DELETE,
    ATVALUE_DELIMITER,
    ATVALUE_DISPLAY,
    ATVALUE_EXIT,
    ATVALUE_EXPIRE,
    ATVALUE_EXPORT,
    ATVALUE_EXPRESSION,
    ATVALUE_EXTRACT,
    ATVALUE_FILE,
    ATVALUE_FLAG,
    ATVALUE_FLDDIR,
    ATVALUE_FREQ,
    ATVALUE_FREQSUM,
    ATVALUE_FROM,
    ATVALUE_FST,
    ATVALUE_FULLINV,
    ATVALUE_GIZMO,
    ATVALUE_HL,
    ATVALUE_IMPORT,
    ATVALUE_INDEXLIST,
    ATVALUE_INVERTED,
    ATVALUE_INVLOAD,
    ATVALUE_JUMP,
    ATVALUE_KEY,
    ATVALUE_KEYFIELD,
    ATVALUE_KEYLENGTH,
    ATVALUE_KEYRANGE,
    ATVALUE_KEYS,
    ATVALUE_KEYSDB,
    ATVALUE_LIST,
    ATVALUE_LOAD,
    ATVALUE_LOCKID,
    ATVALUE_MASTER,
    ATVALUE_MAXLK,
    ATVALUE_MFN,
    ATVALUE_MFNRANGE,
    ATVALUE_MSTSORT,
    ATVALUE_OCC,
    ATVALUE_OUTPUT,
    ATVALUE_POSTING,
    ATVALUE_POSTTAG,
    ATVALUE_PREFIX,
    ATVALUE_RELOAD,
    ATVALUE_REPLACE,
    ATVALUE_RESET,
    ATVALUE_REVERSE,
    ATVALUE_SEARCH,
    ATVALUE_SKIP,
    ATVALUE_SORT,
    ATVALUE_STATUSDB,
    ATVALUE_STATUSFILE,
    ATVALUE_STW,
    ATVALUE_SUFFIX,
    ATVALUE_TEMPFILE,
    ATVALUE_TASK,
    ATVALUE_TO,
    ATVALUE_TYPE,
    ATVALUE_UCTAB,
    ATVALUE_UNLOCK,
    ATVALUE_UPDATE,
    ATVALUE_WRITE,
   /* 27.Nov.2000 */
    ATVALUE_PREFIX_HTMLPFT,
    ATVALUE_SUFFIX_HTMLPFT,
   /* 28.Nov.2000 */
    ATVALUE_FIRST_LINE,
   /* 22.Feb.2001 */
    ATVALUE_ISISXML_STYLE,
    ATVALUE_ISISXML_TABLE,
   /* 05.Apr.2001 */
    ATVALUE_LOG,

   ATVALUE_LIST_QTT
} CPL_ATVALUE;						/* attribute value */

typedef enum {
    COMMENT_NORMAL,				/* <!-- comment --> */
    COMMENT_PROCESS,				/* <?xml version="1.0" ?> */
    COMMENT_DECLARE,				/* <!DOCTYPE type SYSTEM "type.dtd"> */
   COMMENT_LIST_QTT
} CPL_COMMENT;						/* comment */

typedef enum {
    PART_HEADER,
   PART_COMMAND,
   PART_TEXT,
   PART_LIST_QTT
} CPL_COMPILED_PART;				/* compiled block */
/* -------------------------------------------------------------- structures */
typedef struct {
    char *text;
   int len;
} CPL_IDENTIFIER;					/* identifier */

typedef union {
   CPL_ELEMENT element;
   CPL_ATTRIBUTE attribute;
} CPL_INDEX;						/* index */

typedef struct {
    CPL_TOKEN type;
   CPL_INDEX index;
   char *statment;
   char *text;
} CPL_TOKEN_STRU;					/* token */

typedef struct {
   char *text;
   CPL_ATVALUE atValue;
} CMD_ATTRIBUTE_STRU;			/* attribute structure */

typedef struct command_stru {
   CPL_ELEMENT element;                                     /* CPL_CMD_COMPILED */
   CMD_ATTRIBUTE_STRU attributeList[ATTRIBUTE_LIST_QTT];		/* CPL_CMD_COMPILED */
   char *text;																/* CPL_CMD_COMPILED */
   struct command_stru *sub;											/* CPL_CMD_COMPILED */
   struct command_stru *prev;											/* CPL_CMD_COMPILED */
   struct command_stru *next;											/* CPL_CMD_COMPILED */
   struct command_stru *func;
   void *altn;
   BOOLEAN isCompiled;
} CPL_COMMAND;						/* command */

typedef struct save_command_stru {
   CPL_ELEMENT element;
   CMD_ATTRIBUTE_STRU attributeList[ATTRIBUTE_LIST_QTT];
   char *text;
   struct command_stru *sub;
   struct command_stru *prev;
   struct command_stru *next;
} CPL_CMD_COMPILED;				/* command compiled */

typedef struct {
    CPL_COMMAND *first;
    CPL_COMMAND *last;
} CPL_CMD_LIST;					/* command list */

typedef struct {
    EFC_ERROR error;
   BOOLEAN printSource;
   char *pathSource;
   char *compiledExt;
   BOOLEAN cplStarted;
   CPL_COMMAND *cmd;
} CPL_STRUCT;						/* compiler */

typedef struct {
    LONGX versionNumber;
   LONGX aux1;
   LONGX aux2;
} CPL_COMPILED_HEADER;			/* compiled header */
/* -------------------------------------------------------------- prototypes */
CPL_ERROR	cplCompile				(char *script,CPL_STRUCT *program);
BOOLEAN 		cplIsCompiled			(CPL_STRUCT *program,char *scriptName);
CPL_ERROR	cplLoadCompiled		(char *loadFileName,CPL_STRUCT *program,CPL_COMMAND **cmd,CPL_CMD_LIST *cmdList);
CPL_ERROR	cplSaveCompiled		(CPL_STRUCT *program,char *saveFileName);
void			cplFree					(CPL_COMMAND *first);
int 			cplIdentifier			(CPL_IDENTIFIER identList[],int listQtt,char *p);
void			cplPrintCommandClose	(CPL_COMMAND *cmd,BOOLEAN convLT);
void			cplPrintCommand		(CPL_COMMAND *cmd,BOOLEAN convLT);
void 			cplPrintScope			(int scope);

