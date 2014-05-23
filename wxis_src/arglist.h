/* --------------------------------------------------------------- ARGLIST.H */

/* /////////////////////////////////////////////////////////////////////////

   [ Version 1.0 ]
   16.Oct.1998 - Argument list.

   ///////////////////////////////////////////////////////////////////////// */

/* ------------------------------------------------------------- enumeration */
typedef enum {
    ARG_ERROR_OK,
    ARG_ERROR_MAX = FILE_ERROR_LIST_QTT,
    ARG_ERROR_QTT
} ARG_ERROR_LIST;

typedef enum {
   ARG_TYPE_BOOLEAN,
   ARG_TYPE_NUMBER,
   ARG_TYPE_TEXT,
   ARG_TYPE_QTT
} ARG_TYPE;             /* argument type */
/* -------------------------------------------------------------- structures */
typedef struct stru_argument {
   char *id;				/* the argument identification label */
   int id_size;			/* argument identification label size */
   ARG_TYPE id_type;		/* argument type */
   LONGX num;				/* the argument number */
   char *text;				/* the argument text content */
   int status;				/* the argument status, defined by the application */
} STRUCT_ARGUMENT;		/* argument */
/* -------------------------------------------------------------- prototypes */
int  arg_in		(char *argv[],int arg_from,int arg_max,char *filename);
void arg_load	(STRUCT_ARGUMENT *item,char *id,ARG_TYPE id_type);
void arg_read	(STRUCT_ARGUMENT argl[],int argl_max,char *argv[],int arg_from,int arg_max);

