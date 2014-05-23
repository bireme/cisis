/*--------------------------------------------------------------------------*/
/* File:fmt.c                                                               */
/*--------------------------------------------------------------------------*/

#define RPIVA 1         /* to fix Borland 4.5 compilation errors - 12/09/95 */
#define NEST_REP_ALLOWED 0
#define DEB_REF 0
#define DEB_VAR 1
/* Modificacoes:
   A01 - 25/09/93
         Problema:
           Existem algumas situacoes em que o ISIS admite que existam brancos
           entre elementos de formato e a implementacao CISIS nao permite.
           Como exemplo:v20 ( 3,3) -> P/ ISIS correto
                        v20 ( 3,3 )-> P/ ISIS incorreto
           Para corrigir o problema poderia ser feito um levantamento de
           todas as situacoes em que branco deveria ser considerado erro.
           Discutindo com o Adalberto definimos que o CISIS aceitaria os
           dois casos. Assim a implementacao ficou localizada apenas quando
           vai selecionar o proximo symbol. Todos os brancos e tabs sao
           ignorados.
         Efeitos Colaterais:
           Outras situacoes serao aceitas pelo CISIS e nao pelo ISIS.
           Assim nao teremos compatibilidades a nivel de formato.
           Se isso nao for desejavel, a solucao e fazer um levantamento
           mais adequado de quais especificacoes devem ter branco.
           Alem de "espaco" esta sendo ignorado o "tab". Pensei em
           colocar CR LF . Se houver necessidade basta acrescentar no teste
   A02 - 26/09/93
         Problema:
           Existem aplicacoes em em que e' interessante acessar registros
           de outras bases de dados via comando REF.
           Originalmente no ISIS e no CISIS isto nao e' possivel.
         Solucao:
           Discuti com Adalberto e fizemos a seguinte alteracao na  sintaxe:
                   REF(<[nome_da_base]>mfn,...)  onde
           <[nome_da_base]> e' opcional. Quando presente "[" "]" sao
           obrigatorios e nome_da_base e' um string.
         Exemplos:
           REF ( [\cisis\bases\cds]10,v10,v20);
           REF (20,v10,v30);
           No primeiro caso sera executado o formato "v10,v20" para o registro
           10 da base "\cisis\bases\cds" (Provavelmente diferente da base
           default passada como parametro para o interpretador).
           No segundo caso, o formato sera executado para a base default.
         Implementacao:
           Nao precisou fazer alteracoes nas estruturas de dados. O nome
           da base de dados so e necessario na   instrucao READ_MFN.
           Nessa instrucao o campo M_ADD nao tinha funcao nenhuma.
           Entao o nome da base fica armazenado nesse campo, da mesma
           maneira que sao armazenadas as constantes condicionais (U_COND)
           Assim o tratamento e liberacao de area alocada ja e' feito
           pelo interpretador.
         Limitacoes:
           A implementacao do nome da base poderia ser mais generica.
           Dentro da sequencia "[...]" em vez de ter um string poderiamos
           supor que e' um formato.
           A analise , geracao de codigo, e interpretacao seriam mais
           complicadas.( Mas nao impossiveis...).
         Efeitos Colaterais:
           Um formato que contenha a nova sintaxe do REF nao sera
           aceito pelo ISIS.
           Em termos de programas, nao deve haver alteracoes.
         Observacao:
           O Adalberto vai me ajudar (na verdade acho que fazer tudo) a
           implementar a parte de interpretacao. (Nao sei usar direito
           os RECORDS...)
   A03 - 29/09/93
         Problema:
           Nao foi implementado o "[dbname]" para o LOOKUP.
           O Adalberto tinha discutido mas nao fiz na A02.
         Solucao:
           Nao existe uma coerencia entre o REF e L. O primeiro   :
           exige uma expressao numerica , pois corresponde ao mfn.
           O segundo supoe que apos o L vem um formato ISIS que vai
           gerar uma chave.
           Apos a implementacao do "[..]" para REF o Adalberto sugeriu
           que a sintaxe para ambos fosse diferente, algo como;
                        dbname#chave.
           Isto fica correto para  L mas nao para REF uma vez que apos
           o REF e necessario uma expressao numerica  e o dbname nunca seria
           analisado pelo CISIS.
         Implementacao:
           A implementacao ficara igual ao REF. de dados. O nome
         Limitacoes:
         Efeitos Colaterais:
           Um formato que contenha a nova sintaxe do L nao sera
           aceito pelo ISIS.
           Em termos de programas, nao deve haver alteracoes.
         Observacao:
           O Adalberto vai me ajudar (na verdade acho que fazer tudo) a
           implementar a parte de interpretacao. (Nao sei usar direito
           os RECORDS...) (Continua Valido...)
   A04 - 22/11/93
         Problema:
           Implementacao da funcao NP(key).
           Deve retornar o numero de postings da chave.
         Solucao:
           Implementacao igual a do LOOKUP porem em vez de
           carregar na pilha o MFN, carrega o numero de postings
           da chave
  A05 - 25/05/97
        Problema:
           Quando um formato fazia referencia a um arquivo e nesse
           arquivo tivessemos "ifs" ocorria erro.
           Isso acontece porque na implementacao de referencia a arquivo
           a funcao get_ch le o proximo caracter do arquivo, em vez
           de pega-lo no string do formato, e nos casos de "ifs" para
           descobrir que tipo de expressoes (string ou numerica) sao
           referenciadas e' necessario fazer um " look-AHEAD" .
           Depois recomeca a pegar os caracteres a partir do ponto
           de inicio da expressao. Nesse momento nao temos mais
           os caracteres que compoe a expressao.
       Solucao:
           Alterada a get_ch para armazenar os caracteres do arquivo
           em um buffer temporario.

  A06 - 15/09/97
        Problema:
           Permitir que area para analisar constantes do tipo string
           possa ter tamanho dinamico. Atualmente e uma area estatica
           de tamanho 200.
       Solucao:
           Fazer com que a variavel str seja alocada dinamicamente.
           Se durante o uso a area for toda ocupada fazer uma realocacao.
           Muda-se: str e string_length
                    str_read para realocar variavel
  A07 - 05/10/97
       Problema:
         a) Existia erro  em relacao microisis -
            o fmt="...'x',###,%,#,'y',..," e fmt="...'x',###,%,'y',..,"
            produzem o mesmo resultado: muda de linha entre 'x' e 'y'
            Na minha implementacao no segundo caso nao produzia nenhuma linha
	 b) Implementar o comando NEWLINE (<FMT>);
       Solucao:
         a) Na execucao de % verificar se apos retirar "muda-linha" tem
	    #. Em caso afirmativo, ignora essa instrucao.
	 b) Necessario mudar todas as referencias a crlf no programa, o
	    que implica em alterar varias rotinas.

  A08 - 01/11/99
       Problema: Compatibilizar sintaxes Cisis e MicroIsis
                  Vtt[1..]  e  Vtt[1..LAST]
                  Vtt[1]^a  e Vtt^a[1]
                 Permitir o uso das duas formas
       Solucao: Quebrar a rotina field_id para que a analise de subfield seja
                feita separadamente assim permita verificar se existe indexa
                cao antes de "^subfield" ou depois "^subfield"
                Acusara erro se for especificado vtt[1]^a[1]


  A09 - 01/11/99
       Problema: Compatibilizar sintaxes Cisis e MicroIsis
                  mstname e dbname
                 Permitir o uso das duas formas
       Solucao: Criar uma palavra reservada DB que mapeia no mesmo simbolo que
                 que mstname

  A10 - 01/11/99
       Problema:  Implementar funcao SS equivalente a funcao Mid

       Solucao:  Sera criada uma nova funcao, porem a execucao do codigo
                 sera equivalente ao codigo da Mid.

  A11 - 01/11/99
       Problema:  Compatibilizar funcao date

       Solucao:  date(1) = MM-DD-AAAA  HH:MM:SS
                 date(2) = MM-DD_AAAA
                 date(3) = HH:MM:SS
                 date    =
                 date(DATEONLY) = dd/mm/aa
                 date(DATETIME) = dd/mm/aa hh:mm:ss

  A12 - 05/11/99
       Problema: Compatibilizar refencia ao dname com Microisis

       Solucao:

*/
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "cisis.h"
#include "cifmt.h"
/* inicio implementacao [x:y] */
/* como nao podera ser dado intervalo negativo, quando um campo
   nao for explicitado com intervalo o default sera colocado como
   negativo. Na interpretacao sera usado o seguinte criterio:
   Se lower e upper negativos => nao foi indicado intervalo
   Para pegar todas as ocorrencias pegar o ABS desses valores
*/
/* fim  implementacao [x:y] */
/* Conferir...- deveria estar na cisis.h */
#if PC
#if MSC
#define REALLOC realloc
#else /*MSC*/
#define REALLOC farrealloc
#endif
#endif

#define CISIS_REF_DBN   1     /*A12*/
#define MISIS_REF_DBN   0     /*A12*/
#define DEB_ATFILE      0
#define MAX_INT SHRT_MAX                /* +32767 */
#define MIN_INT SHRT_MIN                /* -32768 */
#define TRACE_COMP_F    00              /* printf f_d_n in store_field_def() */
#if !CICPP
#define MAX_NESTED_REPEAT 15
#define res_words_length  8            /*21-07-94*/
#define number_length   30
/*A12*/
int separa_nome_base = 0;
/*A06*/
unsigned int  string_length=MAX_LITER; /* Valor inicial de tamanho de str */
#endif /* CICPP */
/*#define string_length 200*/  /* este parametro deve ser uma variavel */
                            /* inicializada c/ o lw */
                            /* Nao e' possivel, pois o lw so vai ser
                               conhecido em execucao sindo 14/07/97
                            */
#if !CICPP
#define ERRORET         1
#endif /* CICPP */
#define apostrofo      '\''
#define crase          '`'
#define aspa           '\"'
#define exclamacao     '!'
#define atsign         '@'
#define virgula        ','
#define tab            '\t'
/*Existe ambiguidade no uso de numeros pelo isis. Na especificacao de
  extracao de campos V54.5 , 54.5 NAO pode ser interpretado com numero real.
  Assim, precisamos saber na getsymbol se devemos parar quando separamos
  um numero inteiro.
  Quando classe_numero_procurado=PARSEINTNUMB, estamos querendo pegar
  o proximo numero inteiro apenas. Caso contrario, podemos pegar qualquer
  numero
*/
#define PARSEINTNUMB      1
#if !CICPP
static int classe_numero_procurado=0;
#define MXOPNFMTFILES 16
#define CUR_FMT_FILE  opn_fmtfiles[nopn_fmtfiles]
static int opn_fmtfiles[MXOPNFMTFILES];
static int nopn_fmtfiles= -1;
#ifndef MAXPATH
#define MAXPATH CIMPL
#endif
static char fmtfile[MAXPATH+1];
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
#endif /* CICPP */
/*
   Vetor sym_to_instruction
   Usado para converter simbolo em instrucao para facilitar a geracao de
   codigo
*/
#if !CICPP
static instruction_code sym_to_instruction[zzzsym+9];
static char  *pfmt ;     /* aponta para o formato de entrada  */
static char ch = space_char;  /* last character read */
static symbol sym;     /* last symbol read */
static symbol field_fmt_sym;
static int num ;       /* value of last integer number read */
/*f01*/
static LONGX long_num;
static char float_num_str[number_length+1];
/*f01*/
static alfa fmttoken ;        /* word being read */
/*A06*/
static char *str;
static int indvar;  /* Indice de variaveis numericas  vvv */

/* static char str[string_length];*/  /*mudado para dinamico */
static char sub_field_value = 0; /* value of last sub_field_read */
static symbol ssym[128] /* AOT/HB 24/05/2000 - CodeGuard */
= {
zzzsym, zzzsym, zzzsym, zzzsym, zzzsym, zzzsym, zzzsym, zzzsym,
zzzsym, zzzsym, zzzsym, zzzsym, zzzsym, zzzsym, zzzsym, zzzsym,
zzzsym, zzzsym, zzzsym, zzzsym, zzzsym, zzzsym, zzzsym, zzzsym,
zzzsym, zzzsym, zzzsym, zzzsym, zzzsym, zzzsym, zzzsym, zzzsym,
zzzsym, zzzsym, zzzsym, zzzsym, zzzsym, zzzsym, zzzsym, zzzsym,
zzzsym, zzzsym, zzzsym, zzzsym, zzzsym, zzzsym, zzzsym, zzzsym,
zzzsym, zzzsym, zzzsym, zzzsym, zzzsym, zzzsym, zzzsym, zzzsym,
zzzsym, zzzsym, zzzsym, zzzsym, zzzsym, zzzsym, zzzsym, zzzsym,

zzzsym, zzzsym, zzzsym, zzzsym, zzzsym, zzzsym, zzzsym, zzzsym,
zzzsym, zzzsym, zzzsym, zzzsym, zzzsym, zzzsym, zzzsym, zzzsym,
zzzsym, zzzsym, zzzsym, zzzsym, zzzsym, zzzsym, zzzsym, zzzsym,
zzzsym, zzzsym, zzzsym, zzzsym, zzzsym, zzzsym, zzzsym, zzzsym,
zzzsym, zzzsym, zzzsym, zzzsym, zzzsym, zzzsym, zzzsym, zzzsym,
zzzsym, zzzsym, zzzsym, zzzsym, zzzsym, zzzsym, zzzsym, zzzsym,
zzzsym, zzzsym, zzzsym, zzzsym, zzzsym, zzzsym, zzzsym, zzzsym,
zzzsym, zzzsym, zzzsym, zzzsym, zzzsym, zzzsym, zzzsym, zzzsym
}
;
static int source_index=0;       /* index of the source string */
static int last_source_index=0;
LONGX fmt_error=0;
#if ERRORET
LONGX fmt_errof=0;
#endif
#define length_word_buff 256+50+50+50
static char  *word_names[zzzsym+9];
static int next_word_buff;
static char word_buff[length_word_buff];
static l_code   *head_code ;    /* points to the beginning of the intermediate
                            code. Used only as the linked list head
                         */
static l_code   *last_instr;   /* points to the last instruction generate */
static l_code   *p_nulo=nulo;                                   /* AOT 06/04/92 */
static LONGX field_address;
static LONGX address;
static label next_label;    /* stores the last generated label */
static int formatting;       /* if true , the heading commands for formatting
                         a field have already been generated */
static l_code  actual_inst;  /* To be used to store a temporary instruction */
/*char *inst_cnv[100]; */ /* to convert instr_codes from enumerate to string*/
#if PRINT_CODE
 char *inst_cnv[zzzzzz+10];  /*repf*/
                        /* to convert instr_codes from enumerate to string*/
#endif
 static int  repeatable;       /* =true means we are processing repeatable
format */
static int n_instructions;   /* keep the number of instructions generated */
/* endereco n_fields; */

/* static int function_in_process; */
/* static int if_expr_in_process; */
/* static int first_function_in_expression; */
#endif /* CICPP */
/*repf*/
#define  is_not_rep     0       /* Nao esta processando grupo repetitivo  */
#define  is_rep         1       /* Processando grupo repetitivo           */
#define  is_ref         2       /* Processando funco ref                  */
#if !CICPP
typedef struct repeat_groups{
    int repeatable;
    int last_format;
}REPEAT_GROUPS;
static  REPEAT_GROUPS  vet_repeat[MAX_NESTED_REPEAT];
static  int     top_repeat=0;
/* static  int n_ref=0; */ /*  qtade de refs aninhados - not used */
static  int last_format=is_not_rep;
/*---------- break------------------------------------------------------*/
#define MAX_STACK_BREAKS 100   /* Numero maximo de breaks permitido */
static l_code *stack_breaks[MAX_STACK_BREAKS];
static int top_stack_breaks ;
#endif /* CICPP */
/*A05*/
/*-----------------------------------------------------------------------*/
/* Variaveis para resolver problema de IFs em @files                      */
/*-------- --------------------------------------------------------------*/
/* Deve ser dinamico */
#define MAX_buff_look_ahead 160
#if !CICPP
int pf_look_ahead= -1;
int pi_look_ahead=0;
char *buff_look_ahead=NULL;
int LOOKING_AHEAD=0;
#endif /* CICPP */
/*-----------------------------------------------------------------------*/
/* Prototype */
/*-------- --------------------------------------------------------------*/
#if !CICPP
#if ANSI
void fmt_finaliza_fmt(int pt_break);
char  *store_inst_names(char *str);
void  ignora_espacejamento (void);
void parse_fmtfile(void);
int parse_microisis_dbname(void);
int  parse_date(void);
static int cabe_proximo_digito(int x);
static symbol  analisa_numero(char pstr_num[]);
static symbol  analisa_short_int(char pstr_num[]);
static int     analisa_mfn(void);
#else
void fmt_finaliza_fmt();
char  *store_inst_names();
void  ignora_espacejamento();
void parse_fmtfile();
int parse_microisis_dbname();
int pase_date();
static int cabe_proximo_digito();
static symbol  analisa_numero();
static symbol  analisa_short_int();
static int     analisa_mfn();
#endif
#endif /* CICPP */

/*--------------------------------------------------------------------------*/
/*                   cabe_proximo_digito                                   */
/*--------------------------------------------------------------------------*/
/* Verifica se o numero que esta sendo separado do formato de entrada
   tem tamanho dentro do definido
*/
#if CICPP
int FMTSTRU :: cabe_proximo_digito(int x)
#else /* CICPP */
#if ANSI
static int cabe_proximo_digito(int x)
#else /*ANSI*/
static int cabe_proximo_digito(x)
int x;
#endif /*ANSI*/
#endif /* CICPP */
{
 if (x > number_length) erro(6000);
 return x;
}
/*--------------------------------------------------------------------------*/
/*                   analisa_numero                                         */
/*--------------------------------------------------------------------------*/
#if CICPP
FMTSTRU :: symbol  FMTSTRU :: analisa_short_int(char pstr_num[])
#else /* CICPP */
#if ANSI
static symbol  analisa_short_int(char pstr_num[])
#else /*ANSI*/
static symbol  analisa_short_int(pstr_num)
char pstr_num[];
#endif /*ANSI*/
#endif /* CICPP */
{
  symbol symb;
  char keep_ch;
  int keep_source_index;
  int nn;
  keep_ch=ch;
  keep_source_index=source_index;
  nn=0;
  symb=(enum symbol_names)dummy;
  while (isdigit(ch)){
    symb=number;
    pstr_num[cabe_proximo_digito(nn)]=ch;
    nn++;
    get_ch();
  }
  pstr_num[cabe_proximo_digito(nn)]='\0';
  if (symb==dummy) { /* restaura Contexto*/
      ch=keep_ch;
      source_index=keep_source_index;
  }else{
      long_num=atol(pstr_num);
      if (long_num>=(LONGX) MIN_INT && long_num<=(LONGX)MAX_INT){
         num=(int)long_num;
         symb=number;
       }else erro(7000);
   }
  return symb;
}
/*--------------------------------------------------------------------------*/
/*                   analisa_numero                                         */
/*--------------------------------------------------------------------------*/
#if CICPP
FMTSTRU :: symbol  FMTSTRU :: analisa_numero(char pstr_num[])
#else /* CICPP */
#if ANSI
static symbol  analisa_numero(char pstr_num[])
#else /*ANSI*/
static symbol  analisa_numero(pstr_num)
char pstr_num[];
#endif /*ANSI*/
#endif /* CICPP */
{
#define TRUE 1
#define FALSE 0
  symbol symb;
  int tem_num;
  char keep_ch;
  int keep_source_index;
  int nn;
  keep_ch=ch;
  keep_source_index=source_index;
  nn=0;
  symb=(enum symbol_names)dummy;
  while (isdigit(ch)){
        symb=long_number;
        pstr_num[cabe_proximo_digito(nn)]=ch;
        nn++;
        get_ch();
  }
  if (ch=='.') {
      pstr_num[cabe_proximo_digito(nn)]=ch;
      nn++;
      symb=float_number;
      get_ch();
  }
  while(isdigit(ch) ){
      pstr_num[cabe_proximo_digito(nn)]=ch;
      nn++;
      get_ch();
  }
  if ( (ch=='E') || (ch=='e'))  { /* exponencial*/
      pstr_num[cabe_proximo_digito(nn)]=ch;
      nn++;
      symb=float_number;
      get_ch();
      tem_num=FALSE;
      if ((ch=='+')||(ch=='-')) {
        pstr_num[cabe_proximo_digito(nn)]=ch;
        nn++;
        get_ch();
      }
      while (isdigit(ch))  {
        pstr_num[cabe_proximo_digito(nn)]=ch;
        nn++;
        tem_num=TRUE;
        get_ch();
      }
      if(tem_num==FALSE){
       printf("\n Numero invalido");
       exit(1);
      }
  } /* Exponecial*/
    pstr_num[cabe_proximo_digito(nn)]='\0';
    if (symb==dummy) { /* restaura Contexto*/
      ch=keep_ch;
      source_index=keep_source_index;
    }else
       /* Incialmente forca inteiro p/ser long. Depois,se for o caso,
          transforma em int
       */
      if (symb ==long_number){
         long_num=atol(pstr_num);
         if (long_num>=(LONGX) MIN_INT && long_num<=(LONGX)MAX_INT){
            num=(int)long_num;
            symb=number;
         }
       }
  return symb;
}
/*repf*/
/*--------------------------------------------------------------------------*/
/* fmt_pop_breaks                                                           */
/*--------------------------------------------------------------------------*/
#if CICPP
 label FMTSTRU :: fmt_pop_breaks(int pt_stack_breaks)
