 /*-------------------------------------------------------------------------
ciifu.c
SVD 10-03-93
        Rotinas para Inserir/remover postings no arquivo .IFP

	AOT 08-03-95
        Operacao Multiusuario
---------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include "cisis.h"
#include "ciupi.h"

#if CICPP
#include <ciifu.hpp>
#include <cidbx.hpp>
#include <citrm.hpp>
#include <cirec.hpp>
#include <cifst.hpp>
#include <cirun.hpp>
#ifdef USE_ERROR_SYS
#include <ui_win.hpp>
#include <errorsys.hpp>
extern MY_ERROR_SYSTEM * errsys;
extern UIW_WINDOW * wprogress;
extern UIW_WINDOW *mainwindow;
extern MESS_SYSTEM *mess;
extern int DS_OperationStopped; // must be implemented in another way
#define USE_INFO_SYS
#include <textdb.hpp>
#endif
#endif /* CICPP */


#if !CICPP
int ifupd_reset=2;

int ifupd_wrnmsg=1;
#endif /* CICPP */

#define lnk_not_found_msg  "Warning! Link not found. Cannot delete"
#define lnk_dup_msg        "Warning! Duplicate link"

/*-----------------------------------------------------------------------*/
#define AOT            1

#define MULTI_IF         MULTI

#define SHTEST           0

#if !CICPP
#if SHTEST
static char shtest[CIMPL+1];
#endif /* SHTEST */
#endif /* CICPP */

#define TRACEOPT         0
#define TRACEUPIF        DEBIFUPD
#define TRACEUPIF2       0
#define MODIFIED         TRUE
#define NOT_MODIFIED     FALSE
#define PSTADDR(blk,idx) (POSTSTRU *)&blk->ifprec[idx]
#define HEAD(blk,off)    (IFPHEAD *)&blk->ifprec[off]
#define HEADSIZE         sizeof(IFPHEAD)
#define POSTSIZE         sizeof(POSTSTRU)
#define BLK(x)           x->ifpblk

#if !CICPP
static UCHR key[LE2+1],*keyp;
static INVMAP *invp;
static  DBXSTRU *dbxp;

#if LINDLUX /* 2 */
static int luxtree,luxn;
static LONGX luxpunt,luxpos,luxx,luxaddr;
static L0STRU *luxpagp;
static L1STRU *luxp1;
static L2STRU *luxp2;
static char *luxp;
/* <============================ */
#else /* LINDLUX 2 */
#if !LIND /* 8x2 */
typedef struct ifpkeydata {
        BOOLEAN  st;            /* record modified */
        IFPSTRU  reg;
} IFPKEYDATA;

#define  UNDEFINED   -1
#define  CURR_INP    0
#define  SEGH        1
#define  FIRST       2
#define  CURR_OUT    3
#define  PREG(x)     (IFPSTRU *)&(v[x].reg)
#define  BLKX(x)     BLK( (PREG(x)) )
#define  STATUS(x)   v[x].st

static IFPKEYDATA v[4];
static int idx_curr_inp,idx_segh,idx_first,idx_curr_out;
static BOOLEAN dupl_msg,ja_inseriu;
static POSTSTRU *pinitpst,initpst;
static UCHR buf[IFBSIZ],*pbuf;
static IFPAVAILPOS pn;
static INFO totp_inp,nxtb_inp,nxtp_inp,segp_inp,segc_inp,
	    accurate_totp,nxtb_out,nxtp_out,npst_read_in_seg,
	    off_fh_inp,off_ch_inp,blk_inp,off_inp,
	    idxpst_inp,idxpst_out;
static IFPSTRU *pcur_blk,*pcur_blk_out;
#endif /* !LIND 8x2 */
#endif /* LINDLUX 2 */
static LONGX nlidos;
#endif /* CICPP */

/*-------------------------------------------------------------------------*/
#if CNV_PCBINUM
/*25/07/97*/
/* Variaveis para guardar a localizacao ds headers de um segmento */
/* 128/(5+2) */
#define IFLmaxhd  IFPmaxhd*MAXIFPMEM + 1   /* headers em memoria */
#if ! CICPP
static int hdn=-1;       /* numero headers de um ifp */
static INFO hdblk[IFLmaxhd];
static INFO hdoff[IFLmaxhd];
#endif /* CICPP */
#endif /*CNV_PCBINUM */
/*-------------------------------------------------------------------------*/

#if ! CICPP
#if ANSI
void optlnk1(char *lnk1p,LONGX qtylk1,char *olnk1p,LONGX oqtylk1);
void optlnk2(char *lnk2p,LONGX qtylk2,char *olnk2p,LONGX oqtylk2);
void trace_records(LONGX tell);
void posiciona_inicio_seg(void);
void verif_grava_rec(DBXSTRU *dbxp,int idx );
PUNT upif_init_index(DBXSTRU *dbxp, int treecase, UCHR *key,INFO TSTblk,
                                         INFO TSToff);
PUNT  upif_create_root(DBXSTRU *dbxp,PUNT esq,UCHR *key,
        PUNT dir,int treecase,BOOLEAN first);
BOOLEAN upif_find_key_leaf(L0STRU *lp, UCHR *key, int treecase,
                           int *pos);
BOOLEAN upif_find_key_node(N0STRU *np,UCHR *key, int treecase,
               int *pos);
void upif_fnd_pifp(DBXSTRU  *dbxp,UCHR *key,int treecase,
					POSTSTRU *pst,UCHR oper,int PSTFLAG);
void upif_branqueia(UCHR *str, int size);
BOOLEAN upif_insert_index (DBXSTRU *dbxp,
        PUNT punt,int treecase,int level,int isroot,
        UCHR *b_key,PUNT b_punt,
        UCHR *p_key,PUNT *p_punt);
PUNT upif_search_btree(INVMAP *invp,int treecase, UCHR *key);
BOOLEAN upif_insert_leaf(DBXSTRU *dbxp,L0STRU *lp,UCHR *key,int treecase,
                                 UCHR *p_b_key,PUNT *p_b_punt,int pos,
                                 INFO blk,INFO off );
void upif_print_msg(POSTSTRU *ppst,UCHR *msg);
void upif_print_lfitem(int treecase,UCHR *lfitem);
void next_output_element(void);
#else
void optlnk1();
void optlnk2();
void trace_records();
void posiciona_inicio_seg();
PUNT upif_init_index();
PUNT upif_create_root();
BOOLEAN upif_find_key_leaf();
BOOLEAN upif_find_key_node();
void upif_fnd_pifp();
void upif_branqueia();
BOOLEAN upif_insert_index ();
PUNT upif_search_btree();
BOOLEAN upif_insert_leaf();
void upif_print_msg();
void upif_print_lfitem();
void next_output_element();
#endif
#if ANSI
void read_ifpcntrl(void);
void upif_delete_posting(POSTSTRU *ppst);
void upif_delete_elem(void);
void print_post(POSTSTRU *pst);
/* void verif_headers_x_current(IFPSTRU *pcurrent,BOOLEAN *update_flag); */
void update_frst_curr_headers(INFO nelem);
void update_ifpcntrl(INFO nxtb,INFO nxtp);
void next_output_blk(void );
void next_input_blk(void);
void next_input_seg(void);
void next_input_element(void );
void copia_posting(POSTSTRU *ppst,BOOLEAN  *ja_inseriu);
void insere_posting( int idx_ifprec, int idx, POSTSTRU *ppst);
void gethead_counters(IFPSTRU *p,INFO off,
                          INFO  *nxtb , INFO *nxtp,INFO *totp ,
                          INFO  *segp, INFO *segc);
void update_header(IFPSTRU *pifp,INFO off,
                   INFO  nxtb , INFO nxtp,INFO totp ,
                   INFO segp, INFO segc);
void create_new_segment(INFO segp,INFO segc);
void upif_split_segment(POSTSTRU *ppst);
void upif_find_segment(POSTSTRU *ppst);
void upif_insert_into_seg(POSTSTRU *ppst);
#if !LIND /* 11 */
void upif_process_new_key(POSTSTRU *ppst,INFO *blk,INFO *off);
void upif_process_posting(POSTSTRU *ppst,INFO blk, INFO off, UCHR operation);
#endif /* !LIND 11 */
#else
void read_ifpcntrl();
void upif_delete_posting();
void upif_delete_elem();
void print_post();
/* void verif_headers_x_current();*/
void update_frst_curr_headers();
void update_ifpcntrl();
void next_output_blk();
void next_input_blk();
void next_input_seg();
void next_input_element();
void copia_posting();
void insere_posting();
void gethead_counters();
void update_header();
void create_new_segment();
void upif_split_segment();
void upif_find_segment();
void upif_insert_into_seg();
#if !LIND /* 11 */
void upif_process_new_key();
void upif_process_posting();
#endif /* !LIND 11 */
#endif
#endif /* CICPP */

#include "ciifuh.c"
/*------------------------------------------------------------------------*/

#if CICPP
CIIFU :: CIIFU(CISISX *parm_cisisp)
{
 cisisxp = parm_cisisp;

 ifupd_reset=2; 
 ifupd_wrnmsg=0;
#if CNV_PCBINUM
 hdn=-1;       /* numero headers de um ifp */
#endif /*CNV_PCBINUM */

 idx_curr_inp=0;

}
#endif /* CICPP */

#if TRACEUPIF
#if CICPP
void CIIFU :: upif_print_lfitem(int   treecase,
                                UCHR *lfitem)
