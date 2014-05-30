/*-------------------------------------------------------------------------
 ciifl.c
    Faz o load do arquivo invertido (.ifp) e do dicionario (.n0* e .l0*)
 -------------------------------------------------------------------------*/
/* 19-02-95 : Correcao do load de arquivos com + de 32k postings
              O primeiro segmento estava com o numero total de postings
	      corretos. Porem os apontadores dos segmentos seguintes
	      estavam incorretos. Algumas mudancas feitas(types) nao
	      implicavam  no erro.
*/
#define TRACE_HEADER 0
#include <stdio.h>
#include <string.h>
#include "cisis.h"
#include "ciupi.h"

#if CICPP
#include "cirec.hpp"   /* new RECSTRU */
#endif /* CICPP */

#if !CICPP
char *ifllk_gidbnp=NULL;		/* dbn.par */
#endif /* CICPP */

#if CICPP
#include <ciifl.hpp>
#include <cidbx.hpp>
#include <citrm.hpp>
#include <cirun.hpp>
#ifdef USE_ERROR_SYS
#include <ui_win.hpp>
#include <errorsys.hpp>
extern MY_ERROR_SYSTEM * errsys;
extern UIW_WINDOW *wprogress;
extern MESS_SYSTEM *mess;
void ISIS_OemToAnsi(char *in, char *out);
#include <textdb.hpp>
#define USE_INFO_SYS
#endif
#endif /* CICPP */

#if !CICPP
int ifl_balan=1;   /* Faz balanceamento da arvore */
#endif /* CICPP */

#define AOT            1

#define  TSTBALANCE    DEBIFUPD
#define  TRACELOAD     DEBIFUPD
#define  RESUMOLOAD    DEBIFUPD
#define  TRCBUFF       DEBIFUPD
#define  MAXPSTS       MAXIDXPST
#define  BLK(x)        x->ifpblk

/* Esta estrutura guarda  o ultimo no de cada nivel
   Foi definida em cima do maximo valor que sao os
   nos da arvore do tipo 2.
   Poderia ser feita uma alocacao dinamica
*/
#if !CICPP
typedef struct tree_nodes {
  int top;
  N2STRU  idx_node[MAX_TREE_LEVEL];
 } TREE_NODES;
#endif /* CICPP */

/* -----------------------------------------------------------------------*/
/*                 Prototypes                                             */
/* -----------------------------------------------------------------------*/
#if !CICPP
#if ANSI
/*y*/ static void lifp_init_counters(void);
static void lifp_init_leaf(L0STRU *lp,int treecase);
static void lifp_init_node(N0STRU *n0p,int treecase);
static BOOLEAN lifp_insert_leaf(DBXSTRU *dbxp,char *key,char *p_b_key,
                         PUNT *p_b_punt, int treecase,INFX info1,
                         INFO  info2);
static BOOLEAN lifp_insert_node (DBXSTRU *dbxp,N0STRU *n0p,int treecase,
                          int level,
                          int isroot,UCHR *b_key,PUNT b_punt,
                          UCHR *p_key,PUNT *p_punt);
static void lifp_create_root(INVMAP *invp,N0STRU *n0p,PUNT esq,char *key,
		      PUNT dir,int treecase,BOOLEAN first);
#else
/*y*/ static void lifp_init_counters();
static void lifp_init_leaf();
static void lifp_init_node();
static BOOLEAN lifp_insert_leaf();
static BOOLEAN lifp_insert_node ();
static void lifp_create_root();
#endif /* ANSI */
#endif /* CICPP */

#if !CICPP
#if ANSI
static void determina_nova_qtda(int ordem,int a,int b,int *n_a,int *n_b);
static void init_invp_roots_nodes_leaves(INVMAP *invp);
static void balance_nodes(DBXSTRU *dbxp,INVMAP *invp,int treecase,
                 int level,PUNT idxpenultimo,PUNT idxultimo,N0STRU *np);
static void balance_btree(DBXSTRU *dbxp,INVMAP *invp,
                          int level,PUNT punt,int treecase);
static void balance_leaves(DBXSTRU *dbxp,INVMAP *invp,int treecase,
			   PUNT idxpenultimo,PUNT idxultimo,N0STRU *np);
static void lifp_store_btree(UCHR *key,INFX blk,INFO off,int treecase);
static IFPBUFFER *lifp_init(INVMAP *invp); /* antes de LKXONLY: (void) */
static void lifp_close_tree(void);
#if !LIND
static void lifp_grava_ifp(void);
static void lifp_storepst(void);
static void lifp_init_buff(IFPBUFFER  *p,INFO blk);
static void write_buffers(BOOLEAN keep_head,BOOLEAN reestrut_buff);
static void get_room_for (int qt,BOOLEAN opt_keep_head);
static void lifp_upd_pst_header(IFPSTRU *p,INFO offset,INFO nxtb,INFO nxtp,
                         INFO totp,INFO segp,INFO segc);
/*static void lifp_upd_first_rec(IFPAVAILPOS *pn,INFO next_blk,UWORD idxpst); */
static void lifp_upd_first_rec(IFPAVAILPOS *pn,INFO next_blk,INFO next_post);
 /*19-02-95*/
static void lifp_init_ifprec(IFPSTRU *p,INFO blk);
#endif /* !LIND */
#else
static void determina_nova_qtda();
static void init_invp_roots_nodes_leaves();
static void balance_nodes();
static void balance_btree();
static void balance_leaves();
static void lifp_store_btree();
static IFPBUFFER *lifp_init();
static void lifp_close_tree();
#if !LIND
static void lifp_grava_ifp();
static void lifp_storepst();
static void lifp_init_buff();
static void write_buffers();
static void get_room_for ();
static void lifp_upd_pst_header();
static void lifp_upd_first_rec();
static void lifp_init_ifprec();
#endif /* !LIND */
#endif /* ANSI */
#endif /* CICPP */

#if !CICPP
#if ANSI
static int lifp_trans_key(UCHR *keyp, int len, int *treecase);
#else
static int lifp_trans_key();
#endif /* ANSI */
#endif /* CICPP */

/* -----------------------------------------------------------------------*/

#if !CICPP
 static TREE_NODES load_idx[2];
 static PUNT last_root;  /* Guarda a raiz atual antes de criar uma nova raiz
*/
 static L0STRU *l0p;
 static L1STRU *l1p, l1node;
 static L2STRU *l2p, l2node;
 static L1IDXE init_leaf_el1;
 static L2IDXE init_leaf_el2;
 static N1IDXE init_node_el1;
 static N2IDXE init_node_el2;
 static LONGX  print_step=0L;
#if TRACELOAD
  UCHR tkey[LE2+1], *ptkey; /* AOT 23/10/93 */
  int  ti;
#endif
 static POSTSTRU pst,pst_ant;
 static UCHR  *keyp;
 static LONGX  nlido;
 static PUNT p_b_punt,
             key_punt;
 static INVMAP *invp;
 static DBXSTRU *dbxp;
 static N0STRU *n0p;
 static UWORD isroot,i;
 static UCHR  *p_b_key,
              a_p_b_key[LE2+1];
 static BOOLEAN promoted;
 static int topo,
            level,
            keysize,
            treecase,
            treecase_ant;
 static UCHR key_ant[LE2+1],
             key[LE2+1];
 static IFPBUFFER *ifpbuff=NULL; /* AOT - 16/09/93 */
#if !LIND
 static INFO key_blk,
             key_pos;
 /* static int tag,occ,cnt; */   /* AOT - 26/10/93 */
 static IFPAVAILPOS pn;
 static IFPSTRU  *pf,
                 *ph,
                 *pb;
 static INFO next_blk,
             next_pos;
 static INFO totp,
             nxtb,
             nxtp,
             segp,
             segc;
 static INFO off_f,
             off_h,
             npst;
 static int gbuf;
 static IFPSTRU current_seghead,
                first_seghead;
 static IFPHEAD *currenthead,
                *firsthead;
 static BOOLEAN flag_first_seghead,
                flag_current_seghead;
 static UWORD idxpst;
#endif /* !LIND */
#if LIND
#include "ciiflh.c"
#endif /* LIND */
#if LKXONLY
 static int lkxonly;    /* AOT - 14/05/98 */
#endif

/*25/07/97*/
/* Variaveis para guardar a localizacao ds headers de um segmento */
/* 128/(5+2) */
#define IFLmaxhd  IFPmaxhd*MAXIFPMEM + 1   /* headers em memoria */
#if CNV_PCBINUM
static int hdn=-1;       /* numero headers de um ifp */
static INFO hdblk[IFLmaxhd];
static INFO hdoff[IFLmaxhd];
#endif
/* Descricao de Variaveis e Atualizacao do IFP
  pn - pointer para o primeiro registro do arquivo que contem o apontador
       para o proximo (blk,off) disponiveis.
  ph - pointer para o registro onde esta o head do segmento que devera ser
       atualizado
  pb - pointer para o bloco onde estao sendo gravados os postings
  pf - pointer para o primeiro bloco onde e' gravado o total de postings
       quando uma chave possui mais MAXPSTSSEG e deve ser gravada em dois
       segmentos diferentes
  ifpbuff - Buffer de MAXIFPMEM blocos com objetivo de acelerar a atualiacao
            Nesse buffer devem ser mantidos blocos sequenciais " de disco"
            de forma em unico acesso gravar variso blocos
  off_h e off_f - guardam a posicao do header do posting dentro de ph e pf
                  respectivamente
  Algoritmo:
  Em determinado momento  os postings de uma chave podem ser encaixados em
  uma das seguintes condicoes:
  1) Todos os postings cabem em um pedaco do ifpbuff. Neste caso temos:
     ph - aponta para  um bloco do  ifpbuff.
     A varivavel pf nao dever ser considerada.
  2) Embora a quantidade de postings seja pequena e menor que MAXPSTSSEG
     nao cabe nas posicoes disponiveis do ifpbuff. Neste caso devemos:
     a) Gravar ifpbuff;
     b) copiar o bloco onde esta o header do segmento (pb) para uma
        variavel temporaria e apontar ph para essa area. Notar que o
        embora o header seja gravado seus valores estarao incorretos.
         ph=endereco de header_temp;
         copiar: pb para ph;
         off_h = pb.off;
     c) Indicar que agora ph nao aponta para  o buffer sequencial
        flag_current_seghead=true;
    No final o header devera ser regravado com os valores corretos.
 3) O numero de postings da chave ultrapassa MAXPSTSSEG. Neste caso
    sera gerado mais de um segmento.
    O primeiro segmento devera ser mantido em memoria porque e' nele
    que e gravado a quantidade correta de postings do segmento
    Entao havera necessidade de dois apontadores:
    pf - aponta para o primeiro segmento
    ph - aponta para o segmento corrente.
    No memomento que o numero de postings ultrapassa MAXPSTSSEG deve-se
    a) Copiar ph para pf
    b) flag_first_seghead =true
    c)

/*-------------------------------------------------------------------------*/
/* Este algoritmo determina qual a quantidade de chaves que os nos
   mais a direita devem ter. Foi feito para que o load ficasse
   igual ao do ISIS . Acho que nao deveria deste jeito, porque nao
   separa igualmente a quantidade de chaves.
   Inicialmente o algoritmo de load, fazia com que, para todos os niveis,
   os dois nos mais a direita do nivel ficassem desbalanceados. Era possivel
   termos o penultimo com 10 ocorrencias e o ultimo com uma ocorrencia.
   Apos o balanceamento dos nos mais a direita de cada nivel fica da
   seguinte maneira ( a e b sao os dois ultimos nos)
   a+b       nova qta a   nova qta b
   11          5             6
   12          5             7
   13          5             8
   14          5             9
   15          5             10
   16          8             8
   17          8             9
   18          8             10
   19          9             10
   20          10            10
*/

#endif /* CICPP */

#if CICPP
void CIIFL :: determina_nova_qtda(int  ordem,
                                  int  a,
                                  int  b,
                                  int *n_a,
                                  int *n_b)
#else /*CICPP*/
static void determina_nova_qtda(ordem,a,b,n_a,n_b)
int ordem;
int a;
int b;
int *n_a;
int *n_b;
#endif /*CICPP*/
{
 int x,y,soma;
 soma=a+b;
 x=ordem;      /* Caso 1 : a=5; b=6,7,8,9,10 */
 y=soma-x;
 if (y>2*ordem) {
   x=(3*ordem+1)/2; /* Caso 2: a=8; b:8,9,10  */
   y=soma-x;
   if (y>2*ordem) {  /* Caso 3: a=9; b=9,10 ou
            Caso 4: a=10; b=10
             */
      x=soma/2;
      y=soma-x;
   }
 }
 *n_a=x;
 *n_b=y;
}
/* #include "leaves.c" */
/* ------------------------------------------------------------------------*/
#if CICPP
void CIIFL :: balance_leaves(DBXSTRU *dbxp,
                             INVMAP *invp,
                             int     treecase,
                             PUNT    idxesq,
                             PUNT    idxdir,
                             N0STRU *np)