#else /* CICPP */
#if ANSI
 label fmt_pop_breaks(int pt_stack_breaks)
#else /* ANSI */
 label fmt_pop_breaks(pt_stack_breaks)
 int pt_stack_breaks;
#endif /* ANSI */
#endif /* CICPP */
 { label lab;
   l_code *idx;
   lab= -1;
   if (pt_stack_breaks<=top_stack_breaks){
     lab=gen_next_label();
     for (;pt_stack_breaks<=top_stack_breaks;top_stack_breaks--) {
      idx = stack_breaks[top_stack_breaks];
      /* altera o desvio da  instrucao que foi deixada para tras */
      idx->info.add=lab;
     }
   }
  return lab;
 }

/*--------------------------------------------------------------------------*/
/* fmt_push_breaks                                                            */
/*--------------------------------------------------------------------------*/
#if CICPP
 void FMTSTRU :: fmt_push_breaks(l_code *idx)
#else /* CICPP */
#if ANSI
 void  fmt_push_breaks(l_code *idx)
#else /* ANSI */
 void  fmt_push_breaks(idx)
 l_code *idx;
#endif /* ANSI */
#endif /* CICPP */
 {
   if (top_stack_breaks>=MAX_STACK_BREAKS-1){
     erro(333);
   }
   top_stack_breaks++;
   stack_breaks[top_stack_breaks]=idx;
 }

/*--------------------------------------------------------------------------*/
/* fmt_pop_repeat                                                           */
/*--------------------------------------------------------------------------*/
#if CICPP
void FMTSTRU :: fmt_pop_repeat (void)
#else /* CICPP */
void fmt_pop_repeat ()
#endif /* CICPP */
{
if (top_repeat <= 0) {
   fatal("\n Underflow - vetrepeat ");
  }
  repeatable=vet_repeat[top_repeat].repeatable;
  last_format=vet_repeat[top_repeat].last_format;
  top_repeat--;
}
/*--------------------------------------------------------------------------*/
/* fmt_push_repeat                                                          */
/*--------------------------------------------------------------------------*/
#if CICPP
void FMTSTRU :: fmt_push_repeat (void)
#else /* CICPP */
void fmt_push_repeat ()
#endif /* CICPP */
{
if (top_repeat >= MAX_NESTED_REPEAT) {
   fatal("\n Overflow - vetrepeat ");
  }
 top_repeat++;
 vet_repeat[top_repeat].repeatable=repeatable;
 vet_repeat[top_repeat].last_format=last_format;
}
/*--------------------------------------------------------------------------*/
/* store_res_word                                                           */
/*--------------------------------------------------------------------------*/
#if CICPP
char  * FMTSTRU :: store_res_word(char *str)
#else /* CICPP */
#if ANSI
char  *store_res_word(char *str)
#else /* ANSI */
char  *store_res_word(str)
char *str;
#endif /* ANSI */
#endif /* CICPP */
{
  char *temp;
  int len;
  len=strlen(str);
  if (next_word_buff +len >= length_word_buff )
      fatal("No room to store word_definitions/length_word_buff");
  temp = &word_buff[next_word_buff];
  strcpy(temp,str);
  next_word_buff=next_word_buff + len +1;
  return temp;
}
/*--------------------------------------------------------------------------*/
/* store_inst_names                                                         */
/*--------------------------------------------------------------------------*/
#if PRINT_CODE
#if !CICPP
#define inst_name_size 5000
 static char inst_name[inst_name_size];
 static int inst_next;
#endif /* CICPP */

#if CICPP
char  * FMTSTRU :: store_inst_names(char *str)
#else /* CICPP */
#if ANSI
char  *store_inst_names(char *str)
#else /* ANSI */
char  *store_inst_names(str)
char *str;
#endif /* ANSI */
#endif /* CICPP */
{
  char *temp;
  int len;
  len=strlen(str);
  if (inst_next + len +1 >= inst_name_size )
      fatal("No room to store inst_names/inst_name_size");
  temp = &inst_name[inst_next];
  strcpy(temp,str);
  inst_next=inst_next + len +1;
  return temp;
}
#endif /*PRINT_CODE*/
/*--------------------------------------------------------------------------*/
/*                   get_ch                                                 */
/*--------------------------------------------------------------------------*/
/* Ler o proximo caracter da entrada ou de arquivo
   1-Se a entrada for de arquivo utilizando @file podemos ter 2 casos:
     a) Se no formato apareceu um IF, varios caracteres podem ter sido
        lidos a frente para determinar o tipo de expressao sem, na verdade,
        terem sido consumidos. Neste caso eles estao armazenados no
        buff_look_ahead.
    b) Enquanto nao decide qual tipo de expressao do IF devera ser considerada
       os caracters sao armazenados em buff_look_ahead
    c) LOOKING_AHEAD = 1 . Esta sendo analizada uma expressa em IF e o
       os caracteres devem ser armazenados buf_look_ahead
       LOOKING_AHEAD = 0 . Nao esta sendo analizada nenhuma expressao
  2-Se nao tem referencia a arquivo, pega o proximo caracter do string
*/

#if CICPP
void FMTSTRU :: get_ch (void)
#else /* CICPP */
void get_ch ()
#endif /* CICPP */
{
 /*A05*/
    /* Se tem arquivo aberto , Le entrada de arquivo */
  if (pi_look_ahead<=pf_look_ahead  && LOOKING_AHEAD!=1) {
        ch=buff_look_ahead[pi_look_ahead];
        pi_look_ahead++;
  }else {
    if (!(nopn_fmtfiles > -1)){ /* le do fmt de entrada */
       ch = pfmt[source_index];
       if (ch != null_char )source_index++;
    }else { /*le de arquivo */
       if (CIREAD(CUR_FMT_FILE,&ch,sizeof(ch)) <= 0) {
          CLOSE(CUR_FMT_FILE);
          CUR_FMT_FILE= -1;
          nopn_fmtfiles--;
          ch=0x1A;
       };
   }
 }

  if (ch == 0x1A) {     /* 13-12-96, 22-01-97 - Control Z */
      ch=' ';    /* Troquei pra espaco  26-05-97*/
     /* sym=comma;  tirei porque nao funcionava quando terminava em
                    incondicional. A rotina str-read separava o string
                    e  voltava na getsymbol  com symbolo de coma.
                     26-05-97 */
  }
            /* Se estiver analizando expressoes booleanas guarda
                 o formato */

  if (LOOKING_AHEAD){
     if (pf_look_ahead<MAX_buff_look_ahead) {
        pf_look_ahead++;
        buff_look_ahead[pf_look_ahead]=ch;
     }else  fatal("If expression too long/cifm1/LOOKING_AHEAD" );
  }
} /* get_ch */
/*--------------------------------------------------------------------------*/
/*                   erro                                                   */
/*--------------------------------------------------------------------------*/
#if CICPP
void FMTSTRU :: erro (int nerr)
#else /* CICPP */
#if ANSI
void erro (int nerr)
#else /* ANSI */
void erro (nerr)
int nerr;
#endif /* ANSI */
#endif /* CICPP */
{
/* AOT 02/01/91 */
#if !ERRORET
char *p;
#endif
/* AOT 07/01/91 */
/* fatal("Not enough memory for formatting"); */
/* AOT 02/01/91 */
#if ERRORET
if (!fmt_error) {
    fmt_error=nerr;
    fmt_errof=last_source_index-1;
}
#else
p=s+last_source_index-1;
fprintf(stderr,"%.78s\n^^ %d",p,nerr);
fatal("format error");
#endif
} /* erro */
/*--------------------------------------------------------------------------*/
/*                   parse_fmtfile                                     */
/*--------------------------------------------------------------------------*/
#if CICPP
void FMTSTRU :: parse_fmtfile(void)
#else /* CICPP */
void parse_fmtfile()
#endif /* CICPP */
{ int i;
  int xx;
  i=0;
  get_ch();
  while ( (ch!=virgula) && (ch!=space_char) && (ch!=null_char) &&
          (ch!=tab) && (ch!=lf) && (ch!=cr)     ) {
   fmtfile[i++]=ch;
   get_ch();
   if (i >= MAXPATH) erro(12300);                       /* AOT 28/12/94 */
  }
  fmtfile[i]=null_char;
#if DEB_ATFILE
  printf("\Selecionou file=%s",fmtfile);
#endif
  if (i==0) erro(12301);                                /* AOT 28/12/94 */
#if APPENDEXTENSION                                     /* AOT 28/12/94 */
  if (strstr(fmtfile,".")==NULL)strcat(fmtfile,".pft");
#endif
#if DEB_ATFILE
  printf("\Nome do arquivo Selecionado=%s",fmtfile);
#endif
  /* abre o arquivo e guarda na pilha */
  nopn_fmtfiles++;
  if (nopn_fmtfiles >  MXOPNFMTFILES -1 ) erro(12302);  /* AOT 28/12/94 */
  dbxopt_fatal=0; xx=dbxopen(NULL,fmtfile,NULL);        /* AOT 13/12/96 */
  if (xx < 0) erro(12303);                              /* AOT 28/12/94 */
  opn_fmtfiles[nopn_fmtfiles]=xx;
}
/*--------------------------------------------------------------------------*/
/*                   str_read                                               */
/*--------------------------------------------------------------------------*/
#if CICPP
void FMTSTRU :: str_read (char end_ch)
#else /* CICPP */
#if ANSI
void str_read (char end_ch)
#else /* ANSI */
void str_read (end_ch)
char end_ch;
#endif /* ANSI */
#endif /* CICPP */
{
  int i = 0;
  char *tmp;
  get_ch ();
  while ((i<string_length) && ((ch != end_ch) && (ch != 0)) )
  { str[i]=ch;
    i++;
    get_ch ();
    if (i==string_length) {
      str[string_length]=null_char;
      string_length=string_length+(INCR_LITER) ;
#if CICPP
     /*-
      nao sei qual o comando para realocar em c++ -*/
     try
      { tmp=(char *) new char [string_length+1]; }
        catch (BAD_ALLOC)
        { tmp=(char *) NULL; string_length= 0; /* HB - string_length e' unsigned e nao pode ser -1 */
	      fatal("cifm1/str_read/realloc");
	     /* Nao deixa alocar nada */ }
	strcpy(tmp,str);
        delete [] (char *)str;
	str=tmp;
#else /* CICPP */
 /*      str=(char *) REALLOC (str,(ALLOPARM) sizeof(char)*(string_length+1));*/
         tmp=(char *) ALLOC( (ALLOPARM) sizeof(char)*(string_length+1));

         if (tmp != NULL) {
             strcpy(tmp,str);
             FREE(str);
             str=tmp;
           }else fatal("cifm1/str_read/realloc");
#endif /* CICPP */
   }
  }

  str[i] = null_char;
  if ((ch !=null_char) && (ch != end_ch))
   {while ((ch != end_ch) && (ch !=null_char)) get_ch ();
    if (ch == null_char) erro (1);
   }
  get_ch ();
 } /* str_read */
/*--------------------------------------------------------------------------*/
#if CICPP
void   FMTSTRU :: ignora_espacejamento(void)
#else /* CICPP */
void   ignora_espacejamento()
#endif /* CICPP */
{
  while ( ch==space_char || ch==lf || ch==tab || ch==cr )
         get_ch();
}
 /*A01*/
/*--------------------------------------------------------------------------*/
/*                   getsymbol                                              */
/*--------------------------------------------------------------------------*/
#if CICPP
void FMTSTRU :: getsymbol (void)
#else /* CICPP */
void getsymbol ()
#endif /* CICPP */
{
 int ih;                     /* nowrn (int) - AOT, 22/01/2006 */
 int k;
 symbol ks;
 sym = (enum symbol_names)0;
 num = 0;
 str[0] = null_char;
 fmttoken[0] = null_char;
 /*A01*/
  ignora_espacejamento();
 /*A01*/
 last_source_index=source_index;

#if BEFORE990429
#else
/* Tem dois erros: 1) ssym deveria ter 256 elementos
                   2) memset(ssym,zzzsym,128*sizeof(symbol));
   AOT/HB
*/
 ih=(int)ch;                  /* nowrn (int) - AOT, 22/01/2006 */
 if (ih < 0 || ih > 127)   {  /* nowrn (int) - AOT, 22/01/2006 */   /*if (ch < 0 || ch > 127) */
     erro (3);
     get_ch ();
     return;
 }
#endif

 /*A05*/
 while (ssym[ch] == slash) {
    int achou;
    sym=ssym[ch];
    get_ch();
    if (ssym[ch] != times)  return;
    achou=false;
    /* ignora comentario */
    for (;!achou && ch != null_char ;){
           get_ch();
           if (ssym[ch] == times){
              get_ch();
              if (ssym[ch]==slash){
                achou=true;
                get_ch();
                ignora_espacejamento();
              }
           }
    }/*for*/
    if (!achou) fatal("cifm1/end_comment");
 }

 if (ssym[ch] == doispontos) {
  sym=doispontos;
  get_ch(); /* Verifica o proximo char par a verif atribuicao */
  if (ssym[ch] == eql){
            get_ch(); /*  elimina proximo char */
            sym = attribsym ;
  }
  return;
 }
 /*A12  */
 if (separa_nome_base == 1) {
   k=0;
   while (isalnum(ch)) {
     if (k < number_length) float_num_str[k++]=ch;
     get_ch();
   }
   float_num_str[k]=null_char;
   if (k>0)sym=textsym;
   return;
 }


 if (ch == atsign){
#if DEB_ATFILE
   printf("\n ASchou @");
#endif
   parse_fmtfile();
   ch=','; /* 31-10-96 */
   /* get_ch(); */ /*temporario */
 }

 if (isalpha (ch))
  { k = 0;
    while ( (k < res_words_length) && isalpha (ch))  /*vvv*/
     { fmttoken[k] = toupper (ch);
       k++;
       get_ch ();
     } /* while */
    fmttoken[k] = null_char;

   /* Vefifica se 'e expressao */
    if ( (k==1) && (fmttoken[0]=='E' || fmttoken[0]=='S')) {
        /* supoe que 'e variavel */
        sym = intvarsym;
        if (fmttoken[0] == 'S') sym = strvarsym;
        /* verifica se realmente 'e variavel */
        if (isdigit(ch)){
           /* e' variavel. pega indice */
           indvar =  (int) ch - (int) '0' ;
           get_ch();
           return;
        }
    }

#if RPIVA
#if BEFORE981231
    for (ks = firstsym+1; ks < zzzsym; ks++)
#else
    for (ks = (enum symbol_names)(firstsym+1); ks < zzzsym; ks = (enum symbol_names)((int)ks+1))
#endif /* BEFORE981231 */
        {
        if (word_names[ks])
            if (strcmp(fmttoken,word_names[ks]) == 0) break;
        }
    if (ks < zzzsym) sym = ks;
        else erro (2);
#else
    ks = firstsym+1;
    while (( ks < zzzsym ) && ( strcmp (fmttoken, word_names[ks])) )
        ks++;
    if (! strcmp(fmttoken,word_names[ks])) sym = ks;
     else erro (2);
#endif
    return;
   }
     /* if isalpha (ch) */
     /* tenta separar um numero. Se nao conseguir ,o apontador para
         string sendo analisada nao e' modificado
      */
    /* Dependendo do numero procurado, chama rotina para separar
       int ou long/float
    */
 if (classe_numero_procurado==PARSEINTNUMB)
    sym=analisa_short_int(float_num_str);
  else
    sym=analisa_numero(float_num_str);
 if (sym==number || sym==long_number || sym==float_number){
  return;
 }
/*f01*/
 if (( ch == apostrofo) || ( ch == crase) ){
   sym = u_litsym;
   str_read (ch);
 } /* if ch = aspa or ch = crase */
  else
    if ( ch == aspa ){
      sym = c_litsym;
      str_read (ch);
    } else
       if (ch == exclamacao){
         sym = esc_strsym;
         get_ch();
         str_read (ch);
       }else
         if (ch == less_char){
            get_ch ();
            if (ch == equal_char){
               sym = leq;
               get_ch ();
            } else
               if (ch == greater_char){
                 sym = neq;
                 get_ch ();
                }else sym = lss;
         }else
           if (ch == greater_char){
              get_ch ();
              if (ch == equal_char){
                 sym = geq;
                 get_ch ();
              } else sym = gtr;
           } else
              if (ch == null_char)
                sym = nullsym;
                else
                 if (ch == '|'){
                   sym = r_litsym;
                   str_read(ch);
                  } else
                     if (ch== SFLDCHR){
                        get_ch ();
                        sym = s_fieldsym;
                        sub_field_value = ch;
                        get_ch ();
                     }else {
                       sym = ssym[ch];
#if RPIVA
                       if (sym==zzzsym) erro (3);
#else
                       if (sym==1000) erro (3);
#endif
                         get_ch ();
                     }
} /* getsymbol */
/*--------------------------------------------------------------------------*/
/*                   initialize                                             */
/*--------------------------------------------------------------------------*/
#if CICPP
void FMTSTRU :: initialize (void)
#else /* CICPP */
void initialize ()
#endif /* CICPP */
{
symbol  k ;
source_index = 0;
last_source_index=0;
#if BEFORE981231
for (k=firstsym;k<=zzzsym;k++)word_names[k]=nulo;
#else
for (k=firstsym;k<=zzzsym;k=(enum symbol_names)((int)k+1))word_names[k]=nulo;
#endif /* BEFORE981231 */
ch = space_char;
/*A05*/
#if CICPP
  try
  { buff_look_ahead=(char *) new char [MAX_buff_look_ahead]; }
  catch (BAD_ALLOC)
  { buff_look_ahead=(char *) NULL; }
#else /* CICPP */
  buff_look_ahead=(char *) ALLOC((ALLOPARM)MAX_buff_look_ahead);
#endif /* CICPP */

/* A06 - Alocar area de analise de strings */
#if CICPP
  /*-
    verificar se esta correto -*/
  try
  { str=(char *) new char [string_length+1]; }
  catch (BAD_ALLOC)
  {
    str=(char *) NULL; string_length= 0; /* Nao deixa alocar nada */ /* HB - string_length e' unsigned e nao pode ser -1 */
    fatal("cifm1/initialize/realloc");
  }
#else /* CICPP */
/*Conferir... */
  str=(char *) ALLOC((ALLOPARM)(sizeof(char)*(string_length+1)));
#endif /* CICPP */

word_names[csym] =store_res_word("C");
word_names[xsym] =store_res_word("X");
word_names[dsym] =store_res_word("D");
word_names[nsym] =store_res_word("N");
word_names[vsym] =store_res_word("V");
word_names[ifsym]=store_res_word("IF");
word_names[fisym]=store_res_word("FI");
word_names[thensym]=store_res_word("THEN");
word_names[elsesym]=store_res_word("ELSE");
word_names[orsym]=store_res_word("OR");
word_names[andsym]=store_res_word("AND");
word_names[notsym]=store_res_word("NOT");
word_names[mfnsym]=store_res_word("MFN");
word_names[coresym]=store_res_word("CORE");
word_names[maxmfnsym]=store_res_word("MAXMFN");
word_names[datemktimesym]=store_res_word("SECONDS");
word_names[mplsym]=store_res_word("MPL");
word_names[mpusym]=store_res_word("MPU");
word_names[mhlsym]=store_res_word("MHL");
word_names[mhusym]=store_res_word("MHU");
word_names[mdlsym]=store_res_word("MDL");
word_names[mdusym]=store_res_word("MDU");
word_names[fsym]=store_res_word("F");
word_names[s_sym]=store_res_word("S");
word_names[getenv_sym]=store_res_word("GETENV");
word_names[putenv_sym]=store_res_word("PUTENV");
word_names[refsym]=store_res_word("REF");
word_names[asym]=store_res_word("A");
word_names[psym]=store_res_word("P");
word_names[instrsym]=store_res_word("INSTR");
word_names[leftsym]=store_res_word("LEFT");
word_names[datexsym]=store_res_word("DATEX");
word_names[rightsym]=store_res_word("RIGHT");
word_names[replacsym]=store_res_word("REPLACE");
#if CI_XMLELEM
word_names[xmlelemsym]=store_res_word("XMLELEM");
#endif
word_names[midsym]=store_res_word("MID");
word_names[rupxsym]=store_res_word("PROCX");
word_names[rupdsym]=store_res_word("PROC");
word_names[systsym]=store_res_word("SYSTEM");
word_names[rsumsym]=store_res_word("RSUM");
word_names[rminsym]=store_res_word("RMIN");
word_names[rmaxsym]=store_res_word("RMAX");
word_names[ravrsym]=store_res_word("RAVR");
word_names[valsym]=store_res_word("VAL");
word_names[lsym]=store_res_word("L");
word_names[fmtsym]=store_res_word("FMT");
word_names[usym]=store_res_word("U");
word_names[isym]=store_res_word("I");
word_names[lwsym]=store_res_word("LW");
word_names[noccsym]=store_res_word("NOCC");
word_names[ioccsym]=store_res_word("IOCC");
word_names[continuesym]=store_res_word("CONTINUE");
word_names[breaksym]=store_res_word("BREAK");
word_names[sizesym]=store_res_word("SIZE");
word_names[typesym]=store_res_word("TYPE");
word_names[catsym]=store_res_word("CAT");
word_names[nl_sym]=store_res_word("NEWLINE");
word_names[mstsym]=store_res_word("MSTNAME");
word_names[datesym]=store_res_word("DATE");
word_names[date1sym]=store_res_word("DATETIME");    /*28-12-94*/
word_names[date2sym]=store_res_word("DATEONLY");    /*28-12-94*/
word_names[lastsym]=store_res_word("LAST");
word_names[npsym]=store_res_word("NPOST");    /*21-07-94*/
word_names[np2sym]=store_res_word("NPST");
word_names[selsym]=store_res_word("SELECT");
/* ---> mesmo que select word_names[selsym]=store_res_word("SWITCH"); */
word_names[casesym]=store_res_word("CASE");
word_names[elsecasym]=store_res_word("ELSECASE");
word_names[endselsym]=store_res_word("ENDSEL");
word_names[whilesym]=store_res_word("WHILE");
word_names[occsym]=store_res_word("OCC");
word_names[dbsym]=store_res_word("DB"); /*A09*/
word_names[ss_sym]=store_res_word("SS"); /*A10*/
word_names[citypesym]=store_res_word("CITYPE");

/* ---> mesmo que endsel word_names[endselsym]=store_res_word("ENDSW"); */

#if DEB_ATFILE
for (k=firstsym;k<=zzzsym;k++){
 printf("\n word_names[%d]=%s",k,word_names[k]);
}
#endif
#if RPIVA
/*memset(ssym,zzzsym,128*sizeof(symbol)); AOT/HB - 29/04/99 */
#if BEFORE20000524
for (k=(enum symbol_names)0;k<=(enum symbol_names)127;k=(enum symbol_names)(k + 1)) ssym[k]=zzzsym;
#else
/* initializado na declaração, no braço */
#endif
#else
for (k=0;k<=127;k++) ssym[k]=1000;
#endif
ssym[','] = comma;                     ssym['/'] = slash;
ssym['_'] = number_sigx;
ssym['#'] = number_sign;               ssym['%'] = percent;
ssym['('] = lparen;                    ssym[')'] = rparen;
ssym['^'] = flecha;                    ssym['*'] = times;
ssym['+'] = plus;                      ssym[':'] = doispontos;
ssym['='] = eql;                       ssym[' '] = space;
ssym['-'] = minus;                     ssym['.'] = ponto;
ssym['['] = abrcolch;                  ssym[']'] = fchcolch;
ssym['@'] = (enum symbol_names)atsign;
/* aloca cabeca de lista e inicializa pointer para ultima instrucao */
head_code=alloc_pgm_node ();
head_code->next=(LONGX )p_nulo;
head_code->info.lab=0;
head_code->info.instr=dummy;
head_code->info.add=0;
head_code->m_add=0L;
last_instr=head_code;
get_ch ();
} /* initialize */

