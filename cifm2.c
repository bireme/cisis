 /* file cifm2.c  */

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "cisis.h"
#include "cifmt.h"

#if !CICPP
static char fmtnumb[3*max_str_num+1];
static  char tmpform[50];
/*---------------------------------------------------------------------------*/
extern char *nl_STR;
extern int nl_LEN;
#endif /* CICPP */


#if CICPP
int FMTSTRU :: fmt_CRLF (char *v,
                         int   pos)
#else /* CICPP */
#if ANSI
int fmt_CRLF (char *v,int   pos)
#else /* ANSI */
int fmt_CRLF (v,pos)
char *v;
int   pos;
#endif /*ANSI*/
#endif /* CICPP */
{
  char *p;

  if (pos < nl_LEN) return false; /* HB */

  p=(char *) &v[pos-nl_LEN];

  if (strncmp(p,(char *)nl_STR,(size_t) nl_LEN)==0) return true;
    else return false;

}
/*--------------------------- Macros ---------------------------------------*/
/*--------------------------------------------------------------------------*/
/*           Macro   concatena                                              */
/*--------------------------------------------------------------------------*/

#define concatena(out,iout,s,init,end)   \
        for (k=init;k<=end;k++) \
         {                       \
         out[iout]=s[k];       \
         iout++;                                                 \
         }                                                           \
          out[iout]=null_char;                                           \


/*--------------------------------------------------------------------------*/
/*               Out_put_str                                                */
/*--------------------------------------------------------------------------*/
/*-----------------------------------------------------------------*/
#if CICPP
void FMTSTRU :: out_put_str(char out[],
                            LONGX maxsize,
                            int *pout,
                            LONGX lw,
                            int id1,
                            int id2,
                            LONGX *ncc,
                            char s[])
#else /* CICPP */
#if ANSI
void out_put_str(char out[],
                 LONGX maxsize,
                 int *pout,
                 LONGX lw,
                 int id1,
                 int id2,
                 LONGX *ncc,
                 char s[])
#else /*ANSI*/
void out_put_str(out,maxsize,pout,lw,id1,id2,ncc,s)
char out[];
LONGX maxsize;
int *pout;
LONGX lw;
int id1;
int id2;
LONGX *ncc;
char s[];
#endif /*ANSI*/
#endif /* CICPP */
{
int iout;
LONGX nextcc;
int first,len,offset,i;
int gerou_ind,inicio,fim;
int copia;
int t,k;

iout= *pout;
nextcc= *ncc;

first=true;

len=strlen(s);
inicio=0;
fim =len-1;
if (lw == 0) lw = LONGX_MAX;  /* HB - 20071127 */

while (fim>=inicio)
 {
 len=fim-inicio+1;
 gerou_ind=false;
 offset=0;
 if(nextcc >= lw )
        {
          new_line(out,iout,maxsize);
          nextcc=1;
         }
 if(nextcc==1)  /* newline*/
        {
         if (first==true) offset=id1;
                else  offset=id2;
         /* repeat_space(out,iout,offset) */
         memset(out+iout,' ',offset); iout+=offset;
         nextcc=nextcc+offset;
         gerou_ind=true;
         }  /* nextcc=1*/
 if ( (iout+len) <= maxsize )
 {
  if ( (lw-nextcc) >= len)
    { /* cabe na linha vai concatenar*/
     concatena(out,iout,s,inicio,fim);
     nextcc=nextcc+(fim-inicio+1);
     inicio=fim+1;
    }
    else
    {
    /* quebra string */
    t=(int) (lw-nextcc);
    /* Se o atual e o proximo Sao brancos precisa achar outra posicao
       diferente de branco */
     if (s[inicio+t]==' ' && s[inicio+t+1]==' ')
       { while (s[inicio+t]== ' '&& t>=0 ) t--; }
     if (t < 0) i=inicio+(int)(lw-nextcc);                      /* 01/06/93 */
     else for (i=t+inicio; i > inicio && s[i] != ' '; i--) ;    /* 01/06/93 */
     copia=true;
     if(i==inicio)  /* nao consegui quebrar */
      {
       if(gerou_ind==true)
        {
         /* mesmo comecando numa linha nao cabe=> quebra de qq geito*/
         i=t+inicio ;
         copia=true;
        }
        else
        {
         /* fazer nova tentativa quebrando a linha */
         new_line(out,iout,maxsize);
         nextcc=1;
         copia=false;
        }  /* else */

        }

    if (copia==true)
     {
      concatena(out,iout,s,inicio,i);
      new_line(out,iout,maxsize);
      out[iout]=null_char;
      first=false;
      nextcc=1;
          inicio=i+1;
     } /* copia */
   } /* lw */
  } /* iout */
  else
  {
   /* nao cabe na linha */

   if (fmttrace) { /* AOT - 18/03/97 */
   printf("\n There is no room in the buffer for the text - truncating");
   printf("\n maxsize=%d,iout=%d,len=%d,fim=%d,inicio=%d",
            maxsize,iout,len,fim,inicio);
   }
   fim=(int ) ( fim-(iout+len- maxsize));
  }

 }  /* while */
 *pout=iout;
 *ncc=nextcc;
 }
  /*out_put_str */



