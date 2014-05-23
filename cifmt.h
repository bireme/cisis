/*-------------------------------------------------------------------------*/
/* CIFMT.h                                                                 */
/*-------------------------------------------------------------------------*/

#ifndef CIFMT_H
#define CIFMT_H

#define DATEFUNC 0
#define DATETIME 999
#define DATEONLY 998

#define DEB_ATTRIB 0
#define TYPE_PATTERN 1
#define TYPE_NUM  0
#define TY_A    'A' /* tem que ficaR NO CISIS */
#define TY_X    'X'
#define TY_N    'N'
#define NO_MIN_OCC         -1    /* [-NO_MIN_OCC:-NO_MAX_OCC] */
#define NO_MAX_OCC         -9999   /* Numero maximo de ocorrencias ? */
#define FMT_OPTIMIZE 0
#define MAXSTACK      100
#define PRINT_CODE  0
#define l_code  FMT_CODE        /* AOT 14/05/93 */
#define MAX_LITER   500
#define INCR_LITER   500
#define NMAXVAR  10   /* Numero maximo de variaveis E e S */ 
/* fld_def is used to store the possible parameters related to a field */

typedef struct fld_def
             {
                int tag;
                char sub_field;
                int indent1;
                int indent2;
                int offset;
                int length;
/* inicio implementacao [x:y] */
                int lower; /* intervalo [lowocc:uppocc] */
                int upper;
                int execlower; /* intervalo [lowocc:uppocc] -execucao */
                int execupper;
/* fim  implementacao [x:y] */
                int max_occ;
                int actual_occ;
                int first_occ;
                int last_occ;
              } field_definition_node;

typedef  int   endereco;

typedef enum classe_operandos {
    integer,string,logical,float_n,ptr,l_w,xindex,next_cc,long_n,type_recstru
} class_operand;

/* Extern */
#if !CIAPI_SOURCE
extern LONGX fmt_error;  /* gener'ated */                /* AOT 26/12/91 */
extern char *inst_cnv[];/* gener'ated */                /* AOT 26/12/91 */
#endif /* CIAPI_SOURCE */
/* HB 20080901 - a funcao fmt_float_numb nao converte o numero corretamente se float_x for float
#if PC */
#if 1
#define float_x double
#else
#define float_x float
#define float_x_fmt "%f"
#endif
                                  /* Esta dando erro quando define float (?) */
#define   max_str_num 16 /* Tamanho numero formatado quando omite param */


/* Prototypes */
#if ANSI
char *fmt_float_numb (float_x valor,int min,int dec,char fill);
LONGX store_float_const(char *s);
void fmt_chadd(l_code  *pgmp);
int num_expr(void);
#if CICPP
void  sc(char *p);
#else /* CICPP */
void sc(char *p);
#endif /* CICPP */

#if CICPP
int fmt_CRLF (char *v,int   pos);
#else
int fmt_CRLF (char *v,int   pos);
#endif /*CICPP*/

#if CICPP
LONGX lookup(char *keyp,TRMSTRU *trmp,char *dbnp,LONGX nord);
#else /* CICPP */
LONGX lookup(char *keyp,LONGX next_inverted_prat,char *dbnp,LONGX nord);
#endif /* CICPP */
void out_put_str(char out[],LONGX maxsize,int *pout,LONGX lw,
                 int id1,int id2,LONGX *ncc,char s[]);
void print_inter_din(char *fmt,l_code *head);
void print_inter(char *fmt,l_code *head);
int spacing_string (void);
void suffix (void);
label gen_next_label (void);
int  separator (void);
char  *store_res_word(char *str);
void get_ch (void);
void erro (int nerr);
void str_read (char end_ch);
void getsymbol (void);
void initialize (void);
void escrevesym (void);
void semantic_initialize(void);
void semantic_error(char *msg);
void isis_fmt ( void );
void change_pre_cond_by_u_cond(l_code *first, l_code *last);
int dummy_id (l_code  *idx,label lab1);
int  gen(label     lab,
         instruction_code instr,LONGX  add,l_code *index);