#else /*CICPP*/
void upif_print_lfitem(treecase , lfitem)
int treecase;
UCHR *lfitem;
#endif /*CICPP*/
{
  UCHR str [LE2+1];
  int keysize;
  L1IDXE  *lfi1;
  L2IDXE  *lfi2;
  INFO info1,info2;
  keysize = vlex[treecase];
  memmove (str, lfitem, keysize );
  str[keysize]='\0';
  if (treecase == 0) {
     lfi1 = (L1IDXE *)lfitem;
     info1=lfi1->info1;
     info2=lfi1->info2;
   }
   else {
     lfi2 = (L2IDXE *)lfitem;
     info1 = lfi2->info1;
     info2 = lfi2->info2;
   }
    printf(" (%s %"_LD_" %"_LD_")",str, info1, info2);
#if LIND /* x */
  if (treecase == 0) printf(" (%"_LD_")",lfi1->info3info4.info3);
  else               printf(" (%"_LD_")",lfi2->info3info4.info3);
#endif /* LIND x */
}
#endif
/*-----------------------------------------------------------------------*/
#if CICPP
void CIIFU :: upif_print_msg(POSTSTRU *pst,
                             UCHR     *msg)
#else /*CICPP*/
void upif_print_msg(pst,msg)
POSTSTRU *pst;
UCHR *msg;
#endif /*CICPP*/
{
  if (ifupd_wrnmsg) {
   print_post(pst);
   printf(" %s\n",msg);
  }
}
/*------------------------------------------------------------------------*/
#if !LIND /* 3 */
#if CICPP
void CIIFU :: verif_grava_rec(DBXSTRU *dbxp,
                              int      idx)
#else /*CICPP*/
void verif_grava_rec(dbxp,idx)
DBXSTRU *dbxp;
int idx ;
#endif /*CICPP*/
{
#if 1
  IFPSTRU *tmp;
  tmp=PREG(idx);
#endif
  if (STATUS(idx)==MODIFIED) {
#if 0  /* Nao tirar!!!*/
         printf("\n Gravando blk=%"_LD_,BLK(tmp));
#endif
#if CNV_PCBINUM
         ifpwrit(dbxp,(char *)tmp,(LONGX)IFBSIZ,hdblk,hdoff,hdn);
#else /* CNV_PCBINUM */
         ifpwrit(dbxp,(char *)tmp,(LONGX)IFBSIZ);
#endif /* CNV_PCBINUM */
         STATUS(idx)=NOT_MODIFIED;
  }
}
/*-----------------------------------------------------------------------*/
#if CICPP
void CIIFU :: update_frst_curr_headers(INFO nelem)
#else /*CICPP*/
void update_frst_curr_headers(nelem)
INFO nelem;
#endif /*CICPP*/
{  IFPHEAD *h,*fh;
#if CNV_PCBINUM
 ifp_ins_new_hd(BLKX(idx_segh),off_ch_inp,hdblk,hdoff,IFLmaxhd,&hdn);
#endif /*CNV_PCBINUM */


 h=HEAD( (PREG(idx_segh)),off_ch_inp);
 /* 25-08-95: Conclui que: O ifptot so tem valor correto no primeiro
              header do  posting. Nos outros fica sempre com o mesmo valor
	      da capacidade do segmento. Desta forma nao se incrementa o
	      ifptot dos segmentos 2,3,..n .
	      Esta alteracao deve ser feita junto com as outras de 25-08-95
 */

/* 25-08-95
 h->ifptotp+=nelem;
*/
 h->ifpsegp+=nelem;
 STATUS(idx_segh)=MODIFIED;

#if CNV_PCBINUM
/*hh*/  ifp_ins_new_hd(BLKX(idx_first),off_fh_inp,hdblk,hdoff,IFLmaxhd,&hdn);
#endif /*CNV_PCBINUM */


 fh=HEAD( (PREG(idx_first)),(int) off_fh_inp);
 /* 25-08-95
	      Nas linhas a seguir, o teste que era feito para saber se
	      deveria ou nao alterar no primeiro segmento perde o efeito.
	      Sempre devera ser feito. Esse teste tinha sentido para
	      nao incrementar 2 vezes quando estavamos trabalhando com
	      o primeiro segmento.
	      Deixei entre #if .. #endif para nao perder o historico

 */
#if 0
 /* Se first coincide com current nao atualiza first */
  if (fh!=h) {
   fh->ifptotp+=nelem;
   STATUS(idx_first)=MODIFIED;
 }
#else
/* 25-08-95 */
   fh->ifptotp+=nelem;
   STATUS(idx_first)=MODIFIED;
#endif
}
/*------------------------------------------------------------------------*/
#if CICPP
void CIIFU :: read_ifpcntrl()
#else /*CICPP*/
void read_ifpcntrl()
#endif /*CICPP*/
{
  IFPSTRU *tmp;
  tmp=postread(pbuf,invp,1L,0); /* Ler registro de controle */
  pn.ifpblk=1L;
  pn.nxtb=tmp->ifprec[0];
  pn.nxtp=tmp->ifprec[1];
}
#endif /* !LIND 3 */
/*------------------------------------------------------------------------*/
#if BEFORE20000926
#if CICPP
int CIIFU :: upif_end(int pstflag)
#else /*CICPP*/
int upif_end(pstflag)
int pstflag;
#endif /*CICPP*/
#else /* BEFORE20000926 */
#if CICPP
int CIIFU :: upif_end(char *dbnp, int pstflag)
#else /*CICPP*/
int upif_end(dbnp,pstflag)
char *dbnp;
int pstflag;
#endif /*CICPP*/
#endif /* BEFORE20000926 */
{
  int rc=0;
#if !LIND /* 4.0 */
  IFPHEAD *h;
  IFPSTRU *t;
#endif /* !LIND 4.0 */
#if BEFORE20000927
#else /* BEFORE20000926 */
  trmifupd=IFUPDXUP; /* trm IFUPDATE operation + cntwrit */
#endif /* BEFORE20000926 */

#if BEFORE20000926
#else /* BEFORE20000926 */
  dbxp=dbxstorp(dbnp);
#endif /* BEFORE20000926 */
  if (!DBXiinit) fatal("ciifu/upif_end/DBXiinit");
#if !LIND /* 4 */
  if (pstflag == IFUPDICT) {	/* .ifp was not processed */
        idx_curr_inp=CURR_INP;
        off_inp=2L;
        t=PREG(idx_curr_inp);
        BLK(t)=1L;
        update_ifpcntrl(1L,IDXHEADSIZE+PSTSIZE+off_inp);
        h=HEAD(t,(int)off_inp);
        h->ifptotp=0;
        h->ifpsegp=0;
        h->ifpsegc=1;
        h->ifpnxtp=0;
        h->ifpnxtb=0;
        STATUS(idx_curr_inp)=MODIFIED;
        verif_grava_rec(dbxp,idx_curr_inp);
  }
#if CNV_PCBINUM
  /* Nao tem nada para converter */
  ifpwrit(dbxp,(char *)&pn,(LONGX)sizeof(IFPAVAILPOS),hdblk,hdoff,(-1));
#else /* CNV_PCBINUM */
  ifpwrit(dbxp,(char *)&pn,(LONGX)sizeof(IFPAVAILPOS));
#endif /* CNV_PCBINUM */
#else /* !LIND 4 */
  if (pstflag == IFUPDICT) {;}	/* no wrn */
#endif /* !LIND 4 */

#if BEFORE950308
  cntwrit(dbxp); /* acho que nao precisa por causa de nodewrit/leafwrit */
#endif

  DBXiinit=0;

#if BEFORE20000927
#else /* BEFORE20000926 */
  trmifupd=0; /* reset IFUPDXUP */
#endif /* BEFORE20000926 */

#if 0
   printf("\n ----------------------------------------------------------");
   printf("\n =>Total de registros Processados:%"_LD_,nlidos);
   printf("\n ----------------------------------------------------------");
#endif
  return(rc);
}
/*------------------------------------------------------------------------*/
#if CICPP
void CIIFU :: upif_init(char *dbnp)
#else /*CICPP*/
void upif_init(dbnp)
char *dbnp;
#endif /*CICPP*/
{
#if BEFORE20000927
#else /* BEFORE20000926 */
  trmifupd=IFUPDXUP; /* trm IFUPDATE operation + cntwrit */
#endif /* BEFORE20000926 */

  keyp=(UCHR *)&key[0];
  strcpy((char *)keyp,"!012345678");

  if ((dbxp=dbxsrchp(dbnp)) == (DBXSTRU *)NULL) {
	dbxp=dbxstorp(dbnp);
	DBXitrac=0; DBXitell=0; DBXirang=1L;
  }

#if !LIND /* 5 */
  invsetup((UCHR *)dbnp,0L,0L,0L);
#else /* !LIND 5 */
  invsetup((UCHR *)dbnp,0L,0L,-1L);
#endif /* !LIND 5 */
  invp=DBXifmap;

  DBXiinit=1;
#if !MULTI_IF
  DBXirecs=0L;
  DBXipadd[0]=DBXipadd[1]=0L;
  DBXipdel[0]=DBXipdel[1]=0L;
#endif
#if !LIND /* 6 */
  pbuf=(UCHR *)buf;
  pinitpst= &initpst; /* falta inicializar com -1 */
  read_ifpcntrl();
#endif /* !LIND 6 */

#if BEFORE20000927
#else /* BEFORE20000926 */
  trmifupd=0; /* reset IFUPDXUP */
#endif /* BEFORE20000926 */

}
/*------------------------------------------------------------------------*/
#if !LIND /* 7 */
#if CICPP
void CIIFU :: update_ifpcntrl(INFO nxtb,
                              INFO nxtp)
#else /*CICPP*/
void update_ifpcntrl(nxtb,nxtp)
 INFO nxtb;
 INFO nxtp;
