#ifndef CIFMT_HPP
#define CIFMT_HPP

#include <cirec.hpp> // RECSTRU
#include <citrm.hpp> // TRMSTRU
#include <cisisx.hpp>
#include <cifmt.h>

class FMTSTRU
{
#if !CIAPI_SOURCE
 protected:
#else /* !CIAPI_SOURCE */
 public:
#endif /* !CIAPI_SOURCE */

//---------------------- CIFM1.C ----------------------------------

#define MAX_NESTED_REPEAT 15
#define res_words_length  8            /*21-07-94*/
#define number_length   30
#define ERRORET         1
#define MXOPNFMTFILES 16
#define CUR_FMT_FILE  opn_fmtfiles[nopn_fmtfiles]
#ifndef MAXPATH
#define MAXPATH CIMPL
#endif
#define length_word_buff 256+50+50+50
#define MAX_STACK_BREAKS 100   /* Numero maximo de breaks permitido */
#if PRINT_CODE
#define inst_name_size 5000
#endif  /* PRINT_CODE */

//---------------------- CIFM2.C ----------------------------------

#define max_numeric_string 12

//---------------------- CIFM3.C ----------------------------------

#define DINALLOC 1 /* p/ alocar areas originalmente estaticas - AOT 29/12/90 */

#define max_stack       MAXSTACK
#define MAX_TMP_STR     (MAXMFRL/2)
#define MAX_SFIELD      (MAXMFRL/2)
#define MAX_FD_VALUE    (MAXMFRL/2)
#define MAX_FD_TMP      (MAXMFRL/2)

#define infoi(i)        i->info.instr
#define ilab(i)         i->info.lab
#define iadd(i)         i->m_add
#define next_infoi(i)   i->next

#define fld_def_ptr(i)  i->m_add
#define str_const_ptr(i) i->m_add

#define PRT_DBNAME 0
#define DEB_NOCC   0

//-----------------------------------------------------------------

//---------------------- CIFM1.C ----------------------------------

  int separa_nome_base;
  unsigned int string_length; /* Valor inicial de tamanho de str */
  int classe_numero_procurado;
  int opn_fmtfiles[MXOPNFMTFILES];
  int nopn_fmtfiles;

  char fmtfile[MAXPATH+1];
  /*fmt_ref_dbname,*/
  typedef char  alfa[res_words_length+1];
  /* Atencao: Os extremos (firstsym e zzzsym)do enumerado
              abaixo NAO  podem ser modificados e nem mudados de posicao.
              Eles sao usados para e criar vetores .
  */
typedef enum symbol_names {
   firstsym,
   csym,     xsym,     dsym,     nsym,     vsym,     ifsym,    fisym,
   thensym,  elsesym,  orsym,    andsym,   notsym,   mfnsym,   mplsym,
   mpusym,   mhlsym,   mhusym,   mdlsym,   mdusym,   fsym,     s_sym,
   getenv_sym, putenv_sym,
   refsym,   asym,     psym,     ravrsym,  valsym,   lsym,     lwsym,
   rupxsym,
   rupdsym,  systsym,  coresym,  maxmfnsym, datemktimesym,
   rsumsym,  rminsym,  rmaxsym,
   fmtsym,   usym,     isym,
   abrcolch, fchcolch,
   comma,    slash,    ponto,
   number_sigx,
   number_sign,percent,  lparen,   rparen,   flecha,   times,    plus,
   minus,    doispontos, eql,    neq,      lss,      leq,      gtr,
   geq,      space,    u_litsym, c_litsym, esc_strsym,number,  nullsym,
   s_fieldsym,r_litsym,
   long_number,float_number,
   noccsym,  npsym,
   ioccsym, continuesym,breaksym,
   sizesym, typesym,
   mstsym, datesym, date1sym, date2sym,
   selsym, casesym, elsecasym, endselsym,
   instrsym,
   leftsym, datexsym,
   rightsym,
   midsym,
   catsym,
   replacsym,
#if CI_XMLELEM
   xmlelemsym,
#endif   
   nl_sym,
   lastsym,
   intvarsym,
   strvarsym,
   whilesym,
   occsym,
   attribsym,
   dbsym,
   ss_sym,
   np2sym,
   citypesym,
   textsym,    /*A12*/
  zzzsym }  symbol;

