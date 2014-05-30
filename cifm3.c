 /* file CIFM3.C */
#define RETIRAR 1   /* Usado para colocar em rotina alocacao de var  char  */
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#if TESTFREE
#include <conio.h>
#endif

#include "cisis.h"
#include "cifmt.h"

#ifndef FUNPROCX
#define FUNPROCX 0
#endif

#if CICPP
#include "cicgi.hpp"
#include "cifmt.hpp"
#endif /* CICPP */

#define DEBUG_REF 0
#define MICRO_ISIS_COMPATIBLE 0

#if !CICPP /* otherwise DINALLOC is #defined in cifmt.hpp */
#define DINALLOC 1 /* p/ alocar areas originalmente estaticas - AOT 29/12/90 */
#endif /* CICPP */

#define TRACE_REPLACE    0
#define TRACE_STRFUN     0
#define TRACE_REPF       0
#define TRACE_INTER_X    0              /* printf instruction in execution */
#define TRACE_INTER_L    0              /* printf debug in lookup() */
#define TRACE_INTER_2    0              /* printf Apos fmt_load_all_occ */
#define DEB_FLOAT        0
#define TRACE_COMP_M    00              /* printf msg in trata_md() */

#if FATRAP
#define IFERR_GOTO
#define IFERR_RET
#else
#define IFERR_GOTO      if (fmterror) goto ERROR_LABEL
#define IFERR_RET       if (fmterror) return
#endif /* FATRAP */

/* Modificacoes:
   A05 - 19/07/96
    Problema:
     A funcao REF nao esta liberando corretamente as prateleiras onde sao
     lidos os registros do MasterFile, quando um Ref e' utilizado dentro
     de outro Ref.
     Exemplo: REF(val(ref(1,'3'),...)
     Se aplicarmos esse REF para varios registros, dara erro FATAL de
     alocacao.
    Identificacao:
     Sempre que um novo REF e encontrado em um formato, e gurdado no
     vetor "vprats" apontado por "currprat" o numero da prateleira
     disponivel de 255 ..0.
     Quando termina o REF, a prateleira vprats[currprat] devera ser
     liberada.
     O problema acontecia porque "currprat" estava apontando para a proxima
     entrada disponivel de "vprats" e nao para a ultima usada. No momento
     de liberar os apontadores estavam defasados. A rotina de leitura do
     master era  feita numa prateleira(rec_prateleira) que nao estava
     alocada forcando a alocacao. Esse registro nao era liberado.
   Correcao:
     Corridos os apontadores. Para compatibilizacao, a prateleira de
     entrada do formato e' guardada na primeira posicao de vprats para
     sinalizar o registro em uso.
     Foram inseridos alguns traces para achar o erro.
*/



/*--------------------------------------------------------------------------*/
/*                 E e S variaveis                                          */
/*--------------------------------------------------------------------------*/
#if !CICPP
static float_x  E_var[NMAXVAR];
static char    *S_var[NMAXVAR];
#endif /* CICPP */


/*--------------------------------------------------------------------------*/
/*                   NewLine                                                */
/*--------------------------------------------------------------------------*/
/* A07*/
#if !CICPP
char *nl_STR;  /* Armazena o string para tratar caracteres de newline */
int nl_LEN;
int nl_ALLOC;
#endif /* CICPP */

/*-----------------------------------------------------------------------*/
#define ispunctuation(x) \
   ((x==';'||x=='.' || x==':' || x== ',' || x=='!'|| x=='?' )?true:false )
/*-----------------------------------------------------------------------*/
#define NO_INTERVAL(x) (x ? ((x->lower==NO_MIN_OCC) && (x->upper==NO_MAX_OCC)) : false)
/*-----------------------------------------------------------------------*/

/* error - AOT 29/12/90 */
/* #define MAXERRL      60 */

#if !CICPP
#if FATRAP
jmp_buf fmtjumper;
#endif /* FATRAP */
char fmterrxy[MAXERRL+1] = { "" } ;
int  fmterror;
/*A06*/
ALLOPARM literal_len;
char *literal;
/*A06*/
#if ANSI
/*float_x convert_to_float(stack_node *top); */
int  fmt_type_pattern(char *patt,char *fmt);
int  fmt_type_number(int tipo,char *fmt);
void fmt_init_E_S(void);
void free_S(int from,int to);
char *fmt_copy_realloc(char *dest,ALLOPARM *dest_mxlen,char *cte_str);
char *fmt_alloc_char(ALLOPARM tam,char *msg);
int fmt_instr(char *str_source,char *str_sub);
char *fmt_type(char *p);
void trace_field(field_definition_node *i,char *p);     /* AOT 27/12/91 */
LONGX number_of_lines(char *p);
static void sub_field_string(char *q, char sub);
/* char *long_to_char(LONGX valor, int n); substituida*/
char *trata_md( instruction_code mode , char *s,int *final_added,char **p);
void upcase_mode(char *s);
void out_put_str(char out[],LONGX maxsize,int *pout,LONGX lw,
                 int id1,int id2,LONGX *ncc,char s[]);
#else

float_x convert_to_float();
int  fmt_type_pattern();
int  fmt_type_number();

void fmt_init_E_S();
void free_S();

char *fmt_copy_realloc();
char *fmt_alloc_char();
int fmt_instr();
char *fmt_type();
void trace_field();                                     /* AOT 27/12/91 */
LONGX number_of_lines();
static void sub_field_string();
/* char *long_to_char(); substituida */
char *trata_md();
void upcase_mode();
void out_put_str();
#endif
#endif /* CICPP */

/*--------------------------------------------------------------------------*/
/*                   global types                                           */
/*--------------------------------------------------------------------------*/
#if !CICPP
typedef union stack_operands {
    int i;
        float_x r;
    int boolean;
    char *s;
    char *address;
    LONGX l;
#if CICPP  /*ifcmm*/
    RECSTRU *lrecp;
#endif /*CICPP*/  /*endcmm*/
    } ustackopnd;
typedef struct stack_nodex{
   enum classe_operandos classe;
   union stack_operands op;

   }stack_node;

/* prototypes */
#if ANSI
int exec_percent(char *out,int *outs,LONGX *ncc);
void retrieve_two_numeric_operands (void);
stack_node *pop(void);
void inter_error (char *err);
void restore_context(void);
void push ( stack_node  *e);
int  cast_to_int (stack_node  *e);
char *field_value(char *p,LONGX n,int dd);
void retrieve_determine_class(void);
void take_numeric_operands_value(class_operand new_class);
char *sub_string(char source[],int offset,int length);
void  save_context(int modify_pointers);
char *store_tmp_string (char *str);
int is_numeric(class_operand x);
void retrieve_two_operands (void);
#else
int exec_percent();
void retrieve_two_numeric_operands ();
stack_node *pop();
void inter_error();
void restore_context();
void push ();
int  cast_to_int ();
char *field_value();
void retrieve_determine_class();
void take_numeric_operands_value();
char *sub_string();
void  save_context();
char *store_tmp_string ();
int is_numeric();
void retrieve_two_operands ();
#endif
#endif /* CICPP */


/*--------------------------------------------------------------------------*/
/*                   global_variables                                       */
/*--------------------------------------------------------------------------*/
#if !CICPP

#define max_stack       MAXSTACK
#define MAX_TMP_STR     (MAXMFRL/2)
#define MAX_SFIELD      (MAXMFRL/2)
#define MAX_FD_VALUE    (MAXMFRL/2)
#define MAX_FD_TMP      (MAXMFRL/2)

LONGX erro_fatal;
stack_node stack[max_stack];
int    stack_pt;
stack_node op1_node,op2_node,top_node;
stack_node *top,elem,*op1,*op2,*auxpop;

#if DINALLOC
//LONGX fmt_fsiz=MAXMFRL;               /* fmt_inter() - to set max field length - deslocado para cidbx.c */
static LONGX din_fsiz;           /* fmt_inter() - current max field length */
static char *tmp_str;
static LONGX max_tmp_str;
static LONGX max_fd_value;
static LONGX max_fd_tmp;
#else
static char tmp_str[MAX_TMP_STR+1];
static LONGX max_tmp_str=MAX_TMP_STR;
static LONGX max_fd_value=MAX_FD_VALUE;
static LONGX max_fd_tmp=MAX_FD_TMP;
#endif

int  tmp_str_index;
class_operand  new_class,class_of_the_both;
int result;
int cond_code;
instruction_code instruction;
int tempint;
int int_value_1,int_value_2;

float_x tempfloat,float_value_1,float_value_2 ;
LONGX templong ,long_value_1,long_value_2;

int there_is_fd_value;
int there_is_suf;
int rep_group;
int next_rep_occ;
int modify_fmt_pointers=true;
int no_modify_fmt_pointers=false;

#define infoi(i)        i->info.instr
#define ilab(i)         i->info.lab
#define iadd(i)         i->m_add
#define next_infoi(i)   i->next

#define fld_def_ptr(i)  i->m_add
#define str_const_ptr(i) i->m_add


/* mudar de local para global */
char *out;
LONGX lw_inter;
int pout;

char *address_out,*tmp_ptr;
LONGX  nextcc;


/* incio para achar bug de ref */
#if  DEBUG_REF
int iii;
LONGX iix;
#endif
/* fim para achar bug de ref */
/* getc_char */
#define get_char  /* getchar() */
/* 04-02-95 Existia um problema quando havia sufixo e este era
            colocado no fim de linha e logo apos um crlf.
            Mesmo o n sendo zero, isto e, nao tendo sido colocado
            um final do isis padrao pois havia sufixo, por causa
            do crlf eram retirados 2 caracteres. O crlf so pode
            ser levado em conta se tiverem sido acrescentados
            caracteres no final (n>0)
 */

#endif /* CICPP */

#define delete_chars(out,pout,next,n,lw) \
     if(n>0)                            \
     {                                  \
       if(fmt_CRLF(out,pout)) n=n+nl_LEN; /* mudar junto com crlf */ \
       pout=pout-n;                       \
       out[pout]=null_char;                    \
       next=next-n;                       \
       if(next<1) next=lw-n+1;            \
     }                                  \
/*--------------------------------------------------------------------------*/
/*                   retrieve_two_logical_operands                          */
/*--------------------------------------------------------------------------*/
#define  retrieve_two_logical_operands                                \
     auxpop = pop();                                                  \
     if (!fmterror)                                                   \
     {                                                                \
      op2=(stack_node *)memmove(&op2_node,auxpop,sizeof(stack_node));  \
      auxpop = pop();                                                 \
      if (!fmterror)                                                  \
      {                                                               \
       op1=(stack_node *)memmove(&op1_node,auxpop,sizeof(stack_node)); \
       if (op1->classe!=op2->classe || op1->classe != logical)        \
        inter_error(" Program error two logical operands expected");  \
       else                                                           \
        elem.classe=logical;                                          \
      }                                                               \
     }

/*----------------------------------------------------- AOT 27/12/91 -------*/
/*--------------------------------------------------------------------------*/
/*                   sc                                                     */
/* Conversao de strings do tipo "\n" em caracter especial "lf"	            */
/*--------------------------------------------------------------------------*/

#if !CICPP

struct tbc {
 char s_esp[3];
 int  c_esp;
/* } tb_espec[]={"\\r",'R',"\\t",'T',"\\b",'B',"\\n",'N',"\\0",'0'};*/
#if BEFORE20000518
 } tb_espec[]={"\\r",'\r',"\\t",'\t',"\\b",'\b',"\\n",'\n',"\\0",'\0'};
#else
#if BEFORE20000914
 } tb_espec[]={{"\\r",'\r'},{"\\t",'\t'},{"\\b",'\b'},{"\\n",'\n'},{"\\0",'\0'}};
#else
 } tb_espec[]={{"\\\r",'\r'},{"\\\t",'\t'},{"\\\b",'\b'},{"\\\n",'\n'},{"\\\0",'\0'}};
#endif
#endif

#endif /* CICPP */