#else /*CICPP*/
static void balance_leaves(dbxp,invp,treecase,idxesq,idxdir,np)
DBXSTRU *dbxp;
INVMAP *invp;
int treecase;
PUNT idxesq;
PUNT idxdir;
N0STRU *np;
#endif /*CICPP*/
{
  UCHR *lbufp;
  N1STRU *ptmp_pai1;
  N2STRU *ptmp_pai2;
  L0STRU *l0p,*pesq0,*pdir0;
  L1STRU *pesq1,esq1,
         *pdir1,dir1,
         *ptmp_esq1,tmp_esq1,
         *ptmp_dir1,tmp_dir1,
         trml1buf;
  L2STRU *pesq2,esq2,
         *pdir2,dir2,
         *ptmp_esq2,tmp_esq2,
         *ptmp_dir2,tmp_dir2,
         trml2buf;
  PUNT punt;
  int n_esq,n_dir,ock,id,nn;
  keysize=vlex[treecase];
  if (treecase == 0)
     lbufp= (UCHR *)(&trml1buf);
  else
     lbufp= (UCHR *)(&trml2buf);
  if (idxesq==0) return;
  if (idxdir==0) return;
  punt= -idxesq;
  if (punt<0) {
     fatal(" balance_leaves/punt >0");
  }
  l0p=(L0STRU *)leafread(lbufp,invp,treecase,punt,0);
  if (treecase==0) {
    ptmp_esq1= &tmp_esq1;
    pesq1= &esq1;
    memmove((UCHR *)pesq1,(UCHR *)l0p,sizeof(L1STRU ));
  } else {
    ptmp_esq2= &tmp_esq2;
    pesq2= &esq2;
    memmove((UCHR *)pesq2,(UCHR *)l0p,sizeof(L2STRU ));
  }
  punt= -idxdir;
  if (punt<0) {
    fatal(" balance_leaves/punt >0");
  }
  l0p=(L0STRU *)leafread(lbufp,invp,treecase,punt,0);
  if (treecase==0) {
    pdir1= &dir1;
    memmove((UCHR *)pdir1,(UCHR *)l0p,sizeof(L1STRU ));
  } else {
    pdir2= &dir2;
    memmove((UCHR *)pdir2,(UCHR *)l0p,sizeof(L2STRU ));
  }
  if (treecase==0) {
     ptmp_pai1=(N1STRU *)np ;
#if TSTBALANCE
      printf("\n Antes de balancear ");
      upif_print_node ((N0STRU *)ptmp_pai1);
      upif_print_leaf ((L0STRU *)pesq1);
      upif_print_leaf ((L0STRU *)pdir1);
#endif
     determina_nova_qtda(ORDF,pesq1->ock,pdir1->ock,&n_esq,&n_dir);
     ptmp_esq1= &tmp_esq1;
     ptmp_dir1= &tmp_dir1;
     lifp_init_leaf((L0STRU *)ptmp_esq1,treecase);
     lifp_init_leaf((L0STRU *)ptmp_dir1,treecase);
  }else {
     ptmp_pai2=(N2STRU *)np ;
#if TSTBALANCE
      printf("\n Antes de balancear ");
      upif_print_node ((N0STRU *)ptmp_pai2);
      upif_print_leaf ((L0STRU *)pesq2);
      upif_print_leaf ((L0STRU *)pdir2);
#endif
     determina_nova_qtda(ORDF,pesq2->ock,pdir2->ock,&n_esq,&n_dir);
     ptmp_esq2= &tmp_esq2;
     ptmp_dir2= &tmp_dir2;
     lifp_init_leaf((L0STRU *)ptmp_esq2,treecase);
     lifp_init_leaf((L0STRU *)ptmp_dir2,treecase);
  }
   /* Da forma que o algoritmo de load funciona, a celula da esquerda
      contera sempre o numero maximo de chaves . Eles deverao ser
      divididos entre a esquerda e a direita levando em conta o criterio
      (furado!!!) do ISIS.
   */
   /* Atualiza headers.
     Copia  "n_esq"    nos da esquerda para temporaria
   */
  if (treecase == 0) {
     ock=pesq1->ock;
     ptmp_esq1->ock=n_esq;
     ptmp_esq1->pos=pesq1->pos;
     ptmp_esq1->ps= pesq1->ps;
#if LIND
     ptmp_esq1->psb= pesq1->psb;
#endif /* LIND */
     ptmp_esq1->it= pesq1->it;
     ptmp_dir1->ock=n_dir;
     ptmp_dir1->pos=pdir1->pos;
     ptmp_dir1->ps= pdir1->ps;
#if LIND
     ptmp_dir1->psb= pdir1->psb;
#endif /* LIND */
     ptmp_dir1->it= pdir1->it;
  }else {
     ock=pesq2->ock;
     ptmp_esq2->ock=n_esq;
     ptmp_esq2->pos=pesq2->pos;
     ptmp_esq2->ps= pesq2->ps;
#if LIND
     ptmp_esq2->psb= pesq2->psb;
#endif /* LIND */
     ptmp_esq2->it= pesq2->it;
     ptmp_dir2->ock=n_dir;
     ptmp_dir2->pos=pdir2->pos;
     ptmp_dir2->ps= pdir2->ps;
#if LIND
     ptmp_dir2->psb= pdir2->psb;
#endif /* LIND */
     ptmp_dir2->it= pdir2->it;
  }
   for (nn=0;nn<n_esq;nn++){
     if (treecase==0) {
         ptmp_esq1->idx[nn] = pesq1->idx[nn];
     }else {
        ptmp_esq2->idx[nn] = pesq2->idx[nn];
     }
   }
 /* O resto das chaves sao copiados para o no da direita */
   id=0;
   for (;nn<ock;nn++){
      if (treecase==0) {
        ptmp_dir1->idx[id]=pesq1->idx[nn];
      }else {
        ptmp_dir2->idx[id]=pesq2->idx[nn];
      }
      id++;
   }
   /* Acrescenta todas as chaves da direita */
   if (treecase == 0)
      ock=pdir1->ock;
   else
      ock=pdir2->ock;
   for (nn=0; nn<ock;nn++){
      if (treecase==0) {
        ptmp_dir1->idx[id]=pdir1->idx[nn];
      } else {
        ptmp_dir2->idx[id]=pdir2->idx[nn];
      }
      id++;
  }
  /* A primeira chave da direita deve ser promovida para o pai
     Deve substituir a chave mais a direita do pai
  */
   if (treecase == 0){
      id=ptmp_pai1->ock-1;/* Ultimo elemento */
      memmove(ptmp_pai1->idx[id].key,ptmp_dir1->idx[0].key,keysize);
      /* O punt nao precisa mudar. E o mesmo */
   }
   else {
      id=ptmp_pai2->ock-1;/* Ultimo elemento */
      memmove(ptmp_pai2->idx[id].key,ptmp_dir2->idx[0].key,keysize);
   }
 if (treecase==0) {
  n0p=(N0STRU *)ptmp_pai1;
  pesq0=(L0STRU *)ptmp_esq1;
  pdir0=(L0STRU *)ptmp_dir1;
 }else {
  n0p=(N0STRU *)ptmp_pai2;
  pesq0=(L0STRU *)ptmp_esq2;
  pdir0=(L0STRU *)ptmp_dir2;
 }
 /* Como a arvore ja foi gravada correntamente, os valores de level e
    isroot nao tem mais sentido. Estao sendo passados somente para
    poder usar as rotinas ja prontas
 */
  isroot=FALSE;
#if TSTBALANCE
  printf("\n Depois de balancear ");
  upif_print_node (n0p);
  upif_print_leaf (pesq0);
  upif_print_leaf (pdir0);
#endif
  nodewrit(dbxp,n0p,invp->cn[treecase].liv,isroot);
  leafwrit(dbxp,pesq0);
  leafwrit(dbxp,pdir0);
}
/* ------------------------------------------------------------------------*/
#if CICPP
void  CIIFL :: balance_nodes(DBXSTRU *dbxp,
                             INVMAP  *invp,
                             int      treecase,
                             int      level,
                             PUNT     idxpenultimo,
                             PUNT     idxultimo,
                             N0STRU  *np)
#else /*CICPP*/
static void  balance_nodes(dbxp,invp,treecase,level,idxpenultimo,idxultimo,np)
DBXSTRU *dbxp;
INVMAP *invp;
int treecase;
int level;
PUNT idxpenultimo;
PUNT idxultimo;
N0STRU *np;
#endif /*CICPP*/
{
 static   N0STRU *n0p,*pesq0,*pdir0;
 static   N1STRU tmp_esq1,tmp_dir1,
          *ptmp_pai1,*ptmp_esq1,*ptmp_dir1,
          *pesq1,*pdir1,esq1,dir1;
 static  N2STRU tmp_esq2,tmp_dir2,
         *ptmp_pai2,*ptmp_esq2,*ptmp_dir2,
         *pesq2,*pdir2,esq2,dir2;
 static  UCHR *char_pesq,
         *char_pdir;
 int n_esq,n_dir,ock,id,nn;
  keysize=vlex[treecase];
  if (treecase==0) {
       pesq1= &esq1;
       pdir1= &dir1;
       char_pesq=(UCHR *)noderead(invp,treecase,level,idxpenultimo);
       memmove((UCHR *)pesq1,char_pesq,sizeof(N1STRU));
       char_pdir=(UCHR *)noderead(invp,treecase,level,idxultimo);
       memmove((UCHR *)pdir1,char_pdir,sizeof(N1STRU));
  }else {
       pesq2= &esq2;
       pdir2= &dir2;
       char_pesq=(UCHR *)noderead(invp,treecase,level,idxpenultimo);
       memmove((UCHR *)pesq2,char_pesq,sizeof(N2STRU));
       char_pdir=(UCHR *)noderead(invp,treecase,level,idxultimo);
       memmove((UCHR *)pdir2,char_pdir,sizeof(N2STRU));
  }
  if (treecase==0) {
     ptmp_pai1=(N1STRU *)np ;
#if TSTBALANCE
      printf("\n Antes de balancear ");
      upif_print_node ((N0STRU *)ptmp_pai1);
      upif_print_node ((N0STRU *)pesq1);
      upif_print_node ((N0STRU *)pdir1);
#endif
     determina_nova_qtda(ORDN,pesq1->ock,pdir1->ock,&n_esq,&n_dir);
     ptmp_esq1= &tmp_esq1;
     ptmp_dir1= &tmp_dir1;
     lifp_init_node((N0STRU *)ptmp_esq1,treecase);
     lifp_init_node((N0STRU *)ptmp_dir1,treecase);
  }else {
     ptmp_pai2=(N2STRU *)np ;
#if TSTBALANCE
      printf("\n Antes de balancear ");
      upif_print_node ((N0STRU *)ptmp_pai2);
      upif_print_node ((N0STRU *)pesq2);
      upif_print_node ((N0STRU *)pdir2);
#endif
         determina_nova_qtda(ORDN,pesq2->ock,pdir2->ock,&n_esq,&n_dir);
     ptmp_esq2= &tmp_esq2;
     ptmp_dir2= &tmp_dir2;
     lifp_init_node((N0STRU *)ptmp_esq2,treecase);
     lifp_init_node((N0STRU *)ptmp_dir2,treecase);
  }
   /* Da forma que o algoritmo de load funciona, a celula da esquerda
      contera sempre o numero maximo de chaves . Eles deverao ser
      divididos entre a esquerda e a direita levando em conta o criterio
      (furado!!!) do ISIS.
   */
   /* Atualiza headers.
     Copia  "n_esq:    nos da esquerda para temporaria*/
  if (treecase == 0) {
     ock=pesq1->ock;
     ptmp_esq1->ock=n_esq;
     ptmp_esq1->pos=pesq1->pos;
     ptmp_esq1->it=pesq1->it;
     ptmp_dir1->ock=n_dir;
     ptmp_dir1->pos=pdir1->pos;
     ptmp_dir1->it=pdir1->it;
  }else {
     ock=pesq2->ock;
     ptmp_esq2->ock=n_esq;
     ptmp_esq2->pos=pesq2->pos;
     ptmp_esq2->it=pesq2->it;
     ptmp_dir2->ock=n_dir;
     ptmp_dir2->pos=pdir2->pos;
     ptmp_dir2->it=pdir2->it;
  }
   for (nn=0;nn<n_esq;nn++){
     if (treecase==0) {
         ptmp_esq1->idx[nn] = pesq1->idx[nn];
     }else {
         ptmp_esq2->idx[nn] = pesq2->idx[nn];
     }
  }
 /* O resto das chaves sao copiados para o no da direita*/
   id=0;
   for (;nn<ock;nn++){
      if (treecase==0) {
        ptmp_dir1->idx[id]=pesq1->idx[nn];
      }else {
        ptmp_dir2->idx[id]=pesq2->idx[nn];
      }
      id++;
   }
   /* Acrescenta todas as chaves da direita */
   if (treecase == 0)
      ock=pdir1->ock;
   else
      ock=pdir2->ock;
   for (nn=0; nn<ock;nn++){
      if (treecase==0) {
        ptmp_dir1->idx[id]=pdir1->idx[nn];
      } else {
        ptmp_dir2->idx[id]=pdir2->idx[nn];
      }
      id++;
  }
  /* A primeira chave da direita deve ser promovida para o pai
     Deve substituir a chave mais a direita do pai
  */
   if (treecase == 0){
      id=ptmp_pai1->ock-1;/* Ultimo elemento */
      memmove(ptmp_pai1->idx[id].key,ptmp_dir1->idx[0].key,keysize);
      /* O punt nao precisa mudar. E o mesmo */
   }
   else {
      id=ptmp_pai2->ock-1;/* Ultimo elemento */
      memmove(ptmp_pai2->idx[id].key,ptmp_dir2->idx[0].key,keysize);
   }
 if (treecase==0) {
  n0p=(N0STRU *)ptmp_pai1;
  pesq0=(N0STRU *)ptmp_esq1;
  pdir0=(N0STRU *)ptmp_dir1;
 }else {
  n0p=(N0STRU *)ptmp_pai2;
  pesq0=(N0STRU *)ptmp_esq2;
  pdir0=(N0STRU *)ptmp_dir2;
 }
 /* Como a arvore ja foi gravada correntamente, os valores de level e
    isroot nao tem mais sentido. Estao sendo passados somente para
    poder usar as rotinas ja prontas
 */
  isroot=FALSE;
#if TSTBALANCE
  printf("\n Apos de  balancear ");
  upif_print_node (n0p);
  upif_print_node (pesq0);
  upif_print_node (pdir0);
#endif
  nodewrit(dbxp,n0p,invp->cn[treecase].liv,isroot);
  nodewrit(dbxp,pesq0,invp->cn[treecase].liv,isroot);
  nodewrit(dbxp,pdir0,invp->cn[treecase].liv,isroot);
}
/* -----------------------------------------------------------------------*/
#if CICPP
void CIIFL :: balance_btree(DBXSTRU *dbxp,
                            INVMAP  *invp,
                            int      level,
                            PUNT     punt,
                            int      treecase)