  instruction_code sym_to_instruction[zzzsym+9];
  char  *pfmt ;     /* aponta para o formato de entrada  */
  char ch;          /* last character read */
  symbol sym;       /* last symbol read */
  symbol field_fmt_sym;
  int num ;         /* value of last integer number read */
  /*f01*/
  LONGX long_num;
  char float_num_str[number_length];
  /*f01*/
  alfa fmttoken ;        /* word being read */
  /*A06*/
  char *str;
  int indvar;  /* Indice de variaveis numericas  vvv */

  /*  char str[string_length];*/  /*mudado para dinamico */
  char sub_field_value;       /* value of last sub_field_read */
  symbol ssym[128];
  int source_index;       /* index of the source string */
  int last_source_index;
  char  *word_names[zzzsym+9];
  int next_word_buff;
  char word_buff[length_word_buff];
  l_code   *head_code ;    /* points to the beginning of the intermediate
                              code. Used only as the linked list head
                           */
  l_code   *last_instr;   /* points to the last instruction generate */
  l_code   *p_nulo;                                   /* AOT 06/04/92 */
  LONGX field_address;
  LONGX address;
  label next_label;    /* stores the last generated label */
  int formatting;       /* if true , the heading commands for formatting
                           a field have already been generated */
  l_code  actual_inst;  /* To be used to store a temporary instruction */
#if PRINT_CODE
  char *inst_cnv[zzzzzz+10];  /*repf*/
                         /* to convert instr_codes from enumerate to string*/
#endif
  int  repeatable;       /* =true means we are processing repeatable format */
  int n_instructions;   /* keep the number of instructions generated */
  /* endereco n_fields; */

  /*  int function_in_process; */
  /*  int if_expr_in_process; */
  /*  int first_function_in_expression; */

  typedef struct repeat_groups{
    int repeatable;
    int last_format;
  }REPEAT_GROUPS;
  REPEAT_GROUPS  vet_repeat[MAX_NESTED_REPEAT];
  int top_repeat;
  /*   int n_ref=0; */ /*  qtade de refs aninhados - not used */
  int last_format;

  l_code *stack_breaks[MAX_STACK_BREAKS];
  int top_stack_breaks ;
  int pf_look_ahead;
  int pi_look_ahead;
  char *buff_look_ahead;
  int LOOKING_AHEAD;

#if PRINT_CODE
  char inst_name[inst_name_size];
  int inst_next;
#endif  /* PRINT_CODE */

  int first_time;

  char *pnome;
  char nome[15];

  l_code *fmt_pgmp;     // format pseudo-code

  field_definition_node *tmp_field;
  l_code *tmp_node;
  char *tmp_str_aloc;
 /*f01*/
  float_x *tmp_float_aloc;
 /*f01*/
  LONGX ret_str;


//---------------------- CIFM2.C ----------------------------------

  char fmtnumb[3*max_str_num+1];
  char tmpform[50];

  char strn[max_numeric_string+1];
  char *ptstr;

//---------------------- CIFM3.C ----------------------------------

  char *nl_STR;  /* Armazena o string para tratar caracteres de newline */
  int nl_LEN;
  int nl_ALLOC;

#if FATRAP
  jmp_buf fmtjumper;
#endif /* FATRAP */
  char fmterrxy[MAXERRL+1];
  int  fmterror;
/*A06*/
  ALLOPARM literal_len;
  char *literal;

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

  LONGX erro_fatal;
  stack_node stack[max_stack];
  int    stack_pt;
  stack_node op1_node,op2_node,top_node;
  stack_node *top,elem,*op1,*op2,*auxpop;

#if DINALLOC
  LONGX fmt_fsiz;           /* fmt_inter() - to set max field length */
  LONGX din_fsiz;           /* fmt_inter() - current max field length */
  char *tmp_str;
  LONGX max_tmp_str;
  LONGX max_fd_value;
  LONGX max_fd_tmp;
#else
  char tmp_str[MAX_TMP_STR+1];
  LONGX max_tmp_str;
  LONGX max_fd_value;
  LONGX max_fd_tmp;
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
  int modify_fmt_pointers;
  int no_modify_fmt_pointers;

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