#endif /*CICPP*/
 {
        pn.nxtb=nxtb;
        pn.nxtp=nxtp;
 }
#endif /* !LIND 7 */
/*-----------------------------------------------------------------------*/
#if CICPP
void CIIFU :: print_post(POSTSTRU *pstp)
#else /*CICPP*/
void print_post(pstp)
POSTSTRU *pstp;
#endif /*CICPP*/
{
 LONGX mfn;
 unsigned short tag,occ,cnt;
 decodepst(pstp,&mfn,&tag,&occ,&cnt);
#if AOT
 printf("%s %"_LD_"/%d/%d/%d  ",keyp,mfn,tag,occ,cnt);
#else
 printf("\n%s %"_LD_"/%d/%d/%d       ",keyp,mfn,tag,occ,cnt);
#endif
}
/*------------------------------------------------------------------------*/
#if !LIND /* 8 */
#if CICPP
void CIIFU :: next_output_blk()
#else /*CICPP*/
void next_output_blk()
#endif /*CICPP*/
{
        verif_grava_rec(dbxp,idx_curr_out);
    nxtb_out++;
        idx_curr_out=CURR_OUT;
        BLK( (PREG(idx_curr_out)) )=nxtb_out;
        STATUS(idx_curr_out)=MODIFIED;
    idxpst_out=0;
}
/*------------------------------------------------------------------------*/
#if CICPP
void CIIFU :: next_input_blk()
#else /*CICPP*/
void next_input_blk()
#endif /*CICPP*/
{
if (idx_curr_inp==UNDEFINED ){
  idx_curr_inp=CURR_INP;
  idx_first=CURR_INP;
  idx_segh=CURR_INP;
  STATUS(CURR_INP)=NOT_MODIFIED;
  off_fh_inp=nxtp_inp;
  off_ch_inp=nxtp_inp;
}else {
  if ((idx_curr_inp==idx_segh) && (idx_segh!=idx_first)) {
        idx_segh=SEGH;
        v[idx_segh]=v[idx_curr_inp];
        STATUS(idx_curr_inp)=NOT_MODIFIED;
  }else {
        if ((idx_curr_inp==idx_segh)&&(idx_first==idx_segh)) {
          idx_segh=FIRST;
          idx_first=FIRST;
          v[idx_first]=v[idx_curr_inp];
          STATUS(idx_curr_inp)=NOT_MODIFIED;
        }
  }
}
verif_grava_rec(dbxp,idx_curr_inp);
blk_inp++;
memmove(PREG(idx_curr_inp),postread(pbuf,invp,blk_inp,0),IFBSIZ);
idxpst_inp=0;
}
/*-----------------------------------------------------------------*/
#if CICPP
void CIIFU :: next_input_seg()
#else /*CICPP*/
void next_input_seg()
#endif /*CICPP*/
{
 if (idx_curr_inp==UNDEFINED ){
   idx_curr_inp=CURR_INP;
   idx_first=CURR_INP;
   idx_segh=CURR_INP;
   STATUS(CURR_INP)=NOT_MODIFIED;
   off_fh_inp=nxtp_inp;
   off_ch_inp=nxtp_inp;
 }else {
   pcur_blk=PREG(idx_curr_inp);
   if (BLK(pcur_blk)==nxtb_inp) {
         verif_grava_rec(dbxp,idx_segh);
         idx_segh=CURR_INP;
         off_ch_inp=nxtp_inp;
         goto get_counters;
   } else {
         if ((idx_curr_inp==idx_segh)&&(idx_segh==idx_first)){
           idx_first=FIRST;
           v[idx_first]=v[idx_curr_inp];
           STATUS(idx_curr_inp)=NOT_MODIFIED;
         }else {
           if ((idx_curr_inp==idx_segh)&&(idx_segh != idx_first)) {
                 verif_grava_rec(dbxp,idx_segh);
           }else {
                 verif_grava_rec(dbxp,idx_segh);
                 idx_segh=CURR_INP;
           }
         }
   }
 }
verif_grava_rec(dbxp,idx_curr_inp);
memmove(PREG(idx_curr_inp),postread(pbuf,invp,nxtb_inp,0),IFBSIZ);
/* Verifica se apos a leitura o first e segh voltaram a apontar para
   o mesmo bloco e estao apontando para posicoes diferentes na memoria
*/
if (  ( BLKX(idx_first) ==BLKX( idx_segh) )  &&
   (idx_first!=idx_segh)) {
        if (memcmp(PREG(idx_first),PREG(idx_segh),IFBSIZ) !=0)
                fatal("ciifu/first/segh");
        idx_first=idx_segh;
}
off_ch_inp=nxtp_inp;
get_counters:
 blk_inp=nxtb_inp;
 off_inp=nxtp_inp;
 gethead_counters(PREG(idx_segh),off_inp,
                          &nxtb_inp,&nxtp_inp,&totp_inp,&segp_inp,&segc_inp);
 npst_read_in_seg=0;
 idxpst_inp=off_inp+IDXHEADSIZE;
}
/*-----------------------------------------------------------------*/
#if CICPP
void CIIFU :: next_input_element()
#else /*CICPP*/
void next_input_element()
#endif /*CICPP*/
{
 if (npst_read_in_seg <= segp_inp) {
    if (idxpst_inp+PSTSIZE <= MAXIDXPST){
       idxpst_inp+=PSTSIZE;
       }else {
      next_input_blk();
    }
 }else {
    next_input_seg();
 }
 npst_read_in_seg+=1;
}
/*-----------------------------------------------------------------*/
#if CICPP
void CIIFU :: next_output_element()
#else /*CICPP*/
void next_output_element()
#endif /*CICPP*/
{
    if (idxpst_out+PSTSIZE <= MAXIDXPST){
       idxpst_out+=PSTSIZE;
       }else {
      next_output_blk();
    }
}
/*-----------------------------------------------------------------*/
#if CICPP
void CIIFU :: copia_posting(POSTSTRU *ppst,
                            int      *ja_inseriu)
#else /*CICPP*/
void copia_posting(ppst,ja_inseriu)
POSTSTRU *ppst;
int *ja_inseriu;
#endif /*CICPP*/
{
 int x;
 POSTSTRU *source,*dest;
 dest   = PSTADDR( (PREG(idx_curr_out)) ,(int)idxpst_out);
 if (npst_read_in_seg <segp_inp) {
   source = PSTADDR( (PREG(idx_curr_inp)) ,(int)idxpst_inp);
   x=memcmp(source,ppst,sizeof(POSTSTRU));
 }else x=1;
 if (*ja_inseriu==TRUE || x<0){
         memmove(dest,source,sizeof(POSTSTRU));
         STATUS(idx_curr_out)=MODIFIED;
     next_output_element();
     next_input_element();
 }else {
     memmove(dest,ppst,sizeof(POSTSTRU));
         STATUS(idx_curr_out)=MODIFIED;
     *ja_inseriu=TRUE;
     if (x==0)
       upif_print_msg(ppst,(UCHR *)lnk_dup_msg);
     next_output_element();
 }
}
/*-----------------------------------------------------------------*/
#if CICPP
void CIIFU :: insere_posting(int idx_ifprec,
                             int idx,
                             POSTSTRU *ppst)
#else /*CICPP*/
void insere_posting(idx_ifprec,idx,ppst)
 int idx_ifprec;
 int idx;
 POSTSTRU *ppst;
/* Insere um elemento no lugar correto e passa o valor do vetor adiante
   para ser inserido na posicao seguinte
*/
#endif /*CICPP*/
{
   IFPSTRU *pifp;
   int  x;
   POSTSTRU *p,tmp;
   pifp=PREG(idx_ifprec);
   p=PSTADDR(pifp,idx);
   x= memcmp(p,ppst,sizeof(POSTSTRU));
   if (x>0) {
     memmove(&tmp,p,sizeof(POSTSTRU));
     memmove(p,ppst,sizeof(POSTSTRU));
     memmove(ppst,&tmp,sizeof(POSTSTRU));
         STATUS(idx_ifprec)=MODIFIED;
   }else
    if (x==0 && dupl_msg==FALSE){
       upif_print_msg(ppst,(UCHR *)lnk_dup_msg);
       dupl_msg=TRUE;
    }
}
/*-----------------------------------------------------------------------*/
#if CICPP
void CIIFU :: gethead_counters(IFPSTRU *p,
                               INFO   off,
                               INFO  *nxtb,
                               INFO  *nxtp,
                               INFO  *totp,
                               INFO  *segp,
                               INFO  *segc)
#else /*CICPP*/
void gethead_counters(p,off,nxtb,nxtp,totp,segp,segc)
IFPSTRU *p;
INFO off;
INFO  *nxtb , *nxtp,*totp ,*segp, *segc;
#endif /*CICPP*/
{
  IFPHEAD *h;
  h = HEAD(p,(int)off);
  *nxtb = h->ifpnxtb;
  *nxtp = h->ifpnxtp;
  *totp = h->ifptotp;
  *segp = h->ifpsegp;
  *segc = h->ifpsegc;
}
/*-----------------------------------------------------------------------*/
#if CICPP
void CIIFU :: update_header(IFPSTRU *pifp,
                            INFO off,
                            INFO  nxtb,
                            INFO  nxtp,
                            INFO  totp,
                            INFO  segp,
                            INFO  segc)