#else /*CICPP*/
static void balance_btree(dbxp,invp,level,punt,treecase)
DBXSTRU *dbxp;
INVMAP *invp;
int level;
PUNT punt;
int treecase;
#endif /*CICPP*/
{
  UCHR  *np;
  static  N2STRU node2,area_node2;
  static  N1STRU node1,area_node1;
  N2STRU *n2p, *keep_node2;
  N1STRU *n1p, *keep_node1;
  N0STRU *n0local;
  int ock;
  PUNT idxultimo,idxpenultimo;
  keep_node1= &area_node1;
  keep_node2= &area_node2;

  if (punt==0) return;
  if (punt>0) {
    idxultimo=1; /* so para inicializar while */
    while (idxultimo > 0) {
      np=(UCHR *)noderead(invp,treecase,level,punt);
      if (treecase==0){
          memmove((UCHR *)&node1,np,sizeof(N1STRU));
          np=(UCHR *)&node1;
      }
      else {
          memmove((UCHR *)&node2,np,sizeof(N2STRU));
          np=(UCHR *)&node2;
      }
      n1p=(N1STRU *)np;
      n2p=(N2STRU *)np;
      n0p=(N0STRU *)np;
      ock=n0p->ock;
      if(ock <= 1) return; /* nada a reestruturar */
      if (treecase == 0) {
         idxpenultimo=n1p->idx[ock-2].punt;
         idxultimo=n1p->idx[ock-1].punt;
         memmove(keep_node1,np,sizeof(N1STRU));
         n0local=(N0STRU *)keep_node1;
      } else {
         idxpenultimo=n2p->idx[ock-2].punt;
         idxultimo=n2p->idx[ock-1].punt;
         memmove(keep_node2,np,sizeof(N2STRU));
         n0local=(N0STRU *)keep_node2;
      }
     if (idxpenultimo < 0) { /* aponta para folha */
        balance_leaves(dbxp,invp,treecase,
                       idxpenultimo,idxultimo, n0local);
     } else {         /* aponta para no */
        balance_nodes(dbxp,invp,treecase,level,idxpenultimo,idxultimo,
                 n0local);
        punt=idxultimo;
     }
   }
  }/* WHILE*/
}
#if !LIND
/*------------------------------------------------------------------------*/
#if CICPP
void CIIFL :: lifp_upd_first_rec(IFPAVAILPOS *pn,
                                 INFO         next_blk,
                                 INFO         next_pos)
#else /*CICPP*/
static void lifp_upd_first_rec(pn,next_blk,next_pos)
IFPAVAILPOS *pn;
INFO next_blk;
INFO next_pos;
#endif /*CICPP*/
{
 pn->ifpblk=1;
 pn->nxtb=next_blk;
 pn->nxtp=next_pos;
}
/*------------------------------------------------------------------------*/
#if CICPP
void CIIFL :: lifp_init_ifprec(IFPSTRU *p,
                               INFO     blk)
#else /*CICPP*/
static void lifp_init_ifprec(p,blk)
IFPSTRU *p;
INFO blk;
#endif /*CICPP*/
{ int i;
  p->ifpblk=blk;
  for (i=0;i<IFMAXTIV;i++){
     p->ifprec[i]=0;
  }
}
/*------------------------------------------------------------------------*/
#if CICPP
void CIIFL :: lifp_init_buff(IFPBUFFER  *q,
                             INFO        blk)
#else /*CICPP*/
static void lifp_init_buff(q,blk)
IFPBUFFER  *q;
INFO blk;
#endif /*CICPP*/
{ int i;
  IFPSTRU *p;
  for (i=0;i<MAXIFPMEM;i++){
      p=(IFPSTRU *)&q->buff[i];
      lifp_init_ifprec(p,blk);
      blk++;
  }
}
#endif /* !LIND */
/*------------------------------------------------------------------------*/
#if CICPP
IFPBUFFER *CIIFL ::lifp_init(INVMAP *invp)
#else /*CICPP*/
static IFPBUFFER *lifp_init(invp)
INVMAP *invp;
#endif /*CICPP*/
{
  IFPBUFFER *ifpb;
#if !LIND
  next_blk=(INFO )1;
  next_pos=(INFO )2;
#endif /* !LIND */
#if CICPP
  try { ifpb = (IFPBUFFER *) new char [sizeof(IFPBUFFER)]; }
  catch (BAD_ALLOC) { ifpb = (IFPBUFFER *)ALLONULL; }
#else /* CICPP */
  ifpb = (IFPBUFFER *)ALLOC(sizeof(IFPBUFFER));
#endif /* CICPP */
  if (ifpb == (IFPBUFFER *)ALLONULL) {
#if CICPP
	return(NULL);
#else /* CICPP */
	fatal("ALLOC/lifp_init");
#endif /* CICPP */
  }
#if !LIND
  pn.ifpblk=1;
#if LKXONLY
  if (lkxonly & 2) {
    postread((UCHR *)ifpb,invp,1L,0);
    next_blk=((IFPCTRL *)ifpb)->ifprec1;
    next_pos=((IFPCTRL *)ifpb)->ifprec2;
    pn.ifpblk=next_blk;
  }
#endif
  pn.nxtb=next_blk;
  pn.nxtp=next_pos;
  lifp_init_buff(ifpb,next_blk);
  flag_first_seghead=FALSE;
  flag_current_seghead=FALSE;
#endif /* !LIND */
  key_ant[0]='\0';
  treecase_ant=0;	/* Sindo, esta' ok para LKXONLY?  Thanks, AOT */
#if !LIND
  pf=(IFPSTRU *)&first_seghead;
#endif /* !LIND */
  key[0]='\0';
#if !LIND
  totp=0;
  nxtb=0;
  nxtp=0;
  segp=0;
  segc=0;
  idxpst=next_pos;
  gbuf=0;
  ph= &ifpb->buff[gbuf];
  off_h=idxpst;
  pb=(IFPSTRU *)&ifpb->buff[gbuf];
  pb->ifprec[0]=next_blk;
  pb->ifprec[1]=next_pos;
  memset((char *)&pst_ant,0x00,sizeof(POSTSTRU));
#endif /* !LIND */
 return(ifpb);
}
#if !LIND
/*-----------------------------------------------------------------------*/
#if CICPP
void CIIFL :: write_buffers(BOOLEAN keep_head,
                            BOOLEAN reestrut_buff)
#else /*CICPP*/
static void write_buffers( keep_head,reestrut_buff)
BOOLEAN keep_head;
BOOLEAN reestrut_buff;
#endif /*CICPP*/
{
 IFPSTRU tmp;
#if CNV_PCBINUM
 ifpwrit(dbxp,(char *)ifpbuff,(LONGX )sizeof(IFPBUFFER),hdblk,hdoff,hdn);
 ifp_init_hd(&hdn);
#else /* CNV_PCBINUM */
 ifpwrit(dbxp,(char *)ifpbuff,(LONGX )sizeof(IFPBUFFER));
#endif /* CNV_PCBINUM */ 
 if (keep_head==TRUE && flag_current_seghead==FALSE) {
    memmove((UCHR *)&current_seghead,(UCHR *)ph,sizeof(IFPSTRU));
    ph= &current_seghead;
    /* O header  corrente foi convertido para formato UNIX. Como
       vai continuar em memoria, precisa voltar para DOS
    */
#if TRACE_HEADER
    printf("Convertendo para dos blk=%"_LD_" off=%"_LD_" \n",current_seghead.ifpblk,off_h);
#endif
    flag_current_seghead=TRUE;
 }
        if (reestrut_buff==TRUE ){
          memmove((UCHR *)&tmp,(UCHR *)&ifpbuff->buff[gbuf],sizeof(IFPSTRU));
          next_blk=tmp.ifpblk;
          lifp_init_buff(ifpbuff,next_blk);
          memmove((UCHR *)&ifpbuff->buff[0],(UCHR *)&tmp,sizeof(IFPSTRU));
          gbuf=0;
          pb=(IFPSTRU *)&ifpbuff->buff[gbuf];
      }
 if (keep_head==TRUE)gbuf=0;
}
/*-----------------------------------------------------------------------*/
#if CICPP
void CIIFL :: get_room_for (int     qt,
                            BOOLEAN opt_keep_head)
#else /*CICPP*/
static void get_room_for (qt,opt_keep_head)
int qt;
BOOLEAN opt_keep_head;
#endif /*CICPP*/
{
 if (idxpst+qt-1>MAXPSTS+1 ) {
    if (BUFFER_FULL) {
       write_buffers(opt_keep_head,TRUE);
       gbuf++;
       }else {
         gbuf++;
     }
    next_blk++;
    idxpst=0;
    pb=(IFPSTRU *)&ifpbuff->buff[gbuf];
 }
}
/*-----------------------------------------------------------------------*/
#if CICPP
void CIIFL :: lifp_upd_pst_header(IFPSTRU *p,
                                  INFO     offset,
                                  INFO     nxtb,
                                  INFO     nxtp,
                                  INFO     totp,
                                  INFO     segp,
                                  INFO     segc)