/* ----------------------------- cifm1h.c --------------------------- */
/* Esta sendo usado o  simbolo *repf* para identificar as alteracoes
   efetuadas para permitir que possa ser usado um grupo repetitivo
   dentro de um ref que aparece em grupo repetitivo
   15/06/92
*/
#define REF_NEW_CONCEPT 0
/*AOT prototypes -------------------------------------------------------*/
#if !CICPP
#if ANSI
void fmt_push_breaks(l_code *idx);
label fmt_pop_breaks(int pt_stack_breaks);
int  parse_datex_function(void);
int  parse_f_function(void);
int parse_look_np_function(instruction_code first_instr,
                           instruction_code last_instr);
int  parse_ref_function(instruction_code beg_inst,
                        instruction_code end_inst);
int  parse_instr_function(instruction_code beg_inst,
                        instruction_code end_inst);
int  math_factor_function(instruction_code first_instr,
                          instruction_code last_instr);
void  field_indent(int *ind1,int *ind2);
void field_substr (int *off,int *len);
static int parse_int_number(void);
int field_tag(void);
int subfield_id(char *sbf);
int field_occs(int *f,int *l);
void generate_end_function1(void);
l_code *fmtel800(label *lb0,label *lb1);
void  fmtel805(l_code *idx,label lab);
void  fmtel810(int repeatable,label lab0,label lab1);
int verifica_ref_dbname(int cisis_sintaxe);
int parse_ss(void);
int parse_left_right_mid(instruction_code begin,instruction_code end);
int parse_replace(instruction_code begin,instruction_code end);
#if CI_XMLELEM
int parse_xmlelem(instruction_code begin,instruction_code end);
#endif
int function_code_generator(instruction_code begin,instruction_code end);
int c_fmt (void);
int c_fmt_separator(void);
int factor (void);
int field_fmt (void);
int fmt_elem (void);
int mode_parameter (void);
int num_expr (void);
int r_prelit (void);
int separator (void);
int spacing_string (void);
static int string_expr (void);
int termo(void);
label gen_next_label(void);
void boolean_expr (void);
void boolean_factor (void);
void boolean_term (void);
void field (void);
void get_ignore_space (void);
void isis_fmt (void);
static void relation (void);
void rep_isis_fmt (void);
void semantic_initialize(void);
void suffix (void);
#else
void fmt_push_breaks();
label fmt_pop_breaks();
int  parse_datex_function();
int  parse_f_function();
int  parse_ref_function();
int parse_look_np_function();
int  parse_instr_function();
int  math_factor_function();
void  field_indent();
void field_substr ();
static int parse_int_number();
int field_tag();
int subfield_id();
int field_occs();
void generate_end_function1();
void  fmtel805();
l_code *fmtel800();
void  fmtel810();
int verifica_ref_dbname();
int parse_ss();
int parse_left_right_mid();
int parse_replace();
#if CI_XMLELEM
int parse_xmlelem();
#endif
int function_code_generator();
int c_fmt ();
int c_fmt_separator();
int factor ();
int field_fmt ();
int fmt_elem ();
int mode_parameter ();
int num_expr ();
int r_prelit ();
int separator ();
int spacing_string ();
static int string_expr ();
int termo();
label gen_next_label();
void boolean_expr ();
void boolean_factor ();
void boolean_term ();
void field ();
void get_ignore_space ();
void isis_fmt ();
static void relation ();
void rep_isis_fmt ();
void semantic_initialize();
void suffix ();
#endif
#endif /* CICPP */
/*AOT ------------------------------------------------------------------*/
#if !CICPP
int first_time=1;
#endif /* CICPP */
/* -----------------------------------------------------------------------*/
/*                         parse_int_number                               */
/*------------------------------------------------------------------------*/
#if CICPP
int FMTSTRU :: parse_int_number(void)
#else /* CICPP */
static int parse_int_number()
#endif /* CICPP */
{int valor;
 classe_numero_procurado=PARSEINTNUMB;
 getsymbol();
 if(sym != number) erro(8);
 valor=num;
 getsymbol();
 classe_numero_procurado= -PARSEINTNUMB;/* Cuidado com ordem! */
 return valor;
}
/*--------------------------------------------------------------------------*/
/*                   parse_date                                             */
/*--------------------------------------------------------------------------*/
#if CICPP
int FMTSTRU :: parse_date(void)
#else /* CICPP */
int parse_date(void)
#endif /* CICPP */
{ int dateparm,find;
  dateparm=DATEFUNC; /* default */
  get_ignore_space();
  if (sym == lparen) {
    get_ignore_space();
    switch(sym) {
     case date1sym:
          dateparm=DATETIME;
          break;
     case date2sym:
          dateparm=DATEONLY;
          break;
     case number:
          dateparm= num;
          break;
     default :
          erro(1234);
     }
     get_ignore_space();
     if (sym != rparen) erro(1234);
     get_ignore_space();
  }
  find = gen(-1,load_date,dateparm,p_nulo);
  return(find);

}

/* -----------------------------------------------------------------------*/
/*                         Analisa_MFN                                    */
/*------------------------------------------------------------------------*/
#if CICPP
int   FMTSTRU :: analisa_mfn (void)
#else /* CICPP */
static int   analisa_mfn ()
#endif /* CICPP */
{
  int indent;
  get_ignore_space();
  indent=6;
  if (sym == lparen){
        indent=parse_int_number();
        if (sym != rparen) erro(7);
        get_ignore_space();
  }
  return indent;
}

/*--------------------------------------------------------------------------*/
/*                   parse_datex_function                                   */
/*--------------------------------------------------------------------------*/
#if CICPP
int FMTSTRU :: parse_datex_function(void)
#else /* CICPP */
int parse_datex_function(void)
#endif /* CICPP */
{ int find;
  get_ignore_space();
  gen(-1,datex_beg,-1,p_nulo);
  if (sym != lparen) erro (8021);
  get_ignore_space();
  if (!num_expr()) erro (8818);

 get_ignore_space();
 find = gen(-1,datex_end,-1,p_nulo);
 return find;
}
/*-----------------------------------------------------------------------*/
/*                  parse_f_function                                     */
/*-----------------------------------------------------------------------*/
#if CICPP
int FMTSTRU :: parse_f_function(void)
#else /* CICPP */
int parse_f_function()
#endif /* CICPP */
{
   int find;
   find=0;
/*    function_in_process++; */
   get_ignore_space();
   gen(-1,f_beg,-1,p_nulo);
   if (sym != lparen) erro (21);
   get_ignore_space();
   if (!num_expr()) erro (818);
   if (sym != comma){
     if (sym !=rparen) erro(22);
     gen(-1,load_number,-1,p_nulo); /* minimum width */
     find=gen(-1,load_number,-1,p_nulo); /* decimal places */
/*      find=1; */
   }else{
     get_ignore_space();
     if (!num_expr())erro(118);
     if (sym != comma){
       if(sym !=rparen) erro(22);
       find = gen(-1,load_number,-1,p_nulo); /* decimal places */
/*        find=1; */
     }else{
       get_ignore_space();
       if (!num_expr()) erro (218);
         else if (sym != rparen) erro (7);
                else {
/*                find = 1;  */
               }
      }
   }
/*  function_in_process--; */
 get_ignore_space();
 find = gen(-1,f_end,-1,p_nulo);  /* Gerou corretamente */
/* generate_end_function1();*/
 return find;
}
/*-----------------------------------------------------------------------*/
/*                  parse_ref_function                                   */
/*-----------------------------------------------------------------------*/
#if CICPP
int  FMTSTRU :: parse_ref_function(instruction_code beg_inst,
                                    instruction_code end_inst)
#else /* CICPP */
#if ANSI
int  parse_ref_function(instruction_code beg_inst,instruction_code end_inst)
#else /* ANSI */
int  parse_ref_function(beg_inst,end_inst)
instruction_code beg_inst,end_inst;
#endif /* ANSI */
#endif /* CICPP */
{
#if REF_NEW_CONCEPT
  label lab0,lab1;
#endif
  LONGX fmt_existe_dbname;
  int find;
  int sintaxe_dbn;
  find=0;
/*   function_in_process++; */
  fmt_push_repeat();  /*repf*/
#if REF_NEW_CONCEPT
  repeatable=true;  /*repf*/
  last_format=is_ref; /*repf*/
  gen(-1,beg_init_rep,-1,p_nulo); /*repf*/
  lab0=gen_next_label();
  lab1=gen_next_label();
  gen(lab0,begin_rep_gr,-1,p_nulo);
#if !FMT_OPTIMIZE
  gen(-1,set_false_rep,-1,p_nulo);
#endif
#else
#endif
  gen(-1,beg_inst,-1,p_nulo);
  get_ignore_space();
  sintaxe_dbn=parse_microisis_dbname();
/*--------*/

  if (sym !=lparen) erro (21);
    else {
      get_ignore_space();
      /*A02*/
      fmt_existe_dbname=verifica_ref_dbname(sintaxe_dbn);
      /*A02*/
      if (!num_expr()) erro (20);
        else {
          gen(-1,read_mfn,fmt_existe_dbname,p_nulo);/*A02*/
          if (sym != comma) erro (22);
             else {
              getsymbol();
#if REF_NEW_CONCEPT
              fmt_push_repeat();
              repeatable=false;  /*repf*/
              last_format=is_ref; /*repf*/
              gen(-1,beg_init_not_rep,-1,p_nulo);
#else
              repeatable=false;   /*repf*/
              last_format=is_ref; /*repf*/
              gen(-1,beg_init_not_rep,-1,p_nulo); /*repf*/
#endif
              isis_fmt();
              if (sym != rparen) erro (7);
                 else {
                   get_ignore_space();
                   find = 1;
#if REF_NEW_CONCEPT
                   gen(-1,end_init_not_rep,-1,p_nulo); /*repf*/
                   fmt_pop_repeat();
                   gen(-1,end_inst,-1,p_nulo);
                   gen(-1,test_rep,-1,p_nulo);
                   gen(-1,jumpt,lab0,p_nulo);
                   gen(lab1,end_rep_gr,-1,p_nulo); /* ninguem deve apontar
                                                para ele */
                   gen(-1,end_init_rep,-1,p_nulo);/*repf*/
                   fmt_pop_repeat(); /*repf*/
#else
                   gen(-1,end_init_not_rep,-1,p_nulo);/*repf*/
                   gen(-1,end_inst,-1,p_nulo);
                   fmt_pop_repeat(); /*repf*/
#endif
                }
            }
         }
   }
/*     function_in_process--; */
 return find;
}
/*-----------------------------------------------------------------------*/
/*                  fmtel800                                             */
/*-----------------------------------------------------------------------*/
#if CICPP
l_code * FMTSTRU :: fmtel800(label *lb0,
                             label *lb1)
#else /* CICPP */
#if ANSI
l_code *fmtel800(label *lb0,label *lb1)
#else /* ANSI */
l_code *fmtel800(lb0,lb1)
label *lb0;
label *lb1;
#endif /* ANSI */
#endif /* CICPP */
{ label l0,l1;
  l_code *idx;
  /* generate instructions to print and teste Vfields*/
  /* explicar e inicializar completamente */
   formatting=true;
   l0=gen_next_label();
   l1=gen_next_label();
   gen(l0,begin,-1,p_nulo);
   if(repeatable==false) gen(-1,set_false_rep,-1,p_nulo);
   idx=alloc_pgm_node();
   last_instr->next =(LONGX )idx;
   last_instr = alloc_pgm_node( );
   idx->next = (LONGX )last_instr;
   last_instr->next=0L;
   /* 12-03-99 Completa instrucao assumindo que nao aparecera field
               selector e sera ignorada. Caso aparece field selector
               as instrucoes serao alteradas
   */
 idx->info.lab= -1;
 idx->info.instr=ign_cond;
 idx->info.add= -1;
 last_instr->info.lab= -1;
 last_instr->info.instr=ign_cond;
 last_instr->info.add= -1;


   *lb0=l0;
   *lb1=l1;
   return idx;
}
/*-----------------------------------------------------------------------*/
/*             Fmtel810                                             */
/*-----------------------------------------------------------------------*/
#if CICPP
void  FMTSTRU :: fmtel810(int repeatable,
                          label lab0,
                          label lab1)
#else /* CICPP */
#if ANSI
void  fmtel810(int repeatable, label lab0,label lab1)
#else /* ANSI */
void  fmtel810(repeatable,lab0,lab1)
int repeatable;
label lab0;
label lab1;
#endif /* ANSI */
#endif /* CICPP */
{
/* generate instructions to print and teste Vfields*/
if (repeatable==false){
  gen(-1,test_rep,-1,p_nulo);
  gen(-1,jumpt,lab0,p_nulo);
  gen(lab1,end, -1,p_nulo);
 }else {
  gen(lab1,end,-1,p_nulo);
 }
}
/*-----------------------------------------------------------------------*/
/*                  Fmtel805                                             */
/*-----------------------------------------------------------------------*/
#if CICPP
void  FMTSTRU :: fmtel805(l_code *idx,
                          label lab)