#else /*CICPP*/
void update_header(pifp,off,nxtb,nxtp,totp,segp,segc)
IFPSTRU *pifp;
INFO off;
INFO  nxtb , nxtp,totp , segp, segc;
#endif /*CICPP*/
 {
  IFPHEAD *h;
  h = HEAD(pifp,(int)off);
  h->ifpnxtb=nxtb;
  h->ifpnxtp=nxtp;
  h->ifptotp=totp;
  h->ifpsegp=segp;
  h->ifpsegc=segc;
#if CNV_PCBINUM
/*hh*/  ifp_ins_new_hd(pifp->ifpblk,off,hdblk,hdoff,IFLmaxhd,&hdn);
#endif /*CNV_PCBINUM */  
}
/*----------------------------------------------------------------------*/
#if CICPP
void CIIFU :: create_new_segment(INFO segp,
                                 INFO segc)
#else /*CICPP*/
void create_new_segment(segp,segc)
INFO segp;
INFO segc;
#endif /*CICPP*/
{
 IFPSTRU *x;
 nxtb_out=pn.nxtb;
 nxtp_out=pn.nxtp;
 if (segc > MAXPSTSSEG) segc=MAXPSTSSEG;
/* Verifica se e possivel criar um header e um posting na proxima posicao
   disponivel. Se nao tiver espaco, cria um novo bloco.
   Se o bloco onde sera feita a insercao e o mesmo bloco  que esta na me
   moria, modifica o apontador. Caso contrario le o blk onde sera feita
   a insercao.
*/
/* 27/10/95 - Foi feita uma alteracao para que quando estejamos inserindo
              uma nova chave, seja forcada a leitura do ifp do disco e
	      reinicializadas as variaveis, em vez de usarmos o blk que
	      esta na memoria.
*/
 idx_curr_out=CURR_OUT;
 pcur_blk_out=PREG(idx_curr_out);
 pcur_blk=PREG(idx_curr_inp);
 if (BLK(pcur_blk)==nxtb_out) {
   idx_curr_out=idx_curr_inp;
   pcur_blk_out=PREG(idx_curr_out);
 }else {
        x=postread(pbuf,invp,nxtb_out,0);
        memmove(pcur_blk_out,x,IFBSIZ);
        STATUS(idx_curr_out)=NOT_MODIFIED;
    }
 if (nxtp_out+IDXHEADSIZE+PSTSIZE > MAXIDXPST) {
    next_output_blk();
    nxtp_out=idxpst_out;
 }
 /* 25-08-93 totp: nao e 'accurate' nos segmentos que sao
                   criados. O MicroIsis utiliza o valor do
		   segc

 update_header(PREG(idx_curr_out),nxtp_out,
                  nxtb_inp,nxtp_inp,segp,segp,segc);
*/
 update_header(PREG(idx_curr_out),nxtp_out,
                  nxtb_inp,nxtp_inp,segc,segp,segc);

 idxpst_out=nxtp_out+IDXHEADSIZE;
}
/*------------------------------------------------------------------------*/
#if CICPP
void CIIFU :: posiciona_inicio_seg()
#else /*CICPP*/
void posiciona_inicio_seg()
#endif /*CICPP*/
{
  if (idx_curr_inp != idx_segh)  {
         verif_grava_rec(dbxp,idx_curr_inp);
         v[idx_curr_inp]=v[idx_segh];
         /* O segh deve apontar para o novo bloco.
                Se o segh e  igual ao first , muda tambem o first
         */
         if (idx_segh==idx_first) {
           idx_first=idx_curr_inp;
         }
         idx_segh=idx_curr_inp;
         blk_inp=BLK( (PREG(idx_curr_inp) ) );
         gethead_counters(PREG(idx_segh),off_ch_inp,
                           &nxtb_inp,&nxtp_inp,&totp_inp,&segp_inp,&segc_inp);
         npst_read_in_seg=0;
         idxpst_inp=off_ch_inp+IDXHEADSIZE;
  }
}
/*------------------------------------------------------------------------*/
#if CICPP
void CIIFU :: upif_split_segment(POSTSTRU *ppst)
#else /*CICPP*/
void upif_split_segment(ppst)
POSTSTRU *ppst;
#endif /*CICPP*/
{
 INFO segc_out;
 POSTSTRU *out;
 IFPHEAD *h,*fh,*hnew;
 int meio,antes,depois;
 LONGX np,dep;
 meio=(int)(segc_inp/2);
#if 1
  antes=meio+1;
#endif
  depois=(int)(segc_inp-antes)+1; /* O que vai ser inserido */
/*
Aqui deveria ser feito outro algoritmo para que nao
fiquem segmentos com poucos psotings
#if 0
   if (segc_inp + 1 <= 2*accurate_totp/3){
     antes=0;
     depois=segc_inp +1;
  }
#endif
*/
  /* Se o bloco onde esta o head do segmento e igual ao segmento corrente
         nao precisa ler  o bloco do disco.
     Basta copiar do bloco onde esta o head .
         inicio do Post para fazer a quebra
     O idxpst de qualquer maneira deve ser apontado para o inicio
  */
  npst_read_in_seg=0;
  idxpst_inp = off_ch_inp+IDXHEADSIZE;/*precisa ser setado */
  np=1;
#if CNV_PCBINUM
  /*hh*/ /*Marca local do header onde sera feito o split */
  /*hh*/  ifp_ins_new_hd(BLKX(idx_curr_inp),off_ch_inp,hdblk,hdoff,IFLmaxhd,&hdn);
#endif /*CNV_PCBINUM */

  while (np<=antes && np<=segp_inp){
         insere_posting(idx_curr_inp,(int)idxpst_inp,ppst);
         next_input_element();
     np++;
  }
    create_new_segment(depois,accurate_totp);
    hnew=HEAD( (PREG(idx_curr_out) ),(int)nxtp_out); /*yyy*/
    segc_out=hnew->ifpsegc;
    h=HEAD( (PREG(idx_segh)),(int)off_ch_inp);
    h->ifpnxtb=nxtb_out;
    h->ifpnxtp=nxtp_out;
    h->ifptotp-=(depois-1);
    h->ifpsegc=segc_inp;
    h->ifpsegp=antes;
    STATUS(idx_segh)=MODIFIED;
    fh=HEAD( (PREG(idx_first)),(int)off_fh_inp);
   /*        fh->ifptotp+=1; */       /*25-08-1995*/
    fh->ifptotp=accurate_totp+1; /*25-08-1995*/
    STATUS(idx_first)=MODIFIED;

#if CNV_PCBINUM
    /*hh*/ /*Marca local do header do primerio segmento  */
    /*hh*/  ifp_ins_new_hd(BLKX(idx_first),off_fh_inp,hdblk,hdoff,IFLmaxhd,&hdn);
#endif /*CNV_PCBINUM */  

    ja_inseriu=FALSE;
    dep=0;
    while (np <= depois+antes && np<=segp_inp+1) {
      copia_posting(ppst,&ja_inseriu);
      np++;
      dep++;
    }
/* Considera que ja foram lidos (processados) "depois" elementos do
   segmento corrente
*/
   npst_read_in_seg=depois;
/* limpa resto do vetor de saida */
    while (dep < segc_out) {
      out=PSTADDR( (PREG(idx_curr_out)),(int)idxpst_out);
      memmove(out,pinitpst,sizeof(POSTSTRU));
      STATUS(idx_curr_out)=MODIFIED;
      np++;
      dep++;
      next_output_element();
    }
    verif_grava_rec(dbxp,idx_curr_out);
    update_ifpcntrl(nxtb_out,idxpst_out);
 }
/*------------------------------------------------------------------------*/
#if CICPP
void CIIFU :: upif_find_segment(POSTSTRU *ppst)
#else /*CICPP*/
void upif_find_segment(ppst)
POSTSTRU *ppst;
#endif /*CICPP*/
{
 int pst_in_blk,
     /*last_elem, *hh*/  x;
 INFO last_elem;/*hh*/
 BOOLEAN found;
 LONGX segp_tmp;
 found=FALSE;
 segp_tmp=segp_inp;
 for (; segp_tmp >= 0 && found==FALSE ;) {
  if (nxtb_inp == 0) { found=TRUE;
  }else
  if (segp_tmp == 0) {
      next_input_seg();
      segp_tmp=segp_inp;
  }else {
 /*hh*    last_elem=(int)idxpst_inp+((int)segp_tmp-1)*PSTSIZE; */
 /*hh*/   last_elem=(INFO)idxpst_inp+((INFO)segp_tmp-1)*PSTSIZE;
    if (last_elem > MAXIDXPST)last_elem=MAXIDXPST;
    pst_in_blk=(last_elem-(int)idxpst_inp)/PSTSIZE+1;
    last_elem=(int)idxpst_inp+(pst_in_blk-1)*PSTSIZE;
    segp_tmp-= pst_in_blk;
    x=memcmp((UCHR *)ppst,
                PSTADDR( ( PREG(idx_curr_inp) ),last_elem),sizeof(POSTSTRU));
    if ( x <= 0) {
      found=TRUE;
    }
    else {
        if (segp_tmp > 0) {
            npst_read_in_seg+=pst_in_blk;
            next_input_blk();
        }else {
            next_input_seg();
            segp_tmp=segp_inp;
        }
     }
  }
 }
}
/*--------------------------------------------------------------------------*/
#if CICPP
void CIIFU :: upif_insert_into_seg(POSTSTRU *ppst)
#else /*CICPP*/
void upif_insert_into_seg(ppst)
POSTSTRU *ppst;
#endif /*CICPP*/
{
 POSTSTRU *p;
 while (npst_read_in_seg <= segp_inp-1){ /* npst_read_in_seg comeca 0 */
   insere_posting(idx_curr_inp,(int)idxpst_inp,ppst);
   next_input_element();
 }
 /* Insere o ultimo elemento */
 p=PSTADDR( (PREG(idx_curr_inp)),(int)idxpst_inp);
 memmove(p,ppst,sizeof(POSTSTRU));
 STATUS(idx_curr_inp)=MODIFIED;
#if CNV_PCBINUM
 /*hh*/
 /*hh* Nao mexeu com headers. Nao precisa alterar  
     ifp_ins_new_hd(BLKX(idx_curr_inp),off_inp,hdblk,hdoff,IFLmaxhd,&hdn);
     
 */
#endif /*CNV_PCBINUM */

 update_frst_curr_headers((INFO)1);
 /*Deixa proximo elemento disponivel -( se existir) */
 if (npst_read_in_seg<=segp_inp)next_input_element();
}
/*--------------------------------------------------------------------------*/
#if CICPP
void CIIFU :: upif_process_posting(POSTSTRU *ppst,
                                   INFO blk,
                                   INFO off,
                                   UCHR operation)