#else /*CICPP*/
static void  lifp_upd_pst_header(p, offset,nxtb,nxtp,totp,segp,segc)
IFPSTRU *p;
INFO offset;
INFO nxtb,nxtp,totp,segp,segc;
#endif /*CICPP*/
{
 IFPHEAD *head;
 head=(IFPHEAD *)&p->ifprec[offset];
 head->ifpnxtb=nxtb;
 head->ifpnxtp=nxtp;
 head->ifptotp=totp;
 head->ifpsegp=segp;
 head->ifpsegc=segc;
#if CNV_PCBINUM
    ifp_ins_new_hd(p->ifpblk, offset,hdblk,hdoff, IFLmaxhd,&hdn);
#endif
}
/*-----------------------------------------------------------------------*/
#if CICPP
void CIIFL :: lifp_init_counters()
#else /*CICPP*/
static void lifp_init_counters()
#endif /*CICPP*/
{
    nxtb=0;
    nxtp=0;
    segc=0;
    segp=0;
    totp=0;
    npst=0;
}
#endif /* !LIND */
/*------------------------------------------------------------------------*/
#if CICPP
void CIIFL :: init_invp_roots_nodes_leaves(INVMAP *invp)
#else /*CICPP*/
static void   init_invp_roots_nodes_leaves(invp)
INVMAP *invp;
#endif /*CICPP*/
{
  L0STRU *l0p;
  N0STRU *n0p;
  char keytmp[1];/* Somente para chamar create_root */
  int treecase;
  PUNT esq,dir;
  keytmp[0]='\0';
  /* Inicializa elementos "brancos" para os dois tipos de nos e
     folhas
 */
  memset(init_leaf_el1.key,' ',LE1);
  init_leaf_el1.info1=(INFO )0;
  init_leaf_el1.info2=(INFO )0;
  memset(init_leaf_el2.key,' ',LE2);
  init_leaf_el2.info1=(INFO )0;
  init_leaf_el2.info2=(INFO )0;
  memset(init_node_el1.key,' ',LE1);
  init_node_el1.punt=(INFO )0;
  memset(init_node_el2.key,' ',LE2);
  init_node_el2.punt=(INFO )0;
  /* Inicializa as duas arvores do dicionario
  */
  for (treecase=0;treecase<2;treecase++) {
#if LKXONLY
    if (treecase == 0) if (lkxonly & 2) continue;
    if (treecase == 1) if (lkxonly & 1) continue;
#endif
   if (treecase==0) l0p=(L0STRU *)&l1node;
            else    l0p=(L0STRU *)&l2node;
   l0p->ock=0;
   l0p->ps=0;
#if LIND
   l0p->psb=0;
#endif /* LIND */
   l0p->it=treecase+1;
   l0p->pos=1;
   invp->cn[treecase].fmaxpos=1;
 }
  /* Cria as duas primeiras raizes das arvores
     colocando o primeira chave como Branca e
     inicializa a pilha onde aponta para os niveis
     da arvore criados
  */
  for (treecase=0;treecase<2;treecase++) {
#if LKXONLY
    if (treecase == 0) if (lkxonly & 2) continue;
    if (treecase == 1) if (lkxonly & 1) continue;
#endif
    n0p=(N0STRU *)&load_idx[treecase].idx_node[0];
    dir=0;
    esq=invp->cn[treecase].fmaxpos;
    lifp_create_root(invp,n0p,-esq,(char *)keytmp,dir,treecase,TRUE);
    load_idx[treecase].top=0;
  }
}
/*------------------------------------------------------------------------*/
#if CICPP
void CIIFL :: lifp_init_leaf(L0STRU *lp,
                             int     treecase)
#else /*CICPP*/
static void lifp_init_leaf(lp,treecase)
L0STRU *lp;
int treecase;
#endif /*CICPP*/
{
  L1STRU *l1p;
  L2STRU *l2p;
  int i;
  l1p=(L1STRU *)lp;
  l2p=(L2STRU *)lp;
  for (i=0;i<TWORDF;i++){ /* indice comeca de zero*/
        if (treecase == 0 ) {
      l1p->idx[i]=init_leaf_el1;
    }
    else {
      l2p->idx[i]=init_leaf_el2;
        }
  }
}
/*------------------------------------------------------------------------*/
#if CICPP
void CIIFL :: lifp_init_node(N0STRU *np,
                             int     treecase)
#else /*CICPP*/
static void lifp_init_node(np,treecase)
N0STRU *np;
int treecase;
#endif /*CICPP*/
{
  N1STRU *n1p;
  N2STRU *n2p;
  int i;
  n1p=(N1STRU *)np;
  n2p=(N2STRU *)np;
  for (i=0;i<TWORDN;i++){ /* indice comeca de zero*/
    if (treecase == 0 ) {
      n1p->idx[i]=init_node_el1;
    }
    else {
      n2p->idx[i]=init_node_el2;
    }
  }
}
/* -----------------------------------------------------------------------
 .Criar uma nova raiz
 .Se first=true cria a raiz com apenas um elemento igual a branco
   (Isto corresponde a inicializar o arquivo invertido).
 .Se first=false cria uma nova raiz que contem oprimeiro elemento branco
  e o segundo elemento a chave promovida de elementos de um nivel mais baixo
 -------------------------------------------------------------------------*/
#if CICPP
void CIIFL :: lifp_create_root(INVMAP *invp,
                      N0STRU *n0p,
                      PUNT    esq,
                      char   *key,
                      PUNT    dir,
                      int     treecase,
                      BOOLEAN first)
#else /*CICPP*/
static void lifp_create_root(invp,n0p,esq,key,dir,treecase,first)
INVMAP *invp;
N0STRU *n0p;
PUNT esq;
char *key;
PUNT dir;
int treecase;
BOOLEAN first;
#endif /*CICPP*/
{
 N1STRU *n1p;
 N2STRU *n2p;
 PUNT nodenumber;
 int keysize,ind;
 n1p=(N1STRU *)n0p;
 n2p=(N2STRU *)n0p;
 lifp_init_node(n0p,treecase);
 keysize=vlex[treecase];
 nodenumber=invp->cn[treecase].nmaxpos+1;
 invp->cn[treecase].nmaxpos++;
 n0p->it =treecase+1;
 n0p->pos = nodenumber;
 n0p->ock =2;
 if (first==TRUE)n0p->ock=1;
 ind=0;
 if (treecase == 0) {
   n1p->idx[ind]=init_node_el1;    /* Nao e necess. pq foi inicilizado*/
   n1p->idx[ind].punt=esq;         /* altera apontador */
 }
 else {
   n2p->idx[ind]=init_node_el2;    /* Nao e necess. pq foi inicilizado*/
   n2p->idx[ind].punt=esq;         /* altera apontador */
 }
 if (first==FALSE) {
   ind++;
   if (treecase == 0) {
     memmove(n1p->idx[ind].key,key,keysize);
     n1p->idx[ind].punt=dir;
   }
   else {
     memmove(n2p->idx[ind].key,key,keysize);
     n2p->idx[ind].punt=dir;
   }
 }
#if  TRACELOAD
   printf("\n Vai impirmir a raiz criada ");
   upif_print_node(n0p);
#endif
}
/* -----------------------------------------------------------------------*/
#if CICPP
BOOLEAN CIIFL :: lifp_insert_leaf(DBXSTRU *dbxp,
                                  char *key,
                                  char *p_b_key,
                                  PUNT *p_b_punt,
                                  int   treecase,
                                  INFX  info1,
                                  INFO  info2)
#else /*CICPP*/
static BOOLEAN lifp_insert_leaf(dbxp,key,p_b_key,p_b_punt,treecase,info1,info2)
DBXSTRU *dbxp;
char *key;
char *p_b_key;
PUNT *p_b_punt;
 int treecase;
INFX info1;
INFO info2;
#endif /*CICPP*/
{
 INVMAP *invp;
 PUNT leafnumber;
 int ock,keysize;
 L1IDXE leaf_el1;
 L2IDXE leaf_el2;
 L0STRU *l0p;
 invp=DBXifmap;
 keysize=vlex[treecase];
 if (treecase == 0) {
   memmove(leaf_el1.key,key,keysize);
   leaf_el1.info1=info1;
   leaf_el1.info2=info2;
#if LIND
   leaf_el1.info3info4.info3=tlcinfo3;
#endif /* LIND */
   l0p=(L0STRU *)&l1node;
   l1p=(L1STRU *)&l1node;
 }
 else {
   memmove(leaf_el2.key,key,keysize);
   leaf_el2.info1=info1;
   leaf_el2.info2=info2;
#if LIND
   leaf_el2.info3info4.info3=tlcinfo3;
#endif /* LIND */   
   l0p=(L0STRU *)&l2node;
   l2p=(L2STRU *)&l2node;
 }
 ock=l0p->ock;
 if (ock<TWORDF){
   if (treecase == 0) {
      l1p->idx[ock] = leaf_el1;    /* indice comeca de 0 */
   }
   else {
      l2p->idx[ock] = leaf_el2;
   }
   l0p->ock++;                    /* Atualiza ocorrencias       */
#if TRACELOAD
   printf("\n<insert_leaf> Apos insercao ");
   upif_print_leaf(l0p);
#endif
  return (FALSE);                          /* Nao precisou fazer split   */
 }
 /* Nao cabe na pagina.
    Grava a pagina atual.
    Inicializa nova pagina.
  */
 l0p->ps=invp->cn[treecase].fmaxpos+1;
 leafwrit(dbxp,l0p);
 invp->cn[treecase].fmaxpos++;
 leafnumber=invp->cn[treecase].fmaxpos;
 lifp_init_leaf(l0p,treecase);
 l0p->ock=1;
 l0p->it=treecase+1;
 l0p->pos=leafnumber;
 l0p->ps=0;                  /* Supoe que e a ultima */
#if LIND
 l0p->psb=leafnumber-1;
#endif /* LIND */
 ock=0;                    /* O prinmeiro elemento esta na posicao 0 */
 if (treecase == 0 ) {
     l1p->idx[ock]=leaf_el1;
   }
   else {
     l2p->idx[ock]=leaf_el2;
   }
#if TRACELOAD
   printf("\n<insert_leaf> Apos insercao na nova ");
   upif_print_leaf(l0p);
#endif
 /* Retorna o primeiro item da nova pagina  ser promovido */
 if (treecase == 0) {
    memmove(p_b_key,l1p->idx[0].key,keysize);
 }
 else {
    memmove(p_b_key,l2p->idx[0].key,keysize);
 }
 *p_b_punt= -leafnumber;  /* numero da nova folha inserida */
 return(TRUE);
}
/*-------------------------------------------------------------------------
 lifp_insert_node:
/*-------------------------------------------------------------------------*/
#if CICPP
BOOLEAN CIIFL :: lifp_insert_node (DBXSTRU *dbxp,
                                   N0STRU  *n0p,
                                   int      treecase,
                                   int      level,
                                   int      isroot,
                                   UCHR    *b_key,
                                   PUNT     b_punt,
                                   UCHR    *p_key,
                                   PUNT    *p_punt)
#else /*CICPP*/
static BOOLEAN lifp_insert_node (dbxp,n0p,treecase,level,isroot,
                        b_key,b_punt,p_key,p_punt)
DBXSTRU *dbxp;
N0STRU *n0p;
int treecase;
int level;
int isroot;
UCHR *b_key;
PUNT b_punt;
UCHR *p_key;
PUNT *p_punt;
#endif /*CICPP*/
{
 INVMAP *invp;
 UWORD ock;
 PUNT nodenumber;
 int keysize;
 static N1STRU *n1p;
 static N2STRU *n2p;
 N1IDXE no1;
 N2IDXE no2;
 invp=DBXifmap;
 keysize=vlex[treecase];
 no1=init_node_el1;
 no2=init_node_el2;
 if (treecase == 0){
   memmove(no1.key,b_key,keysize);
   no1.punt=b_punt;
 }
 else {
   memmove(no2.key,b_key,keysize);
   no2.punt=b_punt;
 }
 n1p=(N1STRU *)n0p;
 n2p=(N2STRU *)n0p;
 ock = n0p->ock;        /* Ock comeca de 1 e indice de 0 */
 if (ock < TWORDN) {            /* Cabe na pagina */
    if (treecase == 0) {
      n1p->idx[ock] = no1;
   }
   else {
      n2p->idx[ock] = no2;
   }
   ock++;
   n0p->ock++;         /* Atualiza ocorrencias       */
#if TRACELOAD
   printf("\n<insert_node> Apos insercao ");
   upif_print_node(n0p);
#endif
   return (FALSE);    /* Nao precisou fazer split   */
 }
 /* Nao cabe na pagina.
    grava a atual e
    inicializa uma nova pagina
  */
 nodenumber= invp->cn[treecase].nmaxpos+1; /* Proxima posicao livre de idx */
 invp->cn[treecase].nmaxpos++;
 last_root=n0p->pos; /* Sera usada para criar uma nova raiz */
 nodewrit(dbxp,n0p,level,isroot);
 lifp_init_node(n0p,treecase);
 n0p->ock=1;
 n0p->it=treecase+1;
 n0p->pos=nodenumber;
 ock=0; /* indice comeca de zero */
 if (treecase == 0) {
     n1p->idx[ock] = no1;
 }
 else {
      n2p->idx[ock] = no2;
 }
#if TRCBTREE
  printf("\n<insert_node> Apos insercao ");
   upif_print_node(n0p);
#endif
 /* Retorna o primeiro item da nova pagina  ser promovido */
 if (treecase == 0) {
    memmove(p_key,n1p->idx[0].key,keysize);
 }
 else {
    memmove(p_key,n2p->idx[0].key,keysize);
 }
 *p_punt=nodenumber;                   /* numero da folha definida  ??*/
 return(TRUE);
}
/*-----------------------------------------------------------------*/
#if CICPP
void CIIFL :: lifp_store_btree(UCHR *key,
                               INFX  blk,
                               INFO  off,
                               int   treecase)