/*--------------------------------------------------------------------------*/
/*                   upcase_mode                                            */
/*--------------------------------------------------------------------------*/
#if CICPP
void FMTSTRU :: upcase_mode(char *s)
#else /* CICPP */
#if ANSI
void upcase_mode(char *s)
#else /* ANSI */
void upcase_mode(s)
char *s;
#endif /* ANSI */
#endif /* CICPP */
{
    unsigned char *p;                                   /* AOT 01/04/92 */
    for(p=(UCHR *)s; *p; p++) *p=isisuctab[*p];         /* AOT 01/04/92 */
}


/*------------------------------------------------------------------------*/
/*                  sub_field_change                                      */
/*------------------------------------------------------------------------*/
#if CICPP
char * FMTSTRU :: sub_field_change(char *q)
#else /* CICPP */
#if ANSI
char *sub_field_change(char *q)
#else /*ANSI*/
char *sub_field_change(q)
char *q;
#endif /*ANSI*/
#endif /* CICPP */
{
 char ch ;
 char *ps,*q1,*p;
 int end;

p=q;
ps=strchr(q,'^');

if(ps==nulo) return q;

end=false;
/* the first occurrence is substituted by spaces skips the "^a" chars */
if (ps == p) {                                  /* AOT 28/04/92 */
*ps++=null_char;
*ps++=' ';
strcat(q,ps);
}
/* other occurrences of subfields area changed according to the following
   rules:
   ^a               subst. by ";"
   ^b through ^i    subst. by ","
   others                     "."
*/

while(end==false)
 {
 q1=strchr(ps,'^');
 if(q1==nulo) end=true;
  else
    {
     q1++;
     ch=toupper(*q1); /* save the next character */
     *q1=' '; /* the charcter is substituted by space */
     q1--; /* the ^ is substituted by punctuation */
     if(ch== 'A') {*q1=';' ;}
       else { if (ch>='B' && ch <= 'I') {*q1=',' ;}
                 else {*q1='.';}
            }
    }
 }
 return p;
}

/* Rotina: nunmber of lines
   Nome rotina:numeber_of_lines;
   Objetivo:Determinar quantas linhas de impressora/terminais foram geradas
   Entrada: Pointer para uma string terminada por NULL.
   Saida: retorna no nome da funcao a quantidade de linhas.
   Algoritmo: Se a string tem tamanho zero, nao vai gerar nenhuma linha.
              Caso contrario, pelo menos uma linha foi gerada.
              Percorre de caractere em caractere verificando se aparece a
              combinacao de caracteres LFCR. Para cada ocorrencia, incre-
              menta o numero de linhas. Normalmente o numero de linhas
              e equivalente ao total de crlf + 1 , com excecao do
              crlf aparecer no final do string.
*/
#if CICPP
LONGX FMTSTRU :: number_of_lines(char *p)
#else /* CICPP */
#if ANSI
LONGX number_of_lines(char *p)
#else /* ANSI */
LONGX number_of_lines(p)
char *p;
#endif /* ANSI */
#endif /* CICPP */
{
 int i,nl,k;
 nl=0;
 i=0;
 k=strlen(p);
 if(k!=0)
  {
    while (p[i]!=null_char)
       {
#if UNIX | WWWISIS
         if (fmt_CRLF(p,i+1)) nl++;
#else
         if (i && fmt_CRLF(p,i+1)) nl++;
#endif /*UNIX | WWWISIS*/
         i++;
        }
#if COUNTLAST                                    /* aot - 14/08/90 */
    /* se no fim da linha nao aparece crlf , entao conta mais uma linha */
    if (!fmt_CRLF(p,k))nl++;
#endif
  }
#if TRACESINDO
  printf("\n---nl=%d\n",nl);
#endif
  return (nl);
  }