#else /* CICPP */
#if ANSI
void  fmtel805(l_code *idx,label lab)
#else /* ANSI */
void  fmtel805(idx,lab)
l_code *idx;
label lab;
#endif /* ANSI */
#endif /* CICPP */
{
 switch (field_fmt_sym)
 { case vsym : gen(-1,print_field,field_address,p_nulo);
               break;
   case isym : gen(-1,input_fld,field_address,p_nulo);
                    break;
   case usym : gen(-1,update_fld,field_address,p_nulo);
                    break;
 }
/* completa instrucoes deixadas para tras */
gen(-1,test_occ,field_address,idx);
idx=(l_code *)idx->next;
gen(-1,jumpf,lab,idx);
}
/*-----------------------------------------------------------------------*/
/*            Macro verif_numeric_relational_op                          */
/*-----------------------------------------------------------------------*/
#define verif_numeric_relational_op                                      \
 (sym==neq || sym==lss || sym==gtr || sym==leq || sym==geq ||            \
  sym==eql)?true:false                                                   \
/*-----------------------------------------------------------------------*/
/*            Macro verif_string_relational_op                           */
/*-----------------------------------------------------------------------*/
#define verif_string_relational_op                                       \
 (sym==doispontos)?true:false                                            \
/*-----------------------------------------------------------------------*/
/*            Macro verif_numeric_string_relational_op                   */
/*-----------------------------------------------------------------------*/
#define verif_numeric_string_relational_op                               \
 (verif_numeric_relational_op==true) ||                                  \
 ( verif_string_relational_op==true)?true:false                          \
 /*A02 */
/*-------------------------------------------------------------------------*/
/*                   verifica_ref_dbname                                   */
/*-------------------------------------------------------------------------*/
#if CICPP
int FMTSTRU :: verifica_ref_dbname(int sintaxe)
#else /* CICPP */
#if ANSI
int verifica_ref_dbname(int sintaxe)
#else /*ANSI*/
int verifica_ref_dbname(sintaxe)
int sintaxe;
#endif /*ANSI*/
#endif /* CICPP */
{ int find;
  /* Pega o proximo symbol para verificar se e especificacao
   de dbn.
  */
  find=0;
  if (sintaxe == MISIS_REF_DBN) return(1); /*A12*/
  if (sym==abrcolch){
     getsymbol();
     gen(-1,dbname_beg,-1,p_nulo);
     isis_fmt();
     if (sym!=fchcolch)erro(5000);
       else {
        gen(-1,dbname_end,-1,p_nulo);
        getsymbol();
        find=1;
     }
  }
  return find ;
}
 /*A02 */
#if PRINT_CODE
/*--------------------------------------------------------------------------*/
/*                  translate_instruction                                  */
/*--------------------------------------------------------------------------*/
#if !CICPP
static char *pnome;
static char nome[15];
#endif /* CICPP */

#if CICPP
char * FMTSTRU :: translate_instruction(instruction_code i)
#else /* CICPP */
#if ANSI
char *translate_instruction(instruction_code i)
#else /*ANSI*/
char *translate_instruction(i)
instruction_code i;
#endif /*ANSI*/
#endif /* CICPP */
{
  int k,j;
  pnome = &nome[0];
  strcpy(pnome,inst_cnv[i]);
  j=strlen(pnome);
  for (k=j;k<15;){nome[k++]=' ';}
  nome[14]='\0';
 return pnome;
 }
/*--------------------------------------------------------------------------*/
/*                  print_inter_din                                        */
/*--------------------------------------------------------------------------*/
#if CICPP
void FMTSTRU :: print_inter_din(char *fmt,
                                l_code *head)
#else /* CICPP */
void print_inter_din(char *fmt,l_code *head)
#endif /* CICPP */
{
  FILE *code;
  int i;
  int x;
  l_code  *t;
  field_definition_node *ff;
  char *string,nome[15];
  LONGX aa;
   i=0;
   if (  (code=fopen("code_din.lst","w")  )==NULL)
    {
      printf ("\n Nao da pra gravar intermediario ");
      exit(0);
     }
  fprintf(code,"\n-------------------------------------------------------");
 /* O formato nao sera normalmente para ser gravado.
   Este if e som para nao dar WARNING!
 */
 if(i!=0)   fprintf(code,"\nfmt=%s",fmt);
  t=(l_code *)head->next; /* deve ser o primeiro no e nao contem informacao */
  i=0;
fprintf(code,"\n");
fprintf(code,"\n End_Abs   |Seq  |Label  | Instrucao     |");
fprintf(code,"Desv/Str/Fld|  Desv_Abs   |   Next_inst         Fld/String");
fprintf(code,"\n ----------+-----+-------+---------------+");
fprintf(code,"------------+-------------+-----------------------------------");
for (;t!=nulo;){
  i++;
  fprintf(code, "\n %"_LD_,(LONGX) t);
  fprintf(code,"|%5.0d",i);
  x=(int)t->info.lab;
  if (x<0){
    fprintf(code,"|  ");
    x=0;
  }else  fprintf(code,"|  ");
  fprintf(code,"%5.0d",x);
  strcpy(nome,translate_instruction(t->info.instr));
  fprintf (code,"| %s",nome /*translate_instruction(t->info.instr)*/ );
  aa=(LONGX )t->info.add;
  if(aa<0) aa=0L;
  fprintf(code,"|%12.0"_LD_,aa);
  aa=(LONGX)t->m_add;
  fprintf(code,"| %12.0"_LD_,aa);
  fprintf(code,"| %12.0"_LD_,(LONGX) t->next);
  if (t->info.instr ==load_field_all || t->info.instr==test_occ ||
      t->info.instr==print_field ||t->info.instr==load_field_occ ||
      t->info.instr==noccins ||
      t->info.instr==s_end /*substr em S */ ){
     ff=(field_definition_node *)t->info.add;
     fprintf (code,
         "     (tag=%d,subf=%c,ind1=%d,ind2=%d,off=%d,len=%d,low=%d,upp=%d)",
                       ff->tag,ff->sub_field,ff->indent1,
                       ff->indent2 ,ff->offset,ff->length,
                       ff->lower,ff->upper);
  }
  if (t->info.instr==u_cond || t->info.instr==pre_cond ||
      t->info.instr==suf_cond ||
          t->info.instr==pre_r_lit || t->info.instr==pre_r_lit_plus ||
          t->info.instr==suf_r_lit || t->info.instr==suf_r_lit_plus ||
          t->info.instr==suf_cond_null || t->info.instr==load_string ||
          t->info.instr==escape_seq || t->info.instr==load_float){
    string =(char *)t->info.add;
    fprintf(code,"        (%s)",string);
  }
  t=(l_code *)t->next;
 }
 fclose(code);
}
#endif
/*--------------------------------------------------------------------------*/
/*                   semantic_initialize                                    */
/*--------------------------------------------------------------------------*/
#if CICPP
void FMTSTRU :: semantic_initialize(void)
#else /* CICPP */
void semantic_initialize()
#endif /* CICPP */
 {
  int i=0;
  n_instructions=0;
  next_label=1;
  formatting=false;
  repeatable=false;
  last_format=is_not_rep;
  top_stack_breaks= -1;

/*  n_repeatable=0;   *repf*/
#if PRINT_CODE
  inst_next=0;
  inst_cnv[dummy]=store_inst_names("Dummy");
  inst_cnv[jumpf]=store_inst_names("Jumpf");
  inst_cnv[jumpt]=store_inst_names("Jumpt");
  inst_cnv[jump]=store_inst_names("Jump");
  inst_cnv[ign_cond]=store_inst_names("Ign_Cond");
  inst_cnv[pre_cond]=store_inst_names("Pre_Cond");
  inst_cnv[pre_r_lit]=store_inst_names("Pre_R_Lit");
  inst_cnv[pre_r_lit_plus]=store_inst_names("Pre_R_Lit_Plus");
  inst_cnv[suf_cond]=store_inst_names("Suf_Cond");
  inst_cnv[suf_cond_null]=store_inst_names("Suf_Cond_Null");
  inst_cnv[suf_r_lit]=store_inst_names("Suf_R_Lit");
  inst_cnv[suf_r_lit_plus]=store_inst_names("Suf_R_Lit_Plus");
  inst_cnv[set_true_rep]=store_inst_names("Set_True_Rep");
  inst_cnv[set_false_rep]=store_inst_names("Set_False_Rep");
  inst_cnv[test_rep]=store_inst_names("Test_Rep");
  inst_cnv[beg_init_not_rep]=store_inst_names("Beg_Init_N_Rep");  /*repf*/
  inst_cnv[end_init_not_rep]=store_inst_names("End_Init_N_Rep");  /*repf*/
  inst_cnv[beg_init_rep]=store_inst_names("Beg_Init_Rep");      /*repf*/
  inst_cnv[end_init_rep]=store_inst_names("End_Init_Rep");      /*repf*/
  inst_cnv[begin_rep_gr]=store_inst_names("Begin_Rep_Gr");
  inst_cnv[end_rep_gr]=store_inst_names("End_Rep_Gr");
  inst_cnv[begin]=store_inst_names("Begin");
  inst_cnv[end]=store_inst_names("End");
  inst_cnv[test_occ]=store_inst_names("Test_Occ");
  inst_cnv[print_field]=store_inst_names("Print_Field");
  inst_cnv[d_dummy]=store_inst_names("D_Dummy");
  inst_cnv[n_dummy]=store_inst_names("N_Dummy");
  inst_cnv[load_field_all]=store_inst_names("Load_Field_All");
  inst_cnv[load_field_occ]=store_inst_names("Load_Field_Occ");
  inst_cnv[load_number]=store_inst_names("Load_Number");
  inst_cnv[load_float]=store_inst_names("Load_Float");
  inst_cnv[load_mfn]=store_inst_names("Load_MFN");
  inst_cnv[load_core]=store_inst_names("Load_CORE");
  inst_cnv[load_maxmfn]=store_inst_names("Load_MAXMFN");
  inst_cnv[date_mktime_beg]=store_inst_names("MkTime_Beg");
  inst_cnv[date_mktime_end]=store_inst_names("MkTime_End");
  inst_cnv[load_string]=store_inst_names("Load_String");
  inst_cnv[val_end]=store_inst_names("Val_End");
  inst_cnv[x_spac]=store_inst_names("X_Spac");
  inst_cnv[c_spac]=store_inst_names("C_Spac");
  inst_cnv[slash_spac]=store_inst_names("Slash_Spac");
  inst_cnv[n_sign_spac]=store_inst_names("N_Sign_Spac");
  inst_cnv[n_sigx_spac]=store_inst_names("n_sigx_spac");
  inst_cnv[percent_spac]=store_inst_names("Percent_Spac");
  inst_cnv[mpl_par]=store_inst_names("Mpl_Par");
  inst_cnv[mpu_par]=store_inst_names("Mpu_Par");
  inst_cnv[mhl_par]=store_inst_names("Mhl_Par");
  inst_cnv[mhu_par]=store_inst_names("Mhu_Par");
  inst_cnv[mdl_par]=store_inst_names("Mdl_Par");
  inst_cnv[mdu_par]=store_inst_names("Mdu_Par");
  inst_cnv[escape_seq]=store_inst_names("Escape_Seq");
  inst_cnv[u_cond]=store_inst_names("U_Cond");
  inst_cnv[print_mfn]=store_inst_names("Print_Mfn");
  inst_cnv[print_core]=store_inst_names("Print_Core");
  inst_cnv[print_maxmfn]=store_inst_names("Print_MaxMfn");
  inst_cnv[absent]=store_inst_names("Absent");
  inst_cnv[present]=store_inst_names("Present");
  inst_cnv[contains]=store_inst_names("Contains");
  inst_cnv[neq_op]=store_inst_names("Neq_Op");
  inst_cnv[lss_op]=store_inst_names("Lss_Op");
  inst_cnv[gtr_op]=store_inst_names("Gtr_Op");
  inst_cnv[leq_op]=store_inst_names("Leq_Op");
  inst_cnv[geq_op]=store_inst_names("Geq_Op");
  inst_cnv[eql_op]=store_inst_names("Eql_Op");
  inst_cnv[or_op]=store_inst_names("Or_Op");
  inst_cnv[and_op]=store_inst_names("And_Op");
  inst_cnv[not_op]=store_inst_names("Not_Op");
  inst_cnv[plus_op]=store_inst_names("Plus_Op");
  inst_cnv[minus_op]=store_inst_names("Minus_Op");
  inst_cnv[times_op]=store_inst_names("Times_Op");
  inst_cnv[divide_op]=store_inst_names("Divide_Op");
  inst_cnv[init_if]=store_inst_names("Init_If");
  inst_cnv[end_then]=store_inst_names("End_Then");
  inst_cnv[fi]=store_inst_names("Fi");
  inst_cnv[beg_init_not_rep]=store_inst_names("Beg_Init_N_Rep");
  inst_cnv[end_init_not_rep]=store_inst_names("End_Init_N_Rep");
  inst_cnv[beg_init_rep]=store_inst_names("Beg_Init_Rep");
  inst_cnv[end_init_rep]=store_inst_names("End_Init_Rep");
  inst_cnv[end_else]=store_inst_names("End_Else");
  inst_cnv[val_beg]=store_inst_names("Val_Beg");
  inst_cnv[rupx_beg]=store_inst_names("Rupx_Beg");
  inst_cnv[rupx_end]=store_inst_names("Rupx_End");
  inst_cnv[rupd_beg]=store_inst_names("Rupd_Beg");
  inst_cnv[rupd_end]=store_inst_names("Rupd_End");
  inst_cnv[syst_beg]=store_inst_names("Syst_Beg");
  inst_cnv[syst_end]=store_inst_names("Syst_End");
  inst_cnv[rsum_beg]=store_inst_names("Rsum_Beg");
  inst_cnv[rsum_end]=store_inst_names("Rsum_End");
  inst_cnv[rmax_beg]=store_inst_names("Rmax_Beg");
  inst_cnv[rmax_end]=store_inst_names("Rmax_End");
  inst_cnv[rmin_beg]=store_inst_names("Rmin_Beg");
  inst_cnv[rmin_end]=store_inst_names("Rmin_End");
  inst_cnv[ravr_beg]=store_inst_names("Ravr_Beg");
  inst_cnv[ravr_end]=store_inst_names("Ravr_End");
  inst_cnv[l_beg]=store_inst_names("L_Beg");
  inst_cnv[l_end]=store_inst_names("L_End");
  inst_cnv[instr_beg]=store_inst_names("Instr_Beg");
  inst_cnv[instr_end]=store_inst_names("Instr_End");
  inst_cnv[left_beg]=store_inst_names("Left_Beg");
  inst_cnv[left_end]=store_inst_names("Left_End");
  inst_cnv[datex_beg]=store_inst_names("Datex_Beg");
  inst_cnv[datex_end]=store_inst_names("Datex_End");
  inst_cnv[right_beg]=store_inst_names("Right_Beg");
  inst_cnv[right_end]=store_inst_names("Right_End");
  inst_cnv[mid_beg]=store_inst_names("Mid_Beg");
  inst_cnv[mid_end]=store_inst_names("Mid_End");
  inst_cnv[s_beg]=store_inst_names("S_Beg");
  inst_cnv[s_end]=store_inst_names("S_End");
  inst_cnv[getenv_beg]=store_inst_names("Getenv_Beg");
  inst_cnv[getenv_end]=store_inst_names("Getenv_End");
  inst_cnv[putenv_beg]=store_inst_names("Putenv_Beg");
  inst_cnv[putenv_end]=store_inst_names("Putenv_End");
  inst_cnv[fmt_end]=store_inst_names("Fmt_End");
  inst_cnv[fmt_beg]=store_inst_names("Fmt_Beg");
  inst_cnv[prt_str_f]=store_inst_names("Prt_Str_F");
/*  inst_cnv[load_str_f]=store_inst_names("Load_Str_F");*/
  inst_cnv[str_cat]=store_inst_names("Str_Cat");
  inst_cnv[ref_beg]=store_inst_names("Ref_Beg");
  inst_cnv[ref_end]=store_inst_names("Ref_End");
  inst_cnv[refu_beg]=store_inst_names("Refu_Beg");
  inst_cnv[refu_end]=store_inst_names("Refu_End");
  inst_cnv[read_mfn]=store_inst_names("Read_Mfn");
  inst_cnv[f_beg]=store_inst_names("F_Beg");
  inst_cnv[f_end]=store_inst_names("F_End");
  inst_cnv[update_fld]=store_inst_names("Update_Fld");
  inst_cnv[input_fld]=store_inst_names("Input_Fld");
  inst_cnv[lw_beg]=store_inst_names("Lw_Beg");
  inst_cnv[lw_end]=store_inst_names("Lw_End");
  inst_cnv[dbname_beg]=store_inst_names("Dbname_Beg");
  inst_cnv[dbname_end]=store_inst_names("Dbname_End");
  inst_cnv[np_beg]=store_inst_names("Npost_Beg");
  inst_cnv[np_end]=store_inst_names("Npost_End");
  inst_cnv[noccins]=store_inst_names("Nocc");
  inst_cnv[ioccins]=store_inst_names("Iocc");
  inst_cnv[continueins]=store_inst_names("Continue");
  inst_cnv[breakins]=store_inst_names("Break");
  inst_cnv[size_beg]=store_inst_names("Size_Beg");
  inst_cnv[size_end]=store_inst_names("Size_End");
  inst_cnv[type_beg]=store_inst_names("Type_Beg");
  inst_cnv[type_end]=store_inst_names("Type_End");
  inst_cnv[cat_beg]=store_inst_names("Cat_Beg");
  inst_cnv[cat_end]=store_inst_names("Cat_End");
  inst_cnv[nl_beg]=store_inst_names("Nl_Beg");
  inst_cnv[nl_end]=store_inst_names("Nl_End");
  inst_cnv[load_mstnam]=store_inst_names("Load_MstNam");
/*   inst_cnv[print_mstnam]=store_inst_names("Print_MstNam"); */
  inst_cnv[load_date]=store_inst_names("Load_Date");
  inst_cnv[duptop]=store_inst_names("Dup");
  inst_cnv[end_select]=store_inst_names("End_Select");

  inst_cnv[replac_beg]=store_inst_names("Replace_Beg");
  inst_cnv[replac_end]=store_inst_names("Replace_End");
#if CI_XMLELEM
  inst_cnv[xmlelem_beg]=store_inst_names("XmlElem_Beg");
  inst_cnv[xmlelem_end]=store_inst_names("XmlElem_End");
#endif
  inst_cnv[while_beg]=store_inst_names("While_Beg");
  inst_cnv[while_end]=store_inst_names("While_End");
  inst_cnv[occins]=store_inst_names("Occ");
  inst_cnv[eattrib_beg]=store_inst_names("Eatrrib_Beg");
  inst_cnv[eattrib_end]=store_inst_names("Eatrrib_End");
  inst_cnv[sattrib_beg]=store_inst_names("Satrrib_Beg");
  inst_cnv[sattrib_end]=store_inst_names("Satrrib_End");
  inst_cnv[intvvalue]=store_inst_names("IntVValue");
  inst_cnv[strvvalue]=store_inst_names("StrVValue");



/*   inst_cnv[print_date]=store_inst_names("Print_Date"); */
  inst_cnv[zzzzzz]=store_inst_names("zzzzzz");
#endif
  /* Inicialmente assume-se que todos os symbolos nao tem instrucao
         correspondente
  */
  for (i=firstsym;i<=zzzsym;i++){sym_to_instruction[i]= (enum ins)-1;}
  sym_to_instruction[csym ]=c_spac;
  sym_to_instruction[xsym ]=x_spac;
  sym_to_instruction[dsym ]=jumpf;
  sym_to_instruction[nsym ]=jumpt;
  sym_to_instruction[vsym ]= (enum ins)-1;
  sym_to_instruction[ifsym]= (enum ins)-1;
  sym_to_instruction[fisym]= (enum ins)-1;
  sym_to_instruction[thensym]= (enum ins)-1;
  sym_to_instruction[elsesym]= (enum ins)-1;
  sym_to_instruction[orsym ]=or_op;
  sym_to_instruction[andsym]=and_op;
  sym_to_instruction[notsym]=not_op;
  sym_to_instruction[mfnsym]=print_mfn;
  sym_to_instruction[coresym]=print_core;
  sym_to_instruction[maxmfnsym]=print_maxmfn;
  sym_to_instruction[datemktimesym]=date_mktime_beg;
  sym_to_instruction[mplsym]=mpl_par;
  sym_to_instruction[mpusym]=mpu_par;
  sym_to_instruction[mhlsym]=mhl_par;
  sym_to_instruction[mhusym]=mhu_par;
  sym_to_instruction[mdusym]=mdu_par;
  sym_to_instruction[mdlsym]=mdl_par;
  sym_to_instruction[comma ]= (enum ins)-1;
  sym_to_instruction[slash ]=divide_op;
  sym_to_instruction[number_sign]=n_sign_spac;
  sym_to_instruction[number_sigx]=n_sigx_spac;
  sym_to_instruction[percent]=percent_spac;
  sym_to_instruction[lparen ]= (enum ins)-1;
  sym_to_instruction[rparen ]= (enum ins)-1;
  sym_to_instruction[flecha ]= (enum ins)-1;
  sym_to_instruction[times]=times_op;
  sym_to_instruction[plus ]=plus_op;
  sym_to_instruction[minus]=minus_op;
  sym_to_instruction[doispontos]=contains;
  sym_to_instruction[eql]=eql_op;
  sym_to_instruction[neq]=neq_op;
  sym_to_instruction[lss]=lss_op;
  sym_to_instruction[leq]=leq_op;
  sym_to_instruction[gtr]=gtr_op;
  sym_to_instruction[geq]=geq_op;
  sym_to_instruction[space   ]= (enum ins)-1;
  sym_to_instruction[u_litsym]=u_cond;
  sym_to_instruction[c_litsym]= (enum ins)-1;
  sym_to_instruction[esc_strsym]=escape_seq;
  sym_to_instruction[number]= (enum ins)-1;
  sym_to_instruction[nullsym]= (enum ins)-1;
  sym_to_instruction[asym]=absent;
  sym_to_instruction[psym]=present;
  sym_to_instruction[s_fieldsym]= (enum ins)-1;
  sym_to_instruction[r_litsym]= (enum ins)-1;
  sym_to_instruction[fsym]=f_beg;
  sym_to_instruction[s_sym]=s_beg;
  sym_to_instruction[instrsym]=instr_beg;
  sym_to_instruction[leftsym]=left_beg;
  sym_to_instruction[datexsym]=datex_beg;
  sym_to_instruction[replacsym]=replac_beg;
#if CI_XMLELEM
  sym_to_instruction[xmlelemsym]=xmlelem_beg;
#endif
  sym_to_instruction[midsym]=mid_beg;
  sym_to_instruction[getenv_sym]=getenv_beg;
  sym_to_instruction[putenv_sym]=putenv_beg;
  sym_to_instruction[refsym]=ref_beg;
  sym_to_instruction[rupxsym]=rupx_beg;
  sym_to_instruction[rupdsym]=rupd_beg;
  sym_to_instruction[systsym]=syst_beg;
  sym_to_instruction[rsumsym]=rsum_beg;
  sym_to_instruction[rminsym]=rmin_beg;
  sym_to_instruction[rmaxsym]=rmax_beg;
  sym_to_instruction[ravrsym]=ravr_beg;
  sym_to_instruction[valsym]=val_beg;
  sym_to_instruction[lsym]=l_beg;
  sym_to_instruction[ponto]= (enum ins)-1;
  sym_to_instruction[fmtsym]=fmt_beg;
  sym_to_instruction[usym]=update_fld;
  sym_to_instruction[isym]=input_fld;
  sym_to_instruction[lwsym]=lw_beg;
  sym_to_instruction[abrcolch]=dbname_beg;
  sym_to_instruction[fchcolch]=dbname_end;
  sym_to_instruction[noccsym]=noccins;
  sym_to_instruction[ioccsym]=ioccins;
  sym_to_instruction[continuesym]=continueins;
  sym_to_instruction[breaksym]=breakins;
  sym_to_instruction[npsym]=np_beg;
  sym_to_instruction[sizesym]=size_beg;
  sym_to_instruction[typesym]=type_beg;
  sym_to_instruction[catsym]=cat_beg;
  sym_to_instruction[nl_sym]=nl_beg;
/*   sym_to_instruction[mstsym]=print_mstnam; */
/*   sym_to_instruction[datesym]=print_date; */
  sym_to_instruction[whilesym]=while_beg;
  sym_to_instruction[occsym]=occins;
  sym_to_instruction[ss_sym]=ss_beg;
  sym_to_instruction[citypesym]=citype_beg;


  sym_to_instruction[zzzsym]=zzzzzz;

}
/*--------------------------------------------------------------------------*/
/*                   semantic_error                                         */
/*--------------------------------------------------------------------------*/
#if CICPP
void FMTSTRU :: semantic_error(char *msg)
#else /* CICPP */
#if ANSI
void semantic_error(char *msg)
#else /*ANSI*/
void semantic_error(msg)
char *msg;
#endif /*ANSI*/
#endif /* CICPP */
{
    if (msg) fprintf(stderr,"*** %s\n",msg);
    fatal("fmt_gener/semantic_error");
}
/*--------------------------------------------------------------------------*/
/*                   store_field_def                                        */
/*--------------------------------------------------------------------------*/
#if CICPP
LONGX FMTSTRU :: store_field_def(int tag,
                                char subfield,
                                int ind1,
                                int ind2,
                                int off,
                                int len,
                                int low,
                                int upp)