#else /*CICPP*/
static void lifp_store_btree(key,blk,off,treecase)
UCHR *key;
INFX blk;
INFO off;
int treecase;
#endif /*CICPP*/
{
  keysize=vlex[treecase];
   promoted=lifp_insert_leaf(
            dbxp,(char *)key,(char *)p_b_key,&p_b_punt,treecase,blk,off);
  topo=load_idx[treecase].top;
  isroot=FALSE;
  /* Sempre que houver uma promocao devera ser inserida sucessivamente
     nos nos dos indices.
  */
  for (level=0; level<=topo && promoted==TRUE; level++){
     memmove(key,p_b_key,keysize);
     key_punt=p_b_punt;
     n0p=(N0STRU *)&(load_idx[treecase].idx_node[level]);
#if TRACELOAD
     upif_chgtostr(p_b_key,keysize,tkey); /* AOT 23/10/93 */
     printf("\nfnd Vai promover para node=%p keypunt=%"_LD_" key=%s",
				n0p,key_punt,tkey);
#endif
      promoted=lifp_insert_node(dbxp,n0p,treecase,level,isroot,
						  key,key_punt,
						  p_b_key,&p_b_punt);
  } /* for  */
	/* se houve promocao ate a raiz cria outra raiz           */
	/* o ultimo o elemento retirado da pilha a raiz           */
  if (promoted==TRUE){
    memmove(key,p_b_key,keysize);
    load_idx[treecase].top++;
    topo=load_idx[treecase].top;
    if (topo>=MAX_TREE_LEVEL) fatal("MAX_TREE_LEVEL/overflow");
    level=topo;
    n0p=(N0STRU *)&(load_idx[treecase].idx_node[topo]);
#if TRACELOAD
     upif_chgtostr(p_b_key,keysize,tkey); /* AOT 23/10/93 */
     printf("\n[Vai virar raiz]Vai promover para node=%"_LD_" keypunt=%"_LD_" key=%s",
				n0p,key_punt,tkey);
#endif
    memmove(key,p_b_key,keysize);
    key_punt=p_b_punt;
    lifp_create_root(invp,n0p,last_root,(char *)key,key_punt,treecase,FALSE);
  }
}
/*--------------------------------------------------------------------------*/
/* Precisa  gravar controles e todos os ultimos registros dos buffers
   aqui isroot deve ser true para o topo */
/* Precisa testar se foi a folha tem algum dado para gravar e
   acertar o CNT .
   Inicialmente o fmaxpos e nmaxpos aponta para 1. Porem pode
   acontecer dde nao ter tido nenhuma ocorrencia e entao precisa
   voltar a pontar par 0.
*/
#if CICPP
void CIIFL :: lifp_close_tree()
#else /* CICPP */
static void lifp_close_tree()
#endif /* CICPP */
{
   BOOLEAN treecase_occ[2];
   for (treecase=0;treecase<2;treecase++) {
#if LKXONLY
    if (treecase == 0) if (lkxonly & 2) continue;
    if (treecase == 1) if (lkxonly & 1) continue;
#endif
        if (treecase==0){
           l0p=(L0STRU *)&l1node;
           l1p=(L1STRU *)l0p;
          }else {
           l0p=(L0STRU *)&l2node;
           l2p=(L2STRU *)l0p;
	}
	treecase_occ[treecase]=TRUE;
	if(l0p->ock==0) {
	  if (invp->cn[treecase].fmaxpos == 1 ) {
		invp->cn[treecase].fmaxpos=0;
		invp->cn[treecase].nmaxpos=0;
		treecase_occ[treecase]=FALSE;
	  }else {
		fatal("lifp_close_tree/1");
	  }
	}else {
	   leafwrit(dbxp,l0p);
	}
 }
   for (treecase=0;treecase<2 ;treecase++){
#if LKXONLY
    if (treecase == 0) if (lkxonly & 2) continue;
    if (treecase == 1) if (lkxonly & 1) continue;
#endif
     if(treecase_occ[treecase]==TRUE ){
       topo=load_idx[treecase].top;
       isroot=FALSE;
       for (i=0;i<=topo;i++){
	  if(i==topo) isroot=TRUE;
	  n0p=(N0STRU *)&(load_idx[treecase].idx_node[i]);
	  nodewrit(dbxp,n0p,topo,isroot);
       }
     }
   }
}
#if !LIND
/*--------------------------------------------------------------------*/
#if CICPP
void CIIFL :: lifp_grava_ifp(void)
#else /* CICPP */
static void lifp_grava_ifp()
#endif /* CICPP */
{
    BOOLEAN reestrut_buff;
      /* Atualiza o primeiro registro do arquivo */
	key_blk=pn.nxtb;
	key_pos=pn.nxtp;
	if (flag_first_seghead==TRUE) {
	   firsthead=(IFPHEAD *)&pf->ifprec[off_f];
	   firsthead->ifptotp+=npst;
#if CNV_PCBINUM
           ifp_ins_new_hd(pf->ifpblk, off_f,hdblk,hdoff, IFLmaxhd,&hdn);
#endif
	}
/* 	lifp_upd_pst_header(ph,key_pos,nxtb,nxtp,npst,npst,npst); */
        lifp_upd_pst_header(ph,off_h,nxtb,nxtp,npst,npst,npst);  /*19-02-95*/
#if TRCPST
        printf("\nkey_ant=%s blk=%"_LD_" pos=%"_LD_" pst=%d",key_ant,
                key_blk,key_pos,npst);
#endif
        /* Carrega dicionario passando (key_ant,key_blk,key_pos); */
        /* Esta parte esta ineficiente por causa da quebra */
        if (*key_ant !='\0')
             lifp_store_btree(key_ant,key_blk,key_pos,treecase_ant);
        reestrut_buff=FALSE;
        if (flag_first_seghead ==TRUE || flag_current_seghead==TRUE) {
           if (gbuf>0) reestrut_buff=TRUE;
           write_buffers(DONT_KEEP_HEADERS,reestrut_buff);
        }
        if (flag_current_seghead==TRUE) {
#if CNV_PCBINUM
#if TRCBUFF
 printf("\n [PH]");
#endif
	   ifp_ins_new_hd(ph->ifpblk, off_h,hdblk,hdoff, IFLmaxhd,&hdn);
	   ifpwrit(dbxp,(char *)ph,(LONGX )sizeof(IFPSTRU),hdblk,hdoff,hdn);
#else /* CNV_PCBINUM */
	   ifpwrit(dbxp,(char *)ph,(LONGX )sizeof(IFPSTRU));
#endif /* CNV_PCBINUM */ 
           flag_current_seghead=FALSE;
        }
        if (flag_first_seghead==TRUE) {
#if CNV_PCBINUM
#if TRCBUFF
 printf("\n [PF]");
#endif
	   ifp_ins_new_hd(pf->ifpblk, off_f,hdblk,hdoff, IFLmaxhd,&hdn);
	   ifpwrit(dbxp,(char *)pf,(LONGX )sizeof(IFPSTRU),hdblk,hdoff,hdn);
#else /* CNV_PCBINUM */
           ifpwrit(dbxp,(char *)pf,(LONGX )sizeof(IFPSTRU));
#endif /* CNV_PCBINUM */
           flag_first_seghead=FALSE;
        }
              /* acho que tem zer log no inicio apos setar os pn*/
        get_room_for(IDXHEADSIZE + PSTSIZE,DONT_KEEP_HEADERS);
       lifp_upd_first_rec(&pn,next_blk,idxpst);
}
/*------------------------------------------------------------------------*/
#if CICPP
void CIIFL :: lifp_storepst(void)
#else /* CICPP */
static void lifp_storepst()
#endif /* CICPP */
{
    if (strcmp((CONST char *)key_ant,(CONST char *)key)!=0 ) {
        lifp_grava_ifp();
        /* guarda onde comeca o primeiro registro  */
       ph= &ifpbuff->buff[gbuf];
       off_h=idxpst;
       lifp_init_counters();
       lifp_upd_pst_header(ph,off_h,nxtb,nxtp,totp,segp,segc);
       idxpst=idxpst + IDXHEADSIZE;
       treecase_ant=treecase;
       strcpy((char *)key_ant,(CONST char *)key);
  }
 /*  if (npst > MAXPSTSSEG) { */
  if (npst > MAXPSTSSEG-1) {    /*19-02-95*/
     get_room_for(IDXHEADSIZE + PSTSIZE,KEEP_HEADERS);
     nxtb=next_blk;
     nxtp=idxpst;
     segc=npst;
     segp=npst;
     totp=npst;
     if (flag_first_seghead==FALSE ) {
        lifp_upd_pst_header(ph,off_h,nxtb,nxtp,totp,segp,segc);
        memmove((UCHR *)pf,(UCHR *)ph,sizeof(IFPSTRU));
        flag_first_seghead=TRUE;
        flag_current_seghead=FALSE; /*19-02-95*/
        off_f=off_h;
     }else {
        /* atualiza o total de postings do primeiro segmento */
        firsthead= (IFPHEAD *)&pf->ifprec[off_f];
        firsthead->ifptotp+=npst;
#if CNV_PCBINUMxx
        ifp_ins_new_hd(pf->ifpblk, off_f,hdblk,hdoff, IFLmaxhd,&hdn);
#endif
        /* Atualiza apontador do segmento atual */
        lifp_upd_pst_header(ph,off_h,nxtb,nxtp,totp,segc,segp);
        /* Precisa gravar este header porque ele podera estar apontando
           para area de memoria temporaria a qual podera ser ncecessaria
           por um novo segment header
        */
#if CNV_PCBINUM
#if TRCBUFF
 printf("\n [PH2]");
#endif
	ifp_ins_new_hd(ph->ifpblk, off_h,hdblk,hdoff, IFLmaxhd,&hdn);
	ifpwrit(dbxp,(char *)ph,(LONGX )sizeof(IFPSTRU),hdblk,hdoff,hdn);
#else /* CNV_PCBINUM */
        ifpwrit(dbxp,(char *)ph,(LONGX )sizeof(IFPSTRU));
#endif /* CNV_PCBINUM */
        flag_current_seghead=FALSE; /*19-02-95*/
        /* Inicializa o novo header */
     }
   lifp_init_counters();
   ph= &ifpbuff->buff[gbuf];
   off_h=idxpst;
   currenthead=(IFPHEAD *)&ph->ifprec[off_h];
   lifp_upd_pst_header(ph,off_h,nxtb,nxtp,totp,segc,segp);
   idxpst=idxpst+IDXHEADSIZE;
   }
    get_room_for(PSTSIZE,KEEP_HEADERS);
    memmove((UCHR *)&pb->ifprec[idxpst],&pst,sizeof(POSTSTRU));
    idxpst=idxpst+PSTSIZE;
    npst++;
}
#endif /* !LIND */

/* -----------------------------------------------------------------------*/

#if CICPP
int CIIFL :: lifp_trans_key(UCHR *keyp,
                            int   len,
                            int  *treecase)
#else /*CICPP*/
static int lifp_trans_key(keyp,len,treecase)
UCHR *keyp;
int len;
int *treecase;
#endif /*CICPP*/
{
 int k,max;
 if (len <= 0) len=strlen((CONST char *)keyp); /* good */
 /* Adapta para tamanho das chaves */
 max= (len <= LE1)? LE1 : LE2 ;
 *treecase= (max == LE1)? 0: 1;
 for (k=len;k<max;k++) keyp[k]=' ';
 keyp[max]='\0';
 return(len);
}
/*-------------------------------------------------------------------------*/

/*------------------------------------------------------------------------
  svdifload
  Rotina principal, em que devem ser passados o database e o arquivo que
  contem as chaves e atualizar a base de dados.
  O arquivo invertido deve estar vazio.
----------------------------------------------------------------------------*/
/*
	30.Nov.1998 - Author: RP
   -	Entrada dos dados para carga do arquivo invertido via uma base de dados
   	ordenada;
   -	O parametro tell = -1, indica esta forma;
   -	O parametro filekeys_1 indica a base dados ordenada com as chaves curtas
   	(LE1) primeiro e com as longas (LE2) na sequencia;
   -	O parametro filekeys_2 indica os dois tags necessarios para a leitura dos
   	dados: tagkey e tagpost
   -	Para localizar as mudancas procure: load_type
*/
/*
	16.Jun.1999 - Author: RP
   -  Ao inv�s de trabalhar com 2 tags agora trabalha s� com 1 no formato fixo.
   -  Exemplo:
   		n|key       |  mfn   | tag | occ| cnt
   		1|CHAVE10   |12345678|12345|1234|1234
         2|CHAVE MAIOR QUE 10            |12345678|12345|1234|1234
   -	O parametro filekeys_2 indica o tag necessario para a leitura dos
   	dados: tagkey
   -	Para localizar as mudancas procure: postsize
*/

#if CIIFLFIX
#if !CICPP
int ciiflfix=0;                 /* key %8"_LD_" %5d %4d %4d */
int ciiflfim=0;                 /* key %8"_LD_" w/ ciiflfix */
#endif /* CICPP */
#endif

#if LIND
#if CICPP
LONGX CIIFL :: svdiflind (char *dbnp,
                         char *filekeys_1,
                         char *filekeys_2,
                         LONGX  parmxmfn,
                         char *parmlogp,
                         int   pstflag,
                         LONGX  tell)