/*------------------------------------------------------------------------*/
/*               find_numeric_string                                      */
/*------------------------------------------------------------------------*/

#if !CICPP
#define max_numeric_string 12
char strn[max_numeric_string+1];
char *ptstr;
#endif /* CICPP */

#if CICPP
char * FMTSTRU :: find_numeric_string(char v[],
                                      int *p)
#else /* CICPP */
#if ANSI
char *find_numeric_string(char v[],
                          int *p)
#else /*ANSI*/
char *find_numeric_string(v,p)
char v[];
int *p;
#endif /*ANSI*/
#endif /* CICPP */
{
 int n,k,fim,ndec;
 ptstr=strn;
 n=0;
 strn[n]=null_char;
 k= *p;
 fim=false;
 /* posiciona no inicio de numero */
 while  (! (v[k]==null_char || fim==true) ){
   if (isdigit(v[k]) ) fim=true;
     else if (v[k]=='-'||v[k]=='+'){
            if (isdigit(v[k+1]) ) fim=true;
            strn[n++]=v[k++];
            strn[n]=null_char;
          } else k++;
 }
 fim=false;
 ndec=0;
 while ( fim==false && v[k]!=null_char) {
  if ( isdigit(v[k]) ||((v[k]=='.') && (ndec<1)) ){
    if (v[k]=='.') ndec++;
    if (n>=max_numeric_string){
      printf("\n***** Number too LONGX - truncated to %d digits",n);
      *p=k;
      strn[n]=null_char;
      return  ptstr ;
    }

    /* teste_limits(n,max_numeric_string);                             */
    strn[n++]=v[k++];
 }else fim=true;

 } /* while */
 if ( toupper(v[k])=='E') {
     strn[n++]=v[k++];
     if (v[k]!=null_char)
       if (v[k]=='+' || v[k]=='-') strn[n++]=v[k++];
     fim=false;
     while (fim==false && v[k]!=null_char)
      if (isdigit(v[k]))
        strn[n++]=v[k++];
      else fim=true;
 }
 strn[n]=null_char;
 *p=k;
 return (ptstr);
}

/*--------------------------------------------------------------------------*/
/*                  fmt_float_numb                                         */
/*--------------------------------------------------------------------------*/
/* Objetivo: Transformar um numero real (float) em string na execucao da
             funcao f.
   Algoritmo: A Funcao f pode ser chamada de tres maneiras distintas:
              a) f(valor);
              b) f(valor,tam_minimo);
              c) f(valor,tam_minimo,casas_decimais)
              A formatacao consiste em criar, atraves da variavel tmpform e
              da funcao sprintf,uma string com um formato que corresponda
              a chamada atual da funcao. Depois isso a funcao sprintf e
              usada novamente para realmente formatar o valor nesse formato
              Foi convencionado que na chamada da funcao que  tam_minimo e/ou
              casas_decimais = -1, significa omissao de parametro
   O parametro leftfill contem o caracter 0 ou branco para preencher a
   esquerda do numero
*/
#if CICPP
char * FMTSTRU :: fmt_float_numb (float_x valor,
                                  int min,
                                  int dec,
                                  char leftfill)