#else /* CICPP */
#if ANSI
LONGX store_field_def(int tag,
                     char subfield,
                     int ind1,
                     int ind2,
                     int off,
                     int len,
                     int low,
                     int upp)
#else /*ANSI*/
LONGX store_field_def(tag,subfield,ind1,ind2,off,len,low,upp)
int tag;
char subfield;
int ind1;
int ind2;
int off;
int len;
int low;
int upp;
#endif /*ANSI*/
#endif /* CICPP */
   {
     LONGX ret;
     field_definition_node *tmp;
     tmp = alloc_field_definition_node ();
     tmp->tag=tag;
     tmp->sub_field=subfield;
     tmp->indent1=ind1;
     tmp->indent2=ind2;
     tmp->offset=off;
     tmp->length=len;
     tmp->lower=low;
     tmp->upper=upp;
     tmp->max_occ=0;
     tmp->last_occ=0;
#if TRACE_COMP_F
     if (fmttrace)
printf(
"#%d[tag=%d,subf=%c,id1=%d,id2=%d,off=%d,len=%d,low=%d,upp=%d,max=%d,last=%d\n",
             0,
             tmp->tag,
             tmp->sub_field,
             tmp->indent1,
             tmp->indent2,
             tmp->offset,
             tmp->length,
             tmp->lower,
             tmp->upper,
             tmp->max_occ,
             tmp->last_occ);
#endif /* TRACE_COMP_F */
     ret = (LONGX) tmp;
     return ret;
    }
/*-------------------------------------------------------------------------*/
/*                   gen                                                   */
/*-------------------------------------------------------------------------*/
/*   Gen (lab,inst,add,index);                                             */
/*                                                                         */
/* Gera uma instrucao do codigo intermediario                              */
/* Lab   < 0   instrucao nao tem label                                     */
/* add   < 0   instrucao nao tem operandos                                 */
/* add   > 0 dependendo do codigo da instrucao pode apontar par a :        */
/*                                                                         */
/*           a) uma constante real ou  string                              */
/*           b) os parametros da definicao de um format_fields             */
/*           c) desvio para uma outra instrucao                            */
/*           d) Em vez de apontar, ser o valor de uma constante numerica do*/
/*              do tipo LONGX                                               */
/* index    Endereco onde  deve ser armazenada a instrucao                 */
/*          = p_nulo    proxima posicao livre                              */
/*          otherwise  instrucao armazenada na posicao apontada por index  */
/* Retorna 1 se conseguiu gerar instrucao                                  */
/*-------------------------------------------------------------------------*/
#if CICPP
int FMTSTRU :: gen(label   lab,
                   instruction_code instr,
                   LONGX    add,
                   l_code *index)
#else /* CICPP */
#if ANSI
int gen(label   lab,
        instruction_code instr,
        LONGX    add,
        l_code *index)
#else /*ANSI*/
int gen(lab,instr,add,index)
label lab;
instruction_code instr;
LONGX add;
l_code *index;
#endif /*ANSI*/
#endif /* CICPP */
{
 l_code  *i;
 i=index;
 /* Verifica se e necessario gerar uma nova instrucao ou se uma instrucao
    deve ser complementada
 */
#if FMT_OPTIMIZE
 if (index == nulo && lab== -1 ){
   /* verifica se ainstrucao e uma pseudo instrucao. Neste caso ela podera
      ser desconsiderada e a nova instrucao e colocada em cima
   */
     if ( last_instr->info.instr==end ||
          last_instr->info.instr==begin ||
          last_instr->info.instr==init_if ||
          last_instr->info.instr==end_then ){
      last_instr->info.instr=instr;
      last_instr->info.add=add;
      return 1;
     }
   /* Existem instrucoes que nao precisam ser geradas */
    if (instr==end_else) return 1;
 }
#endif
 if (index == nulo) {
   i=alloc_pgm_node( );
  }
 i->info.lab=lab;
 i->info.instr=instr;
 i->info.add=add;
 /* Se gerar uma nova instrucao a ultima instrucao devera ser atualizada e o
    apontador da ultima devera se alterado
 */
 if(index == nulo)
  { i->next = 0L; /*nulo;*/            /* AOT, 22/01/2006 */
    last_instr->next = (LONGX) i;
    last_instr=i;
  }
 n_instructions++;
 return 1;
}
/*--------------------------------------------------------------------------*/
/*                   gen_next_label                                         */
/*--------------------------------------------------------------------------*/
#if CICPP
label FMTSTRU :: gen_next_label(void)
#else /* CICPP */
label gen_next_label()
#endif /* CICPP */
{
 return next_label++;
}
/* ---------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*                   change_pre_cond_by_u_cond                              */
/*--------------------------------------------------------------------------*/
#if CICPP
void FMTSTRU :: change_pre_cond_by_u_cond(l_code *first,
                                           l_code *last)
#else /* CICPP */
#if ANSI
void change_pre_cond_by_u_cond(l_code *first,
                               l_code *last)
#else /*ANSI*/
void change_pre_cond_by_u_cond(first,last)
l_code *first;
l_code *last;
#endif /*ANSI*/
#endif /* CICPP */
{
 int acabou;
 acabou=false;
for (;acabou==false;)
{
if (first==last) acabou=true;
if (first->info.instr==pre_cond )
 {
  first->info.instr= u_cond;
  }
 first=(l_code *)first->next;
 }
}
/*--------------------------------------------------------------------------*/
/*                   parse_left_right_mid                                       */
/*--------------------------------------------------------------------------*/
#if CICPP
int FMTSTRU :: parse_left_right_mid(instruction_code begin,
                                    instruction_code end)
#else /* CICPP */
#if ANSI
int parse_left_right_mid(instruction_code begin,
                         instruction_code end)
#else /*ANSI*/
int parse_left_right_mid(begin,end)
instruction_code begin;
instruction_code end;
#endif /*ANSI*/
#endif /* CICPP */
{
 int find=0;
 gen(-1,begin,-1,p_nulo);
 get_ignore_space();
 if (sym != lparen ) erro (400);
  get_ignore_space();
  if (!string_expr()) erro(401);
  if (sym != comma) erro (402);
  get_ignore_space();
  if (!num_expr()) erro(403);
  if (begin == mid_beg) {
     if (sym != comma) erro (402);
     get_ignore_space();
     if (!num_expr()) erro(405);
  }
  if (sym != rparen) erro(404);
  get_ignore_space();
  gen(-1,end,-1,p_nulo);
  find=1;
  return find;
}
/*--------------------------------------------------------------------------*/
/*                   parse_ss                                               */
/*--------------------------------------------------------------------------*/
#if CICPP
int FMTSTRU :: parse_ss(void)
#else /* CICPP */
#if ANSI
int parse_ss(void)
#else /*ANSI*/
int parse_ss()
#endif /* ANSI */
#endif /* CICPP */
{
  gen(-1,ss_beg,-1,p_nulo);
  get_ignore_space();
  if (sym != lparen ) erro (500);
  get_ignore_space();
  if (!num_expr()) erro(501);
  if (sym != comma) erro (502);
  get_ignore_space();
  if (!num_expr()) erro(503);
  if (sym != comma) erro (502);
  get_ignore_space();
  if (!string_expr()) erro(505);
  if (sym != rparen) erro(404);
  get_ignore_space();
  gen(-1,ss_end,-1,p_nulo);
  return (1);
}
/*--------------------------------------------------------------------------*/
/*                   parse_replace                                           */
/*--------------------------------------------------------------------------*/
#if CICPP
int FMTSTRU :: parse_replace(instruction_code begin,
                             instruction_code end)
#else /* CICPP */
#if ANSI
int parse_replace(instruction_code begin,
                         instruction_code end)
#else /*ANSI*/
int parse_replace(begin,end)
instruction_code begin;
instruction_code end;
#endif /*ANSI*/
#endif /* CICPP */
{
 int find=0;
 gen(-1,begin,-1,p_nulo);
 get_ignore_space();
 if (sym != lparen ) erro (450);
 get_ignore_space();
 if (!string_expr()) erro(451);
 if (sym != comma) erro (452);
 get_ignore_space();
 if (!string_expr()) erro(453);
 if (sym != comma) erro (454);
 get_ignore_space();
 if (!string_expr()) erro(455);
 if (sym != rparen) erro(456);
 get_ignore_space();
  gen(-1,end,-1,p_nulo);
  find=1;
  return find;
}
#if CI_XMLELEM
/*--------------------------------------------------------------------------*/
/*                   parse_xmlelem                                           */
/*--------------------------------------------------------------------------*/
#if CICPP
int FMTSTRU :: parse_xmlelem(instruction_code begin,
                             instruction_code end)
#else /* CICPP */
#if ANSI
int parse_xmlelem(instruction_code begin,
                         instruction_code end)
#else /*ANSI*/
int parse_xmlelem(begin,end)
instruction_code begin;
instruction_code end;
#endif /*ANSI*/
#endif /* CICPP */
{
 /* sintaxe: XMLELEM(data,elemname,occ,attrname,conventity)     */
 int find=0;

 gen(-1,begin,-1,p_nulo);
 get_ignore_space();

 if (sym != lparen ) {
	 erro (1450);
 } else {
	get_ignore_space();
	if (!string_expr()) {
		erro(1451);
	} else if (sym != comma) {
		erro (1452);
	} else {
		get_ignore_space();
		if (!string_expr()) {
			erro(1453);
		} else if (sym != comma) {
			erro (1454);
		} else {
			get_ignore_space();
			if (!num_expr()) {
				erro(1455);
			} else if (sym != comma) {
				erro (1456);
			} else {
				get_ignore_space();
				if (!string_expr()) {
					erro(1457);
                } else if (sym != comma) {
			        erro (1458);
		        } else {
			        get_ignore_space();
			        if (!num_expr()) {
				        erro(1459);
				    } else if (sym != rparen) {
					    erro(1460);
				    } else {
					    get_ignore_space();
					    gen(-1,end,-1,p_nulo);
					    find=1;
                    }
				}
			}
		}
	}
 }

 /*
 int find=0;
 gen(-1,begin,-1,p_nulo);
 get_ignore_space();
 if (sym != lparen ) erro (1450);
 get_ignore_space();
 if (!string_expr()) erro(1451);
 if (sym != comma) erro (1452);
 get_ignore_space();
 if (!string_expr()) erro(1453);
 if (sym != comma) erro (1454);
 get_ignore_space();
 if (!num_expr()) erro(1455);
 if (sym != comma) erro (1456);
 get_ignore_space();
 if (!string_expr()) erro(1457);
 if (sym != rparen) erro(1458);
 get_ignore_space();
 gen(-1,end,-1,p_nulo);
 find=1;
*/

  return find;
}
#endif /* CI_XMLELEM */
/*--------------------------------------------------------------------------*/
/*                   function_code_generator                                */
/*--------------------------------------------------------------------------*/
#if CICPP
int FMTSTRU :: function_code_generator(instruction_code begin,
                                       instruction_code end)
#else /* CICPP */
#if ANSI
int function_code_generator(instruction_code begin,
                            instruction_code end)
#else /*ANSI*/
int function_code_generator(begin,end)
instruction_code begin;
instruction_code end;
#endif /*ANSI*/
#endif /* CICPP */
{
int  find=0;
/*substr em S */
 int off,len;
 static LONGX field_address;
 /* So precisa para S  */
/*substr em S */
 /* substring na funcao S - daria para colocar em outras desta classe */

gen(-1,begin,-1,p_nulo);
getsymbol();
if(sym!=lparen) erro(54);
 getsymbol( );
 isis_fmt();
 if (sym!=rparen) erro(55);
 classe_numero_procurado=PARSEINTNUMB;
 getsymbol();
 classe_numero_procurado= -PARSEINTNUMB;
 field_address= -1L;
 if (begin==s_beg) {
      field_substr(&off,&len);
      field_address=store_field_def(0,0,0,0,off,len,0,0);
 }
 gen(-1,end,field_address,p_nulo);
 find=1;
 return (find);
}
/*--------------------------------------------------------------------------*/
/*                   c_fmt_separator                                        */
/*--------------------------------------------------------------------------*/
#if CICPP
int FMTSTRU :: c_fmt_separator(void)
#else /* CICPP */
int c_fmt_separator()
#endif /* CICPP */
{
int find=0;
if ( sym==space || sym==comma)
{
  getsymbol( );
  find=1;
}
return find;
}
/*--------------------------------------------------------------------------*/
/*                   field_occs                                             */
/*--------------------------------------------------------------------------*/
#if CICPP
int FMTSTRU :: field_occs(int *fst,
                          int *last)
#else /* CICPP */
#if ANSI
int field_occs(int *fst,
               int *last)
#else /*ANSI*/
int field_occs(fst,last)
int *fst;
int *last;
#endif /*ANSI*/
#endif /* CICPP */
{ int find;

  find=0;
  *fst= NO_MIN_OCC;
  *last= NO_MAX_OCC;

  if (sym ==abrcolch){
     find=1;
     classe_numero_procurado=PARSEINTNUMB;
     get_ignore_space();
     if (sym == lastsym) {
        num=LASTVAL;
        sym=number;
     }
    if(sym != number) erro(8);
    *fst=(int) num;
    *last=(int) num; /* supoe que nao tem limite superior */
     get_ignore_space();
     if (sym == ponto) {
       getsymbol();
       if (sym != ponto) erro(123);
       get_ignore_space();
       if (sym == lastsym){
         num=LASTVAL;
         sym=number;
       }
       if( sym !=number) {
           if (sym == fchcolch) {
                 num=LASTVAL;
                 *last=(int) num;
           }
           else erro(8);
       }
       else {
           *last=(int) num;
           get_ignore_space();
       }
     }
    if (sym != fchcolch) erro(81);
    get_ignore_space();
    classe_numero_procurado= -PARSEINTNUMB;
  }
  return (find);
}
/*-------------------------------------------------------------------------*/
/* A08              field_tag                                           */
/*--------------------------------------------------------------------------*/
#if CICPP
int FMTSTRU :: field_tag(void)
#else /* CICPP */
#if ANSI
int field_tag(void)