  struct tbc {
    char s_esp[3];
    int  c_esp;
    /* } tb_espec[]={"\\r",'R',"\\t",'T',"\\b",'B',"\\n",'N',"\\0",'0'};*/
  } tb_espec[5];

  /* rotinas aritmeticas */
  char str_num[30],*pstr_num;
  LONGX ref_mfn_number;
  /* end rotinas aritmeticas */

  /* f function variables*/
  int min_width;
  int dec_places;
  /* end f-function variables */

#if DINALLOC
  char *v_fd_value;
  char *v_fd_tmp;
#else
  char v_fd_value[MAX_FD_VALUE+1];
  char v_fd_tmp[MAX_FD_TMP+1];
#endif

  char *fd_value,*fd_tmp;

  LONGX lineqtt; /* RPIVA/AOT/HB - 16/11/98 */

  /* Continue em grupos repetitivos  17-7-94 */
  int continue_rgroup_maxocc;
  int continue_rgroup;
  char cc;
  int pos;
  int fmt_fim;
  int fmt_n_ref;
  int must_repeat;
  l_code *ni;
  l_code *rpni; /*repf*/
  float_x float_exp_value;
  field_definition_node *itf;
  RECSTRU *recp;
  char *firstdbnp;

  float_x  E_var[NMAXVAR];
  char    *S_var[NMAXVAR];
  
  char *retorno;

//-----------------------------------------------------------------

// Precisa ser em uma unica linha ou o GCC indica erro em mudanca de linha
// de define em arquivo *.hpp
#define new_line(v,i,max) { if (i + nl_LEN < max ){  strcat(v,nl_STR); i=i+nl_LEN; }else {if (fmttrace)printf(" No room for text - truncating");};}

//---------------------- CIFM1.C ----------------------------------

  int cabe_proximo_digito(int x);
  symbol analisa_short_int(char pstr_num[]);
  symbol analisa_numero(char pstr_num[]);
  label fmt_pop_breaks(int pt_stack_breaks);
  void fmt_push_breaks(l_code *idx);
  void fmt_pop_repeat (void);
  void fmt_push_repeat (void);
  char *store_res_word(char *str);
  char *store_inst_names(char *str);
  void get_ch (void);
  void erro (int nerr);
  void parse_fmtfile(void);
  int parse_microisis_dbname(void);  
  void str_read (char end_ch);
  void ignora_espacejamento(void);

  void getsymbol (void);
  void initialize (void);

  int parse_int_number(void);
  int parse_date(void);
  int analisa_mfn (void);
  int parse_datex_function(void);
  int parse_f_function(void);
  int parse_ref_function(instruction_code beg_inst,instruction_code end_inst);
  l_code * fmtel800(label *lb0, label *lb1);
  void fmtel810(int repeatable, label lab0, label lab1);
  void fmtel805(l_code *idx, label lab);
  int verifica_ref_dbname(int cisis_sintaxe);
  char * translate_instruction(instruction_code i);
  void print_inter_din(char *fmt, l_code *head);
  void semantic_initialize(void);
  void semantic_error(char *msg);
  LONGX store_field_def(int tag,char subfield,int ind1,int ind2,int off,int len,int low,int upp);
  int gen(label lab,instruction_code instr,LONGX add,l_code *index);
  label gen_next_label(void);
  void change_pre_cond_by_u_cond(l_code *first,l_code *last);
  int parse_left_right_mid(instruction_code begin,instruction_code end);
  int parse_ss(void);  
  int parse_replace(instruction_code begin,instruction_code end);
#if CI_XMLELEM
  int parse_xmlelem(instruction_code begin,instruction_code end);
#endif
  int function_code_generator(instruction_code begin,instruction_code end);
  int c_fmt_separator(void);
  int field_occs(int *fst,int *last);
  int field_tag(void);
  int subfield_id(char *sbf);    
  int field_id(int *tt,char *sbf);
  int dummy_id (l_code *idx,label lab1);
  int r_prelit (void);
  void suffix(void);
  void field(void);
  void field_substr(int *off,int *len);
  void field_indent(int *ind1,int *ind2);
  int field_fmt (void);
  int mode_parameter (void);
  int c_fmt (void);
  void get_ignore_space (void);
  void rep_isis_fmt (void);
  int parse_instr_function(instruction_code first_instr,instruction_code last_instr);
  int math_factor_function(instruction_code first_instr,instruction_code last_instr);
  int parse_look_np_function(instruction_code first_instr,instruction_code last_instr);
  int factor(void);
  int termo(void);
  int num_expr (void);
  int string_expr (void);
  void relation (void);
  void boolean_factor (void);
  void boolean_term (void);
  void boolean_expr (void);
  int fmt_elem (void);
  int spacing_string (void);
  int separator (void);
  void isis_fmt ();
  void fmt_finaliza_fmt(int pt_break);
  field_definition_node *alloc_field_definition_node (void);
  l_code *alloc_pgm_node (void);
  LONGX store_str_const(char *s);
  LONGX store_float_const(char *s);

//---------------------- CIFM2.C ----------------------------------