void print_code(l_code *lista);
field_definition_node *alloc_field_definition_node (void);
LONGX store_field_def(int tag,char subfield,
                     int ind1,int ind2,int off,int len,int low,int upp);
l_code *alloc_pgm_node (void);
LONGX store_str_const(char *s);

/*versao6 fim ----------------------------------------------------------*/
char *sub_field_change(char *q);
char *translate_instruction( instruction_code i);
char *find_numeric_string(char v[],int *k);

/*AOT ------------------------------------------------------------------*/
void out_coreleft (void);
void boolean_expr (void);
void boolean_factor (void);
void boolean_term (void);

void fmt_push_repeat (void);
void fmt_pop_repeat (void);
/* void fmt_inicio_grupos_vfields(instruction_code i, instruction_code f); */
void fmt_fim_grupos_vfields(void);

#else
char *fmt_float_numb ();
LONGX store_float_const();
void fmt_chadd();
int num_expr();
LONGX lookup();
void out_put_str();
void print_inter_din();
void print_inter();
int spacing_string ();
void suffix ();
label gen_next_label ();
int  separator ();
char  *store_res_word();
void get_ch ();
void erro ();
void str_read ();
void getsymbol ();
void initialize ();
void escrevesym ();
void semantic_initialize();
void semantic_error();
void isis_fmt ();
void change_pre_cond_by_u_cond();
int dummy_id ();
int gen();
void print_code();
field_definition_node *alloc_field_definition_node ();
LONGX store_field_def();
l_code *alloc_pgm_node ();
LONGX store_str_const();

/*versao6 fim ----------------------------------------------------------*/
char *sub_field_change();
char *translate_instruction();
char *find_numeric_string();

/*AOT ------------------------------------------------------------------*/
void out_coreleft ();
void boolean_expr ();
void boolean_factor ();
void boolean_term ();

void fmt_push_repeat ();
void fmt_pop_repeat ();
/* void fmt_inicio_grupos_vfields(); */
void fmt_fim_grupos_vfields();

#endif



/*-------------------------- Defines ---------------------------------------*/
#define null_char               '\0'
#define multiply_char           '*'
#define nulo                    NULL                    /* AOT 06/04/92 */
#define subfield_null           ' '
#define cr                      '\r'
#define lf                      '\n'
#define true                    1
#define false                   0
#define greater_char            '>'
#define less_char               '<'
#define equal_char              '='
#define space_char              ' '
#define LASTVAL                 -2

/*----------------- Defines  retirados de cifm[2,3].c-----------------------*/


/*--------------------------------------------------------------------------*/
/*                   crlf                                                   */
/*--------------------------------------------------------------------------*/
#if 0
#if UNIX | WWWISIS
#define  crlf(v,i)      ((v[i-1]==lf)?true:false)
#define  LEN_crlf       1 /* 04-02-95 Para delete_chars funcionar */
#else
#define  crlf(v,i)      ((v[i-2]==cr && v[i-1]==lf)?true:false)
#define  LEN_crlf       2 /* 04-02-95 Para delete_chars funcionar */
#endif
#endif
/*--------------------------------------------------------------------------*/
/*            Macro  is_upper_mode                                          */
/*--------------------------------------------------------------------------*/

#define is_upper_mode(i)  (i==mpu_par || i==mdu_par || i==mhu_par)

/*--------------------------------------------------------------------------*/
/*                   new_line                                               */
/*--------------------------------------------------------------------------*/
#if 0
 #if UNIX
  #define  new_line(v,i)  {v[i++]=lf;v[i]=null_char;}
 #else
  #define  new_line(v,i)  {v[i++]=cr;v[i++]=lf;v[i]=null_char;}
 #endif
#endif
 
#if !CICPP
#define new_line(v,i,max) { if (i + nl_LEN < max ){  \
   strcat(v,nl_STR);                           \
   i=i+nl_LEN;                                 \
  }else {if (fmttrace)printf(" No room for text - truncating");} \
;}
#endif /* CICPP */

#endif  /* CIFMT_H */ 