#else /*CICPP*/
void upif_process_posting(ppst,blk,off,operation)
POSTSTRU *ppst;
INFO blk,off;
UCHR operation;
#endif /*CICPP*/
{
 int i;
 blk_inp=blk;
 off_inp=off;
 nxtb_inp=blk;
 nxtp_inp=off;
 for (i=0;i<=3;i++)
        STATUS(i)=NOT_MODIFIED;
 idx_curr_inp=UNDEFINED;
 next_input_seg();
 accurate_totp=totp_inp;/* guarda total de postings da chave.
                           Correto somente no primeiro segmento
                        */
 upif_find_segment(ppst);
 posiciona_inicio_seg();
 dupl_msg=FALSE;
 if (operation==INSERTION) {
         if (segp_inp<segc_inp) {        /* Tem espaco para um posting */
         upif_insert_into_seg( ppst);
      } else {
          upif_split_segment(ppst);
	  
     }
 }else{
   if(operation==DELETION){
     upif_delete_posting(ppst);
   }else fatal("ciifu/invalid post operation");
  }
 verif_grava_rec(dbxp,idx_curr_inp);
 verif_grava_rec(dbxp,idx_segh);
 verif_grava_rec(dbxp,idx_first);
 /*hh*/ /* Inicializa marcadores de headers*/
#if CNV_PCBINUM
  ifp_init_hd(&hdn);
#endif /*CNV_PCBINUM */  

 }
/*------------------------------------------------------------------------*/
#if CICPP
void CIIFU :: upif_process_new_key(POSTSTRU *ppst,
                                   INFO     *blk,
                                   INFO     *off)
#else /*CICPP*/
void upif_process_new_key(ppst,blk,off)
POSTSTRU *ppst;
INFO *blk,*off;
#endif /*CICPP*/
{
  POSTSTRU *out;
  idx_curr_out=CURR_OUT;
  npst_read_in_seg=0;
  BLK( (PREG(idx_curr_out)) )=0L; /* para funcionar quando a 1 chave e' nova
*/
  BLK( (PREG(idx_curr_inp)) )=0L; /*27/10/95 */
   /* Correcao do erro que apareceu no atendimento ao sistema de fotocopia.
      O programa estava considerando para idx_curr_inp o registro da ultima
      chave e, em determinado caso, estava utilizando um registro incorreto.
      Esta modificacao vai obrigar, na rotina create_new_segment,
      que seja lido o ifp do disco e forcar que
      os registros sejam reinicializados.
   */
  nxtp_inp=0;
  nxtb_inp=0;
  create_new_segment(1,1);
  out=PSTADDR( (PREG(idx_curr_out)) ,(int)idxpst_out);
  memmove(out,ppst,sizeof(POSTSTRU));
  *blk=nxtb_out;
  *off=nxtp_out;
  STATUS(idx_curr_out)=MODIFIED;
  /* Tentar posionar no proximo elemento para forcar a criar
         novo registro quando estamos no limite maximo do vetor
  */
  next_output_element(); /* se chegou fim  grava e cria novo.
                            Caso contrario so incrementa pointer
                         */
  verif_grava_rec(dbxp,idx_curr_out);/* grava o registro em que
					inseriu o posting ou o novo criado
				     */
  /*hh*/
#if CNV_PCBINUM
  ifp_init_hd(&hdn);
#endif /*CNV_PCBINUM */
  update_ifpcntrl(nxtb_out,idxpst_out);
}
#endif /* !LIND 8 */

/*----------------------------------------------------------------------*/
#if CICPP
void CIIFU :: trace_records(LONGX tell)
#else /*CICPP*/
void trace_records(tell)
LONGX tell;
#endif /*CICPP*/
{
        nlidos=nlidos+1;
        if (tell > 0) {
#if TRACEOPT
           if (nlidos-nlidos/tell*tell==0)
#if 0 /* 0 */
                 printf("\n Leitura %"_LD_"=> %s ",nlidos,key);
#else
                 fprintf(stderr,"+++ %"_LD_" %s\n",nlidos,key);
#endif /* 0 */
#endif
        }
}
/*----------------------------------------------------------------------*/
#if CICPP
void CIIFU :: optlnk2(char *lnk2p,
                      LONGX qtylk2,
                      char *olnk2p,
                      LONGX oqtylk2)
#else /*CICPP*/
void optlnk2( lnk2p, qtylk2, olnk2p,oqtylk2)
char *lnk2p;
LONGX qtylk2;
char *olnk2p;
LONGX oqtylk2;
#endif /*CICPP*/
{
  char *p,*q;
  LONGX qty;
  int ig;
  BOOLEAN achou;
#if TRACEOPT
   char reg[sizeof(LINK2)+1];
#endif
  char branco[LE2+1];
  memset((char *)branco,' ',LE2);
 ig=0;
#if TRACEOPT
  reg[sizeof(LINK2)]='\0';
 printf("\n =========DENTRO optlkn2===============");
#endif
/* Processa arvore do tipo 2*/
 for (p=olnk2p; oqtylk2--; p+=sizeof(LINK2)) {
#if TRACEOPT
       memmove(reg,p,sizeof(LINK2));
       printf("\n Old :%s",reg);
#endif
    qty=qtylk2;
    achou=FALSE;
    for (q=lnk2p; qty-- && achou==FALSE; q+=sizeof(LINK2)){
#if TRACEOPT
       memmove((char *)reg,q,sizeof(LINK2));
       printf("\n  New:%s",reg);
#endif
           if (memcmp(p,q,sizeof(LINK2))==0) {
/*
         memmove(p,(char *)branco,sizeof(LINK2)) ;
         memmove(q,(char *)branco,sizeof(LINK2)) ;
*/
          *p='\0';
          *q='\0';
          ig++;
          achou=TRUE;
#if TRACEOPT
          printf("\n +++ Ignorando:%d",ig);
#endif
        }
    }
 }
}
/*----------------------------------------------------------------------*/
#if CICPP
void CIIFU :: optlnk1(char *lnk1p,
                      LONGX  qtylk1,
                      char *olnk1p,
                      LONGX oqtylk1)
#else /*CICPP*/
void optlnk1( lnk1p, qtylk1, olnk1p,oqtylk1)
char *lnk1p;
LONGX qtylk1;
char *olnk1p;
LONGX oqtylk1;
#endif /*CICPP*/
{
  char *p,*q;
  LONGX qty;
  int ig;
  BOOLEAN achou;
  char branco[LE2+1];
#if TRACEOPT
   char reg[sizeof(LINK2)+1];
#endif
 ig=0;
  memset((char *)branco,' ',LE2);
#if TRACEOPT
  reg[sizeof(LINK1)]='\0';
 printf("\n =========DENTRO optlkn1===============");
#endif
/* Processa arvore do tipo 1*/
 for (p=olnk1p; oqtylk1--; p+=sizeof(LINK1)) {
#if TRACEOPT
       memmove(reg,p,sizeof(LINK1));
       printf("\n Old :%s",reg);
#endif
    qty=qtylk1;
    achou=FALSE;
    for (q=lnk1p; qty-- && achou==FALSE; q+=sizeof(LINK1)){
#if TRACEOPT
       memmove(reg,q,sizeof(LINK1));
       printf("\n  New:%s",reg);
#endif
           if (memcmp(p,q,sizeof(LINK1))==0) {
/*
	 memmove(p,(char *)branco,sizeof(LINK1)) ;
         memmove(q,(char *)branco,sizeof(LINK1)) ;
*/
          *p='\0';
          *q='\0';
          ig++;
          achou=TRUE;
#if TRACEOPT
          printf("\n +++ Ignorando:%d",ig);
#endif
	}
    }
 }
}
/*----------------------------------------------------------------------*/
#if CICPP
void CIIFU :: svdifupd(char *lnk1p,
                       LONGX qtylk1,
                       char *lnk2p,
                       LONGX qtylk2,
                       char *olnk1p,
                       LONGX oqtylk1,
                       char *olnk2p,
                       LONGX oqtylk2,
                       int  pstflag,
                       LONGX tell)
#else /*CICPP*/
void svdifupd( lnk1p, qtylk1, lnk2p, qtylk2,
			  olnk1p,oqtylk1,olnk2p,oqtylk2,pstflag,tell)