#else /*ANSI*/
int field_tag()
#endif /*ANSI*/
#endif /* CICPP */
{

  return(parse_int_number());
}

/*--------------------------------------------------------------------------*/
/* A08               subfield_id                                      */
/*--------------------------------------------------------------------------*/
#if CICPP
int FMTSTRU :: subfield_id(char *sbf)
#else /* CICPP */
#if ANSI
int subfield_id(char *sbf)
#else /*ANSI*/
int subfield_id(sbf)
char *sbf;
#endif /*ANSI*/
#endif /* CICPP */
{
  char subfield;
  subfield=space_char;
  if (sym == s_fieldsym){
     classe_numero_procurado=PARSEINTNUMB; /* 05-01-95 permitir v2^a.6 porque
                                              o getsymbol analisa .6 como float
                                           */
     subfield=sub_field_value;
     getsymbol();
     classe_numero_procurado= -PARSEINTNUMB; /* 05-01-95 permitir v2^a.6 porque
                                              o getsymbol analisa .6 como float
                                             */
  }
  *sbf=subfield;
  return (1);
}
/*--------------------------------------------------------------------------*/
/*                   isis_fmt                                               */
/*--------------------------------------------------------------------------*/
#if CICPP
int FMTSTRU :: dummy_id (l_code *idx,
                         label   lab1)
#else /* CICPP */
#if ANSI
int dummy_id (l_code *idx,
              label   lab1)
#else /*ANSI*/
int dummy_id (idx,lab1)
l_code *idx;
label lab1;
#endif /*ANSI*/
#endif /* CICPP */
{
  instruction_code inst ;
  int tag;
  char subfield;
  int find = 0;
  if ( (sym == dsym) || (sym == nsym)){
      inst=sym_to_instruction[sym];
      tag=field_tag();
      find=subfield_id(&subfield);
      field_address =
        store_field_def(tag,subfield,0,0,-1,-1,NO_MIN_OCC,NO_MAX_OCC);
      /* store instructions behind */
      gen(-1,test_occ,field_address,idx);
      /* The jump instruction depends on dummy field
        if d_dummy the "condition"  is printed if field is present; thus
        a "jumpf"  has to be generated .
       If a n_dummy the "condition is printed if field not present; thus
        a jumpt has to be generated
      */
      idx=(l_code *)idx->next;
      gen(-1,inst,lab1,idx);
      formatting=false;
      /* for Ntag, after the test_occ generated, a jumpt is generated
         meaning     that the conditional fields don't have to be
         printed if present .
         Otherwise the conditional literals have to be printed when
         the field is not present. As pre_cond fields are printed if
         and only if the is presented, the pre_cond generated has  to
         be changed to u_cond fields.
      */
      if(inst==jumpt ) change_pre_cond_by_u_cond(idx,last_instr);
      /* terminates generation */
      gen(lab1,end,-1,p_nulo);
    }
  return find;
} /* dummy_id */
/*--------------------------------------------------------------------------*/
/*                   r_prelit                                               */
/*--------------------------------------------------------------------------*/
#if CICPP
int FMTSTRU :: r_prelit (void)
#else /* CICPP */
int r_prelit ()
#endif /* CICPP */
{
int find = 0;
if (sym == r_litsym){
  address=store_str_const(str);
  actual_inst.info.lab= -1;
  actual_inst.info.instr=pre_r_lit;
  actual_inst.info.add=address;
  getsymbol();
  find = 1;
  if (sym == plus){
         /* change the instruction code */
         actual_inst.info.instr=pre_r_lit_plus;
         getsymbol();
  }
}
/*
 codigo sera gerado por quem chamou
*/
return find;
}/* r_prelit */
/*--------------------------------------------------------------------------*/
/*                   suffix                                                 */
/*--------------------------------------------------------------------------*/
#if CICPP
void FMTSTRU :: suffix (void)
#else /* CICPP */
void suffix ()
#endif /* CICPP */
{
 actual_inst.info.instr=dummy;
 if (sym == plus ){
   actual_inst.info.instr=suf_r_lit_plus;
   getsymbol();
   if (sym != r_litsym) { address=0L; /* AOT/HB - 26/04/99
                                         Sindo: ver se deve inicializar
                                         mais genericamente.
                                         E' necessario para fmt_free nao
                                         fazer delete/free do literal
                                         que nao foi store_str_const'ed.
                                         Teste com formatos (errados) que
                                         terminam por ... V70+
                                      */
                          erro (9);
                        }
          else{
                address=store_str_const(str);
                getsymbol();
          }
  }else
        if (sym == r_litsym){
          address=store_str_const(str);
          actual_inst.info.instr=suf_r_lit;
          getsymbol();
        }
 if (actual_inst.info.instr !=dummy){
   gen(-1,actual_inst.info.instr,address,p_nulo);
 }
 if (sym == c_litsym){
   address=store_str_const(str);
   gen(-1,suf_cond,address,p_nulo);
   /* falta ver se e str null para gera suf_cond_null */
   getsymbol();
  }
}/*  suffix */
/*--------------------------------------------------------------------------*/
/*                   field                                                  */
/*--------------------------------------------------------------------------*/
#if CICPP
void FMTSTRU :: field (void)
#else /* CICPP */
void field ()
#endif /* CICPP */
{
 int tag,off,len;
 int low,upp;
 char subfield;
 int tem_occs;   /*A08 Marca se ja foi analisado indexacao */
 tag=0;
 subfield=' ';
 tag=field_tag();                /*A08*/
 tem_occs = field_occs(&low,&upp);  /*A08*/
 subfield_id(&subfield);      /*A08*/
       /*So permite indexacao se ainda nao apareceu */
 if (tem_occs != 1)  field_occs(&low,&upp);   /*A08 */
 field_substr(&off,&len);
 field_address=store_field_def(tag,subfield,0,0,off,len,low,upp);
} /* field */
/*--------------------------------------------------------------------------*/
/*                   field_substr                                           */
/*--------------------------------------------------------------------------*/
#if CICPP
void FMTSTRU :: field_substr (int *off,
                               int *len)
#else /* CICPP */
#if ANSI
void field_substr (int *off,
                   int *len)
#else /*ANSI*/
void field_substr (off,len)
int *off;
int *len;
#endif /*ANSI*/
#endif /* CICPP */
{
 *off= -1;
 *len= -1;
 if (sym == times || sym == ponto){
    *off=0;
    if (sym == times ){
      *off=parse_int_number();
    }
   *len=SHRT_MAX;
   if (sym == ponto) {
      *len=parse_int_number();
   }
 }
} /* field_substr */
/*--------------------------------------------------------------------------*/
/*                   field_indent                                           */
/*--------------------------------------------------------------------------*/
#if CICPP
void  FMTSTRU :: field_indent(int *ind1,
                              int *ind2)
#else /* CICPP */
#if ANSI
void  field_indent(int *ind1,
                   int *ind2)
#else /*ANSI*/
void  field_indent(ind1,ind2)
int *ind1;
int *ind2;
#endif /*ANSI*/
#endif /* CICPP */
{
  *ind1=0;
  *ind2=0;
   if (sym == lparen){ /* indentation */
      *ind1=parse_int_number();
      if (sym == comma) {
          *ind2=parse_int_number();
      }
      if (sym != rparen) erro (7);
        else  getsymbol();
   } /*lparen */
}
/*--------------------------------------------------------------------------*/
/*                   field_fmt                                              */
/*--------------------------------------------------------------------------*/
#if CICPP
int FMTSTRU :: field_fmt (void)
#else /* CICPP */
int field_fmt ()
#endif /* CICPP */
{
 int find = 0;
 /* local variables */
 int tag,ind1,ind2,off,len,low,upp;
 char subfield;
 int tem_occs;  /*A08*/
 tag=0;
 subfield=' ';
 off= -1;
 len= -1;
 if (sym == vsym || sym == isym || sym ==usym){
   field_fmt_sym=sym;
   tag = field_tag();              /*A08*/
   tem_occs = field_occs(&low,&upp);
   find = subfield_id(&subfield);  /*A08*/
      /*So permite indexacao se ainda nao foi analizada */
   if (tem_occs !=1 ) field_occs(&low,&upp);  /*A08*/
   field_substr(&off,&len);
   field_indent(&ind1,&ind2);
   /* store field specification  in the field table */
   field_address=store_field_def(tag,subfield,ind1,ind2,off,len,low,upp);
 }
 return find;
} /* field_fmt */
/*--------------------------------------------------------------------------*/
/*                   mode_parameter                                         */
/*--------------------------------------------------------------------------*/
#if CICPP
int FMTSTRU :: mode_parameter (void)
#else /* CICPP */
int mode_parameter ()
#endif /* CICPP */
{
int find = 0;
instruction_code t;
switch (sym)
{
 case mplsym:
 case mpusym:
 case mhlsym:
 case mhusym:
 case mdlsym:
 case mdusym:
     t=sym_to_instruction[sym];
     find = 1;
     gen(-1,t,-1,p_nulo);
     getsymbol();
     break;
 }  /* switch */
return find;
} /* mode_parameter */
/*--------------------------------------------------------------------------*/
/*                   c_fmt                                                  */
/*--------------------------------------------------------------------------*/
#if CICPP
int FMTSTRU :: c_fmt (void)
#else /* CICPP */
int c_fmt ()
#endif /* CICPP */
{
int find = 0;
if (sym == esc_strsym){
  find = 1;
  /* acho que nao precisa */
  address=store_str_const(str);
  gen(-1,escape_seq,address,p_nulo);
  getsymbol();
 }
 else if (spacing_string())  find = 1;
        else if (mode_parameter()) find = 1;
return find;
}/* c_fmt */
/*--------------------------------------------------------------------------*/
/*                  get_ignore_space                                        */
/*--------------------------------------------------------------------------*/
#if CICPP
void FMTSTRU :: get_ignore_space (void)
#else /* CICPP */
void get_ignore_space ()
#endif /* CICPP */
{
 getsymbol();
} /* get_ignore_space */
/*--------------------------------------------------------------------------*/
/*                   rep_isis_fmt                                           */
/*--------------------------------------------------------------------------*/
#if CICPP
void FMTSTRU :: rep_isis_fmt (void)
#else /* CICPP */
void rep_isis_fmt ()
#endif /* CICPP */
{
get_ignore_space();
if (sym != lparen ) erro (21);
  else { getsymbol();
         isis_fmt();
         if (sym != rparen) erro (7);
          else { get_ignore_space();
               }
       }
}/* rep_isis_fmt */
/*--------------------------------------------------------------------------*/
/*                   parse_instr_function                                  */
/*--------------------------------------------------------------------------*/
#if CICPP
int FMTSTRU :: parse_instr_function(instruction_code first_instr,
                                     instruction_code last_instr)
#else /* CICPP */
#if ANSI
int parse_instr_function(instruction_code first_instr,
                         instruction_code last_instr)
#else /*ANSI*/
int parse_instr_function(first_instr,last_instr)
instruction_code first_instr;
instruction_code last_instr;
#endif /*ANSI*/
#endif /* CICPP */
{
 gen(-1,first_instr,-1,p_nulo);
 get_ignore_space();
 if (sym != lparen ) erro (21);
  getsymbol();
  string_expr();
  if (sym != comma) erro (7);
  getsymbol();
  string_expr();
  if (sym != rparen) erro(77);
  get_ignore_space();
  gen(-1,last_instr,-1,p_nulo);
  return 1;
}
/*--------------------------------------------------------------------------*/
/*                   math_factor_function                                  */
/*--------------------------------------------------------------------------*/
#if CICPP
int FMTSTRU :: math_factor_function(instruction_code first_instr,
                                    instruction_code last_instr)
#else /* CICPP */
#if ANSI
int math_factor_function(instruction_code first_instr,
                         instruction_code last_instr)
#else /*ANSI*/
int math_factor_function(first_instr,last_instr)
instruction_code first_instr;
instruction_code last_instr;
#endif /*ANSI*/
#endif /* CICPP */
{
/*substr em S */
 int off,len;
 static LONGX field_address;
 /* So precisa para S  */
/*  first_function_in_expression++; */
 gen(-1,first_instr,-1,p_nulo);
 get_ignore_space();
 if (sym != lparen ) erro (21);
   else {
         getsymbol();
         isis_fmt();
         if (sym != rparen) erro (7);
           else {
             /* Se tiver substring tem que aceitar ".digito" como
                especificacao e nao como numero real
             */
             classe_numero_procurado=PARSEINTNUMB;
             get_ignore_space();
             classe_numero_procurado= -PARSEINTNUMB;
         }
   }
/*substr em S */
 /* substring na funcao S - daria para colocar em outras desta classe */
 field_address= -1L;
 if (first_instr==s_beg) {
   field_substr(&off,&len);
   field_address=store_field_def(0,0,0,0,off,len,0,0);
 }
 gen(-1,last_instr,field_address,p_nulo);
/*  first_function_in_expression--; */
 return 1 ; /* se deu algum erro ja capotou antes! */
}/* rep_isis_fmt */
/*-------------------------------------------------------------------------*/
/*                   parse_look_np_function                                */
/*-------------------------------------------------------------------------*/
#if CICPP
int FMTSTRU :: parse_look_np_function(instruction_code first_instr,
                                      instruction_code last_instr)
#else /* CICPP */
#if ANSI
int parse_look_np_function(instruction_code first_instr,
                           instruction_code last_instr)
#else /*ANSI*/
int parse_look_np_function(first_instr,last_instr)
instruction_code first_instr;
instruction_code last_instr;
#endif /*ANSI*/
#endif /* CICPP */