#else /*CICPP*/
LONGX svdiflind (dbnp, filekeys_1,filekeys_2,parmxmfn,parmlogp,pstflag,tell)
char *dbnp;
char *filekeys_1;
char *filekeys_2;
LONGX parmxmfn;
char *parmlogp;
int pstflag;
LONGX tell;
#endif /*CICPP*/
#else
#if CICPP
LONGX CIIFL :: svdifload (char *dbnp,
                         char *filekeys_1,
                         char *filekeys_2,
                         int   pstflag,
                         LONGX  tell)
#else /*CICPP*/
LONGX svdifload (dbnp, filekeys_1,filekeys_2,pstflag,tell)
char *dbnp;
char *filekeys_1;
char *filekeys_2;
int pstflag;
LONGX tell;
#endif /*CICPP*/
#endif
{
#if MPE || UNIX
 int fkeys;
 int mpei,mpelrecl;
 char mpeline[BUFSIZ];
 LONGX mpetag,mpeocc,mpecnt;
#else
 FILE *fkeys;
#endif
 int mpex;
 char *mpep;
 char *files[2],*filnamp;
 LONGX nregs[2];
 int nfile;
 LONGX mfn;
 unsigned short tag,occ,cnt;
 LONGX xtag,xocc,xcnt;	/* AOT&RP 20/05/98 */
 char ignora_char;
#if !LIND
 UWORD idxpst_ant;
 INFO next_blk_ant;
#endif /* !LIND */
 PUNT xroot;
 int xliv,cmp,cmp1;
/*POSTSTRU pst_ant_loc; */
 UCHR key_ant_loc[LE2+1];
#if CIIFLFIX
 char fixline[LE2+1+10+10+10+10+1+1];
 int n;
#endif
 char xbobk[LE2+1];
 int xbobl,xbobn;

   LONGX load_type = 0L; /* RP - 30/11/98 */
   int tagkey;
   RECSTRU *recp=NULL;
#if !CICPP
   LONGX idx;
#endif
   LONGX mfnread = 0L;
   int dirread;
   char auxread[128];
   FFI lenread; /* RP - 30/11/98 */
   LONGX postsize = 25;	/*	 	|  mfn   | tag | occ| cnt 	*/
   							/* 	|12345678|12345|1234|1234 	*/
                        /* 	1 + 8 + 1 +5+ 1+ 4+1 +4  	*/

#ifdef USE_INFO_SYS
	UCHR ansikey[LE2*2];
	BOOL oper_canceled=FALSE;
	char mess_str[255];
//	 char mfnstr[10];
//    int sys_info_open=0;
	UIW_STRING *procstr=NULL;
	UIW_PROGRESSBAR *progbar=NULL;
	if (wprogress) {
		procstr=((UIW_STRING*)wprogress->Get("PROCESSING_STRING"));
		progbar=((UIW_PROGRESSBAR*)wprogress->Get("PROG_BAR"));
		progbar->Assign(0,0);
		progbar->SetColor(2);
		}
#endif

 /* Tudo que tem no arquivo invertido sera perdido.
  */

   if (tell == -1L) { /* RP - 30/11/98 */
      load_type = tell;
      tell = 0L;
   }
   if (load_type == -1L) {
      sscanf(filekeys_2,"%d",&tagkey);
      if (tagkey < 1) fatal("ciifl/tagkey");
      filekeys_2 = filekeys_1;
#if CICPP
      try { recp = new RECSTRU(cisisxp); }
      catch (BAD_ALLOC) { fatal("ciifl/idx"); }
#else
      for (idx = maxnrec-1; vrecp[idx]; idx--);
      if (!idx) fatal("ciifl/idx");
      recallok(idx,BUFSIZ); /* posting + chave */
      recp=vrecp[idx];
#endif
   } /* RP - 30/11/98 */

  print_step=tell;
  files[0]=filekeys_1;
  files[1]=filekeys_2;
  nregs[0]=0L;
  nregs[1]=0L;
  key[0]='\0';
  /* keyp=(UCHR *)&key; */
  keyp=(UCHR *)key;
  trmifupd=IFUPDXLD; /* TRMIFLOAD */
#if BEFORE_INVFLUSH
#else
  invflush(dbnp);
#endif
#if LKXONLY
  lkxonly=0;    /* AOT - 14/05/98 */
  if (strcmp(filekeys_1,"void") == 0) lkxonly|=2;
  if (strcmp(filekeys_2,"void") == 0) lkxonly|=1;
#if CICPP
  if (!lkxonly || lkxonly & 1) trmisis0(cisisxp, dbnp);
#else /* CICPP */
  if (!lkxonly || lkxonly & 1) trmisis0(dbnp);
#endif /* CICPP */
#else
  trmisis0(dbnp);
#endif
  invsetup((unsigned char *)dbnp,0L,0L,0L);
  dbxp=dbxstorp(dbnp);
  invp=DBXifmap;
#if RESUMOLOAD
   printf("\n Situacao do Arquivo Invertido Antes de Atualizar ");
   lifp_print_cnt(invp);
#endif
#if !AOT
if (tell>0) {
   printf("\n Entrada na svdifload tell=%"_LD_" keys1=%s keys2=%s",
	   tell,files[0],files[1]);
}
#endif
   p_b_key=a_p_b_key;
   p_b_punt=0L;
   init_invp_roots_nodes_leaves(invp);
   ifpbuff=lifp_init(invp);
#if CICPP || IFLOADFUN
   if (ifpbuff == NULL) return(-9L);
#endif /* CICPP || IFLOADFUN */
#if LIND
    iyp_storinit(parmxmfn,parmlogp);
#endif /* LIND */
for ( nfile=0;nfile<2;nfile++) {
#ifdef USE_INFO_SYS
//    sys_info_open=0;
#endif
#if LKXONLY
    if (nfile == 0) if (lkxonly & 2) continue;
    if (nfile == 1) if (lkxonly & 1) continue;
#endif
 /*y*/ if (files[nfile]==NULL) goto no_file;
 /*y*/ if (*files[nfile]=='\0') goto no_file;

   if (load_type == 0L) { /* RP - 30/11/98 */
   filnamp=dbxcipar(ifllk_gidbnp,files[nfile],'=');
#if MPE || UNIX
   fkeys=OPEN(filnamp,O_RDONLY);
#else
   fkeys=fopen(filnamp,"r");
#endif
   if(!fkeys) {
#if CICPP || IFLOADFUN
     return(-1L);
#else /* CICPP || IFLOADFUN */
     fatal(filnamp);
#endif /* CICPP || IFLOADFUN */
   }
   if (tell>0) {
#if AOT
#ifdef USE_INFO_SYS
// now in wisis.cpp
//     errsys->sys_info(filnamp);
//     sys_info_open=1;
#else
     fprintf(stderr,"+++ %s\n",filnamp);
#endif
#else
     printf("\n-------------------------------------------------------");
     printf("\nProcessando arquivo:%s",filnamp);
#endif
   }
   } /* RP - 30/11/98 */
   key[0]='\0';
   key_ant_loc[0]='\0';
   memset((char *)&pst_ant,0x00,sizeof(POSTSTRU));
   nlido=0L;

#if MPE || UNIX

#if UNIX
   mpelrecl=25+vlex[nfile]+1; /*        8+1 + 5+1 + 4+1 + 4+1 + LEx + lf */
#if CIIFLFIX
   if (ciiflfix)
      if (ciiflfim) mpelrecl-=16; /*      - ( 5+1 + 4+1 + 4+1 )          */
#endif
#else /* UNIX */
   mpelrecl=25+vlex[nfile]+1; /* null + 8+1 + 5+1 + 4+1 + 4+1 + LEx + lf */
#endif /* UNIX */

   while (1) {
    if (load_type == 0L) { /* RP - 30/11/98 */
     mpex=CIREAD(fkeys,mpeline,mpelrecl);
     if (mpex == 0) break;
     if (mpex != mpelrecl) {
#if CICPP || IFLOADFUN
	return(-6L);
#endif /* CICPP || IFLOADFUN */
	fatal("CIIFL/read");
     }
     mpeline[mpelrecl-1]='\0';
     mpep=mpeline;
#if UNIX
#if CIIFLFIX
     if (ciiflfix) {
	 memmove(keyp,mpep,mpei=vlex[nfile]); mpep+=mpei;
	 if (ciiflfim) {
	    mpex=sscanf(mpep," %8"_LD_,&mfn); mpex+=3; mpetag=mpeocc=mpecnt=1;
	 } else
			/* por %"_LD_" para mpetag,mpeocc,mpecnt - AOT 21/05/98 */
        /*mpex=sscanf(mpep," %8"_LD_" %5"_LD_" %4"_LD_" %4"_LD_,&mfn,&mpetag,&mpeocc,&mpecnt);*/
	    mpex=sscanf(mpep," %8"_LD_" %5d %4d %4d",&mfn,&mpetag,&mpeocc,&mpecnt);
     }
     else {
#endif
     /*mpex=sscanf(mpep,"%8"_LD_" %5"_LD_" %4"_LD_" %4"_LD_" ",&mfn,&mpetag,&mpeocc,&mpecnt);*/
	 mpex=sscanf(mpep,"%8"_LD_" %5d %4d %4d ",&mfn,&mpetag,&mpeocc,&mpecnt);
	 memmove(keyp,mpep+25,vlex[nfile]);
#if CIIFLFIX
     }
#endif
#else /* UNIX */ 
			/* por %"_LD_" para mpetag,mpeocc,mpecnt - AOT 21/05/98 */
     /* mpex=sscanf(mpep+1,"%8"_LD_" %5"_LD_" %4"_LD_" %4"_LD_" ",&mfn,&mpetag,&mpeocc,&mpecnt); */
     mpex=sscanf(mpep+1,"%8"_LD_" %5d %4d %4d ",&mfn,&mpetag,&mpeocc,&mpecnt);
     memmove(keyp,mpep+25,vlex[nfile]);
#endif /* UNIX */
     if (mpex != 4) {
#if CICPP || IFLOADFUN
	 return(-2L);
#endif /* CICPP || IFLOADFUN */
	 printf("*** File: %s  Rec: %"_LD_" \n",filnamp,nlido+1);
	 printf("*** %s \n",mpeline);
	 fatal("CIIFL/invalid link record/mpe");
     }
     tag=mpetag; occ=mpeocc; cnt=mpecnt;
     keyp[vlex[nfile]]='\0';
    } else { /* RP - 30/11/98 */
#if CICPP
      recp->xrecord(files[nfile],++mfnread);
      if (recp->recrc == RCEOF) break;
      memset(keyp,' ',vlex[nfile]);
      dirread = recp->xfieldx(tagkey,1);
#else /* CICPP */
      record(idx,files[nfile],++mfnread);
      if (RECrc == RCEOF) break;
      memset(keyp,' ',vlex[nfile]);
      dirread = fieldx(idx,tagkey,1);
#endif /* CICPP */
      if (dirread < 0) fatal("ciifl/fieldx/tagkey");
      if (*(FIELDP(dirread)) - '1' > nfile) break;
      memmove(keyp,FIELDP(dirread)+2,vlex[nfile]);
      keyp[vlex[nfile]] = '\0';

      memmove(auxread,FIELDP(dirread)+2+vlex[nfile],postsize);
      auxread[postsize] = '\0';
	 	sscanf(auxread,"|%8"_LD_"|%5"_LD_"|%4"_LD_"|%4"_LD_,&mfn,&xtag,&xocc,&xcnt);
		tag = (unsigned short)xtag;
		occ = (unsigned short)xocc;
		cnt = (unsigned short)xcnt;
    } /* RP - 30/11/98 */
     
#else /* MPE || UNIX */
#ifdef USE_INFO_SYS
	progbar->SetColor(nfile+1);
	progbar->Assign(0,0);
#endif

   while (1) {
  	 if (load_type == 0L) { /* RP - 30/11/98 */
   	 if (feof(fkeys)) break;
   /* O teste de fim de arquivo nao funciona. Processa um
      registro vazio. Precisa testar se o fscanf conseguiu
      ler algum campo. Volta -1 quando nao le
   */
#if CIIFLFIX
       if (ciiflfix) {
	if (fgets(fixline,sizeof(fixline),fkeys) == NULL) break;
	memmove(keyp,fixline,n=vlex[nfile]); keyp[n]='\0';
	if (ciiflfim) {
	    mpex=sscanf(fixline+n," %8"_LD_,&mfn); mpex+=3; tag=occ=cnt=1;
	}
	else {
	    /* %d/unsigned short nao funciona no 32bits - AOT&RP 20/05/98 */
	    mpex=sscanf(fixline+n," %8"_LD_" %5"_LD_" %4"_LD_" %4"_LD_,&mfn,&xtag,&xocc,&xcnt);
	}
	if (mpex != 4) {
#if CICPP || IFLOADFUN
	    return(-3L);
#else /* CICPP || IFLOADFUN */
	    printf("*** File: %s  Rec: %"_LD_" \n",filnamp,nlido+1);
	    printf("*** %s \n",fixline);
	    fatal("CIIFL/invalid link record/fix");
#endif /* CICPP || IFLOADFUN */
	}
       }
       else {
#endif /* CIIFLFIX */
	if (fscanf(fkeys,"%"_LD_,&mfn) < 1)
	    if (feof(fkeys)) break;
	    else {
#if CICPP || IFLOADFUN
		return(-4L);
#else /* CICPP || IFLOADFUN */
		fatal("CIIFL/invalid link record/mfn");
#endif /* CICPP || IFLOADFUN */
	    }
	fscanf(fkeys,"%"_LD_,&xtag);
	fscanf(fkeys,"%"_LD_,&xocc);
	fscanf(fkeys,"%"_LD_,&xcnt);
	fscanf(fkeys,"%c",&ignora_char);
	fscanf(fkeys,"%[^\n]\n",keyp);
#if CIIFLFIX
       }
#endif
       tag=(unsigned short)xtag;
       occ=(unsigned short)xocc;
       cnt=(unsigned short)xcnt;
    } else { /* RP - 30/11/98 */
#if CICPP
      recp->xrecord(files[nfile],++mfnread);
      if (recp->recrc == RCEOF) break;
      memset(keyp,' ',vlex[nfile]);
      dirread = recp->xfieldx(tagkey,1);
#else /* CICPP */
      record(idx,files[nfile],++mfnread);
      if (RECrc == RCEOF) break;
      memset(keyp,' ',vlex[nfile]);
      dirread = fieldx(idx,tagkey,1);
#endif /* CICPP */
      if (dirread < 0) fatal("ciifl/fieldx/tagkey");
      if (*(FIELDP(dirread)) - '1' > nfile) break;
      memmove(keyp,FIELDP(dirread)+2,vlex[nfile]);
      keyp[vlex[nfile]] = '\0';

      memmove(auxread,FIELDP(dirread)+2+vlex[nfile],postsize);
      auxread[postsize] = '\0';
	 	sscanf(auxread,"|%8"_LD_"|%5"_LD_"|%4"_LD_"|%4"_LD_,&mfn,&xtag,&xocc,&xcnt);
		tag = (unsigned short)xtag;
		occ = (unsigned short)xocc;
		cnt = (unsigned short)xcnt;
    } /* RP - 30/11/98 */
#endif /* MPE || UNIX */
       xbobl=strlen((CONST char *)keyp); memmove(xbobk,keyp,xbobl); xbobk[xbobl]='\0';
       xbobn=bobkey(xbobk,xbobl,(char *)keyp,LE2,1,1,1);
       lifp_trans_key(keyp,xbobn,&treecase);
       if (ciiflfix) if (ciiflfim) tag=occ=cnt=0; /* AOT 27/09/2001 */
       encodepst(&pst,mfn,tag,occ,cnt);
       if (!nfile && treecase || nfile && !treecase || keyp[0] <= ' ') {
#if CICPP || IFLOADFUN
	     return(-5L);
#else /* CICPP || IFLOADFUN */
	     printf("[%c]%s %8"_LD_" %5d %4d %4d\n",keyp[0],keyp,mfn,tag,occ,cnt);
	     fatal("CIIFL/invalid link record/key");
#endif /* CICPP || IFLOADFUN */
       }
/*   compara post anterior com atual para ver se esta classificado */
     cmp=strcmp((CONST char *)keyp,(CONST char *)key_ant_loc);
     if (cmp < 0) {
#if CICPP || IFLOADFUN
	return(-7L);
#else /* CICPP || IFLOADFUN */
	fatal("CIIFL/keys not sorted");
#endif /* CICPP || IFLOADFUN */
     }
#if !SUNBUG_UNSIGNED
     cmp1=memcmp((char *)&pst,(char *)&pst_ant,sizeof(POSTSTRU));
     if (cmp ==0 && cmp1<0) {
#if CICPP || IFLOADFUN
	return(-8L);
#else /* CICPP || IFLOADFUN */
#if 1
	printf("\n+++ key    : %s",keyp);
	printf("\n+++ pst    : ");
	for (mpep=(char *)&pst,     mpex=sizeof(POSTSTRU); mpex--; mpep++)
	       printf("%02x ",*mpep);
	printf("\n+++ pst_ant: ");
	for (mpep=(char *)&pst_ant, mpex=sizeof(POSTSTRU); mpex--; mpep++)
	       printf("%02x ",*mpep);
	printf("\n+++          ");
#endif
	fatal("CIIFL/keys/postings not sorted");
#endif /* CICPP || IFLOADFUN */
     }
#endif /* SUNBUG_UNSIGNED */
#if !LIND
     if (pstflag==IFUPDICT) {
	if (cmp!=0)
	   lifp_store_btree(keyp,1L,2L,treecase); /* carrega so chaves dif*/
     }else {
	lifp_storepst();
     }
#endif /* !LIND */
#if LIND
    iyp_storlast(cmp && nregs[nfile],key_ant_loc,pstflag);
    iyp_storthis(mfn,parmxmfn,keyp,nregs[nfile]+1,tag,occ,cnt);
#endif /* LIND */
     nregs[nfile]++;
     nlido++;
     if (tell > 0)
	if (nlido - (nlido/tell*tell)== 0)
#if AOT
#ifdef USE_INFO_SYS
				{
//	   errsys->sys_info_data(keyp); //ltoa(mfn,mfnstr,10));
		if (wprogress) {
			LONGX int prog_pos=ftell(fkeys);
			int fhandle=fileno(fkeys);
			LONGX fleng=filelength(fhandle);
			ISIS_OemToAnsi(key,ansikey);
			if (!nfile)
				sprintf(mess_str,"%s %s",mess->get(424),keyp);
			else
				sprintf(mess_str,"%s %s",mess->get(425),keyp);
			procstr->DataSet(mess_str);
			progbar->Assign(prog_pos,fleng);
	//		((UIW_STRING*)wprogress->Get("PHASE_STRING"))->DataSet(NULL);
			UI_EVENT event,event1,event2;
			wprogress->eventManager->Get(event1,Q_NO_BLOCK|Q_BEGIN|Q_DESTROY|Q_POLL);
			wprogress->windowManager->Event(event1);
			wprogress->eventManager->Get(event2,Q_NO_BLOCK|Q_BEGIN|Q_DESTROY|Q_POLL);
			wprogress->windowManager->Event(event2);
			wprogress->eventManager->Get(event,Q_NO_BLOCK|Q_BEGIN|Q_DESTROY|Q_POLL);
			if (event.type==10235 || event1.type==10235 || event2.type==10235) {		// EXPORT_CANCEL) {
					int ok=errsys->ErrorMessage(wprogress->windowManager,WOS_INVALID,mess->get(420));
					if (ok==WOS_INVALID) {
						oper_canceled=TRUE;
						break;
						}
					}
			wprogress->windowManager->Event(event);
			}
		}
#else
	   fprintf(stderr,"+++ %"_LD_" %s\n",nlido,keyp);
#endif
#else
	   printf("\n Rec:%"_LD_" Key:%s",nlido,keyp);
#endif
     strcpy((char *)key_ant_loc,(CONST char *)keyp);
     memmove((char *)&pst_ant,(char *)&pst,sizeof(POSTSTRU));
  }/*while*/
#if LIND
    iyp_storlast(nregs[nfile],key_ant_loc,pstflag);
#endif /* LIND */
 if (load_type == 0L) { /* RP - 30/11/98 */
#if MPE || UNIX
  CLOSE(fkeys);
#else
  fclose(fkeys);
#endif
 } else {
 	mfnread--;
 }/* RP - 30/11/98 */
  /* Esta parte e para forcar a mudar de segmento quando comeca a arvore do
	 tipo 2. Isto e para deixar compativel com ISIS. Nao existe nenhum
	 motivo serio fazer essa compatibilizacao
 */
 /*y*/ no_file:

#ifdef USE_INFO_SYS
// now in wisis.cpp
//     if (sys_info_open) errsys->sys_info_close();
#endif
#if !LIND
    /* Deve verificar se o arquivo .lk2 nao esta' vazio - AOT 950720 */
    if (nfile/*treecase*/==0) {  /* So vale qdo muda da arvore 1 para 2 */
       { /* Ainda nao comeca em novo bloco */
	  if (idxpst+(IDXHEADSIZE + PSTSIZE)-1 > MAXPSTS+1) {
	    /* O algoritmo ja vai quebrar */
	  }else {
	    idxpst=MAXPSTS+1; /* forcar mudanca de bloco */
	  }
       }
    }
#endif /* !LIND */
    ;  /* !LIND - label: ; */
#ifdef USE_INFO_SYS
	if (oper_canceled)
		break;	// exit!!
#endif

}/*for */

#if !LIND
/* Serao guardados idxpst e next_blk porque sera forcada a entrada de uma
   "falsa chave" para gravar o buffer .
   Na gravacao do buffer, next_blk e idxpst serao alterados. Serao guardados
   para atualizar o registro de controle [pn].
   O que e' feito pelo ISIS parece incoerente. Quando muda de arquivo com
   chaves do tipo 1 para o arquivo com chaves do tipo 2  muda
   de bloco.   Quando terminar o segundo arquivo acho que tambem deveria
   mudar, porem nao faz.
*/
    idxpst_ant =idxpst;
    next_blk_ant=next_blk;
    idxpst=MAXPSTS;
    gbuf=MAXIFPMEM-1;
    lifp_grava_ifp();
/*y*/
    /* Caso nao teve nenhum link do tipo 2, precisa acertar o next_blk_ant e
       idxpst_ant
    */
    if (idxpst_ant>MAXPSTS){
       idxpst_ant=0;
       next_blk_ant++;
    }
/*y*/
    lifp_upd_first_rec(&pn,next_blk_ant,idxpst_ant);
#if TRACELOAD
    printf ("\n pn=%"_LD_" %"_LD_" %"_LD_" ",pn.ifpblk,pn.nxtb,pn.nxtp);
#endif
#if CNV_PCBINUM
    ifpwrit(dbxp,(char *)&pn,(LONGX )sizeof(IFPAVAILPOS),hdblk,hdoff,hdn);
#else /* CNV_PCBINUM */ 
    ifpwrit(dbxp,(char *)&pn,(LONGX)sizeof(IFPAVAILPOS));
#endif /* CNV_PCBINUM */ 
#endif /* !LIND */

#if CORRECAO_SINDO
/*
	Nao ativei porque existe a possibilidade de o proprio ifupd/ciifu 
	gerar (pn.nxtp==0)
	
	Ja' alterei a postread para retornar um bloco vazio no EOF; mas
	mesmo assim a ciifu nao trata a seguinte situacao gerada pelo 
	MicroISIS:
		x com 60 registros
		x.fst=1 0 'chave'		-> gera 1 blk cheio
		isis/i/x/f			-> full I/F (ctrl=1/127)
		mx tmp append=x now count=1	-> cria #61
		ifupd x fst=@x.fst		-> atualiza (errado)
		ifkeys x			-> mostra bug (32 postings!)
	AOT
*/
/* 20-07-95  Inicio */
/*Sempre que  a proxima posicao disponivel estiver no inicio
 de um   bloco vazio, isto e, (pn.nxtp==0),
 e' necessario gravar um registro ifp. Caso contrario
 a rotina "postread" da' erro de leitura quando for
 ler esse bloco pois nao consegue ler 512 bytes.
*/
   if (pn.nxtp==0) {
     pb=(IFPSTRU *)&ifpbuff->buff[0];
     lifp_init_ifprec(pb,pn.nxtb);
#if CNV_PCBINUM
     /* Nao tem nada para converter */
     ifpwrit(dbxp,(char *)pb,(LONGX)sizeof(IFPSTRU),hdblk,hdoff,(-1));
#else /* CNV_PCBINUM */ 
     ifpwrit(dbxp,(char *)pb,(LONGX)sizeof(IFPSTRU));
#endif /* CNV_PCBINUM */ 
   }
/* 20-07-95  fim */
#endif

   lifp_close_tree();

/* Resumo das operacoes efetuadas */
#if RESUMOLOAD
   printf("\n Quantidade de Chaves Processadas");
   printf("\n   %s:%"_LD_"\n   %s:%"_LD_"\n   Total:%"_LD_"\n",
	  files[0],nregs[0],files[1],nregs[1],nregs[0]+nregs[1]);
   lifp_print_cnt(invp);
#endif

#if LIND
    iyp_storclos();
#endif /* LIND */
#if CICPP
    if (ifpbuff) delete[] ifpbuff;
#else /* CICPP */
    if (ifpbuff) FREE(ifpbuff); /* AOT - 16/09/93 */
#endif /* CICPP */

    /* Faz balanceamento da arvore */
    if (ifl_balan) {
	for (treecase=0;treecase<2;treecase++) {
#if LKXONLY
    if (treecase == 0) if (lkxonly & 2) continue;
    if (treecase == 1) if (lkxonly & 1) continue;
#endif
	    xroot=invp->cn[treecase].posrx;
	    xliv=invp->cn[treecase].liv;
	    balance_btree(dbxp,invp,xliv,xroot,treecase);
	}
    }

    cntwrit(dbxp);
    invflush(dbnp);

    trmifupd=0; /* reset IFUPDXLD */

#ifdef USE_INFO_SYS
	if (oper_canceled)
		return (-100);
#endif

    if (recp) { /* RP - 30/11/98 */
#if CICPP
        delete recp;
#else /* CICPP */
        FREE(vrecp[idx]); vrecp[idx] = NULL; nrecs--;
#endif /* CICPP */
    }

    return(nregs[0]+nregs[1]);
}

/*---------------------------------------------------------------------------
  svdifmerg - I/F merge: load a b-tree from existing I/F's
              AOT, 25/11/95
----------------------------------------------------------------------------*/
#if !CICPP
LONGX svdifmerg (dbnp,vifnamp,vifmfns,parmxmfn,parmlogp,pstflag,tell)
char *dbnp;
char *vifnamp[];
LONGX vifmfns[];
LONGX parmxmfn;
char *parmlogp;
int pstflag;
LONGX tell;
{
 LONGX nregs[2];
 int nfile;
 LONGX mfn;
 unsigned short tag,occ,cnt;
#if !LIND
 UWORD idxpst_ant;
 INFO next_blk_ant;
#endif /* !LIND */
 PUNT xroot;
 int xliv,cmp,cmp1;
 UCHR key_ant_loc[LE2+1];
 char xbobk[LE2+1];
 int xbobl,xbobn;

 int nifs;			/* no of I/Fs to merge */
 LONGX vifoff[MAXIFMRG];		/* posting's MFN offset */
 LONGX vtidx[MAXIFMRG];		/* index of vtrmp[] */
 int vkuse[MAXIFMRG];		/* to flag a key is to be merged */
 int nkuse;			/* no of vkuse[] active elements */
 char *usekp;			/* current key ptr */
 int xif;			/* I/F index */

 TRMSTRU *trmp;
 LONGX itrm;
 LONGX nlids[2];
 int lex,n;
 char *p;

#if !LIND
  if (!parmlogp) parmlogp=(char *)NULL;	/* no wrn - 21/05/98 */
#endif

 /* setup MFN offset */
#if LIND
 if (parmxmfn < 1) fatal("svdifmerg/parmxmfn");
#endif /* LIND */
 for (nifs=0, vifoff[0]=0; vifnamp[nifs]; nifs++) {
     if (vifmfns[nifs] < 0) fatal("svdifmerg/vifmfns");
     vifoff[nifs+1]=vifoff[nifs]+vifmfns[nifs];
#if 1
     p=dbxcipar(NULL,vifnamp[nifs],'=');
     if (tell) if (dbxcipok) {  /* AOT, 22/06/2001 */
	 n=trmtrace;
	 trmtrace=1;
	 invsetup((unsigned char *)vifnamp[nifs],0L,0L,atol(p));
	 trmtrace=n;
     }
#endif
 }
 if (vifoff[nifs] > parmxmfn) fatal("svdifmerg/vifoff");

 /* setup TRMSTRU shelf's */
 for (xif=0; xif < nifs; xif++) {
    for (vtidx[xif]=EOF, itrm=xif; ; itrm++)
	if (!vtrmp[itrm]) { vtidx[xif]=itrm; break; }
    if (vtidx[xif] == EOF) fatal("svdifmerg/maxntrm");
    if (pstflag == IFUPDICT) trmalloc(itrm,0L); /* don't load .ifp */
    else trmalloc(itrm,IFBSIZ); /* use VTRMrcase() before TERM() */ /* AOT, 22/06/2001 */
 }

  print_step=tell;
  nregs[0]=nlids[0]=0L;
  nregs[1]=nlids[1]=0L;
  key[0]='\0';
  keyp=(UCHR *)key;

  trmifupd=IFUPDXLD; /* TRMIFLOAD */

  invflush(dbnp);
  trmisis0(dbnp);
  invsetup(dbnp,0L,0L,0L);
  dbxp=dbxstorp(dbnp);
  invp=DBXifmap;

   p_b_key=a_p_b_key;
   p_b_punt=0L;
   init_invp_roots_nodes_leaves(invp);
   ifpbuff=lifp_init(invp);

#if LIND
    iyp_storinit(parmxmfn,parmlogp);
#endif /* LIND */

for ( nfile=0;nfile<2;nfile++) {

   lex=vlex[nfile];
   if (tell>0) {
     fprintf(stdout,"+++ merging %d-byte keys \n",vlex[nfile]);
   }

   /* REWIND(); */
   trmifupd=0; /* reset IFUPDXLD */
   for (xif=0; xif < nifs; xif++) {
	VTRMrcase(vtidx[xif])=nfile+1;
	TERM(vtidx[xif],vifnamp[xif],"!");
	vkuse[xif]=0;
   }
   trmifupd=IFUPDXLD; /* TRMIFLOAD */

   key[0]='\0';
   key_ant_loc[0]='\0';
   memset((char *)&pst_ant,0x00,sizeof(POSTSTRU));

   while (1) {

    /* READ(); */
    trmifupd=0; /* reset IFUPDXLD */
    for (xif=0; xif < nifs; xif++) {
	trmp=vtrmp[itrm=vtidx[xif]];
	if (vkuse[xif]) NXTERM(itrm); 
	while (TRMlcase != nfile) { NXTERM(itrm); if (TRMrc == RCEOF) break; }
        vkuse[xif]=0;
#if TRACE_MERGE_KEYS
printf("+++keys#%d=%s(%d)\n",xif,TRMkey,TRMrc);	
#endif
    }
    trmifupd=IFUPDXLD; /* TRMIFLOAD */

    usekp=highv;    
    for (xif=0; xif < nifs; xif++) {
	trmp=vtrmp[vtidx[xif]];
        if (memcmp(TRMkey,usekp,lex) < 0) usekp=TRMkey;
    }
     
    nkuse=0; 
    for (xif=0; xif < nifs; xif++) {
	trmp=vtrmp[vtidx[xif]];
	if (TRMrc == RCEOF) continue;
        if (memcmp(TRMkey,usekp,lex) == 0) {
	    vkuse[xif]++; 
	    nkuse++; 
	    if (pstflag == IFUPDICT) break; /* don't load .ifp */
	}
    }
    if (!nkuse) break;

    /* COPY(); */
    strcpy(keyp,usekp);
    
/* key loop */
    for (xif=0; xif < nifs; xif++) {
	if (!vkuse[xif]) continue;
	trmp=vtrmp[itrm=vtidx[xif]];
#if TRACE_MERGE_ADDKEY
printf("+++add#%d=%s=%s\n",xif,TRMkey,keyp);	
#endif

/* posting loop */
	for (;;) {
	    if (pstflag == IFUPDICT) {
	        mfn=tag=occ=cnt=1;
	    }
	    else {
		trmifupd=0; /* reset IFUPDXLD */
		if (posting(itrm,TRMpost+1) <= 0) break;
		trmifupd=IFUPDXLD; /* TRMIFLOAD */
		if (vifmfns[xif])
		    if (TRMpmfn > vifmfns[xif]) fatal("svdifmerg/TRMpmfn");
		mfn=TRMpmfn+vifoff[xif];
		tag=TRMptag;
		occ=TRMpocc;
		cnt=TRMpcnt;
	    }
#if TRACE_MERGE_ADDPST
printf("+++pst#%d %s#%"_LD_"=%"_LD_"%d,%d,%d\n",xif,keyp,TRMpost,mfn,tag,occ,cnt);
#endif

       xbobl=strlen((CONST char *)keyp); memmove(xbobk,keyp,xbobl); xbobk[xbobl]='\0';
       xbobn=bobkey(xbobk,xbobl,keyp,LE2,1,1,1);
       lifp_trans_key(keyp,xbobn,&treecase);

       encodepst(&pst,mfn,tag,occ,cnt);
       if (!nfile && treecase || nfile && !treecase || keyp[0] <= ' ') {
	     printf("%s %8"_LD_" %5d %4d %4d\n",keyp,mfn,tag,occ,cnt);
	     fatal("CIIFL/invalid link record/key");
       }

/*   compara post anterior com atual para ver se esta classificado */
     cmp=strcmp(keyp,key_ant_loc);
     if (cmp < 0) {
	fatal("CIIFL/keys not sorted");
     }
     cmp1=memcmp((char *)&pst,(char *)&pst_ant,sizeof(POSTSTRU));
     if (cmp ==0 && cmp1<0) {
	fatal("CIIFL/keys/postings not sorted/svdifmerg");
     }

#if !LIND
     if (pstflag==IFUPDICT) {
	if (cmp!=0)
	   lifp_store_btree(keyp,1L,2L,treecase); /* carrega so chaves dif*/
     }else {
	lifp_storepst();
     }
#endif /* !LIND */
#if LIND
    iyp_storlast(cmp && nregs[nfile],key_ant_loc,pstflag);
    iyp_storthis(mfn,parmxmfn,keyp,nregs[nfile]+1,tag,occ,cnt);
#endif /* LIND */

     nregs[nfile]++;
     strcpy((char *)key_ant_loc,keyp);
     memmove((char *)&pst_ant,(char *)&pst,sizeof(POSTSTRU));

	        if (pstflag == IFUPDICT) break;
		
/* posting loop */
	    }

/* key loop */
	}

     nlids[nfile]++;
     if (tell > 0)
	if (nlids[nfile] % tell == 0)
	   fprintf(stdout,"+++ %"_LD_"/%"_LD_" %s\n",nlids[nfile],nregs[nfile],keyp);

  }/*while*/

#if LIND
    iyp_storlast(nregs[nfile],key_ant_loc,pstflag);
#endif /* LIND */

#if !LIND
    if (nfile/*treecase*/==0) {  /* So vale qdo muda da arvore 1 para 2 */
       { /* Ainda nao comeca em novo bloco */
	  if (idxpst+(IDXHEADSIZE + PSTSIZE)-1 > MAXPSTS+1) {
	    /* O algoritmo ja vai quebrar */
	  }else {
	    idxpst=MAXPSTS+1; /* forcar mudanca de bloco */
	  }
       }
    }
#endif /* !LIND */

}/*for */

    /* flush merged I/F's and corresponding vtrmp[] */
    for (xif=0; xif < nifs; xif++) {
	invflush(vifnamp[xif]);
	itrm=vtidx[xif];
	FREE(vtrmp[itrm]); vtrmp[itrm]=NULL; ntrms--;
    }

#if !LIND
    idxpst_ant =idxpst;
    next_blk_ant=next_blk;
    idxpst=MAXPSTS;
    gbuf=MAXIFPMEM-1;
    lifp_grava_ifp();
    if (idxpst_ant>MAXPSTS){
       idxpst_ant=0;
       next_blk_ant++;
    }
    lifp_upd_first_rec(&pn,next_blk_ant,idxpst_ant);
#if CNV_PCBINUM
    ifpwrit(dbxp,(char *)&pn,(LONGX )sizeof(IFPAVAILPOS),hdblk,hdoff,hdn);
#else /* CNV_PCBINUM */ 
    ifpwrit(dbxp,(char *)&pn,(LONGX)sizeof(IFPAVAILPOS));
#endif /* CNV_PCBINUM */ 
#endif /* !LIND */

   lifp_close_tree();

#if LIND
    iyp_storclos();
#endif /* LIND */

    if (ifpbuff) FREE(ifpbuff); /* AOT - 16/09/93 */

    /* Faz balanceamento da arvore */
    if (ifl_balan) {
	for (treecase=0;treecase<2;treecase++) {
	    xroot=invp->cn[treecase].posrx;
	    xliv=invp->cn[treecase].liv;
	    balance_btree(dbxp,invp,xliv,xroot,treecase);
	}
    }

    cntwrit(dbxp);
    invflush(dbnp);

    trmifupd=0; /* reset IFUPDXLD */
    
     if (tell > 0) {
	   fprintf(stdout,"+++ %"_LD_"/%"_LD_" %d-byte keys/postings \n",nlids[0],nregs[0],LE1);
	   fprintf(stdout,"+++ %"_LD_"/%"_LD_" %d-byte keys/postings \n",nlids[1],nregs[1],LE2);
     }

    return(nregs[0]+nregs[1]);
}
#endif /* CICPP */

#if CICPP
CIIFL :: CIIFL(CISISX * parm_cisisxp)
{
 cisisxp = parm_cisisxp;
 ifllk_gidbnp = NULL;
 print_step=0L;
 ifpbuff=NULL; 
}
#endif /* CICPP */