char *lnk1p;
LONGX qtylk1;
char *lnk2p;
LONGX qtylk2;
char *olnk1p;
LONGX oqtylk1;
char *olnk2p;
LONGX oqtylk2;
int pstflag;
LONGX tell;
#endif /*CICPP*/
{
  LINK1 *p1; LINK2 *p2; /* char *p; */
  int treecase;
  char branco[LE2+1];
  POSTSTRU pst;
  UCHR oper;
  memset((char *)branco,' ',LE2);
  keyp[0]='\0';
  treecase=0; /* new */

  trmifupd=IFUPDXUP; /* trm IFUPDATE operation + cntwrit */

#if 1
  optlnk1( lnk1p, qtylk1, olnk1p,oqtylk1);
  optlnk2( lnk2p, qtylk2, olnk2p,oqtylk2);
#endif
/* Processa arvore do tipo 1*/
	 treecase=0;
	 oper=DELETION;
#if TRACEOPT
	 if (tell>0) printf("\n=====> Delecao oqtylk1=%"_LD_,oqtylk1);
#endif
	 for (p1=(LINK1 *)olnk1p; oqtylk1--; p1++ /* LINK1 */) {
		memmove(keyp,p1->key,vlex[treecase]);
		keyp[vlex[treecase]]='\0';
		memmove((char *)&pst, &(p1->post),POSTSIZE);
		/* para futuro: Ignora entradas em branco */
#if TRACEhh
		printf("\n-----------------------------------------------\n");
		upif_print_msg((POSTSTRU *)&pst,"Tree[0] Removendo...");
#endif
		if (*keyp > branco[0]) {
			upif_fnd_pifp(dbxp,keyp,treecase,&pst,oper,pstflag);
		}
		trace_records(tell);
	 }
	 oper=INSERTION;
#if TRACEOPT
	 if (tell>0) printf("\n=====> Insercao qtylk1=%"_LD_,qtylk1);
#endif
	 for (p1=(LINK1 *)lnk1p; qtylk1--; p1++ /* LINK1 */) {
		memmove(keyp,p1->key,vlex[treecase]);
		keyp[vlex[treecase]]='\0';
		memmove((char *)&pst, &(p1->post),POSTSIZE);
#if TRACEhh
		printf("\n------------------------------------ %"_LD_"\n",nlidos);
		upif_print_msg((POSTSTRU *)&pst,"Tree[0] Inserindo...");
#endif
		if (*keyp > branco[0]) {
			upif_fnd_pifp(dbxp,keyp,treecase,&pst,oper,pstflag);
		}
		trace_records(tell);
	  }
/* Processa arvore do tipo 2*/
	 treecase=1;
	 oper=DELETION;
#if TRACEOPT
	 if (tell>0) printf("\n=====> Delecao oqtylk2=%"_LD_,oqtylk2);
#endif
	 for (p2=(LINK2 *)olnk2p; oqtylk2--; p2++ /* LINK2 */) {
		memmove(keyp,p2->key,vlex[treecase]);
		keyp[vlex[treecase]]='\0';
		memmove((char *)&pst, &(p2->post),POSTSIZE);
		/* para futuro: Ignora entradas em branco */
#if TRACEhh
		printf("\n-----------------------------------------------\n");
		upif_print_msg((POSTSTRU *)&pst,"Tree[1] Removendo...");
#endif
		if (*keyp > branco[0]) {
			upif_fnd_pifp(dbxp,keyp,treecase,&pst,oper,pstflag);
		}
		trace_records(tell);
	 }
	 oper=INSERTION;
#if TRACEOPT
	 if (tell>0) printf("\n=====> Insercao qtylk2=%"_LD_,qtylk2);
#endif
	 for (p2=(LINK2 *)lnk2p; qtylk2--; p2++ /* LINK2 */) {
		memmove(keyp,p2->key,vlex[treecase]);
		keyp[vlex[treecase]]='\0';
		memmove((char *)&pst, &(p2->post),POSTSIZE);
#if TRACEhh
		printf("\n-----------------------------------------------\n");
		upif_print_msg((POSTSTRU *)&pst,"Tree[1] Inserindo...");
#endif
		if (*keyp > branco[0]) {
			upif_fnd_pifp(dbxp,keyp,treecase,&pst,oper,pstflag);
		}
		trace_records(tell);
	  }


    trmifupd=0; /* reset IFUPDXUP */
}

#if !LIND /* 9 */
/*----------------------------------------------------------------------*/
#if CICPP
void CIIFU :: upif_delete_elem()
#else /*CICPP*/
void upif_delete_elem()
#endif /*CICPP*/
{
  /* Achou o elemento a ser retirado */
  /* Copia o bloco atual em um novo bloco de saida, eliminando o pst*/
  /* Atualiza firsthead e segment atual */
  /* Incrementa somente o indice de input */
 update_frst_curr_headers((INFO)(-1));
   verif_grava_rec(dbxp,idx_segh);
   verif_grava_rec(dbxp,idx_first);
  idx_curr_out=CURR_OUT;
  v[idx_curr_out]=v[idx_curr_inp];
  nxtb_out=BLK( (PREG(idx_curr_out)) );
  idxpst_out=idxpst_inp; /* Aponta para a mesma posicao - Depois o
			input ficara uma posicao adiante;
						 */
}
/*------------------------------------------------------------------------*/
#if CICPP
void CIIFU :: upif_delete_posting(POSTSTRU *ppst)
#else /*CICPP*/
void upif_delete_posting(ppst)
POSTSTRU *ppst;
#endif /*CICPP*/
{
   int  x;
   POSTSTRU *p,*out,*inp;
   BOOLEAN deleted,fim;
   deleted=FALSE;
   fim=FALSE;
 while (fim==FALSE) {
   if (npst_read_in_seg==segp_inp) fim=TRUE;
   else {
          p=PSTADDR( (PREG(idx_curr_inp)) ,(int)idxpst_inp);
          x= memcmp(p,ppst,sizeof(POSTSTRU));
      if (x==0) {
        upif_delete_elem();
        deleted=TRUE;
        fim=TRUE;
        next_input_element();
      }else{
         if (x>0) {
              fim=TRUE;
              npst_read_in_seg=segp_inp+1; /* forcar nao copiar mais */
            } else {
              next_input_element();
         }
      }
   }
}
if (deleted!=TRUE)
       upif_print_msg(ppst,(UCHR *)lnk_not_found_msg);
 else {
  /* Copia o resto do segmento para saida */
  while (npst_read_in_seg <= segp_inp-1){
        inp = PSTADDR( (PREG(idx_curr_inp)) ,(int)idxpst_inp);
        out = PSTADDR( (PREG(idx_curr_out)) ,(int)idxpst_out);
        memmove(out,inp,sizeof(POSTSTRU));
        STATUS(idx_curr_out)=MODIFIED;
    next_input_element();
        next_output_element();
	if (idxpst_out==0) {/*Precisa mudar de segmento. Como teria que ter
			      copiado em out todo o current para nao perder
			      a parte que nao vai mudar de valor
			    */
	   idx_curr_out=CURR_OUT;
	   memmove( PREG(idx_curr_out),PREG(idx_curr_inp),sizeof(IFPSTRU));
	}
   }
   verif_grava_rec(dbxp,idx_curr_out);
 }/*deleted*/
}
#endif /* !LIND 9 */
/* ============================ rotupd.c ================================ */

#if !CICPP
#if ANSI
LONGX aloca_prat(void);
void libera_prat(LONGX idx_prat);
#else
LONGX aloca_prat();
void libera_prat();
#endif
/*----------------------------------------------------------------------*/
LONGX aloca_prat()
{
 LONGX idx_prat;
#if PRINT
 printf ("\n aloca_prat >inicio>NRECS=%"_LD_, nrecs);
#endif
     for (idx_prat=maxnrec; idx_prat--; ) {
	if (!vrecp[idx_prat]) /* ja' decrementado */
	    break;
     }
     if (idx_prat < 0) fatal("ciifu/readoldrec");
#if PRINT
 printf ("\n aloca_prat >fim>prat=%"_LD_,idx_prat);
#endif
     recallok(idx_prat,rec_maxmfrl);
     return idx_prat;
}
/*----------------------------------------------------------------------*/
void libera_prat(idx_prat)
LONGX idx_prat;
{
#if PRINT
    printf ("\n libera_prat >inicio>NRECS=%"_LD_, nrecs);
#endif
    if (!vrecp[idx_prat]) fatal("ciifu/refrec/free");
    FREE(vrecp[idx_prat]); vrecp[idx_prat]=NULL; nrecs--;
#if PRINT
    printf ("\n libera_prat >fim>NRECS=%"_LD_, nrecs);
#endif
}
#endif /* CICPP */

/*----------------------------------------------------------------------*/
#if !INPUTFILE
#if CICPP
#if USE_GDBFMT
int CIIFU :: ifupdat(FSTSTRU *fst,
                     LONGX mfn1,
                     LONGX mfn2,
                     int pstflag,
                     int endup)
#else
int CIIFU :: ifupdat(char *mdbnp,
                     LONGX mfn1,
                     LONGX mfn2,
                     char *idbnp,
                     FSTSTRU *fst,
                     int pstflag,
                     int endup)