{
 LONGX fmt_existe_dbname;
 int sintaxe_dbn;
/* first_function_in_expression++; */
 gen(-1,first_instr,-1,p_nulo);
 get_ignore_space();
 sintaxe_dbn=parse_microisis_dbname();
/*-------*/
 if (sym != lparen ) erro (21);
   else {
         getsymbol();
         fmt_existe_dbname=verifica_ref_dbname(sintaxe_dbn);
         isis_fmt();
         if (sym != rparen) erro (7);
           else { get_ignore_space();
         }
   }
 gen(-1,last_instr,fmt_existe_dbname,p_nulo);
/*  first_function_in_expression--; */
 return 1 ; /* se deu algum erro ja capotou antes! */
}/* rep_isis_fmt */
/*--------------------------------------------------------------------------*/
/*                   boolean_expr                                           */
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*                   factor                                                 */
/*--------------------------------------------------------------------------*/
#if CICPP
int FMTSTRU :: factor (void)
#else /* CICPP */
int factor ()
#endif /* CICPP */
{
int mfn_indent ;
int find = 0;
int tag;
char subfield;
/* int num_val; */
switch (sym)
{
case number :
         find = 1;
         /*num_val=num;*/
         gen(-1,load_number,(LONGX )num,p_nulo);
         get_ignore_space();
         break;
/* Nas expressoes devem ser aceitos short_int ou int */
/* 06-02-95 */
case long_number :
         find = 1;
         /*num_val=num;*/
         gen(-1,load_number,(LONGX )long_num,p_nulo);
         get_ignore_space();
         break;
case float_number :
         find = 1;
         /*num_val=num;*/
         address=store_float_const(float_num_str);
         gen(-1,load_float,address,p_nulo);
         get_ignore_space();
         break;
case mfnsym :
         find=1;
         mfn_indent=analisa_mfn();
         gen(-1,load_mfn,mfn_indent,p_nulo);
         break;
case coresym :
         find=1;
         gen(-1,load_core,-1L,p_nulo);
         getsymbol();
         break;
case maxmfnsym :
         find=1;
         gen(-1,load_maxmfn,-1L,p_nulo);
         getsymbol();
         break;
case lparen :
         get_ignore_space();
         if (!num_expr()) erro (318);
           else
            if (sym != rparen) erro (7);
              else {
                get_ignore_space();
                find = 1;
              }
         break;
case rsumsym :
          find=math_factor_function(rsum_beg,rsum_end);
          break;
case rmaxsym :
          find=math_factor_function(rmax_beg,rmax_end);
          break;
case rminsym :
          find=math_factor_function(rmin_beg,rmin_end);
          break;
case ravrsym :
          find=math_factor_function(ravr_beg,ravr_end);
          break;
case instrsym :
          find=parse_instr_function(instr_beg,instr_end);
          break;
case valsym :
/*        function_in_process++;*/
          find=math_factor_function(val_beg,val_end);
/*        function_in_process--; */
          break;
case sizesym :
/*        function_in_process++; */
          find=math_factor_function(size_beg,size_end);
/*        function_in_process--;*/
          break;
case datemktimesym:
          find=parse_look_np_function(date_mktime_beg,date_mktime_end);
          break;
case lsym :
          find=parse_look_np_function(l_beg,l_end);
          break;
case npsym :
case np2sym:
          find=parse_look_np_function(np_beg,np_end);
          break;
case noccsym:
          get_ignore_space();
          if (sym != lparen) erro (20);
          get_ignore_space();
          if(sym !=vsym) erro(220);
          tag=field_tag(); /*A08*/
          find=subfield_id(&subfield);
          if (sym != rparen) erro(222);
          field_address =
            store_field_def(tag,subfield,0,0,-1,-1,NO_MIN_OCC,NO_MAX_OCC);
             /* store instructions behind */
          gen(-1,noccins,field_address,p_nulo);
          get_ignore_space();
          break;
case ioccsym:
case occsym:
         find = 1;
         gen(-1,ioccins,-1L,p_nulo);
         get_ignore_space();
         break;
case intvarsym:
         find = 1;
         gen(-1,intvvalue,(LONGX) indvar,p_nulo);
         get_ignore_space();
         break;
case typesym :
        /* para Identificar o tipo de type usado, e guardada como primeiro
           elemento da pilha um valor numerico tal que
           Valor=1 - pattern
           Valor-0 - inteiro
        */

        find=0;
        gen(-1,type_beg,-1,p_nulo);
        getsymbol();
        if (sym != lparen) erro(850);
        getsymbol();
        if (sym == u_litsym) {
          gen(-1,load_number,(LONGX)TYPE_PATTERN,p_nulo);    /*eh pattern*/
          address=store_str_const(str);
          find=gen(-1,load_string,address,p_nulo);
          getsymbol();
        }else {     /* verifica se expressao numerica */
          gen(-1,load_number,(LONGX)TYPE_NUM,p_nulo);    /*eh pattern*/
          if (!num_expr()) erro(853);
        }
        /* parte comum */
        if (sym != comma) erro(851);
        getsymbol();
        isis_fmt();
        if (sym != rparen) erro(852);
        getsymbol();
        gen(-1,type_end,-1,p_nulo);
        find=1;
        break;


}  /* switch */
return find;
}  /* factor */
/*--------------------------------------------------------------------------*/
/*                   termo                                                  */
/*--------------------------------------------------------------------------*/
#if CICPP
int FMTSTRU :: termo (void)
#else /* CICPP */
int termo()
#endif /* CICPP */
{
instruction_code t;
int find = 0;
if (factor())
 { find = 1;
   while ( (sym == times) || (sym == slash) ){
         t=sym_to_instruction[sym];
         get_ignore_space();
         if (!factor()) erro (23);
           else {
                 gen(-1,t,-1,p_nulo);
           }
         }
  }
return find;
} /* termo */
/*--------------------------------------------------------------------------*/
/*                   num_expr                                               */
/*--------------------------------------------------------------------------*/
#if CICPP
int FMTSTRU :: num_expr (void)
#else /* CICPP */
int num_expr ()
#endif /* CICPP */
{
 int find = 0;
 instruction_code t;
 t=dummy;
 if (termo()){
   find = 1;
   while ( (sym == plus) || (sym == minus) ){
         t=sym_to_instruction[sym];
         get_ignore_space();
         if (!termo()) erro (23);
           else {
                 gen(-1,t,-1,p_nulo);
           }
         }
  }
 return find;
} /* num_expr */
/*--------------------------------------------------------------------------*/
/*                   parse_microisis_dbname                                           */
/*--------------------------------------------------------------------------*/
#if CICPP
int FMTSTRU :: parse_microisis_dbname (void)
#else /* CICPP */
int parse_microisis_dbname()
#endif /* CICPP */
{
  int sintaxe_dbn;   /*A12*/

/* A12 Inicio --------*/
/* Analisa  ref( ou ref->   */
  sintaxe_dbn=MISIS_REF_DBN;
  if (sym == lparen) sintaxe_dbn=CISIS_REF_DBN;
  else {
    if (sym != minus ) erro(21);
    getsymbol();
    if (sym != gtr) erro(21);
    separa_nome_base=1;
    getsymbol();
    separa_nome_base=0;
    /* Analise de  A...  ou A...N... ou N... ou N...A.... */
    if (sym == textsym) {
        char *tstr = (char *) float_num_str;
        gen(-1,load_string,store_str_const(tstr),p_nulo);
        getsymbol();
     }else { /* Procura nome de base entre '  ' - Estou supondo
                que pode ser usada  uma funcao que retorna string qualquer
                e nao somente uma constante */
        getsymbol();
        if ( sym != lparen) erro(21);
           else {
             getsymbol();
             if (!string_expr()) erro(21);
             if (sym != rparen) erro(21);
             getsymbol();

           }
      }
   }/*MicroIsis sintaxe */
  return (sintaxe_dbn);

}
/*--------------------------------------------------------------------------*/
/*                   string_expr                                            */
/*--------------------------------------------------------------------------*/
#if CICPP
int FMTSTRU :: string_expr (void)
#else /* CICPP */
static int string_expr ()
#endif /* CICPP */
{
int find = 0;
/* int find_factor = 0; */
/* LONGX fmt_existe_dbname; */
/* do */
/* {  */
/*  find_factor = 0; */
  switch (sym)
  {
    case u_litsym :
/*               find_factor = 1; */
/*               find = 1; */
                 address=store_str_const(str);
                 find=gen(-1,load_string,address,p_nulo);
                 get_ignore_space();
                 break;
    case vsym :
                 field();
                 if (repeatable)
                        find=gen(-1,load_field_occ,field_address,p_nulo);
                   else find=gen(-1,load_field_all,field_address,p_nulo);
                 while (sym == space) getsymbol();
/*               find = 1; */
/*               find_factor = 1; */
                 break;
    case lparen :
                 get_ignore_space();
                 if (!string_expr()) erro (20);
                   else
                     if (sym != rparen) erro (7);
                       else {
                         get_ignore_space();
                         find = 1;
/*                       find_factor = 1; */
                       }
                 break;
    case refsym :
/*           first_function_in_expression++; */
             find=parse_ref_function(refu_beg,refu_end);
/*           find_factor=find; */
/*           first_function_in_expression--; */
             break;
    case s_sym :
/*               function_in_process++;*/
                 find=math_factor_function(s_beg,s_end);
/*               function_in_process--; */
        /*       generate_end_function1(); */
/*               find_factor = 1; */
                 break;
   case leftsym:
        find=parse_left_right_mid(left_beg,left_end);
        break;

   case rightsym:
        find=parse_left_right_mid(right_beg,right_end);
        break;
   case midsym:
        find=parse_left_right_mid(mid_beg,mid_end);
        break;
   case ss_sym:
        find=parse_ss();
        break;
   case replacsym:
        find=parse_replace(replac_beg,replac_end);
        break;
#if CI_XMLELEM
   case xmlelemsym:
        find=parse_xmlelem(xmlelem_beg,xmlelem_end);
        break;
#endif

    case getenv_sym :
/*               function_in_process++;*/
                 find=math_factor_function(getenv_beg,getenv_end);
/*               function_in_process--; */
        /*       generate_end_function1(); */
/*               find_factor = 1; */
                 break;
    case putenv_sym :
/*               function_in_process++;*/
                 find=math_factor_function(putenv_beg,putenv_end);
/*               function_in_process--; */
        /*       generate_end_function1(); */
/*               find_factor = 1; */
                 break;
    case mstsym :
    case dbsym:    /*A09*/
/*       find_factor = 1; */
/*       find = 1; */
         find=gen(-1,load_mstnam,-1,p_nulo);
         get_ignore_space();
         break;

    case datesym :
          /*A11*/
         find = parse_date();
        break;

    case citypesym :
        find=math_factor_function(citype_beg,citype_end);
        break;

    case catsym :
                 find=math_factor_function(cat_beg,cat_end);
                 break;

    case datexsym:
          find=parse_datex_function();
          break;
    case fsym :
/*        first_function_in_expression++; */
          find=parse_f_function();
/*        first_function_in_expression--; */
          break;

    case strvarsym:
          find=1;
          gen (-1,strvvalue,(LONGX) indvar,p_nulo);
          getsymbol();
          break;
  }  /* switch */
/* } while (find_factor); estava errado NAO PODE TER REPETICAO */
return find;
} /* string_expr */
/*--------------------------------------------------------------------------*/
/*                   relation                                               */
/*--------------------------------------------------------------------------*/
#if CICPP
void FMTSTRU :: relation (void)
#else /* CICPP */
static void relation ()
#endif /* CICPP */
{
 instruction_code t;
 t=dummy;
 if (string_expr()){
   if (verif_numeric_string_relational_op==true){
      t=sym_to_instruction[sym];
      get_ignore_space();
   } else { erro(16);}
   if (!string_expr()) erro (17);
   gen(-1,t,-1,p_nulo);
 } else
   if (num_expr()){
     if (verif_numeric_relational_op==true) {
       t=sym_to_instruction[sym];
       get_ignore_space();
     } else {erro(16);}
   if (!num_expr ()) erro (718);
   gen(-1,t,-1,p_nulo);
  }  /* fim de num_expr */
    else {
      if (sym==asym || sym==psym){
        t=sym_to_instruction[sym];
        get_ignore_space();
        if (sym != lparen) erro (20);
        get_ignore_space();
        if (sym != vsym) erro (20);
           else {
             field();
             gen(-1,t,field_address,p_nulo);
             while (sym == space ) getsymbol();
                /*  get_ignore_space();  08/12/93*/
             if(sym != rparen) erro (20);
             get_ignore_space();
           }
            }else erro(19);
    }
} /* relation */
/*--------------------------------------------------------------------------*/
/*                   boolean_factor                                         */
/*--------------------------------------------------------------------------*/
#if CICPP
void FMTSTRU :: boolean_factor (void)
#else /* CICPP */
void boolean_factor ()
#endif /* CICPP */
{
int numerical,stop;

/*
   int keep_source_index,keep_last_source_index;
   char keep_ch;
   symbol keep_sym;
*/
int nparen /*,keep_num*/;
int string_func,numeric_func,relational_op;
instruction_code t;
if (sym == lparen)
 {
  if (first_time==true )
  { first_time=false;
   /* verifica se a expressao e numericca ou string */
   LOOKING_AHEAD=1;
   pi_look_ahead=0;
   pf_look_ahead= -1;

   buff_look_ahead[++pf_look_ahead]='(';
   buff_look_ahead[++pf_look_ahead]=' ';
   buff_look_ahead[++pf_look_ahead]=ch;
/*
   keep_sym=sym;
   keep_source_index=source_index;
   keep_last_source_index=last_source_index;
   keep_num=num;
   keep_ch=ch;
*/
   stop=false;
   numerical=false;
   nparen=1;
   string_func=false;
   numeric_func=false;
   relational_op=false;
   /* Primeiro precisa determinar se o primeiro operando que aparece na
        expressao e' numerico ou string
   */
   /*A05*/
   for (;stop!=true;){
      getsymbol();
      if (sym == rparen )nparen--;
      if (sym == lparen )nparen++;
      if (sym==vsym || sym==u_litsym || sym==refsym || sym==s_sym ||
          sym==getenv_sym || sym==putenv_sym ||
          sym==fsym || sym==citypesym || sym ==mstsym || sym==datesym ||
          sym==replacsym ||
#if CI_XMLELEM
          sym==xmlelemsym ||
#endif
          sym==rightsym || sym==leftsym || sym==midsym || sym==catsym || sym==datexsym ||
          sym==strvarsym || sym==dbsym  || sym==ss_sym
         ) string_func=true;
      if (sym == number || sym == rsumsym || sym==rminsym || sym==valsym ||
          sym == lsym || sym == noccsym  || sym ==npsym || sym==ioccsym ||
          sym == sizesym
          ||  /* 06-02-95 Faltou tratar outros tipos de numericos */
          sym == long_number ||
          sym == float_number ||
          sym == instrsym ||
          sym == np2sym ||
          sym == typesym
         ) numeric_func=true;
      if (sym==nullsym || string_func==true || numeric_func==true)
                stop=true;
   }
 /* Apos determinar qual o tipo de expressao a ser calculada, e' necessario
        verificar se o numero de parenteses ate' encontrar um operador ja foi
        analizado.
        Para expressoes STRING a carta sintatica do ISIS esta correta e sera
        chamada  normalmente. Sao expressoes do tipo "(string ...."
        Para expressoes numericas do tipo "(expr_numer)  = " o primeiro abre
        parentesis e' confundido com o abre-parentesis da boolean-expr e
        devera ser tratado separadamente. Para isso e' necessario que o
        numero de abre-fecha antes de um operador relacional seja 0.
        Essa condicao de erro apresentada pela carta sintatica do ISIS ocorre
        somente para o primeiro abre da expressao. O uso de first_time controla
        essa condicao.
 */
   stop=false;
   for (;stop!=true;){
       getsymbol();
       if (sym == rparen )nparen--;
       if (sym == lparen )nparen++;
       if (sym==eql || sym==neq || sym==lss || sym==gtr || sym==leq ||
           sym==geq || sym==doispontos)relational_op=true;
       if( relational_op == true || sym==nullsym) stop=true;
   }
   /* analise dos resultados */
/*
 ---    if (nparen==0){
         if (numeric_func==true) numerical=true;}
     else numerical=false;
----
*/
  if ( (nparen==0) && (numeric_func==true)) numerical=true;

   /* restaura contexto */

   LOOKING_AHEAD=0;

/*
 ----
    sym=keep_sym;
   last_source_index=keep_last_source_index;
  source_index=keep_source_index;
   num=keep_num;
 ----
*/

  /*  ch=keep_ch; */
   ch=' '; /*forca a pegar o primeiro do buffer */
   getsymbol(); /*para pegar o que foi guardado no buffer */

   if (numerical==true){
      if (!num_expr( ) ) erro(3000);
      if (!(verif_numeric_relational_op==true)) erro(3020);
      t=sym_to_instruction[sym];
      get_ignore_space( );
      if (!num_expr( )) erro(3010);
      gen(-1,t,-1,p_nulo);
   }
   first_time=true;
  }
        if (numerical==false || first_time==false)
          {get_ignore_space();
           boolean_expr();
           if (sym != rparen) erro (7);
                 else {get_ignore_space();}
   }
 }
 else {  t=dummy;
                 if (sym == notsym)
                   { t=not_op;
                         get_ignore_space();
                  }
                relation();
                if(t!=dummy) gen(-1,t,-1,p_nulo);
      }
} /* boolean_factor */
/*--------------------------------------------------------------------------*/
/*                   boolean_term                                           */
/*--------------------------------------------------------------------------*/
#if CICPP
void FMTSTRU :: boolean_term (void)
#else /* CICPP */
void boolean_term ()
#endif /* CICPP */
{
    boolean_factor();
    while (sym == andsym) {
        get_ignore_space ();
        boolean_factor();
        gen(-1,and_op,-1,p_nulo);
    }
} /* boolean_term */
/*--------------------------------------------------------------------------*/
/*                   boolean_expr                                           */
/*--------------------------------------------------------------------------*/
#if CICPP
void FMTSTRU :: boolean_expr (void)
#else /* CICPP */
void boolean_expr ()
#endif /* CICPP */
{
    boolean_term();
    while (sym == orsym) {
        get_ignore_space();
        boolean_term();
        gen(-1,or_op,-1,p_nulo);
    }
} /* boolean_expr */
/*--------------------------------------------------------------------------*/
/*                   fmt_elem                                               */
/*--------------------------------------------------------------------------*/
#if CICPP
int FMTSTRU :: fmt_elem (void)
#else /* CICPP */
int fmt_elem ()
#endif /* CICPP */
{
int find;
l_code  *idx ; /* points to the instruction where must be completed
                   with a "test_occ " and "jump(f/t) instructrion
                */
label lab0,lab1; /* labels necessary for loop all occourrencies */
label lab_break;
int mfn_indent ;
instruction_code begin_instruction,end_instruction,t ;
int pt_stack_breaks; /* aponta para a primeira intrucao break que podera ser
                       gerada. Inicialmente =top-1 para indicar que nao tem
                       nenhuma
                    */

/*A02 */
/* LONGX fmt_existe_dbname; */
/*A02*/
 find = 0;
switch (sym)
{
case u_litsym:
case esc_strsym:
        t=sym_to_instruction[sym];
        address=store_str_const(str);
        gen(-1,t,address,p_nulo);
        find = 1;
        getsymbol();
        break;
case lparen:
        pt_stack_breaks=top_stack_breaks+1;
#if ! NEST_REP_ALLOWED
        if (repeatable==true)
           { semantic_error (" Nested repeatable format not allowed"); }
#endif
         fmt_push_repeat();  /*repf*/
         repeatable=true;   /*repf*/
         last_format=is_rep; /*repf*/
         gen(-1,beg_init_rep,-1,p_nulo); /*repf*/
        /* gera proximos labels */
        lab0=gen_next_label();
        lab1=gen_next_label();
        gen(lab0,begin_rep_gr,-1,p_nulo);
#if !FMT_OPTIMIZE
        gen(-1,set_false_rep,-1,p_nulo);
#endif
        getsymbol();
        isis_fmt();
        if (sym != rparen ) erro (7);
         else {
                /* format fim de repeatable */
                gen(-1,test_rep,-1,p_nulo);
                gen(-1,jumpt,lab0,p_nulo);
                gen(lab1,end_rep_gr,-1,p_nulo); /* ninguem deve apontar
                                                                para ele */
                /* se gerou instrucoes de break complemeta-as para desviarem
                    para o end_init_rep
                */
                lab_break=fmt_pop_breaks(pt_stack_breaks);
                gen(lab_break,end_init_rep,-1,p_nulo);/*repf*/
                fmt_pop_repeat(); /*repf*/
                getsymbol();
                find = 1;
              }
        break;
case mfnsym:
         find=1;
         mfn_indent=analisa_mfn();
         gen(-1,print_mfn,mfn_indent,p_nulo);
         break;
case coresym:
         find=1;
         gen(-1,print_core,-1L,p_nulo);
         getsymbol();
         break;
case maxmfnsym:
         find=1;
         gen(-1,print_maxmfn,-1L,p_nulo);
         getsymbol();
         break;
case whilesym:
        lab0=gen_next_label();
        lab1=gen_next_label();
        get_ignore_space();
        gen(lab0,while_beg,-1,p_nulo);
        boolean_expr();
        gen(-1,jumpf,lab1,p_nulo);
        if (sym != lparen ) erro (14);
        getsymbol();
        isis_fmt();
        gen(-1,jump,lab0,p_nulo);
        gen(lab1,while_end,-1,p_nulo);
        if (sym != rparen ) erro (15);
        getsymbol();
        find = 1;
        break   ;
case ifsym:
/* If_expr_in_process is used to generate different code for the function "s"
   When the "s" function is found inside then if_conditon the result has
   to be put onto the stack
*/
/*         if_expr_in_process=true; */
        gen(-1,init_if,-1,p_nulo);
        lab0=gen_next_label();
        lab1=gen_next_label();
        get_ignore_space();
        boolean_expr();
        gen(-1,jumpf,lab0,p_nulo);
        if (sym != thensym ) erro (14);
         else { /* if_expr_in_process=false; */
                getsymbol();
                isis_fmt();
                gen(-1,jump,lab1,p_nulo);
                gen(lab0,end_then,-1,p_nulo);
                if (sym == elsesym)
                  { getsymbol();
                    isis_fmt();
                    gen(-1,end_else,-1,p_nulo);
                  }
                if (sym != fisym) erro (15);
                 else { find = 1;
                        gen(lab1,fi,-1,p_nulo);
                        getsymbol();
                      }
                  }
        break   ;
case selsym:
      {
        int num_type,str_type;
        getsymbol();
        num_type=true;
        str_type=false;
        if (!num_expr()) {
           num_type=false;
           str_type=true;
           if (!string_expr())   fatal(" cifm1/select/expr");
        }
        lab0=gen_next_label();
        lab1=gen_next_label();
        if (sym != casesym) fatal("cifm1/select/case");
        while (sym == casesym) {
          getsymbol();
          gen(lab1,duptop,-1,p_nulo);
          lab1=gen_next_label();
          if (num_type) {
            if (!(num_expr()) )  fatal("cifm1/select/number/string");
          } else {
            if (str_type) {
              if (!(string_expr()) ) fatal("cifm1/select/number/string");
            }
          }
          if (sym != doispontos) fatal("cifm1/select/doispontos");
          gen(-1,eql_op,-1,p_nulo);
          gen(-1,jumpf,lab1,p_nulo);
          getsymbol();
          isis_fmt();
          gen(-1,jump,lab0,p_nulo);
        }
         gen(lab1,dummy,-1,p_nulo);
         if (sym == elsecasym) {
           getsymbol();
           isis_fmt();

         }
         if (sym != endselsym) fatal("cifm1/select/endselect");
         getsymbol();
         gen(lab0,end_select,-1,p_nulo);
         find=1;
         break   ;
     }
case c_litsym:
         idx=fmtel800(&lab0,&lab1);
        /* c_string */
        while (sym == c_litsym)
        {
        /* store conditional string */
        address=store_str_const(str);
        gen(-1,pre_cond,address,p_nulo);
          getsymbol();
          while (c_fmt() || c_fmt_separator( ) );
        }
        /* se dummy_id */
        if (dummy_id (idx,lab1 ))
         { find = 1;
         }
         /* prefix */
         else {
                if (r_prelit())
                 {
                 gen(actual_inst.info.lab,actual_inst.info.instr,
                     actual_inst.info.add,p_nulo);
                 }
                while (c_fmt_separator( ));
                if (! field_fmt()) {
                 /* 12-03-99 Apos pre_cond aceita outros comandos de formatacao
                             para compatibilidade com MicroIsis
                 */
                 /*    erro (14);*/   /* field_fmt esperado */
                 find=1;
                 }
                 else {
                         find = 1;
                         fmtel805(idx,lab1);
                         suffix();
                         fmtel810(repeatable,lab0,lab1);
                      }
               }
        break;
case r_litsym:
        if (r_prelit())
        {
         idx=fmtel800(&lab0,&lab1);
          gen(actual_inst.info.lab,actual_inst.info.instr,
              actual_inst.info.add,p_nulo);
          while (c_fmt_separator( ));
         if (!field_fmt() ) erro (14);
          else {
                 find = 1;
                 fmtel805(idx,lab1);
                 suffix();
                 fmtel810(repeatable,lab0,lab1);
                }
         }
        break;
case vsym:
case isym:
case usym:
         field_fmt_sym=sym;
         idx=fmtel800(&lab0,&lab1);
        if (field_fmt())
         { find = 1;
           fmtel805(idx,lab1);
           suffix();
           fmtel810(repeatable,lab0,lab1);
         }
         break;
case intvarsym:
        {
         int localindvar;
         find=0;
         #if DEB_ATTRIB
          printf( "\n reconheceu variavel sym=%d indvar=%d", sym,indvar);
         #endif
        /* TIRARRRRRRRRRR vvv*/
         getsymbol();
         if (sym == attribsym) { /*Verificar se atribuicao ou referencia */
            localindvar = indvar;
            getsymbol();
            gen(-1,eattrib_beg,-1,p_nulo);
            if (!num_expr()) erro(333);
            gen(-1,eattrib_end,(LONGX) localindvar,p_nulo);
            /*getsymbol(); */
         }else {
            gen (-1,intvvalue,(LONGX) localindvar,p_nulo);
            /* symbol ja disponivel */
        }
        find=1;

        break;
       }

case strvarsym:
       { int localindvar;
        find=1;
        localindvar = indvar;

      #if DEB_ATTRIB
       printf( "\n reconheceu variavel sym=%d indvar=%d", sym,indvar);
      #endif
        getsymbol();
       if (sym == attribsym) { /*Verificar se atribuicao ou referencia */
            getsymbol();
            if (sym!=lparen) erro(334);
            getsymbol();
            gen(-1,sattrib_beg,-1,p_nulo);
            isis_fmt();
            if (sym != rparen) erro(336);

            gen(-1,sattrib_end,(LONGX) localindvar,p_nulo);
            getsymbol();
         }else {
            gen (-1,strvvalue,(LONGX) localindvar,p_nulo);
            gen(-1,prt_str_f,-1,p_nulo);
           /* symbol ja disponivel */
        }
      find=1;
     break;
    }
case s_sym:
/*         function_in_process++; */
        begin_instruction=s_beg;
        end_instruction=s_end;
        find=function_code_generator(begin_instruction,end_instruction);
/*         function_in_process--; */
        gen(-1,prt_str_f,-1,p_nulo);
        /*      generate_end_function1(); */
        break;

case leftsym:
        find=parse_left_right_mid(left_beg,left_end);
        gen(-1,prt_str_f,-1,p_nulo);
        break;

case rightsym:
        find=parse_left_right_mid(right_beg,right_end);
        gen(-1,prt_str_f,-1,p_nulo);
        break;
case midsym:
        find=parse_left_right_mid(mid_beg,mid_end);
        gen(-1,prt_str_f,-1,p_nulo);
        break;
case ss_sym:
        find=parse_ss();
        gen(-1,prt_str_f,-1,p_nulo);
        break;

case replacsym:
        find=parse_replace(replac_beg,replac_end);
        gen(-1,prt_str_f,-1,p_nulo);
        break;

#if CI_XMLELEM
case xmlelemsym:
        find=parse_xmlelem(xmlelem_beg,xmlelem_end);
        gen(-1,prt_str_f,-1,p_nulo);
        break;
#endif


case getenv_sym:
/*         function_in_process++; */
        begin_instruction=getenv_beg;
        end_instruction=getenv_end;
        find=function_code_generator(begin_instruction,end_instruction);
/*         function_in_process--; */
        gen(-1,prt_str_f,-1,p_nulo);
        /*      generate_end_function1(); */
        break;

case putenv_sym:
/*         function_in_process++; */
        begin_instruction=putenv_beg;
        end_instruction=putenv_end;
        find=function_code_generator(begin_instruction,end_instruction);
/*         function_in_process--; */
        gen(-1,prt_str_f,-1,p_nulo);
        /*      generate_end_function1(); */
        break;

case nl_sym :
        /*A07*/
        find=function_code_generator(nl_beg,nl_end);
         /* O valor gerado sera usado para setar variaveis de nl_STR */
        break;

case rupxsym :
          find=parse_look_np_function(rupx_beg,rupx_end);
          break;
case rupdsym :
          find=parse_look_np_function(rupd_beg,rupd_end);
          break;
case systsym :
          find=parse_look_np_function(syst_beg,syst_end);
          break;
/* case datemktimesym : */
/*           find=parse_look_np_function(date_mktime_beg,date_mktime_end); */
/*           break; */
case mstsym:
case dbsym: /*A09*/
         find=1;
         gen(-1,load_mstnam,-1, p_nulo);
         gen(-1,prt_str_f,-1,p_nulo);
         get_ignore_space();
         break;
case datesym:
         find=parse_date();
         gen(-1,prt_str_f,-1,p_nulo);
         break;
case citypesym:
/*         function_in_process++; */
        begin_instruction=citype_beg;
        end_instruction=citype_end;

        find=function_code_generator(begin_instruction,end_instruction);
/*         function_in_process--; */
        gen(-1,prt_str_f,-1,p_nulo);
/*      generate_end_function1(); */
        break;
case catsym:
        find=function_code_generator(cat_beg,cat_end);
        gen(-1,prt_str_f,-1,p_nulo);
        break;

case fmtsym:
/*         function_in_process++; */
        begin_instruction=fmt_beg;
        end_instruction=fmt_end;
        find=function_code_generator(begin_instruction,end_instruction);
/*         function_in_process--;*/
        break;
case refsym :
        find=parse_ref_function(ref_beg,ref_end);
        break;
case datexsym:
        find=parse_datex_function();
        gen(-1,prt_str_f,-1,p_nulo);
        break;
case fsym :
        find=parse_f_function();
        gen(-1,prt_str_f,-1,p_nulo);
        break;
case lwsym :
        gen(-1,lw_beg,-1,p_nulo);
        getsymbol();
        find = 1;
        if (sym != lparen) erro(4010);
         else { getsymbol();
                if (!num_expr( ) ) erro(4000);
                else { if (sym != rparen) erro(4020);
                         else {
                             getsymbol();
                             gen(-1,lw_end,-1,p_nulo);
                         }
                }
         }
         break;
case continuesym:
        find=gen(-1,continueins,-1,p_nulo);
        getsymbol();
        break;
case breaksym:
        find=gen(-1,breakins,-1,p_nulo); /* gera instrucao incompleta e empilha
                                        endereco para ser complementada depois
                                        A instrucao gerada fica em last_instr
                                    */
        fmt_push_breaks(last_instr);
        getsymbol();
        break;
default:
        if (spacing_string() ) find = 1;
         else if (mode_parameter() ) find = 1;
                else if (dummy_id (idx,lab1)) find = 1; /* talvez tenha erro*/
}  /* switch */
return find;
}
/*--------------------------------------------------------------------------*/
/*                   spacing_string                                         */
/*--------------------------------------------------------------------------*/
#if CICPP
int FMTSTRU :: spacing_string (void)
#else /* CICPP */
int spacing_string ()
#endif /* CICPP */
{
int find;
int operand;
/*x01*/
instruction_code t;
find = 0;
for (;;){
 t=sym_to_instruction[sym];
 if (sym==slash) t=slash_spac; /* Ambiguidade entre divisao e muda linha
                                  na tabela de conversao
                               */
 switch (sym)
 {
 case number_sigx:
        operand=parse_int_number();
        find=1;
        gen(-1,t,operand,p_nulo);
        break;
 case slash:
 case number_sign:
 case percent :
        gen(-1,t,-1,p_nulo);
        getsymbol();
        find = 1;
        break;
 case csym :
 case xsym:
        operand=parse_int_number();
        find=1;
        gen(-1,t,operand,p_nulo);
        break;
default:
        return find; /* quebra loop infinito */
}/* for */
} /* switch */
} /* spacing_string */
/*--------------------------------------------------------------------------*/
/*                   separator                                              */
/*--------------------------------------------------------------------------*/
#if CICPP
int FMTSTRU :: separator (void)
#else /* CICPP */
int separator ()
#endif /* CICPP */
{
int find=0;
for (;;)
{
 find = 1; /* separator is always present */
switch (sym)
 {
  case comma:
  case space:
       getsymbol();
       break;
  default:
      return find;
 }  /* switch, for */
}
} /* separator */
/*--------------------------------------------------------------------------*/
/*                   isis_fmt                                               */
/*--------------------------------------------------------------------------*/
#if CICPP
void FMTSTRU :: isis_fmt (void)
#else /* CICPP */
void isis_fmt ()
#endif /* CICPP */
{
 while (separator())
  if ( ! fmt_elem()) return ;
}
/*--------------------------------------------------------------------------*/
/*                   fmt_finaliza_fmt                                       */
/*--------------------------------------------------------------------------*/
#if CICPP
void FMTSTRU :: fmt_finaliza_fmt(int pt_break)
#else /* CICPP */
#if ANSI
void fmt_finaliza_fmt(int pt_break)
#else /*ANSI*/
void fmt_finaliza_fmt(pt_break)
int pt_break;
#endif /*ANSI*/
#endif /* CICPP */
{
 label lab_break;
 lab_break=fmt_pop_breaks(pt_break);
 gen(lab_break,end,-1,p_nulo);
}