  int fmt_CRLF (char *v,int pos);
  void out_put_str(char out[],LONGX maxsize,int *pout,LONGX lw,int id1,int id2,LONGX *ncc,char s[]);
  void upcase_mode(char *s);
  char * sub_field_change(char *q);
  LONGX number_of_lines(char *p);
  char * find_numeric_string(char v[],int *p);
  char * fmt_float_numb (float_x valor,int min,int dec,char leftfill);
  void fmt_chadd(l_code *pgm);

//---------------------- CIFM3.C ----------------------------------

  void sc(char *pp);
  void trace_field(field_definition_node *i,char *p);
  int fmt_type_number(int tipo,char *fmt);
  int fmt_type_pattern(char *patt,char *fmt);
  char * fmt_type(char *p);
  void fmt_init_E_S(void);
  void fmt_free_S(int from,int to);
  char * fmt_copy_realloc(char *pdest,ALLOPARM *dest_mxlen,char *cte_str);
  char * fmt_alloc_char(ALLOPARM tam,char *msg);
  int fmt_instr(char *str_source,char *str_sub);
  void take_numeric_operands_value(class_operand new_class);
  void retrieve_determine_class(void);
  void  retrieve_two_numeric_operands(void);
  void retrieve_two_operands(void);
  int is_numeric(class_operand x);
  float_x convert_to_float(stack_node *top);
  void save_context(int modify_pointers);
  void restore_context(void);
  char * sub_string(char source[],int offset,int length);
  char * store_tmp_string (char *str);
  int cast_to_int (stack_node *e);
  void push (stack_node *e);
  stack_node * pop(void);
  int exec_percent(char *out,int *outs,LONGX *ncc);
  char * trata_md(instruction_code mode,char *s,int *final_added,char **tofreep);
  void sub_field_string(char *q,char sub);
  char * field_value(char *sfieldvalue,LONGX max_sfield,int dd);
  char  * fmt_get_date(int parmdate, LONGX secs0);
  LONGX fmt_cnv_to_long(stack_node *top);
  float_x fmt_cnv_to_float(stack_node *top);
  void fmt_init_vinstr(l_code *ptr_ins,int nnested);
  void fmt_inicio_grupos_vfields(instruction_code instr_inicial,instruction_code instr_final);
  void fmt_fim_grupos_vfields(void);
  int fmt_load_all_occ(void);
  void fmt_load_next_occ(void);
  void fmt_comum_ref_beg(void);
  void fmt_comum_ref_end(void);
  LONGX lookup(char *keyp,TRMSTRU *trmp,char *dbnp,LONGX nord);

//-----------------------------------------------------------------

 public:

  CISISX *cisisxp;

//---------------------- CIFM1.C ----------------------------------

  LONGX fmt_error;
#if ERRORET
  LONGX fmt_errof;
#endif

//---------------------- CIFM1.C ----------------------------------

  LONGX xfmt_gener(char *fmt);
  void xfmt_free(void);

//---------------------- CIFM2.C ----------------------------------

//---------------------- CIFM3.C ----------------------------------
  LONGX xfmt_inter(RECSTRU *parmrecp,LONGX par_lw,char *par_out,LONGX outsize);
  void inter_error(char *err);
  FMTSTRU(CISISX *cpx);
  ~FMTSTRU(void);

//-----------------------------------------------------------------
};

#endif // CIFMT_HPP