#else /* CICPP */
#if ANSI
char *fmt_float_numb (float_x valor,
                      int min,
                      int dec,
                      char leftfill)
#else /*ANSI*/
char *fmt_float_numb (valor,min,dec,leftfill)
float_x valor;
int min;
int dec;
char leftfill;
#endif /*ANSI*/
#endif /* CICPP */

{
  char v[2];
  float_x ff;
   /* Foi usada funcao f com os 3 parametros - A formatacao e' um
    numero real com ponto decimal
 */
 /* Inicialmente fiz a formatacao errada:
    Quando se deseja faze pad a esquerda com branco, no formato de
    sprintf nao pode ter nada entre o '%' e o "numero".
    Quando se deseja fazer pad com zero precisa ter  %0n
    O vetor v serve para esse objetivo.
 */
 v[0]=leftfill;
 v[1]='\0';
 if (leftfill==' ')v[0]='\0';

 ff=valor;
 if (min > 3*max_str_num ) min=3*max_str_num;
 if (dec-3 >3*max_str_num)dec=3*max_str_num-3;

 if(min!= -1 && dec!= -1){
#if 0 /* HB 20080901 - veja definicao de float_x */
   sprintf(tmpform,"%c%s%d%c%d%c",'%',v,min,'.',dec,'f');
   sprintf(fmtnumb,tmpform,(float )ff);
#endif
   sprintf(tmpform,"%c%s%d%c%d%s",'%',v,min,'.',dec,"lf");
   sprintf(fmtnumb,tmpform,ff);
 } else
    /* Foi usada funcao f com 2 parametros (valor,minimo)
       Neste caso a quantidade de casas decimais equivale ao
       tamano minimo- - 4 ( os caracteres E+00)
                       -1 ( o ponto decimal)
                       -1 ( o sinal do numero)
    */
    if(min!= -1 && dec== -1){
      dec=min-4-1-1;
      if (dec<0)dec=0;
      sprintf(tmpform,"%c%c%d%c%d%c",'%',leftfill,min,'.',dec,'E');
      sprintf(fmtnumb,tmpform,(float )ff);
   }
  else {
     /* Fomato com unico parametro. Neste caso o default usado e'
        de 9 casas decimais e tamano  minimo de de 16 caracters
     */
     min=16;
     dec=9;
    sprintf(tmpform,"%c%s%d%c%d%c",'%',v,min,'.',dec+1,'E');
     sprintf(fmtnumb,tmpform,(float )ff);
  }
  return (char *)fmtnumb;

}



/*--------------------------------------------------------------------------*/
/*                  ch_add                                                  */
/*--------------------------------------------------------------------------*/
/* Objetivo: Transforma o enderecamento relativo gerado por instrucoes do
             tipo JUMP em enderecos absolutos.
   Algoritmo: Pesquisa nos campos de LABEL quais as intrucoes em que
              aparece um label e constroi um vetor onde o indice corresponde
              ao label e o conteudo contem o endereco da instrucao em que
              aparece esse label. Suponha que o Programa a ser interpretado
              contenha as instrucoes abaixo:

                End Instr     Label       Instr      Add      Next
                ---------     -----       -----      ----     ----
                        1       -1        Begin       -1       2
                        2                 Jumpt        1       3
                        3                 Print_f     1000     4
                        4        1        U_cond      2000     5
             Para executar esse programa, o endereco apontado na instrucao
             2 , (Add=1) indica que deve ser executada a instrucao numero 4.
             Portanto o endereco (Add=1) devera ser substituido por (add=4)
             Para esse Trecho o vetor de Rotulos devera ter os seguintes va-
             lores:
               R[0]= ...
               R[1]= 4
               Os outros dependem do resto do programa.
             Esse vetor devera ser dinamico e ter tamanho igual ao numero de
             instrucoes geradas+1.
             (A posicao 0 nao tem informacao).
             Todos os enderecos sao copiados para enderecamento dinamico para
             facilitar o interpretador.
*/