#if CICPP
void FMTSTRU :: sc(char *pp)
#else /* CICPP */
#if ANSI
void sc(char *pp)
#else /* ANSI */
void sc(pp)
char *pp;
#endif /* ANSI */
#endif /* CICPP */
{
 char *tmp,*p,*r;
 int k;
 ALLOPARM cte_siz;
 cte_siz=(ALLOPARM) strlen((char *)pp);
 tmp=fmt_alloc_char( (ALLOPARM)(cte_siz+1),"cifm2/sc/alloc");
 /* if (tmp == NULL) IFERR_GOTO; Label ERROR_LABEL is defined at fmt_inter */
 if (tmp == NULL) fatal("cifm2/sc/alloc/2");
 strcpy(tmp,pp);
 p=pp;
 r=tmp;
 for (; *tmp ;p++,tmp++) {
   if (*tmp!='\\') {
    *p=*tmp;
   }else{
   for (k=0; tb_espec[k].c_esp;k++){
#if BEFORE20000914
      if (strncmp(tb_espec[k].s_esp,(char *)tmp,2)==0) {
#else
      if (strncmp(tb_espec[k].s_esp,(char *)tmp,strlen(tb_espec[k].s_esp))==0) {
#endif
	 *p=(char)tb_espec[k].c_esp;
	 tmp++;
#if 01
#ifndef CICPP
	  printf("\n++ achou|%s|", tb_espec[k].s_esp);
#endif /* CICPP */
#endif
	 break;
      }
   }
   }
 }
 *p=null_char;
#if 0
 printf ("\nDentro sc=|%s|",q);
 printf ("\nDentro tmp=|%s|",tmp);
#endif
#if CICPP
        delete [] r;
#else
        FREE(r);
#endif

}

/*--------------------------------------------------------------------------*/
/*                   trace_field                                            */
/*--------------------------------------------------------------------------*/
#if CICPP
void FMTSTRU :: trace_field(field_definition_node *i,
                            char *p)
#else /* CICPP */
#if ANSI
void trace_field(field_definition_node *i,
                 char *p)
#else /*ANSI*/
void trace_field(i,p)
field_definition_node *i;
char *p;
#endif /*ANSI*/
#endif /* CICPP */
{
int n=0;
printf(
"#%d[tag=%d,subf=%c,id1=%d,id2=%d,off=%d,len=%d,max=%d,last=%d\n=%s,\n",
             n,
             i->tag,
             i->sub_field,
             i->indent1,
             i->indent2,
             i->offset,
             i->length,
             i->max_occ,
             i->last_occ,p);
printf(
"act=%d,first=%d\n",
             i->actual_occ,
             i->first_occ);
 }

/*--------------------------------------------------------------------------*/
/*                   fmt_type_number                                        */
/*--------------------------------------------------------------------------*/
/* type(tipo,formato)
  tipo =1 alphanumeric
  tipo =2 alphabetic
  tipo =3 numeric
  tipo =4 inteiro decimal. Opcional sinal
  tipo =5 numero decimal incluindo scientific

*/
#if CICPP
int FMTSTRU :: fmt_type_number(int tipo,
                               char *fmt)
#else /* CICPP */
#if ANSI
int  fmt_type_number(int tipo,char *fmt)
#else
int  fmt_type_number(tipo,fmt)
int  tipo;
char *fmt;
#endif /*ANSI*/
#endif /* CICPP */
{ char *p,*pt_num;
  int res,a,n,len,scien;
  res= 0;
  p=fmt;
  len=strlen(p);
  if (len==0) return(1); /* Vazio retorna sempre 1 */

 switch (tipo) {
  case 1 :
  case 2 :
  case 3 :
       a=0;n=0;
       while (*p) {
         if (isiswctab[*p]) a++;
         if (isdigit(*p)) n++;
         p++;
       }
       if (tipo == 1 &&  a+n == len) res=1 ;
         else if (tipo == 2 &&  a == len) res=1;
                else if (tipo == 3 && n == len) res=1;

       break;
  case 4 :
  case 5 :
      a=0;
      scien=0;
      n=0; /* supor numero inteiro */
      pt_num=find_numeric_string(p,&a);
      if ( (strchr(pt_num,'e') != nulo) ||
           (strchr(pt_num,'E') !=nulo ) )  scien=1;
      if (strchr(pt_num,'.') !=nulo) n=1;

      p=fmt;
      a=1;
      while (a) {  /* Micro isis aceita espacos `a esqueda */
        if (*p != space_char || *p == null_char )a=0;
        else p++ ;
      }
      if ((size_t)strlen(pt_num)==(size_t)strlen(p)){
        if (tipo==4  && scien == 0 && n==0 ) res=1;
          else if (tipo == 5) res=1;
      }
      break;

 }
 return(res);
}
/*--------------------------------------------------------------------------*/
/*                   fmt_type_pattern                                       */
/*--------------------------------------------------------------------------*/
/* Compara um string com um padrao  do tipo
   patt=xx-xx-99-xx-aa
   str =ca-dd-11-1a-bb
   Se tamanhos s~ao difentes, entao resultado falso
   Senao compara caracter a caracter
*/
#if CICPP
int FMTSTRU :: fmt_type_pattern(char *patt,
                                char *fmt)
#else /* CICPP */
#if ANSI
int  fmt_type_pattern(char *patt,char *fmt)
#else
int  fmt_type_pattern(patt,fmt)
char *patt;
char *fmt;
#endif /*ANSI*/
#endif /* CICPP */
{ char *p,*q;
  int res,l;
  p=patt; q=fmt; res=1;
  l=strlen(patt);
  if (l != strlen(fmt)) return(0);
  while(res && *p ) {
    switch ( toupper(*p)) {
     case 'A' :
         res = isiswctab[*q];
         break;
     case 'X' :
         res = 1;
         break;
    case '9' :
        if ( !isdigit(*q)) res=0;
        break;
    default :
        res = (*q == *p);
    }
    p++;q++;
  }
  return (res);

}
/*----------------------------------------------------- AOT 27/12/91 -------*/
/*--------------------------------------------------------------------------*/
/*                   fmt_type                                               */
/*--------------------------------------------------------------------------*/
/* Usa rotina que separa dentro de um string o proximo numero
   Se o tamanho do numero separado e igual ao tamanho do string e'porque
   o string e numerico.
   Caso contrario verifica se contem apenas letras para ser
   alfabetico.
   Caso contrario e alfanumerico
*/
#if CICPP
char * FMTSTRU :: fmt_type(char *p)
#else /* CICPP */
#if ANSI
char *fmt_type(char *p)
#else /*ANSI*/
char *fmt_type(p)
char *p;
#endif /*ANSI*/
#endif /* CICPP */
{
  static char tmp_ty[2];
  int i;
  char *q,*pt_num;
  q=p;
  tmp_ty[1]=null_char;
  tmp_ty[0]=TY_X;
  i=0;
  pt_num=find_numeric_string(p,&i);
  if ((size_t)strlen(pt_num)==(size_t)strlen(p)){
      tmp_ty[0]=TY_N;
  }else{
      while (isiswctab[*q] || *q==' ') q++;
      if (*q==null_char)tmp_ty[0]=TY_A;
  }
  /*03-11-99 Alterar para retornar ''  quando string vazio */
  if ( (size_t)strlen(p) == (size_t)0 ) tmp_ty[0]=null_char;
  return (char *)tmp_ty;
}
/*--------------------------------------------------------------------------*/
/*                fmt_init_E_S                                               */
/*--------------------------------------------------------------------------*/
#if CICPP
void FMTSTRU :: fmt_init_E_S(void)
#else /* CICPP */
#if ANSI
void fmt_init_E_S()
#else /*ANSI*/
void fmt_init_E_S()
#endif
#endif /* CICPP */
{ int i;
  for (i=0;i<NMAXVAR;i++) {
      E_var[i]= (float_x) 0;
      S_var[i]= NULL;
  }
}
/*--------------------------------------------------------------------------*/
/*                  fmt_free_S                                            */
/*--------------------------------------------------------------------------*/
#if CICPP
void FMTSTRU :: fmt_free_S(int from,
                           int to)
#else /* CICPP */
#if ANSI
void fmt_free_S(int from, int to)
#else /*ANSI*/
void fmt_free_S(from ,to)
int from, to;
#endif
#endif /* CICPP */
{ int i;

  for (i=from;i<=to;i++) {

      if  (S_var[i] != NULL)
#if 0
       printf("Lib=[%p]\n", S_var[i]);
#endif
#if CICPP
                delete [] (char *)S_var[i];
#else /* CICPP */
                FREE(S_var[i]);
#endif /* CICPP */
      S_var[i]=NULL;
  }
}


/*--------------------------------------------------------------------------*/
/*                   fmt_copy_realloc                                       */
/*--------------------------------------------------------------------------*/
#if CICPP
char * FMTSTRU :: fmt_copy_realloc(char *pdest,
                                   ALLOPARM *dest_mxlen,
                                   char *cte_str)
#else /* CICPP */
#if ANSI
char *fmt_copy_realloc(char *pdest,ALLOPARM *dest_mxlen,char *cte_str)
#else /*ANSI*/
char *fmt_copy_realloc(pdest,dest_mxlen,cte_str)
                            /*Armazena cte_str em pdest        */
  char *pdest;               /*Se necessario realloca pdest para*/
  ALLOPARM *dest_mxlen;     /*que str_cte caiba e muda tamanho   */
  char *cte_str;            /* max do pdest.Retorna novo endereco*/
#endif /*ANSI*/
#endif /* CICPP */
 {
      char *p;
      ALLOPARM cte_siz;
      cte_siz=(ALLOPARM) strlen((char *)cte_str);
      if (*dest_mxlen<cte_siz){ /* realloca string */
        p=fmt_alloc_char( (ALLOPARM)(cte_siz+1),"cifm3/fmt_realloc");
        /* if (p == NULL) IFERR_GOTO; Label ERROR_LABEL is defined at fmt_inter */
        if (p == NULL) fatal("cifm3/fmt_realloc/2");
        strcpy(p,cte_str);
#if CICPP
        delete [] pdest;
#else
        FREE(pdest);
#endif

        *dest_mxlen=cte_siz;
        return p;
      }else {  /* copia apenas cte_str */
        strcpy(pdest,cte_str);
        return pdest; /*para efeito de compatibilizar */
      }
}
/*--------------------------------------------------------------------------*/
/*                   fmt_alloc_char                                         */
/*--------------------------------------------------------------------------*/

#if CICPP
char * FMTSTRU :: fmt_alloc_char(ALLOPARM tam,
                                 char *msg)
#else /* CICPP */
#if ANSI
char *fmt_alloc_char(ALLOPARM tam,char *msg)
#else /*ANSI*/
char *fmt_alloc_char(tam,msg)
ALLOPARM tam;
char *msg;
#endif /*ANSI*/
#endif /* CICPP */
{
 char *t;
#if CICPP
  try
  { t=(char *)new char [(tam)]; }
  catch (BAD_ALLOC)
  { t=(char *)ALLONULL; }
#else /* CICPP */
  t=(char *)ALLOC((ALLOPARM)(tam));
#endif /* CICPP */
  if (t == (char *)ALLONULL) {inter_error(msg); return NULL; }
  return t;
}


/*--------------------------------------------------------------------------*/
/*                   fmt_instr                                              */
/*--------------------------------------------------------------------------*/
#if CICPP
int FMTSTRU :: fmt_instr(char *str_source,
                         char *str_sub)
#else /* CICPP */
#if ANSI
int fmt_instr(char *str_source,char *str_sub)
#else /*ANSI*/
int fmt_instr(str_source,str_sub)
char *str_source,*str_sub;
#endif /*ANSI*/
#endif /* CICPP */
{
  char *p,*res;
  int  pos;
  pos=0;
  res=NULL;
  p=str_source;
  res = strstr(p,str_sub);
  if  (res!=NULL && p!=NULL && str_sub!=NULL ) {
    if (*res && *str_sub) {
      int ll;
      ll=strlen(str_sub);
      pos=1;
      while (memcmp(p,str_sub,ll)!=0)  {
        p++; pos++;
     }
   }
 }
  return pos;
}
/*--------------------------------------------------------------------------*/
/*                   retrieve_two_numeric_operands                          */
/*--------------------------------------------------------------------------*/
#if CICPP
void FMTSTRU :: take_numeric_operands_value(class_operand new_class)
#else /* CICPP */
#if ANSI
void take_numeric_operands_value(class_operand new_class)
#else /*ANSI*/
void take_numeric_operands_value(new_class)
class_operand new_class;
#endif /*ANSI*/
#endif /* CICPP */
{
  int_value_1=0;
  int_value_2=0;
  long_value_1=0L;
  long_value_2=0L;
  float_value_1=0.0;
  float_value_2=0.0;


 switch(new_class)
 { case integer :
        int_value_1=op1->op.i;
        int_value_2=op2->op.i;
        break;
   case long_n :
        if(op1->classe==integer) long_value_1=(LONGX) op1->op.i;
        if(op1->classe==long_n)  long_value_1=op1->op.l;
        if(op2->classe==integer) long_value_2=(LONGX) op2->op.i;
        if(op2->classe==long_n)  long_value_2=op2->op.l;
        break;
   case float_n :
        if(op1->classe==integer)    float_value_1=(float) op1->op.i;
        if(op1->classe==long_n)     float_value_1=(float) op1->op.l;
        if(op1->classe==float_n)    float_value_1=op1->op.r;
        if(op2->classe==integer)    float_value_2=(float) op2->op.i;
        if(op2->classe==long_n)     float_value_2=(float) op2->op.l;
        if(op2->classe==float_n)    float_value_2=op2->op.r;

        break;

}
}

#if CICPP
void FMTSTRU :: retrieve_determine_class(void)
#else /* CICPP */
void retrieve_determine_class()
#endif /* CICPP */
{
 auxpop = pop(); IFERR_RET;
 op2=(stack_node *)memmove(&op2_node,auxpop,sizeof(stack_node));
 auxpop = pop(); IFERR_RET;
 op1=(stack_node *)memmove(&op1_node,auxpop,sizeof(stack_node));
 if (op1->classe==string && op2->classe==string){new_class=string;}
  else
  { if ( (op1->classe==integer||op1->classe==float_n ||
          op1->classe==long_n
         ) &&
      (op2->classe==integer || op2->classe==float_n || op2->classe==long_n) )
      {if (op1->classe == op2->classe)
          { new_class=op1->classe ; }
       else {if (( op1->classe == integer || op1->classe == long_n ) &&
                 (op2->classe == integer  || op2->classe == long_n)
                ) new_class=long_n ;
                else new_class=float_n;
             }
      }
      else
      {
       inter_error("Invalid class of operands for aritmetic/string operators");
       return;
      }
 }
}

/*-----------------------------------------------------------------------*/
#if CICPP
void  FMTSTRU :: retrieve_two_numeric_operands (void)
#else /* CICPP */
void  retrieve_two_numeric_operands ()
#endif /* CICPP */
{
 retrieve_determine_class();
 IFERR_RET;
 if(new_class==string)
 {
    inter_error("Invalid class of operands for aritmetic operators");
    return;
 }
 else  take_numeric_operands_value(new_class);

} /* precedure */
/*--------------------------------------------------------------------------*/
/*                   retrieve_two_operands                                  */
/*--------------------------------------------------------------------------*/
#if CICPP
void FMTSTRU :: retrieve_two_operands(void)
#else /* CICPP */
void retrieve_two_operands()
#endif /* CICPP */
{
retrieve_determine_class();
IFERR_RET;
class_of_the_both=new_class;
if (new_class!=string) take_numeric_operands_value(new_class);

}


/*-------------------------------------------------------------------------*/
/*                   is_not_of_class                                       */
/*-------------------------------------------------------------------------*/
#define is_not_of_class(x,cl)                                    \
  if(x!=cl) inter_error("Invalid operand class ");                 \


/*--------------------------------------------------------------------------*/
/*                   is_numeric                                             */
/*--------------------------------------------------------------------------*/

#if CICPP
int FMTSTRU :: is_numeric(class_operand x)
#else /* CICPP */
#if ANSI
int is_numeric(class_operand x)
#else /*ANSI*/
int is_numeric(x)
class_operand x;
#endif /*ANSI*/
#endif /* CICPP */
{
   if ( !( x==integer || x==float_n || x==long_n) )
      { inter_error("Aritmetic operand expected "); }
 return(1);
}


/*--------------------------------------------------------------------------*/
/*                   convert_to_float                                       */
/*--------------------------------------------------------------------------*/
#if CICPP
float_x FMTSTRU :: convert_to_float(stack_node *top)
#else /* CICPP */
#if ANSI
float_x convert_to_float(stack_node *top)
#else /*ANSI*/
float_x convert_to_float(top)
stack_node *top;
#endif /*ANSI*/
#endif /* CICPP */
{ float_x x;
   is_numeric(top->classe);
   if (top->classe == integer) x = (float_x) top->op.i;
   if (top->classe == float_n) x = (float_x) top->op.r;
   if (top->classe == long_n)  x = (float_x) top->op.l;
   return (x);
}



/*------------------------------------------------------------------------*/
/*                   save_context                                         */
/*------------------------------------------------------------------------*/

#if CICPP
void  FMTSTRU :: save_context(int modify_pointers)
#else /* CICPP */
#if ANSI
void  save_context(int modify_pointers)
#else /*ANSI*/
void  save_context(modify_pointers)
int modify_pointers;
#endif /*ANSI*/
#endif /* CICPP */
{

#if DEB_FLOAT
   printf("\n++Save_context l_w=%"_LD_" nextcc=%"_LD_" pout=%d \nout=%s|",
          lw_inter,nextcc,pout,out);
#endif
      elem.classe=next_cc;
      elem.op.l=nextcc;
      push(&elem);
      IFERR_RET;

      elem.classe=xindex;
      elem.op.i=pout;
      push(&elem);
      IFERR_RET;

      elem.classe=l_w;
      elem.op.l=lw_inter;
      push(&elem);
      IFERR_RET;

      elem.classe=ptr;
      elem.op.address= &out[0];
      push(&elem);
      IFERR_RET;

      /* define new context */

        out= &out[pout];
        pout=0;
#if TRACESINDO
      address_out=out;
#endif
       /* These pointers must not be modified for the REF function */
      if (modify_pointers==true)
       {
                 lw_inter= MAXMFRL; /* no limits*/ /* aot - svd 8000L */
                 nextcc=1;
       }

}

/*--------------------------------------------------------------------------*/
/*                   restore_context                                        */
/*--------------------------------------------------------------------------*/

#if CICPP
void FMTSTRU :: restore_context(void)
#else /* CICPP */
void restore_context()
#endif /* CICPP */
{
#if DEB_FLOAT
     int i;
#endif
     tmp_ptr= &out[0];  /*save old string */

      /*restore  the  context */
     auxpop = pop(); IFERR_RET;
     top=(stack_node *)memmove(&top_node,auxpop,sizeof(stack_node));
     is_not_of_class(top->classe,ptr);
     IFERR_RET;
     out=top->op.address;
     address_out=out;

     auxpop = pop(); IFERR_RET;
     top=(stack_node *)memmove(&top_node,auxpop,sizeof(stack_node));
     is_not_of_class(top->classe,l_w);
     IFERR_RET;
     lw_inter=top->op.l;

     auxpop = pop(); IFERR_RET;
     top=(stack_node *)memmove(&top_node,auxpop,sizeof(stack_node));
     is_not_of_class(top->classe,xindex);
     if (fmt_error) return;
     pout=top->op.i;

     auxpop = pop(); IFERR_RET;
     top=(stack_node *)memmove(&top_node,auxpop,sizeof(stack_node));
     is_not_of_class(top->classe,next_cc);
     IFERR_RET;
      nextcc=top->op.l;
#if DEB_FLOAT
printf("\n++Rest_ctext l_w=%"_LD_" nextcc=%"_LD_" pout=%d \nout=%s|\n",
          lw_inter,nextcc,pout,out);

     if (pout>5) {
            for (i=pout-5;i<=pout;i++)printf("%c",out[i]);
                  printf("*");
     }
#endif

}

/*--------------------------------------------------------------------------*/
/*                   sub_string                                             */
/*--------------------------------------------------------------------------*/
#if CICPP
char * FMTSTRU :: sub_string(char source[],
                             int offset,
                             int length)
#else /* CICPP */
#if ANSI
char *sub_string(char source[],
                 int offset,
                 int length)
#else /*ANSI*/
char *sub_string(source,offset,length)
char source[];
int offset;
int length;
#endif /*ANSI*/
#endif /* CICPP */
{
int fd_len;
char *q;

q=source;

fd_len=strlen(source);
if(offset<0 || length<0) return(q);
if(offset >= fd_len) *q=null_char;
 else
  {
  q= &source[offset];
  fd_len=fd_len-offset;
  if(fd_len>length)q[length]=null_char;
  }
 return q;
 }

#if !CICPP
static char *retorno;
#endif /* CICPP */


/*--------------------------------------------------------------------------*/
/*                   store tmp_string                                       */
/*--------------------------------------------------------------------------*/
#if CICPP
char * FMTSTRU :: store_tmp_string (char *str)
#else /* CICPP */
#if ANSI
char *store_tmp_string (char *str)
#else /*ANSI*/
char *store_tmp_string (str)
char *str;
#endif /*ANSI*/
#endif /* CICPP */
{
 int len;

 if (!str) str=""; /* AOT 04/03/97 */
 len=strlen(str);
#if DEB_FLOAT
  printf("\n--Storetmp str=%s|\nlen=%d",str,len);
#endif
 if(tmp_str_index+len > max_tmp_str)
 {
   inter_error("No room to store temporary strings - change MAX_TMP_STR");
   return NULL;
 }
 strcpy(&tmp_str[tmp_str_index],str);
 retorno= &tmp_str[tmp_str_index];
 tmp_str_index += len+1;
 return retorno;
 }




/*--------------------------------------------------------------------------*/
/*                   cast_to_int                                            */
/*--------------------------------------------------------------------------*/
#if CICPP
int FMTSTRU :: cast_to_int (stack_node *e)
#else /* CICPP */
#if ANSI
int cast_to_int (stack_node *e)
#else /*ANSI*/
int cast_to_int (e)
stack_node *e;
#endif /*ANSI*/
#endif /* CICPP */
{int tmp;
 if (e->classe == integer) tmp=e->op.i;
 if (e->classe == long_n ) tmp=(int )e->op.l;
 if (e->classe == float_n) tmp= (int) e->op.r;
 return (tmp);
}
/*--------------------------------------------------------------------------*/
/*                   push                                                   */
/*--------------------------------------------------------------------------*/
#if CICPP
void FMTSTRU :: push (stack_node *e)
#else /* CICPP */
#if ANSI
void push (stack_node *e)
#else /*ANSI*/
void push (e)
stack_node *e;
#endif /*ANSI*/
#endif /* CICPP */
{
 stack_pt++;
 if(stack_pt >= max_stack)                              /* AOT - 27/11/91 */
 {
   inter_error("Stack overflow - expression too complex ");
   return;
 }
 stack[stack_pt].classe=e->classe;
 stack[stack_pt].op=e->op;
 }

/*--------------------------------------------------------------------------*/
/*                   pop                                                    */
/*--------------------------------------------------------------------------*/
#if CICPP
FMTSTRU :: stack_node * FMTSTRU :: pop(void)
#else /* CICPP */
stack_node *pop()
#endif /* CICPP */
{
  static stack_node rr;
  static stack_node *prr;
 prr= &rr;
 if(stack_pt<1){inter_error("Stack underflow"); return NULL;}
 rr.classe=stack[stack_pt].classe;
 rr.op=stack[stack_pt].op;
 stack_pt--;
 return prr;
}


/*--------------------------------------------------------------------------*/
/*                   exec_percent                                           */
/* Applies de % rules to the output string "out" pointed by "outs"          */
/* A07 - Rotina com erro: retirava todos os "newline" do fim da linha e     */
/*       na verdade tem que deixar apenas um	                            */
/*       Retorna 0 se nao ficaram caracteres de "muda-linha"                */
/*              >0 caso contrario	                                    */
/*--------------------------------------------------------------------------*/

#if CICPP
int FMTSTRU :: exec_percent(char *out,
                            int  *outs,
                            LONGX *ncc)
#else /* CICPP */
#if ANSI
int exec_percent(char *out,
                  int  *outs,
                  LONGX *ncc)
#else /*ANSI*/
int exec_percent(out,outs,ncc)
char *out;
int *outs;
LONGX *ncc;
#endif /*ANSI*/
#endif /* CICPP */
{
 int nnl,k,noend;
 k= *outs;
 noend=true;
 nnl=0;

 /* Determina quantos "muda_linha existem no string" */
 while( noend==true && k>nl_LEN){
   noend=fmt_CRLF(out,k);
   if (noend==true){
       k=k-nl_LEN;
       nnl++;
   }
 } /*while*/

 if (nnl>1) {
#if MICRO_ISIS_COMPATIBLE
   /* Retira os nnl-1 "muda-linha" excedentes */
   nnl--;
#endif
   k=*outs-nnl*nl_LEN;
   out[k]=null_char;
   *outs=k;
   *ncc=1;
 }
 return nnl;
#if 0
 *outs=k;
 /* end string and modify pointer */
 /* calculate the  lentgh of the last line to set up nextcc */
 nnl=1;
 while( !fmt_CRLF(out,k) &&k>1)
 { l++;k--;}
 *ncc=l;
#endif
} /*percent */



/*--------------------------------------------------------------------------*/
/*                   trata_md                                               */
/*--------------------------------------------------------------------------*/

#if CICPP
char * FMTSTRU :: trata_md(instruction_code mode,
                           char            *s,
                           int             *final_added,
                           char           **tofreep)
#else /* CICPP */
#if ANSI
char *trata_md(instruction_code mode,
               char            *s,
               int             *final_added,
               char           **tofreep)
#else /*ANSI*/
char *trata_md(mode,s,final_added,tofreep)
instruction_code mode;
char *s;
int *final_added;
char **tofreep;
#endif /*ANSI*/
#endif /* CICPP */
{
 char *q;               /* now via ALLOC() */
 char *pq;

 char c;
 int i,j,k,fim,len;
 int menor;

 *tofreep=NULL;

 /* for mpl and mpu modes there's no change in the filling information */

 if(mode==mpu_par || mode==mpl_par)
 {
  return s;
 }

 if((len=strlen(s))==0) return s;
#if !RETIRAR
  Consultar Adalberto/Piva
  #if CICPP
    try
    { q=(char *)new char [(len+3+9+1)]; }
    catch (BAD_ALLOC)
    { q=(char *)ALLONULL; }
  #else /* CICPP */
    q=(char *)ALLOC((ALLOPARM)(len+3+9+1));
  #endif /* CICPP */
  if (q == (char *)ALLONULL) {inter_error("trata_md/ALLOC"); return NULL; }
#else
  q=fmt_alloc_char((ALLOPARM)(len+3+9+1),"trata_md/ALLOC");
#endif /*RETIRAR */

 *tofreep=q;

 pq=q;
 i=0;
 j=0;
 menor=false;
 fim=false;
 *final_added=0;
 q[0]=null_char;

while (fim==false)
{
#if TRACE_COMP_M
if( s[i]==null_char ) if (fmttrace) printf(" \n achou null no comeco mode");
#endif /* TRACE_COMP_M */
c= s[i];

/* first stores a character into the output; then verifies if the
   character had to be stored or must be eliminated */

 q[j]=c;
 i++;
 j++;

if (c==null_char)
 { /* string analisis has finished
      for mdu and mdl two blanks and a punctuation mark( if none)
      has to be added;
      for mhu and mhl nothing is added
   */


  if(mode==mhl_par || mode==mhu_par)
       {
         pq= sub_field_change(pq);
         return pq;
       }
  /* aplica pontuacao */
  fim=false;
  for (k=j-1;k>=0 && fim==false; k--)
   {if(q[k]!=' ') fim=true;}
  if( !ispunctuation(q[k]) )
      {
      *final_added=1;
       k++;
       q[k]='.';
      }
  k++;
  q[k++]=' ';
  q[k++]=' ';
  q[k]=null_char;
  *final_added= *final_added+2;
   pq= sub_field_change(pq);
   return pq;
  }

  if ((c==equal_char) && (menor==true))
   {  /* ignores the string after "="
                                for the constructions like <100=hundred> */
     while (( s[i]!=null_char) && (s[i]!=greater_char)) {
           i=i+1;}
     /* ignore all chars befor e < */
    if (s[i]==greater_char)
       {
         if (s[i+1]==less_char)
           { q[j-1]=';' ;
             q[j]=' ';
             j++;
             i++;
           }
          else {
                menor=false;
                j--;
                }
       }
      else {  /* s[i]=null_char. Retira o "=" */
            j--;
            q[j]=null_char;
            fim=true;
            fim=false; /* SVD - 25/09/94  Erro Mas Aceita */
           }
    } /*c=eql_char..*/
  else { if (c==less_char)
          {  menor=true;
            j--;
           }
          else
             { if (c==greater_char)
                   {
                    if (s[i]==less_char)
                      { q[j-1]=';';
                        q[j]=' ';
                        j++;
                        i++;
                        menor=true;
                        }
                       else {
                              j--;
                             }
                 }
             }
        }

   }

/* 25-09-94
  Esta dando erro quando um string tem no fim uma sequencia  do tipo "<x=y"
   O MicroIsis assume um ">" e eu estava indicando erro.
   Para deixar como ISIS , retorna o string que foi conseguido  enquanto
   procurava o ">".
  ( Acho que a  parte de tratamento de sequencias  "..><.." pelo MicroIsis
    esta incoerente com a  documentacao .
    Nao vou mexer na minha implementacao sem saber qual a implementacao correta
  )
    E importante notar que se o campo contem ">" ou "<"estes sao sempre
   ignorados.

   Precisa forcar a colocar a pontuacao e tratamento de subfield.
   Para isso basta mudar a condicao de fim=true  para fim=false quando
   nao achou   o ">". Assim forca a repetir o loop e o proximo caracter
   a ser tratado e'c=null_char o que vai fazer com que execute a pontuacao

   return("rot.c/trata_md/<100=hundred>");
*/

   return(s);   /* AOT/RP - 21/10/94 (ver com Sindo) */
}


/*------------------------------------------------------------------------*/
/*                  sub_field_string                                      */
/*------------------------------------------------------------------------*/
#if CICPP
void FMTSTRU :: sub_field_string(char *q,
                                 char sub)
#else /* CICPP */
#if ANSI
static void sub_field_string(char *q,char sub)
#else /*ANSI*/
static void sub_field_string(q,sub)
char *q;
char sub;
#endif /*ANSI*/
#endif /* CICPP */
{
char *ss,*p;
char *t,r[3];
int len;

len=strlen(q);

if(len==0) return;

if(sub==subfield_null) return;
#if !RETIRAR
#if CICPP
 try
 { ss=(char *)new char [(len+1)]; }
  catch (BAD_ALLOC)
 { ss=(char *)ALLONULL; }
#else /* CICPP */
ss=(char *)ALLOC((ALLOPARM)(len+1));
#endif /* CICPP */
 if (ss == (char *)ALLONULL) {inter_error("sub_field_string/ALLOC"); return; }
#else
 ss=fmt_alloc_char((ALLOPARM)(len+1),"sub_field_string/ALLOC");
#endif
 if (ss == (char *)ALLONULL) return;

ss[0]=null_char;
r[0]=SFLDCHR;
r[1]=' ';
r[2]=null_char;
t=q;

if(sub==multiply_char)     /* any subfield */
 {
 if (len >=2 )
   {
   if (*t==SFLDCHR) {t++; t++;}  /* ignores 2 char " ^char" */
   }
   /*  len < 2  or not SFLDCHR => It is not a field with subfield */
 }
 else
 { /* search the substring "^char"  using upper and lower cases */
  r[1]=tolower(sub);
  t=strstr(q,r);
  if(t==nulo)
   {
     r[1]=toupper(sub);
     t=strstr(q,r);
   }

   if(t==nulo) {
       q[0]=null_char;
#if CICPP  /*ifcmm*/
      delete [] ss;
#else /*elsecmm*/
      FREE(ss);
#endif /*CICPP*/  /*endcmm*/
      return;
  }

   /* copies the substring from t to the end or until another substring delimiter */
   t++;
   t++;  /* ignores the delimiters */
 }

 p=ss;
 while ( (*t!=null_char) && (*t!=SFLDCHR) ) {
        if (*t == '\\') if (*(t+1) == SFLDCHR) t++;
        if (*t == null_char) break;
        *p++ = *t++;
 }

 /* string termination */
 *p=null_char;
 strcpy(q,ss);
#if CICPP
 delete [] (char *)ss;
#else /* CICPP */
 FREE(ss);
#endif /* CICPP */
 return;
}


#if !CICPP

/* rotinas aritmeticas */
static char str_num[30],*pstr_num;
static LONGX ref_mfn_number;
/* end rotinas aritmeticas */

/* f function variables*/
static int min_width;
static int dec_places;
/* end f-function variables */

#if DINALLOC
static char *v_fd_value;
static char *v_fd_tmp;
#else
static char v_fd_value[MAX_FD_VALUE+1];
static char v_fd_tmp[MAX_FD_TMP+1];
#endif

static char *fd_value,*fd_tmp;

static LONGX lineqtt; /* RPIVA/AOT/HB - 16/11/98 */

#endif /* CICPP */

/* ----------------------------- cifm3h.c --------------------------- */

/*------------------------------------------------------------------------*/
/*    file: cifm3h.c                                                      */
/*------------------------------------------------------------------------*/
#if !CICPP

#define PRT_DBNAME 0
#define DEB_NOCC   0
/* Continue em grupos repetitivos  17-7-94 */
static int continue_rgroup_maxocc;
static int continue_rgroup;
static char cc;
static int pos;
static int fmt_fim;
static int fmt_n_ref;
static int must_repeat;
static l_code *ni;
static l_code *rpni; /*repf*/
static float_x float_exp_value;
static field_definition_node *itf;
static RECSTRU *recp;
#if !CICPP /*if!cmm*/
#define MAXREFS 16
static LONGX vprats[MAXREFS];
static int currprat=0;
static LONGX trm_prat;
static LONGX irec;
#endif /*CICPP*/  /*endcmm*/
static char *firstdbnp;

#if ANSI
static  char *fmt_get_date(int parmdate, LONGX secs0);
static  float_x fmt_cnv_to_float(stack_node *top);
static  LONGX    fmt_cnv_to_long(stack_node *top);
static  void fmt_init_vinstr(l_code *pins,int nnested );
static  int fmt_load_all_occ(void);
static  void fmt_load_next_occ(void);
static  void fmt_comum_ref_beg(void);
static  void fmt_comum_ref_end(void);
static   void   fmt_inicio_grupos_vfields(instruction_code instr_inicial,
                                          instruction_code instr_final);
#else
static  char *fmt_get_date();
static  float_x fmt_cnv_to_float();
static  LONGX    fmt_cnv_to_long();
static  void fmt_init_vinstr();
static  int fmt_load_all_occ();
static  void fmt_load_next_occ();
static  void fmt_comum_ref_beg();
static  void  fmt_comum_ref_end();
static  void  fmt_inicio_grupos_vfields();
#endif

#endif /* CICPP */

/* ------------------------field value  ----------------------------- */

#if CICPP
char * FMTSTRU :: field_value(char *sfieldvalue,
                              LONGX  max_sfield,
                              int   dd)
#else /* CICPP */
#if ANSI
char *field_value(char *sfieldvalue,
                  LONGX  max_sfield,
                  int   dd)
#else /*ANSI*/
char *field_value(sfieldvalue,max_sfield,dd)
char *sfieldvalue;
LONGX max_sfield;
int dd;
#endif /*ANSI*/
#endif /* CICPP */
{
    int slen;

    if (dd < 0 || dd >= (int)(MFRnvf) ) {
      sfieldvalue[0]=null_char;
      if (fmttrace)
        printf("field_value - dd=%d tag=undefined '%s'\n",
                                        dd,sfieldvalue);
      return (sfieldvalue);
    }

    slen=DIRlen(dd);

    if (slen > max_sfield) {
        slen=max_sfield-3;
        memmove(sfieldvalue,FIELDP(dd),slen);
        sprintf(sfieldvalue+slen,"+++");
        if (fmttrace) printf("%s MAX_SFIELD=%"_LD_"\n",sfieldvalue+slen,slen);
    }
    else {
        memmove(sfieldvalue,FIELDP(dd),slen);
        sfieldvalue[slen]='\0';
    }
    if (fmttrace)
        printf("field_value - dd=%d tag=%u '%s'\n",
                                        dd,DIRtag(dd),sfieldvalue);
    return (sfieldvalue);
}


/*-------------------------------------------------------------------------*/
/*          fmt_get_date                                                   */
/*-------------------------------------------------------------------------*/
#if CICPP
char  * FMTSTRU :: fmt_get_date(int parmdate, LONGX secs0)
#else /* CICPP */
#if ANSI
char  *fmt_get_date(int parmdate, LONGX secs0)
#else /*ANSI*/
char  *fmt_get_date(parmdate,secs0)
int parmdate;
LONGX secs0;
#endif /*ANSI*/
#endif /* CICPP */
{
 static char fmt_date[21+1]; /* aaaammdd hhmmss WDAY YDAY */
                             /* 1234567890123456 7  8 901 */
 time_t secs_now;
 struct tm *tp;
#if BEFORE20010216
#else
 if (secs0 >= 0) {
/*The allowable range of calendar times is Jan 1 1970 00:00:00 to Jan 19 2038 03:14:07.
*/
     if (secs0 < 86400)      secs0=86400;      /* seconds('19700102 000000') */
     if (secs0 > 2147408047) secs0=2147408047; /* seconds('20380118 031407') */
     secs_now=secs0;
 } else
#endif
 time(&secs_now);
/*time gives the current time, in seconds, elapsed since 00:00:00 GMT, January 1, 1970,
 and stores that value in the location pointed to by timer, provided that timer is not a NULL pointer.
*/
 tp=localtime(&secs_now);
/*localtime accepts the address of a value returned by time and returns a pointer to the structure of type tm containing the time elements.
It corrects for the time zone and possible daylight saving time.
The global LONGX variable _timezone contains the difference in seconds between GMT and local standard time (in PST, _timezone is 8 x 60 x 60). The global variable daylight contains nonzero only if the standard U.S. daylight saving time conversion should be applied. These values are set by tzset, not by the user program directly.
*/
 switch (parmdate) {
 case DATEONLY:
   sprintf(fmt_date,"%02d/%02d/%02d",
                     tp->tm_mday,tp->tm_mon+1,tp->tm_year%100);
 break;
 case DATETIME:
   sprintf(fmt_date,"%02d/%02d/%02d %02d:%02d:%02d",
                     tp->tm_mday,tp->tm_mon+1,tp->tm_year%100,
                     tp->tm_hour,tp->tm_min,tp->tm_sec);
   break;
 case 1:   /*A11 compatibilidade com microisis */
   sprintf(fmt_date,"%02d-%02d-%02d  %02d:%02d:%02d",
                     tp->tm_mon+1,tp->tm_mday,tp->tm_year%100,
                     tp->tm_hour,tp->tm_min,tp->tm_sec);

   break;
 case 2:
   sprintf(fmt_date,"%02d-%02d-%02d",
                     tp->tm_mon+1,tp->tm_mday,tp->tm_year%100);
   break;
 case 3:
    sprintf(fmt_date,"%02d:%02d:%02d",
                     tp->tm_hour,tp->tm_min,tp->tm_sec);
    break;
 default:
 sprintf(fmt_date,"%04d%02d%02d %02d%02d%02d %1d %3d",
                   1900+tp->tm_year,tp->tm_mon+1,tp->tm_mday,
                   tp->tm_hour,tp->tm_min,tp->tm_sec,
                   tp->tm_wday,tp->tm_yday);
 }
 return (char *)fmt_date;

}
/*-------------------------------------------------------------------------*/
/*          fmt_cnv_to_long                                                */
/*-------------------------------------------------------------------------*/
#if CICPP
LONGX FMTSTRU :: fmt_cnv_to_long(stack_node *top)
#else /* CICPP */
#if ANSI
LONGX fmt_cnv_to_long(stack_node *top)
#else /*ANSI*/
LONGX fmt_cnv_to_long(top)
stack_node *top;
#endif /*ANSI*/
#endif /* CICPP */
{ static LONGX tmp_long;

 if (top->classe==long_n) tmp_long=top->op.l;
   else
    if (top->classe==float_n) tmp_long=(LONGX) top->op.r;
      else if(top->classe==integer) tmp_long =(LONGX )top->op.i;
          else fatal("fmt_inter/Invalid number type ");
 return tmp_long;
}
/*-------------------------------------------------------------------------*/
/*          fmt_cnv_to_float                                               */
/*-------------------------------------------------------------------------*/
#if CICPP
float_x FMTSTRU :: fmt_cnv_to_float(stack_node *top)
#else /* CICPP */
#if ANSI
float_x fmt_cnv_to_float(stack_node *top)
#else /*ANSI*/
float_x fmt_cnv_to_float(top)
stack_node *top;
#endif /*ANSI*/
#endif /* CICPP */
{ static float_x tmp_float;

 if (top->classe==long_n) tmp_float=(float_x ) (top->op.l);
   else
    if (top->classe==float_n) tmp_float=(float_x) top->op.r;
      else if(top->classe==integer) tmp_float=(float_x )top->op.i;
          else fatal("fmt_inter/Invalid number type ");
 return tmp_float;
}

/*-------------------------------------------------------------------------*/
/*          fmt_init_vinstr                                                */
/*-------------------------------------------------------------------------*/
#if CICPP
void FMTSTRU :: fmt_init_vinstr(l_code *ptr_ins,
                                int     nnested)
#else /* CICPP */
#if ANSI
void fmt_init_vinstr(l_code *ptr_ins,
                     int     nnested)
#else /*ANSI*/
void fmt_init_vinstr(ptr_ins, nnested )
l_code *ptr_ins;
int  nnested;
#endif /*ANSI*/
#endif /* CICPP */
{
  int tmp_occ;
  if ( (infoi(ptr_ins)==test_occ)  ||  (infoi(ptr_ins)==load_field_all) ||
       (infoi(ptr_ins)==absent)    ||  (infoi(ptr_ins)==load_field_occ) ||
       (infoi(ptr_ins)==present)   ||  (infoi(ptr_ins)==print_field)    ||
       (infoi(ptr_ins)==noccins)   )
  { itf=(field_definition_node *) iadd(ptr_ins);
    itf->max_occ= -1;
    itf->actual_occ=0;
    itf->last_occ=0;
    itf->first_occ=0;
    /* Implementar continue dentro de groupo repetitivo  17-7-94*/
     if(nnested==1){
#if CICPP  /*ifcmm*/
       tmp_occ=recp->xnocc(itf->tag);
#else /*elsecmm*/
       tmp_occ=nocc(irec,itf->tag);
#endif /*CICPP*/  /*endcmm*/
       if (tmp_occ>continue_rgroup_maxocc)continue_rgroup_maxocc=tmp_occ;
    }
   }
}
/*-------------------------------------------------------------------------*/
/*          fmt_inicio_grupos_vfields                                      */
/*-------------------------------------------------------------------------*/
/*DOC 010:
 No inicio de um novo grupo e' necesario guardar o contexto
 em que o formato esta sendo executado e inicializar essas
 variaveis. Esse contexto e representado pelas variaveis
  rep_group
  next_rep_occ
  must_repeat.
 Os contadores das  instrucoes que referenciam Fields (Vfields)
 devem ser inicializados para permitir repeticao dentro
 de repeticao para implementar o novo conceito de fields repetitivos.
 O grupo de instrucoes que deve ser inicializado e' delemitado pelas
 variaveis (instrucao_inicial, instrucao_final).
 No caso de grupo repetitivo  a proxima instrucao gerada vai incializar
 as variaveis do contexto.
*/
#if CICPP
void   FMTSTRU :: fmt_inicio_grupos_vfields(instruction_code instr_inicial,
                                            instruction_code instr_final)
#else /* CICPP */
#if ANSI
void   fmt_inicio_grupos_vfields(instruction_code instr_inicial,
                                 instruction_code instr_final)
#else /*ANSI*/
void   fmt_inicio_grupos_vfields(instr_inicial,instr_final)
instruction_code instr_inicial,instr_final;
#endif /*ANSI*/
#endif /* CICPP */
{
   elem.classe=logical;
   elem.op.l=rep_group;
   push(&elem);
   IFERR_RET;
   rep_group=false;

   elem.classe=integer;
   elem.op.i=next_rep_occ;
   push(&elem);
   IFERR_RET;
   next_rep_occ=0;

   elem.classe=logical;
   elem.op.boolean=must_repeat;
   push(&elem);
   IFERR_RET;
   must_repeat=false;
/* Continue em grupos repetitivos  17-7-94 */

   elem.classe=integer;
   elem.op.i=continue_rgroup_maxocc;
   push(&elem);
   IFERR_RET;
   continue_rgroup_maxocc=0;
#if TRACE_REPF
       printf("\n [Beg_init_rep <push>] rep_goup=%d next_rep_occ=%d",
               rep_group,next_rep_occ);
#endif
   fmt_fim=false;
   fmt_n_ref=1;
   /* Continue em grupos repetitivos 17-7-94 */
   continue_rgroup_maxocc=0;
   rpni=(l_code *)ni->next;
   while (fmt_fim==false) {
      if(infoi(rpni)==instr_inicial) fmt_n_ref++;
      if(infoi(rpni)==instr_final  ) fmt_n_ref--;
      if(fmt_n_ref==0)  fmt_fim=true;
        else {
           /* Continue em grupos repetitivos 17-7-94 */
           /* Inicializa contadores de instrucoes que referenciam fields
              para determinar o numero maximo de ocorrencias dos campos
              Notar que quando funcionar grupos repetitivos aninhados
              so interessam o do primeiro nivel. Para o microsisis
              original , esse aninhamento nao e possivel. */
           fmt_init_vinstr(rpni,fmt_n_ref);
      }
      rpni=(l_code *)rpni->next;
    }
}
/*-------------------------------------------------------------------------*/
/*          fmt_fim_grupos_vfields                                         */
/*-------------------------------------------------------------------------*/
#if CICPP
void    FMTSTRU :: fmt_fim_grupos_vfields(void)
#else /* CICPP */
void    fmt_fim_grupos_vfields()
#endif /* CICPP */
{
  /* Implementacao de continue 17-7-94 */
  auxpop = pop(); IFERR_RET;
  op1=(stack_node *)memmove(&op1_node,auxpop,sizeof(stack_node));
  is_not_of_class(op1->classe,integer);
  IFERR_RET;
  continue_rgroup_maxocc=op1->op.i ;

  auxpop = pop(); IFERR_RET;
  op1=(stack_node *)memmove(&op1_node,auxpop,sizeof(stack_node));
  is_not_of_class(op1->classe,logical);
  IFERR_RET;
  must_repeat=op1->op.boolean ;

  auxpop = pop(); IFERR_RET;
  op1=(stack_node *)memmove(&op1_node,auxpop,sizeof(stack_node));
  is_not_of_class(op1->classe,integer);
  IFERR_RET;
  next_rep_occ=op1->op.i ;

  auxpop = pop(); IFERR_RET;
  op1=(stack_node *)memmove(&op1_node,auxpop,sizeof(stack_node));
  is_not_of_class(op1->classe,logical);
  IFERR_RET;
  rep_group=op1->op.boolean ;
}

/*-------------------------------------------------------------------------*/
/*          fmt_load_all_occ                                            */
/*-------------------------------------------------------------------------*/
#if CICPP
int FMTSTRU :: fmt_load_all_occ(void)
#else /* CICPP */
int fmt_load_all_occ()
#endif /* CICPP */
{
   int occ;
   occ=0;
   *fd_value=null_char;
   itf=(field_definition_node *) iadd(ni);
#if CICPP  /*ifcmm*/
   itf->max_occ=recp->xnocc(itf->tag);
#else /*elsecmm*/
   itf->max_occ=nocc(irec,itf->tag);
#endif /*CICPP*/  /*endcmm*/
   /* inicio implementacao [x:y] */
   itf->execlower=abs(itf->lower);
   itf->execupper=abs(itf->upper);
   if (itf->lower==LASTVAL) itf->execlower=itf->max_occ;
   if (itf->upper==LASTVAL) itf->execupper=itf->max_occ;
   /* fim  implementacao [x:y] */
   /* we must verify in which occurrences the subfield appears */
   while (itf->max_occ > itf->actual_occ ) {
        if ( ( itf->actual_occ+1 < itf->execlower)  ||
             ( itf->actual_occ+1> itf->execupper) ) {
             itf->actual_occ ++;
             continue ;
        }
      *fd_tmp=null_char;
      itf->actual_occ++;
#if CICPP  /*ifcmm*/
      pos=recp->xfieldx(itf->tag,itf->actual_occ );
#else /*elsecmm*/
      pos=fieldx(irec,itf->tag,itf->actual_occ );
#endif /*CICPP*/  /*endcmm*/
      field_value(fd_tmp,max_fd_tmp,pos);
#if DEB_NOCC
      printf("\n fd_tmp=%s",fd_tmp);
#endif
      sub_field_string(fd_tmp,itf->sub_field);
#if !FATRAP
      if (fmterror) return -1;
#endif
#if DEB_NOCC
      printf("\n fd_tmp=%s",fd_tmp);
#endif
      if (itf->offset != -1 && itf->length != -1)
         fd_tmp=sub_string(fd_tmp,itf->offset,itf->length);
#if DEB_NOCC
      printf("\n fd_tmp=%s",fd_tmp);
#endif
       if(strlen(fd_tmp)!=0) {
         occ++;
         if(itf->first_occ==0) itf->first_occ=itf->actual_occ;
         itf->last_occ=itf->actual_occ;
       }
       strcat(fd_value,fd_tmp);
#if DEB_NOCC
      printf("\n fd_value=%s occ=%d",fd_value,occ);
#endif
#if DEB_FLOAT
       trace_field(itf,fd_tmp);
       trace_field(itf,fd_value);
#endif
   }
   itf->actual_occ=0;

   if (fmttrace) trace_field(itf,fd_tmp);        /* AOT 26/12/91 */
   return occ;
}
/*-------------------------------------------------------------------------*/
/*          fmt_load_next_occ                                             */
/*-------------------------------------------------------------------------*/
#if CICPP
void FMTSTRU :: fmt_load_next_occ(void)
#else /* CICPP */
void fmt_load_next_occ()
#endif /* CICPP */
{ /*[:] Para grupo repetitivo pega somente e sempre o limite
        inferior da ocorrencia
  */
  int occx;
  *fd_value=null_char;
  occx=itf->actual_occ;
  if (rep_group==true && !NO_INTERVAL(itf) ) occx=itf->execlower;
  if ( (occx < itf->execlower) || (occx > itf->execupper) ) return;
  /*[:]*/
#if CICPP  /*ifcmm*/
  pos=recp->xfieldx(itf->tag,occx);
#else /*elsecmm*/
  pos=fieldx(irec,itf->tag,occx);
#endif /*CICPP*/  /*endcmm*/
  field_value(fd_value,max_fd_value,pos);
  /* subfields */
  cc=itf->sub_field;
  if (cc!=subfield_null){
     sub_field_string(fd_value,cc);
     IFERR_RET;
  }
  /*substring*/
#if BEFORE_20120618 /* fix wrong output value in 64 bits version */ 
  if(itf->offset!= -1 && itf->length!= -1)
          strcpy(fd_value,sub_string(fd_value,itf->offset,itf->length));
#else
  if(itf->offset!= -1 && itf->length!= -1) {
     memmove(fd_value, sub_string(fd_value,itf->offset,itf->length), itf->length+1);	  
  }
#endif  
}
/*-------------------------------------------------------------------------*/
/*          fmt_comum_ref_beg                                              */
/*-------------------------------------------------------------------------*/
#if CICPP  /*ifcmm*/
void FMTSTRU :: fmt_comum_ref_beg(void)
{
     try { recp=new RECSTRU(cisisxp); }
     catch (BAD_ALLOC) { fatal("fmt/inter/refrec/ALLOC"); }
     recp->xrecalloc(MAXMFRL); /* p/ manter check: pode nao usar */
     /* recp is global */
}
#else /*elsecmm*/
void fmt_comum_ref_beg()
{
     if (currprat >= MAXREFS) fatal("fmt/inter/refrec/MAXREFS");
     if (!nrecs) fatal("fmt/inter/refrec/recinit");
     for (irec=maxnrec; irec--; )
       if (!vrecp[irec]) /* ja' decrementado */ break;
     if (irec < 0) fatal("fmt/inter/refrec/next");
     currprat++;
     vprats[currprat]=irec;
     recallok(irec,MAXMFRL); /* p/ manter check: pode nao usar */
     recp=vrecp[irec]; /* mandatory for defines REC RDB MF0 MFR DIR, FIELDP */
}
#endif /*CICPP*/  /*endcmm*/
/*-------------------------------------------------------------------------*/
/*          fmt_comum_ref_end                                              */
/*-------------------------------------------------------------------------*/
#if CICPP  /*ifcmm*/
void FMTSTRU :: fmt_comum_ref_end(void)
{
      auxpop = pop(); IFERR_RET;
      top=(stack_node *)memmove(&top_node,auxpop,sizeof(stack_node));
      is_not_of_class(top->classe,type_recstru);
      IFERR_RET;
#if BEFORE990414
      // nao liberava nada! - AOT/HB
#else
      if (ref_mfn_number>0){
          delete recp;
      }
#endif
      recp=top->op.lrecp;
}
#else /*elsecmm*/
void fmt_comum_ref_end()
{
 LONGX trec;
      auxpop = pop(); IFERR_RET;
      top=(stack_node *)memmove(&top_node,auxpop,sizeof(stack_node));
      is_not_of_class(top->classe,long_n);
      IFERR_RET;
      trec=top->op.l;
      /* A alocacao so foi  feita se foi gerado mfn valido(>0) pelo ref  */
      if (ref_mfn_number>0){
         /* recp=vrecp[trec];*/   /* e' necessario? */     /* YES */
         if (currprat < 0 || !vrecp[vprats[currprat]]) fatal("fmt/inter/refrec/free");
         irec=vprats[currprat];
         FREE(vrecp[irec]); vrecp[irec]=NULL; nrecs--;
         currprat--;   /*Posiciona na  prateleira anterior */

       /*  irec=vprats[currprat];*/
      }
      irec=trec;
      recp=vrecp[irec];/* mandatory for defines REC RDB MF0 MFR DIR, FIELDP */
}
#endif /* CICPP */  /*endcmm*/

/*----------- inter --------------------------------------*/
#if CICPP
LONGX FMTSTRU :: xfmt_inter(RECSTRU *parmrecp,
                           LONGX par_lw,
                           char *par_out,
                           LONGX outsize)
#else /* CICPP */
LONGX fmt_inter(pgm,parmirec,par_lw,par_out,outsize)
l_code *pgm;
LONGX parmirec;
LONGX par_lw;
char *par_out;
LONGX outsize;
#endif /* CICPP */
{
 char v_mfn_str[20],*mfn_str;
/*A06*/
/* char literal[MAX_LITER+1]; virou dinamico e global*/
 char *new_dbname;
 instruction_code actual_mode;
 int fim_pgm;
 int i;
/* para colocar string na saida */
 char *pre_literal,*suf_literal,nothing;
 char *s;
 int id1;
 int id2;
 int final_added;
 LONGX qty;
 instruction_code next_ins;
 l_code *next_ni;
#if DINALLOC
 int xdir;
 LONGX xfsiz;
#endif

    if (!ndbxs) dbxinit();  /* init vdbxp/vrecp/vtrmp if not yet init - AOT, 28/10/2005 */

#if CICPP
    l_code *pgm = fmt_pgmp;
#endif /* CICPP */
/*--------------------------------------------------------------------------*/
/*                   NewLine-Valor default                                  */
/*--------------------------------------------------------------------------*/
/* A07*/

#if UNIX | WWWISIS
nl_LEN=1;
nl_STR=fmt_alloc_char((ALLOPARM)(nl_LEN+1),"cifm3/alloc/nl_STR");
nl_STR[0]=lf; nl_STR[1]=null_char;
#else
nl_LEN=2;
nl_STR=fmt_alloc_char((ALLOPARM)(nl_LEN+1),"cifm3/alloc/nl_STR");
nl_STR[0]=cr; nl_STR[1]=lf; nl_STR[2]=null_char;
#endif
    if (fmttrace) printf("+++ fmt_inter - begin \n");   /* AOT 27/12/91 */
    strcpy(fmterrxy,"");
#if FATRAP
    fmterror = setjmp(fmtjumper);
#else /* FATRAP */
    fmterror = 0;
ERROR_LABEL:
#endif /* FATRAP */
    if (fmterror != 0) {
        return((fmterror < 0) ? (LONGX)fmterror : (LONGX)(-fmterror));
    }

    if (par_lw <= 0)       par_lw=MAXMFRL-1;  /* AOT, 07/06/2005 */
    if (par_lw >= outsize) par_lw=outsize-1;  /* AOT, 07/06/2005 */

    /* set current master file record */
#if CICPP  /*ifcmm*/
    recp=parmrecp;
#else /*elsecmm*/
    irec=parmirec;
    recp=vrecp[irec];
    vprats[currprat]=irec;
#endif /*CICPP*/  /*endcmm*/

    firstdbnp=RDBname;

/* Alocacao das maiores areas originalmente estaticas - AOT 29/12/90 */
#if DINALLOC
    max_tmp_str=outsize; /* parm asize */
#if !RETIRAR
#if CICPP
    try
    { tmp_str= (char *)new char [(max_tmp_str+1)]; }
    catch (BAD_ALLOC)
    { tmp_str= (char *) NULL; }
#else /* CICPP */
    tmp_str=(char *)ALLOC((ALLOPARM)(max_tmp_str+1));
#endif /* CICPP */
#else /*RETIRAR */
    tmp_str=fmt_alloc_char((ALLOPARM)(max_tmp_str+1),"1001");
#endif /*RETIRAR */
    if (tmp_str == NULL)
    {
#if !RETIRAR
        inter_error("1001");
#endif
        IFERR_GOTO;
    }
/*A06*/
    literal_len =(ALLOPARM)MAX_LITER;
    literal=fmt_alloc_char((ALLOPARM)(literal_len+1),"cifm3/alloc/literal");
    if (literal == NULL) IFERR_GOTO;

    if (fmt_fsiz) {
        max_fd_value=fmt_fsiz;
        max_fd_tmp=fmt_fsiz;
    }
    else {
        for (xdir=MFRnvf, din_fsiz=0; xdir--; )
            if (DIRlen(xdir) > din_fsiz) din_fsiz=DIRlen(xdir);
        if (fmttrace) printf("+++ fmt_inter - din_fsiz=%"_LD_"\n",din_fsiz);
        if (din_fsiz < par_lw) din_fsiz=par_lw; /* bug lw=999 */
        if (fmttrace) printf("+++ fmt_inter - din_fsiz=%"_LD_"\n",din_fsiz);
        max_fd_value=din_fsiz;
        max_fd_tmp=din_fsiz;
    }

#if CICPP
    try
    { v_fd_value= (char *) new char [(max_fd_value+1)]; }
    catch (BAD_ALLOC)
    { v_fd_value= NULL; }
#else /* CICPP */
    v_fd_value= (char *)ALLOC((ALLOPARM)(max_fd_value+1));
#endif /* CICPP */
    if (v_fd_value==(char *)NULL)
    {
        inter_error("1003");
        IFERR_GOTO;
    }
#if CICPP
    try
    { v_fd_tmp= (char *) new char [(max_fd_tmp+1)]; }
    catch (BAD_ALLOC)
    { v_fd_tmp= (char *) NULL;}
#else /* CICPP */
    v_fd_tmp=   (char *)ALLOC((ALLOPARM)(max_fd_tmp+1));
#endif /* CICPP */
    if (v_fd_tmp==   (char *)NULL)
    {
        inter_error("1004");
        IFERR_GOTO;
    }
#endif

/* Sindo's code */
 continue_rgroup_maxocc=0;
 continue_rgroup=false;

/* Inicializacao do stack[] - AOT 27/11/91 */
stack_pt=0;     /* push() e pop() usam a partir de 1 - comunicar */

op1= &op1_node;
op2= &op2_node;
top= &top_node;
out= &par_out[0];
lw_inter=par_lw;

/* incializacoes */
fd_value=v_fd_value;    /* DINALLOC */
mfn_str=v_mfn_str;
must_repeat=false;
rep_group=false;
next_rep_occ=0;
pstr_num=str_num;

nextcc=1;
tmp_str_index=0;
nothing=null_char;
pre_literal= &nothing;
suf_literal= &nothing;
erro_fatal=0;

out[0]=null_char;
pout= 0;
address_out=out;

/* defaults */

actual_mode=mpl_par;
final_added=0;

fim_pgm= ~false;
if (pgm)
if (pgm->next) {
   /* inicializa A tabela de definicao de fields */
   ni=(l_code *)pgm->next; /* next instruction */
   for (;next_infoi(ni)!=(int)nulo;){
      fmt_init_vinstr(ni,0);
      ni=(l_code *)ni->next;
   }
   fim_pgm=false;
   ni=(l_code *)pgm->next; /* next intruction */
}

/* 22/10/99 Inicializa  as variaveis E e S */
fmt_init_E_S();


while (fim_pgm==false){
  fd_tmp=v_fd_tmp;

#if TRACE_INTER_X
  if (fmttrace)
  printf(" #%d[%d,%s,%d]\n",0,ni->info.lab,
     inst_cnv[ni->info.instr], ni->info.add);
#endif /* TRACE_INTER_X */
#if DEBUG_REF
    printf("\n #%d[%d,%s,%d]\n",0,ni->info.lab,
     inst_cnv[ni->info.instr], ni->info.add);

 printf("\n---- ESTADO         da pilha ----- termos %d \n",ntrms);
 for (iii=stack_pt;iii>0;iii--) {
    printf("\n  %d  %d   %d ",iii,stack[iii].classe, stack[iii].op);
    if (stack[iii].classe==string) printf(" string");
    if (stack[iii].classe==long_n) printf(" long_n %"_LD_" ",stack[iii].op.l);
    if (stack[iii].classe==integer) printf(" integer");
    if (stack[iii].classe==float_n) printf(" float_n  ");
    if (stack[iii].classe==logical) printf(" logical %d ",stack[iii].op.i);
    if (stack[iii].classe==ptr) printf(" ptr %"_LD_" (%p)",
                                 stack[iii].op.address,stack[iii].op.address);
    if (stack[iii].classe==l_w) printf(" l_w =%"_LD_" ",stack[iii].op.l);
    if (stack[iii].classe==xindex) printf(" index %d ",stack[iii].op.i);
    if (stack[iii].classe==next_cc) printf(" next_cc %"_LD_,stack[iii].op.l);

 }
#if 0
 printf("\n ---- Alocacoes  invertido --- ntrms=%d",
        ntrms);
 for (iix=maxntrm; iix>=trm_prat; iix--) {
    printf ("\n vtrmp[%"_LD_"]= %"_LD_" (%p )", iix,vtrmp[iix],vtrmp[iix]);
 }
#endif
#endif
/*loop*/
 switch(infoi(ni))
  {
  case  test_occ:
    {
      itf=(field_definition_node *) iadd(ni);
      there_is_fd_value=false;
      *fd_value=null_char;                                      /* 08/05/92 */
      *fd_tmp=null_char;                                        /* 08/05/92 */
      if (itf->max_occ == -1)
      { fmt_load_all_occ(); IFERR_GOTO; }

      if(rep_group==true)itf->actual_occ=next_rep_occ;
                    else itf->actual_occ++;
      /* Tentativa de Implementar v70[1] dentro de repetitivo */
      /* 30-12-94 */
      if (rep_group==true   &&  !NO_INTERVAL(itf)) {
        /* Intervalo dentro de repetitivo so pega  o lower
           O comando abaixo forca o valor de itf->actual_occ
           somente com o objetivo de entrar no proximo teste,
           pois de qualquer forma o numero da ocorrencia sera
           aquele especificado no lower.
           Esta implementacao e muito suja. No caso de um
           formato do tipo ( v70[2],c20,v26[3]) se nocc(70)=5
           serao geradas 5 linhas iguais !!!!
        */
        itf->actual_occ=itf->max_occ;
       }

      if (itf->max_occ >= itf->actual_occ){
         there_is_fd_value=false;
         elem.classe=logical;
         elem.op.boolean=true;
         fmt_load_next_occ();
         IFERR_GOTO;
         if(strlen(fd_value)!=0)there_is_fd_value=true;

         if (fmttrace) trace_field(itf,fd_value);/* AOT 26/12/91 */
      }
      else {
        *fd_value=null_char;
        elem.classe=logical;
        elem.op.boolean=false;
        if (fmttrace) trace_field(itf,"");      /* AOT 26/12/91 */
      }

    push(&elem);
    IFERR_GOTO;
    ni=(l_code *) next_infoi(ni); /* proxima instrucao */
    break;
   }
 case suf_cond :
 case suf_cond_null :
 case suf_r_lit :
 case suf_r_lit_plus :
 {
   ni=(l_code *) next_infoi(ni);
   break;
  }

 case ign_cond :
 {
   there_is_fd_value=false;
   ni=(l_code *) next_infoi(ni);
   break;
  }

 case pre_cond :
  {
   if (there_is_fd_value==true)
   {
   if(itf->actual_occ==itf->first_occ)
     {
      literal[0]=null_char;
#if !RETIRAR
      strcpy(literal, (char *) iadd(ni) );
#else
     literal=fmt_copy_realloc((char *)literal, &literal_len,
                                               (char *)iadd(ni));
#endif
       if(is_upper_mode(actual_mode)) upcase_mode(literal);
       id1=0;
       id2=0;
      out_put_str(out,outsize, &pout,lw_inter,id1,id2, &nextcc,literal);
     }  /* actual_occ=... */
     }

   ni=(l_code *) next_infoi(ni);
   break;
  }

 case pre_r_lit_plus :
  {
   pre_literal= &nothing;

   if(itf->actual_occ!=itf->first_occ && there_is_fd_value==true)
     {
      pre_literal=(char *) iadd(ni);

     }  /* actual_occ=... */
   ni=(l_code *) next_infoi(ni);
   break;
  }

 case pre_r_lit :
  {

      if(there_is_fd_value==true) pre_literal=(char *) iadd(ni);
   ni=(l_code *) next_infoi(ni);
   break;
  }


 case u_cond :
  {   /*A06*/

/*       literal[0]=null_char;
         strcpy(literal,(char *) iadd(ni) );
*/
      literal=fmt_copy_realloc((char *)literal, &literal_len,
                                               (char *)iadd(ni));
      if(is_upper_mode(actual_mode)) upcase_mode(literal);
      id1=0;
      id2=0;
      out_put_str(out,outsize, &pout,lw_inter,id1,id2, &nextcc,literal);
      ni=(l_code *) next_infoi(ni);
      break;
  }

 case escape_seq :
  {
      literal[0]=(char)27; /*escape sequence */
      literal[1]=null_char;
 /*A06*/
       id1=0;
       id2=0;
      out_put_str(out,outsize, &pout,lw_inter,id1,id2, &nextcc,literal);

/* A06      strcat(literal, (char *) iadd(ni) );
*/
      literal=fmt_copy_realloc((char *)literal, &literal_len,
                                               (char *)iadd(ni));
      out_put_str(out,outsize, &pout,lw_inter,id1,id2, &nextcc,literal);
      ni=(l_code *) next_infoi(ni);
      break;

  }
   case fmt_beg:
     {
     save_context(modify_fmt_pointers);
     IFERR_GOTO;
     ni=(l_code *) next_infoi(ni);
     break;
     }

   case fmt_end:
   {


   ni=(l_code *) next_infoi(ni);

   break;
  }
 case print_mfn :
  {
/*   strcpy(mfn_str,long_to_char(MFRmfn,(int )iadd(ni)) );*/
   float_exp_value= (float_x ) MFRmfn;
   strcpy((char *)mfn_str,
      fmt_float_numb(float_exp_value,(int )iadd(ni) ,(int) 0,'0'));
   id1=0;
   id2=0;
   out_put_str(out,outsize, &pout,lw_inter,id1,id2, &nextcc,mfn_str);
   ni=(l_code *) next_infoi(ni);
   break;
  }

 case print_maxmfn :
  {
   float_exp_value= (float_x ) 0;
   if (RECdbxp) float_exp_value= (float_x ) RDBmsmfn; /* il fault faire.. */
   strcpy((char *)mfn_str,
      fmt_float_numb(float_exp_value,(int )iadd(ni) ,(int) 0,'0'));
   id1=0;
   id2=0;
   out_put_str(out,outsize, &pout,lw_inter,id1,id2, &nextcc,mfn_str);
   ni=(l_code *) next_infoi(ni);
   break;
  }

 case print_core :
  {
#if CICPP
   float_exp_value= (float_x ) 0;
#else /* CICPP */
   float_exp_value= (float_x ) CORELEFT();
#endif /* CICPP */
   strcpy((char *)mfn_str,
   fmt_float_numb(float_exp_value,(int )iadd(ni) ,(int) 0,'0'));
   id1=0;
   id2=0;
   out_put_str(out,outsize, &pout,lw_inter,id1,id2, &nextcc,mfn_str);
   ni=(l_code *) next_infoi(ni);
   break;
  }

  case print_field:
  {
     char *tofree;

     if(itf->max_occ>itf->actual_occ) must_repeat=true;
     if (there_is_fd_value==true)
    {
        *fd_tmp=null_char;
        strcpy(fd_tmp,pre_literal);
        strcat(fd_tmp,fd_value);
/*      exec_suf_literal; */
        next_ni=(l_code *)next_infoi(ni); /* precisa ver 1 passo a frente*/
        next_ins=infoi(next_ni);
        there_is_suf=false;

        if (next_ins==suf_r_lit )
         {
           ni=(l_code *) next_infoi(ni);
           suf_literal=(char *) iadd(ni);
           there_is_suf=true;
         }

        next_ni=(l_code *)next_infoi(ni); /* precisa ver 1 passo a frente*/
        next_ins=infoi(next_ni);

         if (next_ins==suf_r_lit_plus)
       { ni=(l_code *) next_infoi(ni);
                /* ignores the last  occurrence */
                if(itf->actual_occ!=itf->last_occ)
                 {
                  suf_literal= (char *) iadd(ni);
                 there_is_suf=true;
             }
       }
      strcat(fd_tmp,suf_literal);
      literal[0]=null_char;
          next_ni=(l_code *)next_infoi(ni); /* precisa ver 1 passo a frente*/
          next_ins=infoi(next_ni);

      if (next_ins==suf_cond_null || next_ins==suf_cond)
          { ni=(l_code *) next_infoi(ni);
                if(itf->actual_occ==itf->last_occ )
                  {
/*A06                strcpy(literal, (char *) iadd(ni) );
*/
                     literal=fmt_copy_realloc((char *)literal, &literal_len,
                                                          (char *)iadd(ni));
                     there_is_suf=true;
                   }  /* else ignores command */
          }  /* suf_cond */
          strcat(fd_tmp,literal);
          s=trata_md(actual_mode,fd_tmp,&final_added,&tofree);
          IFERR_GOTO;
          if(is_upper_mode(actual_mode)) upcase_mode(s);
          id1=itf->indent1;
          id2=itf->indent2;
          out_put_str(out,outsize, &pout,lw_inter,id1,id2, &nextcc,s);
          if (tofree) {

#if CICPP
                delete [] (char *)tofree;
#else /* CICPP */
                FREE(tofree);
#endif /* CICPP */

          }
          if(there_is_suf==true)
          {
           delete_chars(out,pout,nextcc,final_added,lw_inter);
          }

    }
        pre_literal= &nothing;
        suf_literal= &nothing;
        ni=(l_code *) next_infoi(ni);
        break;

   }

   case load_field_all:
  {
        fmt_load_all_occ();
        IFERR_GOTO;
#if TRACE_INTER_2
        printf("\n Apos fmt_load_all_occ/1 fd_value=%s",fd_value);
#endif
        /* copia para temporaria */

        elem.op.s=store_tmp_string(fd_value);
        IFERR_GOTO;
        elem.classe=string;
        push(&elem);
        IFERR_GOTO;
        ni=(l_code *) next_infoi(ni);
        break;
   }
   case load_field_occ:
  {
     itf=(field_definition_node  *)iadd(ni);
      if( itf->max_occ == -1)
        {
#if CICPP  /*ifcmm*/
         itf->max_occ=recp->xnocc(itf->tag);
#else /*elsecmm*/
         itf->max_occ=nocc(irec,itf->tag);
#endif /*CICPP*/  /*endcmm*/
         itf->execlower=abs(itf->lower);
         itf->execupper=abs(itf->upper);
         if (itf->lower==LASTVAL) itf->execlower=itf->max_occ;
         if (itf->upper==LASTVAL) itf->execupper=itf->max_occ;
         /* fim  implementacao [x:y] */

         itf->actual_occ=0;
         }
       *fd_value=null_char;
       if (rep_group==true)itf->actual_occ=next_rep_occ;
                      else itf->actual_occ++;

       if (itf->max_occ > itf->actual_occ) must_repeat=true; /* SVD 11/11/91 */

       if (itf->max_occ >= itf->actual_occ || !NO_INTERVAL(itf)){
          *fd_value=null_char;
           fmt_load_next_occ();
           IFERR_GOTO;
       }

       elem.op.s=store_tmp_string(fd_value);
       IFERR_GOTO;
       elem.classe=string;
       push(&elem);
       IFERR_GOTO;
       ni=(l_code *) next_infoi(ni);
       break;
   }

   case load_string:
  {
        elem.op.s=(char *) iadd(ni);
        elem.classe=string;
        push(&elem);
        IFERR_GOTO;
        ni=(l_code *) next_infoi(ni);
        break;
   }
   case load_mstnam:
  {
      tmp_ptr=store_tmp_string(RDBname);
      IFERR_GOTO;
      if(is_upper_mode(actual_mode)) upcase_mode(tmp_ptr);

      elem.op.s=tmp_ptr;
      elem.classe=string;
      push(&elem);
      IFERR_GOTO;
      ni=(l_code *) next_infoi(ni);
      break;
   }
   case load_date:
  {
      char *tmp_ptr;
      int parmdate;
      parmdate=(int)iadd(ni);

      tmp_ptr=fmt_get_date(parmdate,-1L);
      if(is_upper_mode(actual_mode)) upcase_mode(tmp_ptr);

      elem.op.s=store_tmp_string(tmp_ptr);
      IFERR_GOTO;
      elem.classe=string;
      push(&elem);
      IFERR_GOTO;
      ni=(l_code *) next_infoi(ni);
      break;
   }
  case load_number:
  {                        /* Only implemented for LONGX numbers */
        elem.op.l=(LONGX)iadd(ni);
        elem.classe=long_n;
        push(&elem);
        IFERR_GOTO;
        ni=(l_code *) next_infoi(ni);
        break;
   }
  case intvvalue:
  {                        /* Only implemented for LONGX numbers */
        elem.op.r=E_var[(LONGX)iadd(ni)];
        elem.classe=float_n;
        push(&elem);
        IFERR_GOTO;
        ni=(l_code *) next_infoi(ni);
        break;
   }
   case load_float:
  {
        elem.op.r= *(float_x *)(iadd(ni)) ;
#if DEB_FLOAT
        printf("\n Empilhando float=%f",elem.op.r);
#endif
        elem.classe=float_n;
        push(&elem);
        IFERR_GOTO;
        ni=(l_code *) next_infoi(ni);
        break;
   }
   case load_mfn:
  {                        /* tamanho de caracteres sao ignorados  */
        elem.op.l=(LONGX)MFRmfn;
        elem.classe=long_n;
        push(&elem);
        IFERR_GOTO;
        ni=(l_code *) next_infoi(ni);
        break;
   }

   case load_maxmfn:
  {                        /* tamanho de caracteres sao ignorados  */
        elem.op.l= (float_x ) 0;
        if (RECdbxp) elem.op.l= (float_x ) RDBmsmfn; /* il fault faire.. */
        elem.classe=long_n;
        push(&elem);
        IFERR_GOTO;
        ni=(l_code *) next_infoi(ni);
        break;
   }
   case load_core:
  {                        /* tamanho de caracteres sao ignorados  */
#if CICPP
        elem.op.l= (float_x ) 0;
#else /* CICPP */
        elem.op.l= (float_x ) CORELEFT();
#endif /* CICPP */
        elem.classe=long_n;
        push(&elem);
        IFERR_GOTO;
        ni=(l_code *) next_infoi(ni);
        break;
   }

   case noccins:
  {
        elem.op.l=fmt_load_all_occ();
        IFERR_GOTO;
        elem.classe=long_n;
        push(&elem);
        IFERR_GOTO;
#if DEB_NOCC
        printf("\n Numero de ocorrencias=%d\n",elem.op.l);
#endif
        ni=(l_code *) next_infoi(ni);
        break;
   }

   case ioccins:
  {
        elem.op.l=(LONGX )next_rep_occ;
        elem.classe=long_n;
        push(&elem);
        IFERR_GOTO;
#if DEB_NOCC
        printf("\n Numero de ocorrencias=%d\n",elem.op.l);
#endif
        ni=(l_code *) next_infoi(ni);
        break;
   }

   case present :
   case absent :
  {
#if ONE_MORE_OCCS
       if (itf->max_occ>itf->actual_occ) must_repeat=true; /* SVD 21/11/91 */
#endif
        if (rep_group==true)
    {
        /* copia para temporaria */
     itf=(field_definition_node *)iadd(ni);
      if( itf->max_occ == -1)
        {
          fmt_load_all_occ(); /* para inicializar intervalos 27-12-94 */
          IFERR_GOTO;
          itf->actual_occ=0;
         }
       *fd_value=null_char;
       itf->actual_occ=next_rep_occ;
#if !ONE_MORE_OCCS
       if (itf->max_occ>itf->actual_occ) must_repeat=true; /* SVD 21/11/91 */
#endif
       if (itf->max_occ >= itf->actual_occ) {fmt_load_next_occ();IFERR_GOTO;}
     }
     else {
        fmt_load_all_occ();
        IFERR_GOTO;
#if TRACE_INTER_2
        printf("\n Apos fmt_load_all_occ/2 fd_value=%s",fd_value);
#endif
     }
     elem.classe=logical;
     if (infoi(ni)==absent)  elem.op.boolean= *fd_value==null_char?true:false;
     if (infoi(ni)==present) elem.op.boolean= *fd_value==null_char?false:true;
     push(&elem);
     IFERR_GOTO;
     ni=(l_code *) next_infoi(ni);
     break;
   }

 case duptop: /*duplica a expressao do select para testar com cases */
  {
   auxpop = pop(); IFERR_GOTO;
   push(auxpop);   IFERR_GOTO;
   push(auxpop);   IFERR_GOTO;
   ni=(l_code *) next_infoi(ni);
   break;
  }
 case end_select: /* retira a expressao do select do topo */
 {
  auxpop =pop(); IFERR_GOTO;
   ni=(l_code *) next_infoi(ni);

  break;
 }
 case jumpf:
  {
   auxpop = pop(); IFERR_GOTO;
   top=(stack_node *)memmove(&top_node,auxpop,sizeof(stack_node));
   is_not_of_class(top->classe,logical);
   IFERR_GOTO;
   if(top->op.boolean==false) ni=(l_code *) iadd(ni);
                       else ni=(l_code *) next_infoi(ni);
   break;

  }

 case jumpt:
  {
   auxpop = pop(); IFERR_GOTO;
   top=(stack_node *)memmove(&top_node,auxpop,sizeof(stack_node));
   is_not_of_class(top->classe,logical);
   IFERR_GOTO;

   if(top->op.boolean==true) ni=(l_code *) iadd(ni);
                    else ni=(l_code *) next_infoi(ni);

   break;
  }

 case jump:
 case breakins:    /* pulo incondicional */
  {
    ni=(l_code *) iadd(ni);
   break;
  }
 case or_op :
 case and_op:
  {
   retrieve_two_logical_operands;
   IFERR_GOTO;
   if(infoi(ni)==and_op)elem.op.boolean=op1->op.boolean && op2->op.boolean;
   if(infoi(ni)==or_op )elem.op.boolean=op1->op.boolean || op2->op.boolean;
   push(&elem);
   IFERR_GOTO;
   ni=(l_code *) next_infoi(ni);
   break;
  }
 case not_op:
  {
   auxpop = pop(); IFERR_GOTO;
   op1=(stack_node *)memmove(&op1_node,auxpop,sizeof(stack_node));
   is_not_of_class(op1->classe,logical);
   IFERR_GOTO;
   elem.classe=logical;
   elem.op.boolean=!op1->op.boolean ;
   push(&elem);
   IFERR_GOTO;
   ni=(l_code *) next_infoi(ni);
   break;
  }
 case neq_op :
 case gtr_op :
 case lss_op :
 case leq_op :
 case geq_op :
 case eql_op :
  {
   instruction=infoi(ni);
   retrieve_two_operands ();
   IFERR_GOTO;
   switch (class_of_the_both)
   {

   case string :
    {
    result=strcmp(op1->op.s,op2->op.s);
    switch (instruction) {
    case neq_op: cond_code= (result != 0)?true:false; break;
    case gtr_op: cond_code= (result  > 0)?true:false; break;
    case lss_op: cond_code= (result  < 0)?true:false; break;
    case leq_op: cond_code= (result <= 0)?true:false; break;
    case geq_op: cond_code= (result >= 0)?true:false; break;
    case eql_op: cond_code= (result == 0)?true:false; break;
    }
    break;
    }/* string */
   case long_n  :
    {
    switch (instruction) {
    case neq_op: cond_code= (long_value_1 != long_value_2 )?true:false; break;
    case gtr_op: cond_code= (long_value_1 >  long_value_2 )?true:false; break;
    case lss_op: cond_code= (long_value_1 <  long_value_2 )?true:false; break;
    case leq_op: cond_code= (long_value_1 <= long_value_2 )?true:false; break;
    case geq_op: cond_code= (long_value_1 >= long_value_2 )?true:false; break;
    case eql_op: cond_code= (long_value_1 == long_value_2 )?true:false; break;
    }
    break;
    }/*integer */
   case float_n   :
    {
#if DEB_FLOAT
    printf("\n Vai comparar (%f) com (%f)",float_value_1,float_value_2);
#endif
    switch (instruction) {
    case neq_op: cond_code=(float_value_1 != float_value_2)?true:false; break;
    case gtr_op: cond_code=(float_value_1 >  float_value_2)?true:false; break;
    case lss_op: cond_code=(float_value_1 <  float_value_2)?true:false; break;
    case leq_op: cond_code=(float_value_1 <= float_value_2)?true:false; break;
    case geq_op: cond_code=(float_value_1 >= float_value_2)?true:false; break;
    case eql_op: cond_code=(float_value_1 == float_value_2)?true:false; break;
    }
    break;
    }/*integer */

   case integer :

    {
    switch (instruction) {
    case neq_op: cond_code= (int_value_1 != int_value_2 )?true:false; break;
    case gtr_op: cond_code= (int_value_1 >  int_value_2 )?true:false; break;
    case lss_op: cond_code= (int_value_1 <  int_value_2 )?true:false; break;
    case leq_op: cond_code= (int_value_1 <= int_value_2 )?true:false; break;
    case geq_op: cond_code= (int_value_1 >= int_value_2 )?true:false; break;
    case eql_op: cond_code= (int_value_1 == int_value_2 )?true:false; break;
    }
    break;
    }/*integer */

   default:
   {
   inter_error("Operations for these classes of operands not implemented");
   IFERR_GOTO;
   }
   }  /* switch */
   elem.classe=logical;
   elem.op.boolean=cond_code;
   push(&elem);
   IFERR_GOTO;
   ni=(l_code *) next_infoi(ni);
   break;

  }
 case plus_op :
 case minus_op :
 case divide_op :
 case times_op :
  {
   instruction=infoi(ni);
   retrieve_two_numeric_operands();
   IFERR_GOTO;


   switch(new_class)
   {
   case integer:
      {
      if (instruction==plus_op)  tempint=int_value_1 + int_value_2;
      if (instruction==minus_op) tempint=int_value_1 - int_value_2;
      if (instruction==times_op) tempint=int_value_1 * int_value_2;
      if (instruction==divide_op)tempint=int_value_1 / int_value_2;
      elem.classe=new_class;
      elem.op.i=tempint;
      break;
      }
   case long_n:
      {
      if (instruction==plus_op)  templong=long_value_1 + long_value_2;
      if (instruction==minus_op) templong=long_value_1 - long_value_2;
      if (instruction==times_op) templong=long_value_1 * long_value_2;
      if (instruction==divide_op)templong=long_value_1 / long_value_2;
      elem.classe=long_n;
      elem.op.l=templong;
      break;
      }

   case float_n:
      {
      if (instruction==plus_op)  tempfloat=float_value_1 + float_value_2;
      if (instruction==minus_op) tempfloat=float_value_1 - float_value_2;
      if (instruction==times_op) tempfloat=float_value_1 * float_value_2;
      if (instruction==divide_op)tempfloat=float_value_1 / float_value_2;

      elem.classe=float_n;
      elem.op.r=tempfloat;
      break;
      }

   default:
   {
   inter_error("Operations for these classes of operands not implemented");
   IFERR_GOTO;
   break;
   }
  }  /* switch */
   push(&elem);
   IFERR_GOTO;
   ni=(l_code *) next_infoi(ni);
   break;
 }

case x_spac:
 {
  qty=iadd(ni);
  if( (lw_inter-nextcc)>qty )  /* is room */
   {
    /* repeat_space(out,pout,qty) */
    memset(out+pout,' ',qty); pout+=qty;
    nextcc=nextcc+qty;
    out[pout]=null_char;
   }
   else
    { /* there's no room */
     /*A07  new_line(out,pout); */
     new_line(out,pout,outsize);
     nextcc=1;
    }



  ni=(l_code *) next_infoi(ni);
  break;
 }

case c_spac:
 {
  qty=iadd(ni);
  if(qty<=lw_inter)
  {

    if( nextcc > qty  )
      {
        /*A07 new_line(out,pout); */
        new_line(out,pout,outsize);
        nextcc=1;
      }
    while (nextcc<qty )
      { out[pout++]=' ';
        out[pout]=null_char;
        nextcc++;
       } /*while*/
  }  /* number spaces > lw_inter - ignores the command */
    ni=(l_code *) next_infoi(ni);
    break;
 }

case percent_spac:
 {
#if MICRO_ISIS_COMPATIBLE
   int nn;
    nn=exec_percent(out,&pout,&nextcc);
#endif
    ni=(l_code *) next_infoi(ni);
#if MICRO_ISIS_COMPATIBLE
    /* se ja existe mudanca de linha e prox instrucao #, ignora-a */
    if ( (nn>0) && (infoi(ni)==n_sign_spac) )
             ni=(l_code *) next_infoi(ni);
#endif
    break;
 }

case n_sigx_spac:
/* see case x_spac: */
 {
  qty=iadd(ni);
  while (qty > 0) {
         char *p,*q;
         int mlen,hlen,qlen;
         mlen=RECnbytes-(MFRmfrl+sizeof(DIRSTRU));
         if (mlen <= (hlen=1+6+1+9+1)+2) break;
         p=MFX+(MFRmfrl+sizeof(DIRSTRU));
         sprintf(p,"H%6"_LD_" %9"_LD_" ",(LONGX)qty,0L);
         q=p+hlen; if (!fgets(q, mlen-hlen, stdin)) break;
         qlen=strlen(q); if (!qlen) break;
         if (q[--qlen] != '\n') break;
         sprintf(p+hlen-(9+1),"%9"_LD_,(LONGX)qlen); p[hlen-1]=' ';
#if CICPP
         p=recp->xfldupdat(p);
#else /* CICPP */
         p=fldupdat(irec,p);
#endif /* CICPP */
         if (p) fatal(p);
         qty=0;
  }
  ni=(l_code *) next_infoi(ni);
  break;
 }

case n_sign_spac:
 {
  /*A07  new_line(out,pout); */
  new_line(out,pout,outsize);
  nextcc=1; /* upadtes next position  avaialble in line */
  ni=(l_code *) next_infoi(ni);
  break;
 }
case slash_spac:
 {
  if(! (fmt_CRLF(out,pout) || pout==0) )
    {
     /*A07 new_line(out,pout); */
     new_line(out,pout,outsize);
     nextcc=1; /* updates next position available in the output */
    }
  ni=(l_code *) next_infoi(ni);
  break;

 }

 case test_rep:
  {
   /* Implementacao do continue 17-7-94 */
   if (continue_rgroup==true){
     if (next_rep_occ <= /* ou < ?*/ continue_rgroup_maxocc)
            must_repeat=true;
   }

   /* Tentativa de Implementar v70[1] dentro de repetitivo */
   /* 30-12-94 */
   if (rep_group==true   &&  !NO_INTERVAL(itf)) {
     if (next_rep_occ <  /* ou <= ?*/ continue_rgroup_maxocc)
            must_repeat=true;
   }
   elem.classe=logical;
   elem.op.boolean=must_repeat;
   push(&elem);
   IFERR_GOTO;
   ni=(l_code *) next_infoi(ni);
   break;
  }

 case set_true_rep:
  {
   must_repeat=true;
   ni=(l_code *) next_infoi(ni);
   break;
  }
 case set_false_rep:
  {
   must_repeat=false;
   /* Implementar continue 17-7-94 */
   continue_rgroup=false;
   ni=(l_code *) next_infoi(ni);
   break;
  }
  case mpl_par:
  case mpu_par:
  case mdl_par:
  case mdu_par:
  case mhl_par:
  case mhu_par:
  {
   actual_mode=infoi(ni);
   ni=(l_code *) next_infoi(ni);
   break;
  }

  case contains:
  {
   auxpop = pop(); IFERR_GOTO;
   op2=(stack_node *)memmove(&op2_node,auxpop,sizeof(stack_node));
   auxpop = pop(); IFERR_GOTO;
   op1=(stack_node *)memmove(&op1_node,auxpop,sizeof(stack_node));
   elem.classe=logical;
   if (op1->classe!=op2->classe || op1->classe!=string)
     {inter_error("Invalid operands for ':' operator ");
      IFERR_GOTO;
     }
    else
    {if(strstr(op1->op.s,op2->op.s)!=null_char)elem.op.boolean=true;
       else elem.op.boolean=false;

    }
    push(&elem);
    IFERR_GOTO;
    ni=(l_code *) next_infoi(ni);
   break;

  }

  case instr_end:
  {
   auxpop = pop(); IFERR_GOTO;
   op2=(stack_node *)memmove(&op2_node,auxpop,sizeof(stack_node));
   auxpop = pop(); IFERR_GOTO;
   op1=(stack_node *)memmove(&op1_node,auxpop,sizeof(stack_node));
   if (op1->classe!=op2->classe || op1->classe!=string)
     {inter_error("Invalid operands for 'instr' operator ");
      IFERR_GOTO;
     }
    else{
     restore_context();
     elem.classe=long_n;
     elem.op.l= (LONGX) fmt_instr(op1->op.s,op2->op.s);
    }
    push(&elem);
    IFERR_GOTO;
    ni=(l_code *) next_infoi(ni);
   break;

  }

  case right_end:
  case left_end:
  {
   int lng,xn;
   char *tt;
   auxpop = pop(); IFERR_GOTO;
   op2=(stack_node *)memmove(&op2_node,auxpop,sizeof(stack_node));
   auxpop = pop(); IFERR_GOTO;
   op1=(stack_node *)memmove(&op1_node,auxpop,sizeof(stack_node));
   if (op1->classe!=string || !is_numeric(op2->classe)){
      inter_error("Invalid operands for ':' operator ");
      IFERR_GOTO;
   }else{
      tt=(char *)op1->op.s;
      lng=strlen(tt);
      xn=cast_to_int(op2);
      if (xn<0) xn=0;
      if (infoi(ni)==right_end){
#if TRACE_STRFUN
         printf( "\n lng=%d caracters a direita de =%s=",xn,op1->op.s);
#endif
         lng=lng-xn;
         if (lng<0) lng=0;
         tt= (char *) (tt + lng);
#if TRACE_STRFUN
          printf ("%s\n",tt);
#endif
      }
      if (infoi(ni)==left_end){
#if TRACE_STRFUN
        printf( "\n lng=%d caracters a esquerda de =%s=",xn,op1->op.s);
#endif
        if ( lng> xn) lng=xn;
        if (lng <0) lng=0;
        tt[lng]=null_char;
#if TRACE_STRFUN
        printf ("%s\n",tt);
#endif
      }
   }
    restore_context();
    elem.op.s=tt;
    elem.classe=string;
    push(&elem);
    IFERR_GOTO;
    ni=(l_code *) next_infoi(ni);
   break;
  }

 case mid_end:   /*    mid(string,start,lenght) */
 case ss_end:    /*A10* ss(pos,length,string) */
  {
       /* op1 - string
          op2 - start
          op3 -length
       */
   int lng,xn_inic,xn_fim;
   stack_node op3_node;
   stack_node *op3;
   char *tt;
   if (infoi(ni)==mid_end){
     auxpop = pop(); IFERR_GOTO;
     op3=(stack_node *)memmove(&op3_node,auxpop,sizeof(stack_node));
     auxpop = pop(); IFERR_GOTO;
     op2=(stack_node *)memmove(&op2_node,auxpop,sizeof(stack_node));
     auxpop = pop(); IFERR_GOTO;
     op1=(stack_node *)memmove(&op1_node,auxpop,sizeof(stack_node));
   }
   if (infoi(ni)==ss_end) {
     auxpop = pop(); IFERR_GOTO;
     op1=(stack_node *)memmove(&op3_node,auxpop,sizeof(stack_node));
     auxpop = pop(); IFERR_GOTO;
     op3=(stack_node *)memmove(&op2_node,auxpop,sizeof(stack_node));
     auxpop = pop(); IFERR_GOTO;
     op2=(stack_node *)memmove(&op1_node,auxpop,sizeof(stack_node));
   }
   if (op1->classe!=string || !is_numeric(op2->classe) ||
                              !is_numeric(op3->classe )){
      inter_error("Invalid operands for ':' operator ");
      IFERR_GOTO;
   }else{
      tt=(char *)op1->op.s;
      lng=strlen(tt);
      xn_inic=cast_to_int(op2);
      xn_fim=cast_to_int(op3);
#if TRACE_STRFUN
         printf( "\n inic=%d fim=%d a direita de =%s=",xn_inic,xn_fim,tt);
#endif
         if (xn_inic<=0) xn_inic=1;
         if (xn_inic>lng) xn_inic=lng+1;
         tt=(char *) (tt + xn_inic-1);
         lng=strlen(tt);
         if (xn_fim<0) xn_fim=0;
         if (xn_fim>lng) xn_fim=lng;
         tt[xn_fim]=null_char;
#if TRACE_STRFUN
         printf ("%s\n",tt);
#endif
        restore_context();
        elem.op.s=tt;
        elem.classe=string;
        push(&elem);
        IFERR_GOTO;
        ni=(l_code *) next_infoi(ni);
       break;
  }
  }

#if CI_XMLELEM
#include "exmlelem.c"
#endif

 case replac_end:
  {
   int lng,nx,lngfrom,lngto,difflen;
   stack_node op3_node;
   stack_node *op3;
   char *strtorep,*strfrom,*strto,*p1,*p2,*pnew;
   /* sintaxe: REPLACE(strtorep,strfrom,strto)     */
    nx=0;
   auxpop = pop(); IFERR_GOTO;
   op3=(stack_node *)memmove(&op3_node,auxpop,sizeof(stack_node));
   auxpop = pop(); IFERR_GOTO;
   op2=(stack_node *)memmove(&op2_node,auxpop,sizeof(stack_node));
   auxpop = pop(); IFERR_GOTO;
   op1=(stack_node *)memmove(&op1_node,auxpop,sizeof(stack_node));
   if (op1->classe!=string || op2->classe!=string  ||
                              op3->classe!=string ){
      inter_error("Invalid operands for 'replace' function ");
      IFERR_GOTO;
   }else{
      strtorep=(char *)op1->op.s;
      strfrom=(char *)op2->op.s;
      strto=(char *)op3->op.s;
      lng=strlen(strtorep);
      lngfrom=strlen(strfrom);
      lngto=strlen(strto);
      difflen=0;
      if (lngto>lngfrom) difflen=lngto-lngfrom;
#if TRACE_REPLACE
         printf( "\nRepl_str= |%d| |%s|",lng,strtorep);
         printf("\n Repl_from=|%d| |%s|",lngfrom,strfrom);
         printf("\n Repl_to=  |%d| |%s|",lngto,strto);
#endif
      nx=0;
      p1=strtorep;
      if (lngfrom <=0) p1=NULL; /* Nao tem o que substituir. O
                                Algoritmo entraria em loop  */
     /* Deternina a quantidade de substituicoes a serem feitas para
         calcular o tamanho do novo string a ser alocado */
      while (p1!=NULL){
#if TRACE_REPLACE
        printf("\n Testando oc nx:|%d| em:|%s|",nx,p1);
#endif
        p2=strstr(p1,strfrom);
        if (p2 !=NULL) {
           p2=p2+lngfrom;
           nx++;
        }
        p1=p2;
      }
     if (nx ==0 ) {
      pnew=strtorep;  /* Nao tem nada a substituir */
     }else {
       pnew=fmt_alloc_char( (ALLOPARM)(lng+1+nx*difflen),
                                         "cifm3/replace/alloc");
       if (pnew == NULL) IFERR_GOTO;
       *pnew=null_char;
       p1=strtorep;
       while (p1!=NULL) {
#if TRACE_REPLACE
          printf("\n Substiutindo orig:|%s|",p1);
          printf("\n Substituindo new :|%s|",pnew);
#endif
          p2=strstr(p1,strfrom);
          if (p2!=NULL) {
             *p2=null_char;
             strcat(pnew,p1);
             strcat(pnew,strto);
             p2=p2+lngfrom;
          }else strcat(pnew,p1); /* concatena ultimo pedaco */
          p1=p2;
       }/*while */
     } /* else */
     restore_context();
     elem.op.s=store_tmp_string(pnew);
     elem.classe=string;
     push(&elem);
     IFERR_GOTO;
   if (nx > 0) { /* houve alocacao  de memoria */
#if CICPP
        delete [] (char *)pnew;;
#else
        FREE(pnew);
#endif
    }
        ni=(l_code *) next_infoi(ni);
       break;
  }
  }



   case begin_rep_gr:
      {
      if(rep_group==false)
       {rep_group=true;
        next_rep_occ=1;
       }
      else next_rep_occ++;
#if FMT_OPTIMIZE
      must_repeat=false;
      /* Implementar continue 17-7-94 */
      continue_rgroup=false;
#endif
      ni=(l_code *) next_infoi(ni);
      break;
      }
   case ref_beg:
     {
#if CICPP  /*ifcmm*/
      elem.classe=type_recstru;
      elem.op.lrecp=recp;
#else /*elsecmm*/
      elem.classe=long_n;
      elem.op.l=irec;
#endif /*CICPP*/  /*endcmm*/
      push(&elem);
      IFERR_GOTO;
#if 0 /* AOT+RP -  15/01/97 - read_mfn ja' chama fmt_comum_ref_beg() */
      fmt_comum_ref_beg();
#endif
      ni=(l_code *) next_infoi(ni);
      break;
     }

   case ref_end:
   {
    fmt_comum_ref_end();
    IFERR_GOTO;
    ni=(l_code *) next_infoi(ni);
    break;
   }
   case refu_beg:
    {
     save_context(no_modify_fmt_pointers);
     IFERR_GOTO;
#if CICPP  /*ifcmm*/
     elem.classe=type_recstru;
     elem.op.lrecp=recp;
#else /*elsecmm*/
     elem.classe=long_n;
     elem.op.l=irec;
#endif /*CICPP*/  /*endcmm*/
     push(&elem);
     IFERR_GOTO;
#if 0 /* AOT+RP -  15/01/97 - read_mfn ja' chama fmt_comum_ref_beg() */
     fmt_comum_ref_beg();
#endif
     ni=(l_code *) next_infoi(ni);
     break;
     }

   case refu_end:
   {
    fmt_comum_ref_end();
    IFERR_GOTO;
    restore_context();
    IFERR_GOTO;
    tmp_ptr=store_tmp_string(tmp_ptr);
    IFERR_GOTO;
    out[pout]=null_char;
    elem.op.s=tmp_ptr;
    elem.classe=string;
    push(&elem);
    IFERR_GOTO;
    ni=(l_code *) next_infoi(ni);
    break;
   }

   case read_mfn:
   {
    auxpop = pop(); IFERR_GOTO;
    top=(stack_node *)memmove(&top_node,auxpop,sizeof(stack_node));
    ref_mfn_number=fmt_cnv_to_long(top);
    /* se mfn=0; nao ha registro para formatar
       precisa ignorar todas as instrucoes relativas ao formato do ref
       deixando como instrucao a ultima ref_end
        Quando existe base de dados, o nome dela  esta na pilha
        campo iadd(i), vale 1L. Caso contrario vale 0L
    */
    literal[0]=null_char;
    if ((LONGX )iadd(ni) ==1L){
          auxpop = pop(); IFERR_GOTO;
          top=(stack_node *)memmove(&top_node,auxpop,sizeof(stack_node));
          strcpy(literal, (char *) top->op.s );
 /*A06 Nao realoquei porque o nome do dbn tem que ser menor que 255*/
    }else strcpy(literal, firstdbnp);
#if BEFORE20030514
    if (ref_mfn_number==0L){
#else
    if (ref_mfn_number<=0L){
#endif
     fmt_fim=false;
     fmt_n_ref=1;
     while (fmt_fim==false){
       if(infoi(ni)==ref_beg || infoi(ni)==refu_beg) fmt_n_ref++;
       if(infoi(ni)==ref_end || infoi(ni)==refu_end) fmt_n_ref--;
       if(fmt_n_ref==0) fmt_fim=true; else ni=(l_code *) next_infoi(ni);
     }
    }else{
#define KEEPDBXR 0
#if KEEPDBXR
      LONGX idbx,keepndbx=ndbxs;
#endif /* KEEPDBXR */

      fmt_comum_ref_beg();

#if CICPP  /*ifcmm*/
      recp->xrecord(literal,ref_mfn_number);
#else /*elsecmm*/
      RECORD(irec,literal,ref_mfn_number);
#endif /*CICPP*/  /*endcmm*/

#define FMTMFLUSH 0
#if FMTMFLUSH
      if (strcmp(firstdbnp,literal)) mstflush(literal);
#endif

#if KEEPDBXR
      for (idbx=ndbxs-1; idbx>=keepndbx; idbx--)
          dbxflush(vdbxp[idbx]->dbxname);
#endif /* KEEPDBXR */

/* Alocacao das maiores areas originalmente estaticas - AOT 29/12/90 */
#if DINALLOC
    if (!fmt_fsiz) {
      for (xdir=MFRnvf, xfsiz=0; xdir--; )
        if (DIRlen(xdir) > xfsiz) xfsiz=DIRlen(xdir);
      if (xfsiz > din_fsiz) {
        din_fsiz=xfsiz;
        if (fmttrace) printf("+++ fmt_inter - din_fsiz=%"_LD_"\n",din_fsiz);
        max_fd_value=din_fsiz;
        max_fd_tmp=din_fsiz;
#if CICPP
        delete [] (char *)v_fd_value;
        delete [] (char *)v_fd_tmp;
#else /* CICPP */
        FREE(v_fd_value);
        FREE(v_fd_tmp);
#endif /* CICPP */
#if CICPP
        try
        { v_fd_value= (char *)new char [(max_fd_value+1)]; }
        catch (BAD_ALLOC)
        { v_fd_value= (char *)NULL; }
#else /* CICPP */
        v_fd_value= (char *)ALLOC((ALLOPARM)(max_fd_value+1));
#endif /* CICPP */
        if (v_fd_value== (char *)NULL)
        {
          inter_error("1003");
          IFERR_GOTO;
        }
#if CICPP
        try
        { v_fd_tmp= (char *) new char[(max_fd_tmp+1)]; }
        catch (BAD_ALLOC)
        { v_fd_tmp= (char *)NULL; }
#else /* CICPP */
        v_fd_tmp=   (char *)ALLOC((ALLOPARM)(max_fd_tmp+1));
#endif /* CICPP */
        if (v_fd_tmp==   (char *)NULL)
        {
          inter_error("1004");
          IFERR_GOTO;
        }
        fd_value=v_fd_value;    /* DINALLOC */
      }
    }
#endif
      ni=(l_code *) next_infoi(ni);
     }
   break;
   }

   case rupd_beg:
   case rupx_beg:
   case syst_beg:
   case date_mktime_beg:
   case l_beg:
   case np_beg:
   {
    save_context(modify_fmt_pointers);
    IFERR_GOTO;
    /* o formato a ser processado devera ser usado na lookup */
    ni=(l_code *)next_infoi(ni);
    break;
   }

   case rupd_end:
   case rupx_end:
   case syst_end:
   case date_mktime_end:
   case l_end:
   case np_end:
   {
    LONGX p_mfn_lookup;
    LONGX nord;

    /* Se houve referencia a database ele esta na pilha */
     new_dbname=NULL;
     if ((LONGX )iadd(ni) ==1L){
          auxpop = pop(); IFERR_GOTO;
          top=(stack_node *)memmove(&top_node,auxpop,sizeof(stack_node));
          new_dbname=top->op.s; /* antes copiava para outra area*/
     }

    /*copia para tmp_str e restaura contexto */
     restore_context();
     IFERR_GOTO;
     tmp_ptr=store_tmp_string(tmp_ptr);
     IFERR_GOTO;
     out[pout]=null_char;

     if (infoi(ni) == rupd_end) {
         char *p;
#if CICPP
         p=recp->xfldupdat(tmp_ptr);
#else /* CICPP */
         p=fldupdat(irec,tmp_ptr);
#endif /* CICPP */
         if (p) fatal(p);
         ni=(l_code *)next_infoi(ni);
         break;
     }
     if (infoi(ni) == rupx_end) {
         /* procx.c */
#if FUNPROCX
#if MXFUN || IFLOADFUN
        char *p=tmp_ptr;
        while (isspace(*p)) p++;
#if MXFUN
        if (strncmp(p,"mx",2) == 0 && isspace(*(p+2))) { cisis_mx(p+2+1); }
#endif /* MXFUN */
#if IFLOADFUN
        if (strncmp(p,"ifload",6) == 0 && isspace(*(p+6))) { cisis_ifload(p+6+1); }
#endif /* IFLOADFUN */
#endif /* MXFUN || IFLOADFUN */
#endif /* FUNPROCX */
         ni=(l_code *)next_infoi(ni);
         break;
     }

     if (infoi(ni) == syst_end) {
         /* system(tmp_ptr); AOT - 19/03/97 */
         char *shp,*p;
         for (shp=tmp_ptr; *shp; ) {
             for (p=shp; *p; p++)
                 if (*p == '\n' || *p == '\r') {
                     *p++ = '\0';
                     if (*p == '\n' || *p == '\r') *p++ = '\0';
                     break;
                 }
             if (*shp) system(shp);
             shp = p;
         }
         ni=(l_code *)next_infoi(ni);
         break;
     }

     if (infoi(ni) == date_mktime_end) {
         /* mktime(<fmt>) AOT - 16/02/2001 */
         int year,mon,day,hour,min,sec;
         struct tm tpx;
         struct tm *tp= &tpx;
         char *p;
/*The allowable range of calendar times is Jan 1 1970 00:00:00 to Jan 19 2038 03:14:07.
*/
         if (strcmp(tmp_ptr,(p="19700102 000000")) < 0) strcpy(tmp_str,p);
         if (strcmp(tmp_ptr,(p="20380118 031407")) > 0) strcpy(tmp_str,p);
         sscanf(tmp_ptr,"%4d%2d%2d",&year,&mon,&day);
         hour=min=sec=0;
         if (strlen(tmp_ptr)>=11) sscanf(tmp_ptr+8, "%*c%2d",&hour);
         if (strlen(tmp_ptr)>=13) sscanf(tmp_ptr+11,"%2d",&min);
         if (strlen(tmp_ptr)>=15) sscanf(tmp_ptr+13,"%2d",&sec);
         tpx.tm_year=year-1900;
         tpx.tm_mon=mon-1;
         tpx.tm_mday=day;
         tpx.tm_hour=hour;
         tpx.tm_min=min;
         tpx.tm_sec=sec;
         tpx.tm_wday=tpx.tm_yday=0;
#if 0
time_t mktime(struct tm *t);
Converts the time in the structure pointed to by t
into a calendar time with the same format used by the time function.
#endif
         elem.op.l=mktime(tp);
         elem.classe=long_n;
         push(&elem);
         IFERR_GOTO;
         ni=(l_code *)next_infoi(ni);
         break;
     }


#if CICPP
     TRMSTRU *trmp;
     try { trmp=new TRMSTRU(cisisxp); }
     catch (BAD_ALLOC) { fatal("fmt/inter/reftrm/next"); }
#else /* CICPP */
     for (trm_prat=maxntrm; trm_prat--; ){
        if (!vtrmp[trm_prat]) /* ja' decrementado */
            break;
     }
     if (!trm_prat) fatal("fmt/inter/reftrm/next");
#endif /* CICPP */
     /*A03*/
     /*A04*/
     /* Modifica o tipo de acesso de posting para pedir total de
       posting  para instrucao NP ou o primeiro MFN em caso de
       da instrucao LOOKUP
     */
     nord=0L; /* supoe NP */
     if ( infoi(ni)==l_end)nord=1L;

     if (new_dbname==NULL) new_dbname=firstdbnp; /* nao muda o acesso */

     upcase_mode(tmp_ptr);
#if CICPP
     p_mfn_lookup=lookup(tmp_ptr,trmp,new_dbname,nord);
#else /* CICPP */
     p_mfn_lookup=lookup(tmp_ptr,trm_prat,new_dbname,nord);
#endif /* CICPP */

    /*A03*/
#if CICPP
     delete trmp;
#else /* CICPP */
     if (vtrmp[trm_prat]!=NULL) { /* SVD 14-05-92 */
          FREE(vtrmp[trm_prat]); vtrmp[trm_prat]=NULL; ntrms--;
     }                           /* SVD 14-05-92 */
#endif /* CICPP */

     if (p_mfn_lookup <0) p_mfn_lookup=0;

     elem.op.l=p_mfn_lookup;
     elem.classe=long_n;
     push(&elem);
     IFERR_GOTO;

     ni=(l_code *)next_infoi(ni);
     break;
   }

   case lw_beg:
     {
     ni=(l_code *) next_infoi(ni);
     break;
     }

   case lw_end:
        { /* valor do numero gerado da pilha  */
         fd_value[0]=null_char;
     auxpop = pop(); IFERR_GOTO;
     top=(stack_node *)memmove(&top_node,auxpop,sizeof(stack_node));
     is_numeric(top->classe);
     IFERR_GOTO;
         lw_inter=(LONGX)top->op.l;
         ni=(l_code *) next_infoi(ni);
         break;
        }
   case f_beg:
   case instr_beg:
   case left_beg:
   case right_beg:
   case mid_beg:
   case replac_beg:
#if CI_XMLELEM
   case xmlelem_beg:
#endif
   case ss_beg:        /*A10*/
   case datex_beg:
     {
     save_context(modify_fmt_pointers);
     IFERR_GOTO;
     ni=(l_code *) next_infoi(ni);
     break;
     }

   case datex_end:
    {
     char *tmpch;
     fd_value[0]=null_char;
     auxpop = pop(); IFERR_GOTO;
     top=(stack_node *)memmove(&top_node,auxpop,sizeof(stack_node));
     is_numeric(top->classe);
     IFERR_GOTO;
     float_exp_value =  fmt_cnv_to_float(top);
      tmpch=fmt_get_date(0,float_exp_value);
      if(is_upper_mode(actual_mode)) upcase_mode(tmpch);
/*
   tmpch=store_tmp_string(tmp_ptr);
   IFERR_GOTO;
*/
   restore_context();
   IFERR_GOTO;
        elem.op.s=tmpch;
        elem.classe=string;
        push(&elem);
        IFERR_GOTO;
   ni=(l_code *) next_infoi(ni);
   break;
   }

   case f_end:                          /* SVD alterado em 25/11/90 */
    { /* retirar os tres parametros da pilha */
     char *tmpch;
     fd_value[0]=null_char;
                   /* AOT - Correcao SVD 21/08/90 */
     auxpop = pop(); IFERR_GOTO;
     top=(stack_node *)memmove(&top_node,auxpop,sizeof(stack_node));
         is_not_of_class(top->classe,long_n);
         IFERR_GOTO;
     is_numeric(top->classe);
     IFERR_GOTO;
         dec_places=(int)top->op.l;

     auxpop = pop(); IFERR_GOTO;
     top=(stack_node *)memmove(&top_node,auxpop,sizeof(stack_node));
         is_not_of_class(top->classe,long_n);
         IFERR_GOTO;
     is_numeric(top->classe);
     IFERR_GOTO;
         min_width=(int)top->op.l;


     auxpop = pop(); IFERR_GOTO;
     top=(stack_node *)memmove(&top_node,auxpop,sizeof(stack_node));
     is_numeric(top->classe);
     IFERR_GOTO;
     float_exp_value =  fmt_cnv_to_float(top);
#if DEB_FLOAT
 printf("\n++Antes de formatar out=");
 for (i=0;i<=pout;i++)printf("%c",out+i);
  printf("|");

#endif
#if MPE || VAX || UNIX
    tmpch=fmt_float_numb(float_exp_value,min_width,dec_places,space_char);
#else
    tmpch=fmt_float_numb(float_exp_value,min_width,dec_places,space_char);
#endif

#if DEB_FLOAT
 printf("\n++Apos formatar out=");
  for (i=0;i<=pout;i++)printf("%c",out+i);
  printf("|");

 printf("\ntmpch=%s|",tmpch);
#endif

   restore_context();
   IFERR_GOTO;
   tmp_ptr=store_tmp_string(tmpch);
   IFERR_GOTO;
        elem.op.s=tmp_ptr;
        elem.classe=string;
        push(&elem);
        IFERR_GOTO;

/*     strcpy(out,ss); agora ja gerou um print-string,load-string ou nada */
   ni=(l_code *) next_infoi(ni);
   break;
   }
   case s_beg:
   case getenv_beg:
   case putenv_beg:
   case citype_beg:
   case cat_beg:
   case nl_beg:
   case sattrib_beg:
   case type_beg:
     {
     save_context(modify_fmt_pointers);
     IFERR_GOTO;
     ni=(l_code *) next_infoi(ni);
     break;
     }

   case s_end:
   {
      restore_context();
      IFERR_GOTO;
      /* substring em S */
      itf=(field_definition_node *) iadd(ni);
      if(itf->offset!= -1 && itf->length!= -1)
          strcpy(tmp_ptr,sub_string(tmp_ptr,itf->offset,itf->length));
      tmp_ptr=store_tmp_string(tmp_ptr);
      IFERR_GOTO;
      out[pout]=null_char;
      elem.op.s=tmp_ptr;
      elem.classe=string;
      push(&elem);
      IFERR_GOTO;
      ni=(l_code *) next_infoi(ni);
      break;
   }

   case sattrib_end:
   {  char *q;
      int ind;
      restore_context();
      IFERR_GOTO;
      ind= (int)iadd(ni);
      fmt_free_S(ind,ind);
      q=strdup(tmp_ptr); /*??? como testar retorno */
      IFERR_GOTO;
      S_var[ind] = q;
#if DEB_ATTRIB
  printf("\ntmp_ptr=|%s|,S_var[%d]=%s",tmp_ptr,(int)iadd(ni),S_var[(int) iadd(ni)]);
#endif
      out[pout]=null_char;

      ni=(l_code *) next_infoi(ni);
      break;
   }
   case strvvalue:
   {  char *t;
      t=store_tmp_string(S_var[(int) iadd(ni)]);
      elem.op.s=t;
      elem.classe=string;
      push(&elem);
      IFERR_GOTO;
      ni=(l_code *) next_infoi(ni);
      break;
   }

   case getenv_end:
   {
      char *ep;
      restore_context();
      IFERR_GOTO;
      /* substring em S */
      itf=(field_definition_node *) iadd(ni);
      ep=NULL; if (*tmp_ptr) ep=getenv(tmp_ptr); /* AOT 04/03/97 */
      if (ep) {
          tmp_ptr=store_tmp_string(ep);
      }
      else
      if (strncmp(tmp_ptr,"tmp=",4) == 0) {
          tmp_ptr+=4; /* tmp= */
          ep=dbxtmpnm(NULL/*"CI_TEMPDIR"*/,0,tmp_ptr);
          tmp_ptr=store_tmp_string(ep);
      }
#if CICGI /* get cgi - AOT 12/09/2000 */
      else
      if (strncmp(tmp_ptr,"cgi=",4) == 0) {
#if CICPP
        //CGIClass *cgi = new CGIClass(cisisxp);
        //cgi->cicgi0(NULL, NULL, &wwwp, (UWORD)1, "tag"); /* H1 10 ^ndbn^vcdsH1 10 ^ncount^v2H1 7 ^nnow^vH1 10 ^nbtell^v0H1 20 ^nbool^vplants*waterH1 15 ^npft^vmfn/v24/ */
        //delete cgi;
#else /* CICPP */
#if CIWTF                                /* WTFUN support */
        tmp_ptr+=4; /* cgi= */
        if (!ciahcgi) {
            char *wwwp=NULL;
            cicgi0(NULL, NULL, &wwwp, (UWORD)2000, "tag"); /* H2000 10 ^ndbn^vcdsH2000 10 ^ncount^v2H2000 7 ^nnow^vH2000 10 ^nbtell^v0H2000 20 ^nbool^vplants*waterH2000 15 ^npft^vmfn/v24/ */
            ciahcgi=wwwp;
        }
        if (ciahcgi) {
            char *p,cnam[3+LE2+2+1]; //.^nxxx^v
            int cnamlen;
            if (strlen(tmp_ptr)>LE2) tmp_ptr[LE2]='\0'; sprintf(cnam," ^n%s^v",tmp_ptr); cnamlen=strlen(cnam);
            for (p=ciahcgi; (p=strstr(p,"H2000 ")) != NULL; ) {
                int s;
                char k;
                p+=6; for (s=0; *p && isdigit(*p); p++) s=s*10+(int)(*p)-(int)'0';
                if (!s) continue;
                if (strncmp(p,cnam,cnamlen)) continue;
                p+=cnamlen; ep=p;
                k=ep[s]; ep[s]='\0'; tmp_ptr=store_tmp_string(ep); ep[s]=k;
                break;
            }
        }
#else /* CIWTF */
        static char *fmtwwwp=NULL;
        char *wwwp=NULL;
        int len,qn=0;
        FFI ffilen;
        char *p,*vp,*q;
        tmp_ptr+=4; /* cgi= */
        if (!fmtwwwp) {
            cicgi0(NULL, NULL, &wwwp, (UWORD)1, "tag"); /* H1 10 ^ndbn^vcdsH1 10 ^ncount^v2H1 7 ^nnow^vH1 10 ^nbtell^v0H1 20 ^nbool^vplants*waterH1 15 ^npft^vmfn/v24/ */
            fmtwwwp=wwwp;
        }
        wwwp=fmtwwwp;
        for (q=p=wwwp; *p; ) {
          if (*p++ != 'H') break; if (*p++ != '1') break; if (*p++ != ' ') break;
          len=atoi(p); while(isdigit(*p)) p++; if (len < 5) break; if (*p++ != ' ') break;
          ffilen=(FFI)len; vp=(char *)subfldp((unsigned char *)p,'v',&ffilen); if (!vp) break;
          *(vp-2)='\0';
          if (strcmp(p+2,tmp_ptr) == 0) {
              /* if (!ffilen) ep=p+2; else { ep=vp; ep[ffilen]='\0'; } break; */
              if (!ffilen) { ep=p+2; ffilen=strlen(ep); } else ep=vp;
              if (qn) *q++='%'; strncpy(q,ep,ffilen); q+=ffilen; *q='\0'; qn++;
          }
          p+=len;
        }
        if (qn) strcpy(ep=tmp_ptr,wwwp);
        tmp_ptr=store_tmp_string(ep);
#endif /* CIWTF */
#endif /* CICPP */
      }
#endif /* get cgi - AOT 12/09/2000 */
      if (!ep) tmp_ptr=store_tmp_string(ep);
      IFERR_GOTO;
      out[pout]=null_char;
      elem.op.s=tmp_ptr;
      elem.classe=string;
      push(&elem);
      IFERR_GOTO;
      ni=(l_code *) next_infoi(ni);
      break;
   }

   case putenv_end:
   {
      char *ep;
      restore_context();
      IFERR_GOTO;
      /* substring em S */
      itf=(field_definition_node *) iadd(ni);
#if BEFORE20000913
      /* bug: vai alocar e nao liberar antes do fim de exec. - AOT 03/09/97 */
      ep=NULL; if (*tmp_ptr) ep=putenv(strdup(tmp_ptr)) ? tmp_ptr : NULL;
#else
      ep=NULL;
      if (*tmp_ptr) {
          char *ip,*op;
          ip=strchr(tmp_ptr,'=');
          if (ip) {
              *ip='\0'; op=getenv(tmp_ptr); *ip='=';
              if (op) {
                  char c;
                  /* To delete the variable from the environment, use getenv("name=") */
                  ip++; c= *ip; *ip='\0'; getenv(tmp_ptr); *ip=c;
              }
          }
          ep=putenv(strdup(tmp_ptr)) ? tmp_ptr : NULL;
      }
#endif
      tmp_ptr=store_tmp_string(ep);
      IFERR_GOTO;
      out[pout]=null_char;
      elem.op.s=tmp_ptr;
      elem.classe=string;
      push(&elem);
      IFERR_GOTO;
      ni=(l_code *) next_infoi(ni);
      break;
   }

   case citype_end:
   {
     char *tt;
     restore_context(); /* Temporario tem que analizar o resultado */
     IFERR_GOTO;
     tt=fmt_type(tmp_ptr);
     tmp_ptr=store_tmp_string(tt);
     IFERR_GOTO;
     out[pout]=null_char;
     elem.op.s=tmp_ptr;
     elem.classe=string;
     push(&elem);
     IFERR_GOTO;
     ni=(l_code *) next_infoi(ni);
     break;
   }

   case type_end:
   {
     LONGX rr ;
     rr=0;
     /* pattern ou valor numerico */
     auxpop = pop(); IFERR_GOTO;
     op1=(stack_node *)memmove(&op1_node,auxpop,sizeof(stack_node));

     /* como deve ser interpretado o operador 1 */
     auxpop = pop(); IFERR_GOTO;
     op2=(stack_node *)memmove(&op2_node,auxpop,sizeof(stack_node));
     is_not_of_class(op2->classe,long_n);
      IFERR_GOTO;
     tmp_ptr= &out[0];
     if(op2->op.l == TYPE_PATTERN ){
      rr = fmt_type_pattern(op1->op.s,tmp_ptr);
     }else {
       if ( op2->op.l == TYPE_NUM ){
        rr = fmt_type_number(op1->op.l,tmp_ptr);
       }

     }
     restore_context();
     IFERR_GOTO;

     out[pout]=null_char;
     elem.op.l=rr;
     elem.classe=long_n;
     push(&elem);
     IFERR_GOTO;
     ni=(l_code *) next_infoi(ni);
     break;
   }

   case cat_end:
   {
    /* Para poder usar cat como funcao e como elemento de formato
       e' necessario gerar um resultado para ser testado ou formatado
       na proxima instrucao. Por enquanto um valor "\0" esta sendo
       armzenado. Os testes de if nao funcionarao porque sempre
       sera esse valor armazenado. Deve ser discutido com AOT/ABEL
       Solucao: No cifm1
          - Quando for elemento gerar "\0" .
	  - Quando for if/expre/ gerar 0 se erro e 1 se ok.
    */

     int  fp;
     char ch[2];
     int keep_ordwr; /* AOT, 28/05/99 */

     restore_context(); /* Temporario tem que analizar o resultado */
     IFERR_GOTO;
     if (strcmp(tmp_str,"+fields") == 0) {
#if 0
       char line..
       ch[1]=null_char;
       while (CIREAD(fp,&ch[0],sizeof(char))> 0)
          out_put_str(out,outsize, &pout,lw_inter,0,0, &nextcc,(char *)&ch);
#endif
     }
     else { /* AOT 05/10/02 */
     dbxopt_fatal=0;
     keep_ordwr=dbxopt_ordwr; dbxopt_ordwr=O_RDONLY; /* AOT 28/05/99 */
     fp=dbxopen(NULL,tmp_ptr,NULL);
     dbxopt_ordwr=keep_ordwr; /* AOT 28/05/99 */
     ch[1]=null_char;
     if (fp >= 0){
       while (CIREAD(fp,&ch[0],sizeof(char))> 0) {
          out_put_str(out,outsize, &pout,lw_inter,0,0, &nextcc,(char *)&ch);
       }
       CLOSE(fp);
     }else{

         erro(1708);
/*       elem.op.s=store_tmp_string("\0"); */
      }
     } /* AOT 05/10/02 */
     IFERR_GOTO;
     out[pout]=null_char;
     elem.op.s=store_tmp_string("\0");
     elem.classe=string;
     push(&elem);
     IFERR_GOTO;

     ni=(l_code *) next_infoi(ni);
     break;
   }
   case nl_end:
   {
     int len;
     restore_context(); /* Temporario tem que analizar o resultado */
     IFERR_GOTO;
#if TRACESINDO
     printf("\n Valor gerado de tmp_ptr=|%s|",tmp_ptr);
#endif
     len=strlen(tmp_ptr);
     if (len > nl_LEN) {
#if CICPP
        delete [] (char *)nl_STR;
#else
        FREE(nl_STR);
#endif
        nl_STR=fmt_alloc_char((ALLOPARM)(len+1),"cifm3/alloc/nl_end");
     }
/*     strcpy((char *)nl_STR,(char *)tmp_ptr); */
/*        sprintf((char *)nl_STR,(char *)tmp_ptr,"");  Nao funcionou porque
          (eu acho) que o c traduz \n diretamente para 1 caracter e nao
	  dois carcteres \ seguido de n */
#if 1
     sc((char *)tmp_ptr);
#endif
     strcpy(nl_STR,tmp_ptr);
     nl_LEN=strlen(nl_STR);
#if TRACESINDO
     printf("\n Novo Valor gerado de nl_STR=|%s|",(char *)nl_STR);
#endif

     out[pout]=null_char;
     ni=(l_code *) next_infoi(ni);
     break;
   }

   case dbname_beg:
     {
     save_context(modify_fmt_pointers);
     IFERR_GOTO;
     ni=(l_code *) next_infoi(ni);
     break;
     }

   case dbname_end:
   {
      restore_context();
      IFERR_GOTO;
      tmp_ptr=store_tmp_string(tmp_ptr);
      IFERR_GOTO;
      out[pout]=null_char;
      elem.op.s=tmp_ptr;
      elem.classe=string;
      push(&elem);
      IFERR_GOTO;
      ni=(l_code *) next_infoi(ni);
      break;
   }

   case str_cat:
   {
   auxpop = pop(); IFERR_GOTO;
   top=(stack_node *)memmove(&top_node,auxpop,sizeof(stack_node));
   is_not_of_class(top->classe,string);
   IFERR_GOTO;
   out_put_str(out,outsize, &pout,lw_inter,0,0, &nextcc,top->op.s);
   ni=(l_code *) next_infoi(ni);
   break;
   }
   case prt_str_f:
   {

/* Quem chama deve inicializar os pointers de acordo . Acho que o nextcc
   tem que ser sempre o mesmo entre todas as chamadas. Quem deve tomar
   conta e o usuario. Inicialmente eu achava que ao retornar deveria
   formatar nas condicoes que estava ANTEs da chamada , mas agora acho que
   tem apenas que concatenar . Ver no isis e depois testar

      out_put_str(out,outsize, &pout,lw_inter,0,0, &nextcc,top->op.s);
*/
/*   strcat(out,top->op.s); */            /* SVD correcao 20/11/90*/

/*   nextcc=deve_ser o ultimo chamado; */
   auxpop = pop(); IFERR_GOTO;
   top=(stack_node *)memmove(&top_node,auxpop,sizeof(stack_node));
   is_not_of_class(top->classe,string);
   IFERR_GOTO;
#if DEB_FLOAT
   printf("\n--Desempilhou=%s|",top->op.s);
   printf("\n--out=%s|",out);
#endif
      out_put_str(out,outsize, &pout,lw_inter,0,0, &nextcc,top->op.s);

   ni=(l_code *) next_infoi(ni);
   break;
   }
/*    case load_str_f: */

   case rsum_beg:
   case ravr_beg:
   case rmin_beg:
   case rmax_beg:
   case val_beg:
   case size_beg:
   case eattrib_beg:
  {
     save_context(modify_fmt_pointers);
     IFERR_GOTO;
     ni=(l_code *) next_infoi(ni);
     break;
     }
   case rsum_end:
   case ravr_end:
  {    float_x sum;
#if !PC
       float_x x;
#endif
       int k;

        restore_context();
        IFERR_GOTO;
        sum=(float_x ) 0;
        i=0;
        k=0;
        pstr_num=find_numeric_string(tmp_ptr,&i);
        while ((size_t)strlen(pstr_num)>(size_t)0 )
        {
         k++;
/* HB 20080901 - a funcao fmt_float_numb nao converte corretamente se float_x for float
#if PC
*/
#if 1
         sum=sum+ (float_x) atof(pstr_num);
#else
         sscanf(pstr_num,float_x_fmt,&x);
         sum=sum+ x;
#endif
         pstr_num=find_numeric_string(tmp_ptr,&i);
        }

        if (infoi(ni)==ravr_end && k!=0)sum=sum/k;

        elem.op.r=sum;
        elem.classe=float_n;
        push(&elem);
        IFERR_GOTO;

        out[pout]=null_char;
        ni=(l_code *) next_infoi(ni);
        break;
   }
   case rmin_end:
   case rmax_end:
  {
        float_x sum,min_value,max_value;

        restore_context();
        IFERR_GOTO;
        i=0;
        pstr_num=find_numeric_string(tmp_ptr,&i);
/* HB 20080901 - a funcao fmt_float_numb nao converte corretamente se float_x for float
#if PC
*/
#if 1
        sum=(float_x) atof(pstr_num);
#else
        sscanf(pstr_num,float_x_fmt,&sum);
#endif
        min_value=sum;
        if (infoi(ni)==rmax_end) max_value=sum;
        while ((size_t)strlen(pstr_num)>(size_t)0 )
        {
/* HB 20080901 - a funcao fmt_float_numb nao converte corretamente se float_x for float
#if PC
*/
#if 1         
         sum=(float_x) atof(pstr_num);
#else
         sscanf(pstr_num,float_x_fmt,&sum);
#endif
         if( infoi(ni)==rmin_end && min_value>sum)min_value=sum;
         if( infoi(ni)==rmax_end && max_value<sum)max_value=sum;
         pstr_num=find_numeric_string(tmp_ptr,&i);
        }
        elem.op.r=min_value;
        if (infoi(ni)==rmax_end) elem.op.r=max_value;
        elem.classe=float_n;
        push(&elem);
        IFERR_GOTO;

        out[pout]=null_char;
        ni=(l_code *) next_infoi(ni);
        break;
   }
   case val_end:
  {
/*
         auxpop = pop(); IFERR_GOTO;
        top=(stack_node *)memmove(&top_node,auxpop,sizeof(stack_node));
        is_not_of_class(top->classe,string)
        IFERR_GOTO;
*/
        tmp_ptr=out;
        i=0; /* pointer inicial dentro da string */
        pstr_num=find_numeric_string(tmp_ptr,&i);
/* HB 20080901 - a funcao fmt_float_numb nao converte corretamente se float_x for float
#if PC
*/
#if 1
        elem.op.r=(float_x) atof(pstr_num);
#else
        sscanf(pstr_num,float_x_fmt,&(elem.op.r));
        /* elem.op.r = strtod(pstr_num,NULL); */
#endif
        elem.classe=float_n;

        restore_context();
        IFERR_GOTO;
        out[pout]=null_char;
        push(&elem);
        IFERR_GOTO;

        ni=(l_code *) next_infoi(ni);
        break;
   }

   case size_end:
  {
        elem.op.l= (LONGX ) strlen(out);
        elem.classe=long_n;
        restore_context();
        IFERR_GOTO;
        push(&elem);
        IFERR_GOTO;
        out[pout]=null_char;
        ni=(l_code *) next_infoi(ni);
        break;
   }

   case eattrib_end:
  {
     float_x x;
     auxpop = pop(); IFERR_GOTO;
     x = convert_to_float(auxpop);
     E_var[(int) iadd(ni)]= x;

     restore_context();
     IFERR_GOTO;
     out[pout]=null_char;
     ni=(l_code *) next_infoi(ni);
     break;
   }

   case end_rep_gr:
      {
        rep_group=false;
        next_rep_occ=0;
        ni=(l_code *) next_infoi(ni);
        break;
      }
   case beg_init_not_rep:
      {
       fmt_inicio_grupos_vfields(beg_init_not_rep,end_init_not_rep);
       IFERR_GOTO;
       ni=(l_code *)ni->next;
       break;
      }
   case end_init_not_rep:
      {
        fmt_fim_grupos_vfields();
        IFERR_GOTO;

#if TRACE_REPF
      printf("\n [out=]%s",out);
      printf("\n [End_init_not_rep <pop>] rep_goup=%d next_rep_occ=%d",
               rep_group,next_rep_occ);
#endif
        ni=(l_code *)ni->next;
        break;
      }
   case beg_init_rep:
      {
       fmt_inicio_grupos_vfields(beg_init_rep,end_init_rep);
       IFERR_GOTO;
#if DEB_NOCC
       printf("\n processando ocorrenci=%"_LD_,next_rep_occ);
#endif
       ni=(l_code *)ni->next;
       break;
      }
   case end_init_rep:
      {
        fmt_fim_grupos_vfields();
        IFERR_GOTO;
#if TRACE_REPF
      printf("\n [out=]%s",out);
      printf("\n [End_init_rep <pop>] rep_goup=%d next_rep_occ=%d",
               rep_group,next_rep_occ);
#endif
        ni=(l_code *)ni->next;
        break;
      }
/* Continue em grupos repetitivos  17-7-94 */
   case continueins:
      {
        continue_rgroup=true;
        ni=(l_code *)ni->next;
        break;
      }
 default:
  {
     ni=(l_code *) next_infoi(ni);
     break;
  }
}  /* switch */
  if (ni==nulo) fim_pgm=true;
}  /* while */


/* Liberacao das maiores areas originalmente estaticas - AOT 29/12/90 */
#if DINALLOC
#if CICPP
    delete [] (char *)tmp_str;
    delete [] (char *)v_fd_value;
    delete [] (char *)v_fd_tmp;
/*A06*/
    if (literal !=NULL) delete [] (char *)literal;
/*A07*/
    lineqtt = number_of_lines(out); /* RPIVA - use nl_STR before FREE */
    delete [] (char *)nl_STR;
#else /* CICPP */
    FREE(tmp_str);
    FREE(v_fd_value);
    FREE(v_fd_tmp);
/*A06*/
    if (literal != NULL) {
       FREE(literal);
    }
#if BEFORE20000405
    fmt_free_S(0,NMAXVAR);
#else
    fmt_free_S(0,NMAXVAR-1); /* AOT/HB - pois fmt_free_S assume from e to */
#endif
/*A07*/
    lineqtt = number_of_lines(out); /* RPIVA - use nl_STR before FREE */
    FREE(nl_STR);
#endif /* CICPP */
#endif
    if (fmttrace) printf("+++ fmt_inter - success \n"); /* AOT 27/12/91 */

if (erro_fatal==0 )
  {
    return (lineqtt);
  }
  else return(erro_fatal);

}

/*-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*                   inter_error                                            */
/*--------------------------------------------------------------------------*/

#if CICPP
void FMTSTRU :: inter_error(char *err)
#else /* CICPP */
#if ANSI
void inter_error(char *err)
#else /* ANSI */
void inter_error(err)
char *err;
#endif /* ANSI */
#endif /* CICPP */
{
    int errl;
#if TRACE_REPF
    printf("\n *** INTER -%s",err);
#endif

#if DINALLOC
#if CICPP
    delete [] (char *)tmp_str;
    delete [] (char *)v_fd_value;
    delete [] (char *)v_fd_tmp;
/*A06*/
    if (literal != NULL) delete [] (char *)literal;
/*A07*/
    delete [] (char *)nl_STR;
#else /* CICPP */
    FREE(tmp_str);
    FREE(v_fd_value);
    FREE(v_fd_tmp);
/*A06*/
    if (literal != NULL) FREE(literal);
/*A07*/
    FREE(nl_STR);
#endif /* CICPP */
#endif

    errl=strlen(err);
    if (errl >= MAXERRL) errl=MAXERRL;

    strncpy(fmterrxy,err,errl); fmterrxy[errl]='\0';

#if FATRAP
    longjmp(fmtjumper,-1);
#else  /* FATRAP */
    fmterror = -1;
    return;
#endif /* FATRAP */
    fatal("inter_error"); /* just in case */
}


/* --------------------LOOKUP.C --------------------------------*/

#if CICPP
LONGX FMTSTRU :: lookup(char *keyp,
                       TRMSTRU *trmp,
                       char *dbnp,
                       LONGX nord)
#else /* CICPP */
#if ANSI
LONGX lookup(char *keyp,LONGX next_inverted_prat,char *dbnp,LONGX nord)
#else /* ANSI */
LONGX lookup(keyp,next_inverted_prat,dbnp,nord)
char *keyp;
LONGX next_inverted_prat;
char *dbnp;
LONGX nord;
#endif /* ANSI */
#endif /* CICPP */
{
#if !CICPP
    TRMSTRU *trmp;
#endif
    LONGX pmfn;
    pmfn=0;
        if (*keyp)
        {
#define KEEPDBXT 0
#if KEEPDBXT
            LONGX idbx,keepndbx=ndbxs;
#endif /* KEEPDBXT */
#if CICPP
            trmp->xterm((UCHR *)dbnp,(UCHR *)keyp);
#else /* CICPP */
            TERM(next_inverted_prat,(UCHR *)dbnp,(UCHR *)keyp);
#endif /* CICPP */
            if (TRMrc == RCNORMAL) {
#if CICPP
                pmfn=trmp->xposting(nord);
#else /* CICPP */
                pmfn=posting(next_inverted_prat,nord);
#endif /* CICPP */
            }
#define FMTIFLUSH 0
#if FMTIFLUSH
            if (strcmp(firstdbnp,dbnp)) invflush(dbnp);
#endif
#if KEEPDBXT
            for (idbx=ndbxs-1; idbx>=keepndbx; idbx--)
                dbxflush(vdbxp[idbx]->dbxname);
#endif /* KEEPDBXT */
        }
  return pmfn;
}



/*--------------------------------------------------------------------------*/
/*                   recfmt.c                                               */
/*--------------------------------------------------------------------------*/

#if !CICPP || CIAPI_SOURCE
#if CIAPI_SOURCE
LONGX recfmtcmp(RECSTRU* recp, LONGX lw, char *fmt, char *area, LONGX asize,
                    FMTSTRU **ptpgm, int flagfree)
#else /* CIAPI_SOURCE */
LONGX recfmtcmp (idx,lw,fmt,area,asize,ptpgm,flagfree)
LONGX idx;
LONGX lw;
char *fmt;
char *area;
LONGX asize;
l_code **ptpgm;
int flagfree;
#endif /* CIAPI_SOURCE */
{
        LONGX lines;
        if (!*ptpgm)
#if CIAPI_SOURCE
                if ((*ptpgm)->xfmt_gener(fmt)) {
                        return (-(*ptpgm)->fmt_error);
#else /* CIAPI_SOURCE */
                if (fmt_gener (ptpgm, fmt)) {
                        return (-fmt_error);
#endif /* CIAPI_SOURCE */
                        /* printf ("\n Deu erro de formato"); */
                }
#if PRINT_CODE
/*svd 15-05-92 */ print_inter_din(fmt,*ptpgm);
#endif

#if CIAPI_SOURCE
        lines = (*ptpgm)->xfmt_inter (recp, lw, area, asize);
#else /* CIAPI_SOURCE */
        lines = fmt_inter (*ptpgm, idx, lw, area, asize);
#endif /* CIAPI_SOURCE */
        if (flagfree) {
#if CIAPI_SOURCE
                (*ptpgm)->xfmt_free();
#else /* CIAPI_SOURCE */
                fmt_free (*ptpgm);
                *ptpgm = NULL;
#endif /* CIAPI_SOURCE */
        }
        return (lines);
}

#if CIAPI_SOURCE
LONGX recfmt( RECSTRU* recp, LONGX lw, char *fmt, char *area, LONGX asize)
#else /* CIAPI_SOURCE */
LONGX recfmt (idx,lw,fmt,area,asize)
LONGX idx;
LONGX lw;
char *fmt;
char *area;
LONGX asize;
#endif /* CIAPI_SOURCE */
{
#if CIAPI_SOURCE
			FMTSTRU *lc = NULL;
#else /* CIAPI_SOURCE */
        l_code *lc = NULL;
#endif /* CIAPI_SOURCE */
        LONGX n;
#if TESTFREE
        unsigned LONGX m1,m2,x;
        UCHR  *pantes,*pdepois;

        pantes= (UCHR *)ALLOC((ALLOPARM) sizeof(UCHR));
        cprintf("pantesALL(%Fp)\n",pantes);getch();

        m1 = CORELEFT();
#if CIAPI_SOURCE
        n=recfmtcmp(recp, lw, fmt, area, asize, &lc, 1);
#else /* CIAPI_SOURCE */
        n=recfmtcmp(idx, lw, fmt, area, asize, &lc, 1);
#endif /* CIAPI_SOURCE */

        m2 = CORELEFT();
        x = m1-m2;
        pdepois= (UCHR *)ALLOC((ALLOPARM) sizeof(UCHR));
        cprintf("pdepois ALL(%Fp)\n Diferenca=%lu",pdepois,
         (unsigned)pdepois-(unsigned)pantes);getch();
        FREE(pantes);
        FREE(pdepois);
        if (m1 != m2) {
                clrscr();
        cprintf("\n\r*** coreleft=%lu-%lu=%lu\n\rfmt:\n\r%s", m1, m2, x ,fmt);
        }
#else
#if CIAPI_SOURCE
        n=recfmtcmp(recp, lw, fmt, area, asize, &lc, 1);
#else /* CIAPI_SOURCE */
        n=recfmtcmp(idx, lw, fmt, area, asize, &lc, 1);
#endif /* CIAPI_SOURCE */
#endif
        return (n);
}
#endif /* CICPP || CIAPI_SOURCE */

#if CICPP
FMTSTRU :: FMTSTRU (CISISX *cpx)
{
 cisisxp = cpx;

//---------------------- CIFM1.C ----------------------------------
 fmt_pgmp = NULL;
 separa_nome_base=0;
 string_length=MAX_LITER;
 classe_numero_procurado=0;
 nopn_fmtfiles= -1;
 ch = space_char;
 sub_field_value = 0;
 source_index=0;
 last_source_index=0;
 fmt_error=0;
#if ERRORET
 fmt_errof=0;
#endif
 p_nulo=nulo;
 top_repeat=0;
 last_format=is_not_rep;
 pf_look_ahead= -1;
 pi_look_ahead=0;
 buff_look_ahead=NULL;
 LOOKING_AHEAD=0;
 first_time=1;

//---------------------- CIFM3.C ----------------------------------
 memset(fmterrxy, 0x00, sizeof(fmterrxy));
#if DINALLOC
 fmt_fsiz = MAXMFRL;
#else /* DINALLOC */
 max_tmp_str=MAX_TMP_STR;
 max_fd_value=MAX_FD_VALUE;
 max_fd_tmp=MAX_FD_TMP;
#endif /* DINALLOC */
 modify_fmt_pointers=true;
 no_modify_fmt_pointers=false;

 /*
 tb_espec[0]={"\\r",'\r'};
 tb_espec[1]={"\\t",'\t'};
 tb_espec[2]={"\\b",'\b'};
 tb_espec[3]={"\\n",'\n'};
 tb_espec[4]={"\\0",'\0'}; */

 strcpy(tb_espec[0].s_esp, "\\r");
 tb_espec[0].c_esp = '\r';  /* 'R' */
 strcpy(tb_espec[1].s_esp, "\\t");
 tb_espec[1].c_esp = '\t';  /* 'T' */
 strcpy(tb_espec[2].s_esp, "\\b");
 tb_espec[2].c_esp = '\b';  /* 'B' */
 strcpy(tb_espec[3].s_esp, "\\n");
 tb_espec[3].c_esp = '\n';  /* 'N' */
 strcpy(tb_espec[4].s_esp, "\\0");
 tb_espec[4].c_esp = '\0';  /* '0' */
}

#if 0
FMTSTRU :: FMTSTRU(void): fmt_pgmp(NULL),
#if NACLASSE
                          fmt_error(0), fmt_errof(0) ,
#endif
                          fmt_fsiz(MAXMFRL) {};
#endif // 0

FMTSTRU :: ~FMTSTRU(void)
{
 xfmt_free();
}

#endif /* CICPP */