/*- ----------------------------- cifm1h.c / end --------------------- */

/*--------------------------------------------------------------------------*/
/*                   gen_int                                                */
/*--------------------------------------------------------------------------*/
#if CICPP
LONGX FMTSTRU :: xfmt_gener(char *fmt)
#else /* CICPP */
LONGX fmt_gener(pgm,fmt)
l_code **pgm;
char *fmt;
#endif /* CICPP */
{
    int pt_breaks;

    if (!ndbxs) dbxinit();  /* init vdbxp/vrecp/vtrmp if not yet init - AOT, 28/10/2005 */

    /* initialize global pointers */
    pfmt=fmt;
    next_word_buff=0;
    n_instructions=0;
    fmt_error=0;
#if ERRORET
    fmt_errof=0;
#endif
#if !CICPP
#if CDMDL
#if BEFORE981106 /* RPIVA usa MAXMFRL=32K */ /* cifm3.c aloca 2 areas! */
    if (CORELEFT() < 3 * MAXMFRL) fatal("Gener/ALLOC");
#endif
#endif
#endif /*!CICPP*/
    initialize();
    semantic_initialize();
    /* marca inicio de pipha de breaks */
    pt_breaks=top_stack_breaks+1;
    getsymbol ();
    isis_fmt ();
    fmt_finaliza_fmt(pt_breaks);
    if (sym != nullsym) {
        if (fmttrace) printf("+++ fmt_gener: error %d\n",sym);
        erro ( 6 );
#if !ERRORET
        if (fmt_error == 0) fmt_error=1;        /* AOT 26/12/91 */
#endif
    }
    else {
        if (fmttrace) printf("+++ fmt_gener: success\n");
#if !ERRORET
        if (fmt_error != 0) fmt_error=0;        /* AOT 26/12/91 */
#endif
    }

/*A05*/
#if CICPP
 delete [] (char *)buff_look_ahead;
#else /* CICPP */
 FREE(buff_look_ahead);
#endif /* CICPP */
/*A05*/

/*A06*/
#if CICPP
 /*-
  verificar -*/
 delete [] (char *)str;
#else /* CICPP */
 FREE(str);
#endif /* CICPP */
/*A06*/
    /* set output variables */
    head_code->info.lab = (label)n_instructions;
#if CICPP
    fmt_pgmp=head_code;
#else /* CICPP */
    *pgm=head_code;
#endif /* CICPP */
#if PRINT_CODE
/*svd 15-05-92 */ print_inter_din(fmt,*pgm);
#endif
#if ERRORET
    fmt_chadd(head_code);
#if CICPP
    if (fmt_error) xfmt_free();
#else /* CICPP */
    if (fmt_error) fmt_free(head_code);
#endif /* CICPP */
#else
    if (fmt_error == 0) fmt_chadd(head_code);   /* AOT 26/12/91 */
#endif
    return(fmt_error);
}
#define TRACE_ALOC 0
#if !CICPP
static  field_definition_node *tmp_field;
static  l_code *tmp_node;
static  char *tmp_str_aloc;
 /*f01*/
static  float_x *tmp_float_aloc;
 /*f01*/
static  LONGX ret_str;
#endif /* !CICPP */
/*------------------------------------------------------------------------*/
/*       alloc_field_definition_node                                      */
/*------------------------------------------------------------------------*/
#if CICPP
field_definition_node * FMTSTRU :: alloc_field_definition_node (void)
#else /* CICPP */
field_definition_node *alloc_field_definition_node ()
#endif /* CICPP */
{
#if CICPP
 try
 { tmp_field = (field_definition_node *) new char[sizeof(field_definition_node)]; }
 catch (BAD_ALLOC)
 { tmp_field = (field_definition_node *)NULL; }
#else /* CICPP */
 tmp_field =
 (field_definition_node *)ALLOC((ALLOPARM) sizeof(field_definition_node));
#endif /* CICPP */
#if TRACE_ALOC
  cprintf(" ALL(%Fp)", tmp_field); getch();
#endif
 if(tmp_field==NULL)
   { printf ("\nAlloc_Field_definition_node  - no memory available");
     exit(1);
   }
  return tmp_field;
}
/*------------------------------------------------------------------------*/
/*              alloc_pgm_node                                            */
/*------------------------------------------------------------------------*/
#if CICPP
l_code * FMTSTRU :: alloc_pgm_node (void)
#else /* CICPP */
l_code *alloc_pgm_node ()
#endif /* CICPP */
{
#if CICPP
  try
  { tmp_node=(l_code  *) new char [sizeof(l_code)]; }
  catch (BAD_ALLOC)
  { tmp_node=(l_code  *) NULL; }
#else /* CICPP */
  tmp_node=(l_code  *) ALLOC((ALLOPARM)sizeof(l_code));
#endif /* CICPP */
#if TRACE_ALOC
   cprintf (" ALL(%Fp)",tmp_node); getch();
#endif
  if(tmp_node==NULL)
       {printf("Aloc pgm_node  - no memory available");
                exit(0);
       }
  return  tmp_node ;
 }
/*--------------------------------------------------------------------------*/
/*                   store_str_const                                        */
/*--------------------------------------------------------------------------*/
#if CICPP
LONGX FMTSTRU :: store_str_const(char *s)
#else /* CICPP */
#if ANSI
LONGX store_str_const(char *s)
#else /*ANSI*/
LONGX store_str_const(s)
char *s;
#endif /*ANSI*/
#endif /* CICPP */
{
  int tam;
  tam=strlen(s);
  tam++; /* NULL char */
#if CICPP
  try
  { tmp_str_aloc=(char *) new char [tam *  sizeof(char)]; }
  catch (BAD_ALLOC)
  { tmp_str_aloc=(char *) NULL; }
#else /* CICPP */
  tmp_str_aloc=(char *) ALLOC((ALLOPARM)tam *  sizeof(char));
#endif /* CICPP */
#if TRACE_ALOC
         cprintf (" ALL(%Fp)",tmp_str_aloc); getch();
#endif
  if(tmp_str_aloc==NULL)
    {
      printf("\nStore_str_const - no memory available");
      exit(1);
    }
  strcpy(tmp_str_aloc,s);
  ret_str =(LONGX) tmp_str_aloc;
  return  ret_str   ;
 }
/*f01*/
/*--------------------------------------------------------------------------*/
/*                   store_float_const                                      */
/*--------------------------------------------------------------------------*/
#if CICPP
LONGX FMTSTRU :: store_float_const(char *s)
#else /* CICPP */
#if ANSI
LONGX store_float_const(char *s)
#else /*ANSI*/
LONGX store_float_const(s)
char *s;
#endif /*ANSI*/
#endif /* CICPP */
{
  int tam;
  tam=strlen(s);
  tam++; /* NULL char */
#if CICPP
  try
  { tmp_float_aloc=(float_x *) new char [sizeof(float_x)]; }
  catch (BAD_ALLOC)
  { tmp_float_aloc=(float_x *) NULL; }
#else /* CICPP */
  tmp_float_aloc=(float_x *) ALLOC((ALLOPARM) sizeof(float_x));
#endif /* CICPP */
#if TRACE_ALOC
         cprintf (" ALL(%Fp)",tmp_float_aloc); getch();
#endif
  if(tmp_float_aloc==NULL)
    {
      printf("\nStore_float_const - no memory available");
      exit(1);
    }
/* HB 20080901 - a funcao fmt_float_numb nao converte corretamente se float_x for float
#if PC
*/
#if 1
  *tmp_float_aloc=(float_x) atof(s);
#else
  sscanf(s,float_x_fmt,tmp_float_aloc);
#endif
  ret_str =(LONGX) tmp_float_aloc;
  return  ret_str;
 }
/*f01*/
/*--------------------------------------------------------------------------*/
/*                   free_fmt                                               */
/*--------------------------------------------------------------------------*/
#if CICPP
void FMTSTRU :: xfmt_free(void)
#else /* CICPP */
#if ANSI
void fmt_free(l_code *head)
#else /* ANSI */
void fmt_free(head)
l_code *head;
#endif /* ANSI */
#endif /* CICPP */
{
#if CICPP
 l_code *head = fmt_pgmp;
#endif /* CICPP */
  l_code  *t,*tmp;
  if (!head) return;  /* AOT 20/05/93 */
  t=(l_code *)head->next; /* deve ser o primeiro no e nao contem informacao */
  for (;t!=nulo;)
    {
      /* Verifica se a instrucao aponta para um no de definicao de field */
          /* load_field_all,
                 test_occ,
                 present,
                 absent,
                 load_field_occ ( parece que nao esta sendo  gerada)
                 Funcao S (s_end) (substr) apontar para mesmo tipo no V
          */
        if ( t->info.instr==test_occ || t->info.instr==load_field_all||
                 t->info.instr==absent || t->info.instr==present ||
                 t->info.instr==load_field_occ ||
                 t->info.instr==s_end /*substr em S */)
            {
#if TRACE_ALOC
        cprintf(" FREE(%Fp)", t->m_add); getch();
#endif
#if CICPP
            delete [] (char *)t->m_add;
#else /* CICPP */
            FREE( (field_definition_node *)t->m_add);
#endif /* CICPP */
           }
     /* verifica se aponta para um string         */
   if ( t->info.instr==u_cond || t->info.instr==pre_cond ||
        t->info.instr==suf_cond ||
        t->info.instr==pre_r_lit || t->info.instr==pre_r_lit_plus ||
        t->info.instr==suf_r_lit || t->info.instr==suf_r_lit_plus ||
        t->info.instr==suf_cond_null || t->info.instr==load_string ||
        t->info.instr==escape_seq  ||
        t->info.instr==load_float){
#if TRACE_ALOC
        cprintf(" FREE(%Fp)", t->info.add); getch();
#endif
       if (t->info.add) /* AOT/HB - 26/04/99 (vide suffix()) */
#if CICPP
            delete [] (char *) t->info.add;
#else /* CICPP */
            FREE ( (char *)t->info.add );
#endif /* CICPP */
   }
        /* guarda instrucao atual para ser deletda */
    tmp=t;
    /* aponta para proxima instrucao */
    t=(l_code *)t->next;
#if TRACE_ALOC
        cprintf(" FREE(%Fp)", tmp); getch();
#endif
#if CICPP
    delete [] (char *)tmp;
#else /* CICPP */
    FREE((l_code *)tmp);
#endif /* CICPP */
        }
        /* libera a cabeca da lista */
#if TRACE_ALOC
        cprintf(" FREE(%Fp)", head); getch();
#endif
#if CICPP
        delete [] (char *)head;
        fmt_pgmp = NULL;
#else /* CICPP */
        FREE    ((l_code *)head);
#endif /* CICPP */
}