#if CICPP
void FMTSTRU :: fmt_chadd(l_code *pgm)
#else /* CICPP */
#if ANSI
void fmt_chadd(l_code *pgm)
#else /*ANSI*/
void fmt_chadd(pgm)
l_code *pgm;
#endif /*ANSI*/
#endif /* CICPP */
{
 int j;
 l_code *t;
 LONGX aux_add;
 instruction_code  aux_instr;
 LONGX *rotulos;
 /* A cabeca de lista do programa gerado contem a quantidade de instrucoes
    geradas.
 */
 j=(int ) pgm->info.lab+1;
 if (fmttrace) printf("fmt_chadd - %d \n",j);                /* AOT 27/12/91 */
 /* numero de instrucoes geradas */
#if CICPP
 try
 { rotulos=(LONGX *) new char [j*sizeof(LONGX)]; }
 catch (BAD_ALLOC)
 { rotulos=(LONGX *)NULL; }
#else /* CICPP */
 rotulos=(LONGX *)ALLOC((ALLOPARM)j*sizeof(LONGX));            /* AOT 30/03/92 */
#endif /* CICPP */

 if(!rotulos) fatal("fmt_chadd/ALLOC");
 memset(rotulos,0x00,j*sizeof(LONGX));                        /* AOT 27/12/91 */

 /* constroi enderecos direto */
 t=(l_code *)pgm->next;
 for (;t!=nulo;)
  {
   t->m_add=t->info.add; /* copia todos os enderecos para endereco dinamico */
   j= (label ) t->info.lab;
   if (j>0)              /* se for label valido guarda o endereco */
      {
       rotulos[j]=(LONGX )t;
      }
   t=(l_code *)t->next;
  }
 /* Para as instrucoes que geram enderecos relativos, transforma em endereco
    dinamico
 */
 t=(l_code *)pgm->next;
 for (;t!=nulo;)
  {
   aux_instr=t->info.instr;
   if ( aux_instr==jumpf ||                     /* date/etc nao entra aqui! */
        aux_instr==jumpt || aux_instr==jump ||
        aux_instr==breakins)
      {
       aux_add=t->info.add;
       t->m_add=rotulos[(int )aux_add];
      }
   t=(l_code *)t->next;
  }
 if (fmttrace) printf("fmt_chadd - success \n");        /* AOT 27/12/91 */
#if CICPP
 delete [] (char *)rotulos;
#else /* CICPP */
 FREE(rotulos);
#endif /* CICPP */
}


/*--------------------------------------------------------------------------*/
/*                    UTF-8 functions
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*                  strlen_UTF8                                             */
/*--------------------------------------------------------------------------*/
/*
   Calcula o comprimento de uma string quando codificado com o encoding UTF-8
*/
size_t strlen_UTF8(const char * str) {
    size_t len_out = 0;
    unsigned char* p = str;
    int bt;

    while (*p != 0) {
        if ((*p & 0x80) == 0) { // 1000 0000
            p += 1;
        } else if ((*p >> 5) == 6)  { // 1100 0000
            p += 2;
        } else if ((*p >> 4) == 14) { // 1110 0000
            p += 3;
        } else if ((*p >> 3) == 30) { // 1111 0000
            p += 4;
        } else {
            fatal("strlen_UTF8/p");
        }
       len_out++;
    }
    return len_out;
}


/*--------------------------------------------------------------------------*/
/*                  skipchar_UTF8                                           */
/*--------------------------------------------------------------------------*/
/*
    Pula n caracteres de uma string codificada com o encoding UTF-8
*/
char* skipchar_UTF8(const char * str, int qtt) {
    size_t skip = 0;
    unsigned char* p = str;
    
    if (qtt <0) qtt = 0;
    
    while ((*p != 0) && (skip < qtt)) {
        if ((*p & 0x80) == 0) { // 1000 0000
            p += 1;
        } else if ((*p >> 5) == 6)  { // 1100 0000
            p += 2;
        } else if ((*p >> 4) == 14) { // 1110 0000
            p += 3;
        } else if ((*p >> 3) == 30) { // 1111 0000
            p += 4;
        } else {
            fatal("skipchar_UTF8/p");
        }
        skip++;
    }
    
    return (char*)p;
}