#endif /* USE_GDBFMT */
#else
int ifupdat(mdbnp,mfn1,mfn2,idbnp,fstp,stwp,maxlk1,maxlk2,pstflag,endup)
char *mdbnp;
LONGX mfn1;
LONGX mfn2;
char *idbnp;
FST_CODE *fstp;
char *stwp;
LONGX maxlk1;
LONGX maxlk2;
int pstflag;
int endup;
#endif /* CICPP */
{
#if CICPP
#if USE_GDBFMT
    MST_FILE *mst = fst->fst_mst;
    char *mdbnp = fst->fst_mst->getdbase();
    char *idbnp = fst->fst_btree->getdbase();
#endif /* USE_GDBFMT */
    char *lnk1p=fst->fst_area1p;
    char *lnk2p=fst->fst_area2p;
    char *olnk1p=fst->fst_oarea1p;
    char *olnk2p=fst->fst_oarea2p;

#else /* CICPP */
    LONGX irec;
    static char *lnk1p=NULL;
    static char *lnk2p=NULL;
    static char *olnk1p=NULL;
    static char *olnk2p=NULL;
#endif /* CICPP */

    RECSTRU *recp; /* mandatory for defines REC RDB MF0 MFR DIR, FIELDP */
    LONGX mfn;
    LONGX qtylk1;
    LONGX qtylk2;
    DBXSTRU *dbxp;
    LONGX pmfn;
    UWORD ptag,pocc,pcnt;
    LINK1 *p1; LINK2 *p2; /* char *p; */
    LONGX loop;
    int rc=0;

#if !LIND /* 18 */
#if !CICPP
    LONGX orec;
#endif /* CICPP */
    int flagnew,flagmod;
    LONGX oqtylk1;
    LONGX oqtylk2;
#endif /* !LIND 18 */

#ifdef USE_INFO_SYS
	 char mess_str[255];
	UIW_STRING *procstr=NULL;
	UIW_PROGRESSBAR *progbar=NULL;
	if (wprogress) {
		procstr=((UIW_STRING*)wprogress->Get("PROCESSING_STRING"));
		progbar=((UIW_PROGRESSBAR*)wprogress->Get("PROG_BAR"));
		progbar->Assign(0,0);
		progbar->SetColor(8);
		}
	LONGX prog_to, prog_from=mfn1;
	prog_to=mfn2; if (prog_to>mst->getmaxmfn()) prog_to=mst->getmaxmfn();

#endif

    dbxp=dbxstorp(idbnp);               /* inverted file ! */

#if MULTI_IF
    if (dbxtrace) printf("+++ ifupdat - %s (%s/%"_LD_"/%"_LD_") netws=%d\n",
    				idbnp,mdbnp,mfn1,mfn2,DBXnetws);
#endif

    trmifupd=IFUPDXUP; /* trm IFUPDATE operation + cntwrit */

#if MULTI_IF
    if (DBXnetws != MONONETS) {
	if (endup == IFUPCORE) fatal("ciifu/MULTI/IFUPCORE");
	if (mfn1 != mfn2) fatal("ciifu/MULTI/mfn1!=mfn2");
    }
    else
#endif /* MULTI_IF */
    if (!DBXiinit){
	upif_init(idbnp);
    }

#if !LIND /* 21 */
#if CICPP
    RECSTRU *orecp = new RECSTRU(cisisxp);
#if BEFORE970428
    orecp->recdbxp=dbxp; /* recread */
#else
    orecp->recdbxp=dbxstorp(mdbnp); /* recread */
#endif
#else /* CICPP */
    orec=aloca_prat();			/* old version */
#if BEFORE970428
    VRECdbxp(orec)=dbxp; /* recread */
#else
    VRECdbxp(orec)=dbxstorp(mdbnp); /* recread */
#endif
#endif /* CICPP */
#endif /* !LIND 21 */

#if CICPP
#if USE_GDBFMT
    recp = fst->xfst_getrecp();
    recp->xrecord(mdbnp,1L); // good - ver
#else
    recp = fst->xfst_getrecp();		    // <===============================
    recp->xrecord(mdbnp,1L);		    // <===============================
#endif /* USE_GDBFMT */
#else /* CICPP */
    irec=aloca_prat();                  /* current */
#endif /* CICPP */

    for (mfn=mfn1; mfn <= mfn2; mfn++) {

	if (mfn <= 0) break;            /* endup */

#if MULTI_IF
	if (DBXnetws != MONONETS && mfn1 > 0) {
#if SHTEST
printf("0."); gets(shtest);
if (shtest[0]=='.') system(shtest+1);
#endif /* SHTEST */
	    INVMAP *lastinvp=DBXifmap;
	    /* lock MF/check EWL/lock IF/set EWL/unlock MF => keep IF locked */
	    if (dbxilock(dbxp,mdbnp,"L"))
		if (dbxewlrc) {rc=RCLOCK; break;} else fatal("ciifu/lock");
	    upif_init(idbnp);   /* invsetup() and DBXinit */
	    /* if instead of invflush only invclose() is called,
	       invsetup() does not load the .cnt again! */
	    if (lastinvp) cntread(dbxp,NULL,NULL,NULL); /* load .cnt from locked IF */
#if SHTEST
printf("1."); gets(shtest);
if (shtest[0]=='.') system(shtest+1);
#endif /* SHTEST */
	}
#endif /* MULTI_IF */

#ifdef USE_INFO_SYS
		if (wprogress) {
			sprintf(mess_str,"MFN %"_LD_" (%s %5"_LD_")",mfn,mess->get(452),DBXirecs);
			procstr->DataSet(mess_str);
			progbar->Assign(mfn-prog_from,prog_to-prog_from);
			UI_EVENT event,event1,event2;
			wprogress->eventManager->Get(event1,Q_NO_BLOCK|Q_BEGIN|Q_DESTROY|Q_POLL);
			mainwindow->windowManager->Event(event1);
			wprogress->eventManager->Get(event2,Q_NO_BLOCK|Q_BEGIN|Q_DESTROY|Q_POLL);
			mainwindow->windowManager->Event(event2);
			wprogress->eventManager->Get(event,Q_NO_BLOCK|Q_BEGIN|Q_DESTROY|Q_POLL);
			if (event.type==10235 || event1.type==10235 || event2.type==10235) {		// EXPORT_CANCEL) {
				int ok=errsys->ErrorMessage(wprogress->windowManager,WOS_INVALID,mess->get(420));
				if (ok==WOS_INVALID) {
					mfn=-1;    // STOP
					DS_OperationStopped=1;
					continue;
					}
				}
			mainwindow->windowManager->Event(event);
			}
#endif

#if CICPP
#if USE_GDBFMT
	mst->getrec(mfn);
	int status=mst->getstatus();
	if (mst->getstatus_eof()) RECrc=RCEOF;
	else switch (status) {
	       case REC_LOCKED:	       RECrc=RCLOCK; break;
	       case REC_ACTIV:	       RECrc=RCNORMAL; break;
	       case REC_LOG_DEL:       RECrc=RCLDEL; break;
               case MST_FIL_BAD:
	       case REC_PHYS_DEL:      RECrc=RCPDEL; break;
	     }
	if (RECrc == RCNORMAL || RECrc == RCLDEL)
	    memmove(MFX,mst->getrecbuf(),mst->getreclen());
#else /* USE_GDBFMT */
	recp->xrecord(mdbnp,mfn);
#endif /* USE_GDBFMT */
#else /* CICPP */
	RECORD(irec,mdbnp,mfn);
#endif /* CICPP */

	if (RECrc == RCEOF) break;
	if (RECrc == RCPDEL) continue;

#if !CICPP
#if GEN_UTL
	if (DBXitrac) prtleader(recp,MFRmfn);
#endif /* GEN_UTL */
#endif /* CICPP */
#if !LIND /* 19 */
	flagnew=recxrefn; flagmod=recxrefm;
	if (flagnew && flagmod) fatal("ciifu/check/flags");
	if (!flagnew && !flagmod) continue;
#endif /* !LIND 19 */

/*
irec
recp
*/
#if CICPP
	    if(fst->xfst_inter(0) == -2) {
		 fatal("ciifu/fst_inter0");
		}
		lnk1p=fst->fst_area1p;
		lnk2p=fst->fst_area2p;
		qtylk1=fst->fst_qtylk1;
		qtylk2=fst->fst_qtylk2;
#else /* CICPP */
	fst_inter(fstp,irec,stwp,
			&lnk1p,maxlk1,&lnk2p,maxlk2,&qtylk1,&qtylk2);
#endif /* CICPP */

	if (DBXitrac) {
		for (p1=(LINK1 *)lnk1p, loop=qtylk1; loop--; p1++ /* LINK1 */) {
		    pstdecod(&(p1->post),&pmfn,&ptag,&pocc,&pcnt);
		    prtlink("[ ] ",pmfn,ptag,pocc,pcnt,p1->key,LE1);
		}
		for (p2=(LINK2 *)lnk2p, loop=qtylk2; loop--; p2++ /* LINK2 */) {
		    pstdecod(&(p2->post),&pmfn,&ptag,&pocc,&pcnt);
		    prtlink("[ ] ",pmfn,ptag,pocc,pcnt,p2->key,LE2);
		}
	}

#if !LIND /* 17 */
/*      if (MFRstatus == ACTIVE && MFRmfbwb) { */
	if (flagmod) {
#if BEFORE970204
	  if (!MFRmfbwb) fatal("ciifu/check/MFRmfbwb");
#endif
	  if (MFRmfbwb) {
	    recreadb=MFRmfbwb;
	    recreadp=MFRmfbwp;
#if CICPP
        if (!orecp->recnbytes) orecp->xrecalloc(rec_maxmfrl); // alloc
	    recread(orecp,MFRmfn); /* get old */
	    orecp->recrc=RCNORMAL; /* good */
#else /* CICPP */
	    recread(vrecp[orec],MFRmfn); /* get old */
	    VRECrc(orec)=RCNORMAL; /* good */
#if GEN_UTL
	    if (DBXitrac) prtleader(vrecp[orec],0L);
#endif /* GEN_UTL */
#endif /* CICPP */

#if CICPP
	    fst->xfst_setrecp(orecp);
		if (fst->xfst_inter(1) == -2) {
			fatal("ciifu/fst_inter1");
		}
		olnk1p=fst->fst_oarea1p;
		olnk2p=fst->fst_oarea2p;
		oqtylk1=fst->fst_oqtylk1;
		oqtylk2=fst->fst_oqtylk2;
	    fst->xfst_setrecp(recp);	// recp will be deleted in ~FSTSTRU
#else /* CICPP */
	    fst_inter(fstp,orec,stwp,
			&olnk1p,maxlk1,&olnk2p,maxlk2,&oqtylk1,&oqtylk2);
#endif /* CICPP */
	    if (DBXitrac) {
		for (p1=(LINK1 *)olnk1p, loop=oqtylk1; loop--; p1++ /* LINK1 */) {
		    pstdecod(&(p1->post),&pmfn,&ptag,&pocc,&pcnt);
		    prtlink("[B] ",pmfn,ptag,pocc,pcnt,p1->key,LE1);
		}
		for (p2=(LINK2 *)olnk2p, loop=oqtylk2; loop--; p2++ /* LINK2 */) {
		    pstdecod(&(p2->post),&pmfn,&ptag,&pocc,&pcnt);
		    prtlink("[B] ",pmfn,ptag,pocc,pcnt,p2->key,LE2);
		}
	    }
	  } /* MFRmfbwb */
	  else { /* see recwrite: !pend && rc!=RCNORMAL */
#if CICPP
	    if (!orecp->recnbytes) orecp->xrecalloc(LEADER); // alloc
	    orecp->recmfp->m1.m1status=DELETED;
#else /* CICPP */
	    VMFRstatus(orec)=DELETED;
#endif /* CICPP */
	  } /* MFRmfbwb */
	} /* flagmod */
	
	/* update the inverted file */
	if (flagnew)
#endif /* !LIND 17 */
	    if (MFRstatus == ACTIVE) {
		if (DBXitrac) {
            prtlink("[ ]+",0L,0,0,0,(UCHR *)"OK new",6);
		}
/*//*/		svdifupd(lnk1p,qtylk1,lnk2p,qtylk2,             /* add new */
/*//*/			 NULL,0,NULL,0,                         /*         */
/*//*/			 pstflag,DBXitell);
		DBXipadd[0]+=qtylk1;  DBXipadd[1]+=qtylk2;
	    }
#if !LIND /* 16 */
	if (flagmod)
	    if (MFRstatus == DELETED) {			/* new is deleted */
#if CICPP
	      if (orecp->recmfp->m1.m1status != DELETED) {
#else /* CICPP */
	      if (VMFRstatus(orec) != DELETED) {	/* old is active */
#endif /* CICPP */
		if (DBXitrac) {
            prtlink("[B]-",0L,0,0,0,(unsigned char*)"OK",2);
		}
/*//*/		svdifupd(NULL,0,NULL,0,                         /*         */
/*//*/			 olnk1p,oqtylk1,olnk2p,oqtylk2,         /* del old */
/*//*/			 pstflag,DBXitell);
		DBXipdel[0]+=oqtylk1; DBXipdel[1]+=oqtylk2;
	      }
	    }
	    else {					/* new is active */
#if CICPP
	      if (orecp->recmfp->m1.m1status == DELETED) {
#else /* CICPP */
	      if (VMFRstatus(orec) == DELETED) {	/* old is deleted */
#endif /* CICPP */
		if (DBXitrac) {
            prtlink("[ ]+",0L,0,0,0,(unsigned char*)"OK mod",6);
		}
/*//*/		svdifupd(lnk1p,qtylk1,lnk2p,qtylk2,             /* add new */
/*//*/			 NULL,0,NULL,0,                         /*         */
/*//*/			 pstflag,DBXitell);
		DBXipadd[0]+=qtylk1;  DBXipadd[1]+=qtylk2;
	      }
	      else {					/* new is active */
		if (DBXitrac) {				/* old is active */
		    prtlink("[ ]+",0L,0,0,0,(unsigned char*)"OK mod",6);
		    prtlink("[B]-",0L,0,0,0,(unsigned char*)"OK",2);
		}
/*//*/		svdifupd(lnk1p,qtylk1,lnk2p,qtylk2,             /* add new */
/*//*/			 olnk1p,oqtylk1,olnk2p,oqtylk2,         /* del old */
/*//*/			 pstflag,DBXitell);
		DBXipadd[0]+=qtylk1;  DBXipadd[1]+=qtylk2;
		DBXipdel[0]+=oqtylk1; DBXipdel[1]+=oqtylk2;
	      }
	    }

#if MULTI_IF
	/* unlock IF */
	if (DBXnetws != MONONETS) {
	    upif_end(idbnp,pstflag);
	    rc=dbxilock(dbxp,NULL,"UI"); 
	    invflush(idbnp); /* invclose(dbxp); */
	    if (rc)
	        if (dbxewlrc) {rc=RCLOCK; break;} else fatal("ciifu/unlock/I");
	}
#endif /* MULTI_IF */

	/* reset .mst bw ptrs and .xrf flags */
#if CICPP
	if (ifupd_reset) recreset(mdbnp,0L,0L,recp,0L);
#else /* CICPP */
	if (ifupd_reset) recreset(mdbnp,0L,0L,irec,0L);
#endif /* CICPP */

#if MULTI_IF
	/* lock MF/reset EWL/unlock MF */
	if (DBXnetws != MONONETS) {
	    rc=dbxilock(NULL,mdbnp,"UM");
	    if (rc)
	        if (dbxewlrc) {rc=RCLOCK; break;} else fatal("ciifu/unlock/M");
	}
#endif /* MULTI_IF */

#endif /* !LIND 16 */
	DBXirecs++;
	if (DBXitell)
/*	    if ((10 * (DBXirecs*100/DBXirang) + 5) / 10)% DBXitell == 0) */
	    if (DBXirecs % DBXitell == 0) {
#if 0
fprintf(stderr,"+++ name=%s irecs/itell/irang=%"_LD_"/%"_LD_"/%"_LD_" \n",
 DBXname,DBXirecs,DBXitell,DBXirang);
#endif
#if LINDLUX /* x2999 */
		fprintf(stderr,"%s %5"_LD_"  %3"_LD_"%% \n",
		    (invp->lvxiflind) ? "+ +" : " + ",
		    DBXirecs,
		    (10 * (DBXirecs*100/DBXirang) + 5) / 10);
#else /* LINDLUX x2999 */
#ifdef USE_INFO_SYS
//		errsys->sys_info_data(ltoa(mfn,mfnstr,10));
		if (wprogress) {
			sprintf(mess_str,"MFN %"_LD_" (%s %5"_LD_")",mfn,mess->get(452),DBXirecs);
			procstr->DataSet(mess_str);
			progbar->Assign(mfn-prog_from,prog_to-prog_from);
			UI_EVENT event;
			wprogress->eventManager->Get(event,Q_NO_BLOCK|Q_BEGIN|Q_DESTROY|Q_POLL);
			if (event.type==10235) {		// EXPORT_CANCEL) {
				int ok=errsys->ErrorMessage(wprogress->windowManager,WOS_INVALID,mess->get(420));
				if (ok==WOS_INVALID) {
					mfn=-1;    // STOP
					DS_OperationStopped=1;
					continue;
					}
				}
			mainwindow->windowManager->Event(event);
			}
#else
		fprintf(stderr,"%s %5"_LD_"  %3"_LD_"%%  [mfn=%"_LD_"]\n",
		    "+++",
		    DBXirecs,
		    (10 * (DBXirecs*100/DBXirang) + 5) / 10,
		    mfn);
#endif
#endif /* LINDLUX x2999 */
	    }
    } /* end for */

#if MULTI_IF
    /* undo dbxilock/L: RCEOF/etc or dbxilock/UI error */
    if (DBXnetws != MONONETS) {
	/* unlock IF */
	if (DBXifmap) if (DBIifopv) {
	    if (DBXiinit) upif_end(idbnp,pstflag);
	    rc=dbxilock(dbxp,NULL,"UI");
	    invflush(idbnp); /* invclose(dbxp); */
	    if (rc)
	        if (dbxewlrc) rc=RCLOCK; else fatal("ciifu/unlock/I2");
	}
	/* lock MF/reset EWL/unlock MF */
        if (dbxstorp(mdbnp)->dbxewlxx) {
	    rc=dbxilock(NULL,mdbnp,"UM");
#if BEFORE951103
	    mstclose(dbxp);
#endif
	    if (rc)
	        if (dbxewlrc) rc=RCLOCK; else fatal("ciifu/unlock/M2");
	}
    }
#endif /* MULTI_IF */

#if SHTEST
printf("2."); gets(shtest);
if (shtest[0]=='.') system(shtest+1);
#endif /* SHTEST */

#if CICPP
    /* don't delete recp: recp = fst->fst_recp */
#else /* CICPP */
    libera_prat(irec);
#endif /* CICPP */

#if !LIND /* 22 */
#if CICPP
    delete orecp;
#else /* CICPP */
    libera_prat(orec);
#endif /* CICPP */
#endif /* !LIND 22 */

#if MULTI_IF
    if (DBXnetws == MONONETS)
#endif
    if (endup & IFUPWRIT || endup & IFUPCLOS) {
	upif_end(idbnp,pstflag);
	invflush(idbnp); /* invclose(dbxp); */
#if LIND /* 2y */
#include "ciifu2.c"
#endif /* LIND 2y */
    }

#if !CICPP
    FREE(lnk1p); lnk1p=NULL;
    FREE(lnk2p); lnk2p=NULL;
    FREE(olnk1p); olnk1p=NULL;
    FREE(olnk2p); olnk2p=NULL;
#endif /* CICPP */

#if PRINT || LUXPRINT
    printf ("\n fim de ifupdat() - nrecs=%"_LD_, nrecs);
#endif

    trmifupd=0; /* reset IFUPDXUP */

    return(rc);
}
#endif /* INPUTFILE */
