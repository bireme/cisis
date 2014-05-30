#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "cisis.h"
#include "ciupi.h"
#if TRACEhh
void print_head(IFPSTRU *p,INFO off);
#endif

#if CICPP
#include <cidbx.hpp>
#include <citrm.hpp>
#include <cirun.hpp>
#endif /* CICPP */

#define CONVERTE    0 /* <========== ver quando (e porque!) foi inserido */
#define TRCTRACE    0
#define TRNTRACE    0
#define UNITRACE    0
#define TRLTRACE    0
#define TRPTRACE    0
#define TRPTRAC2    0 /* sindo */
#define TR0TRACE    0


#if CICPP
int cntwrit(DBXSTRU *dbxp)
#else /*CICPP*/
int cntwrit(dbxp)                           /*
------------
            grava .cnt;
            retorna RCNORMAL ou abenda;
                                    */
DBXSTRU *dbxp;           /* ptr descritor da base de dados */
#endif /*CICPP*/
{
    INVMAP *invp;
    int fd,treecase;
    char *unip;
    int unin;

#if CICPP
    CISISX *cisisxp = dbxp->cisisxp;
#endif /* CICPP */

    if (!dbxp) fatal("cntwrit/dbxp");
    invp=DBXifmap;
    if (!invp) fatal("cntwrit/invp");
    if ((fd=invp->cnopn) <= 0 || !invp->cnopw)
	fd=dbxopenw(DBXname,DBXname,cx1extp,&invp->cnopn,&invp->cnopw,"cntwrit/cnopn/w");

    if (fd <= 0) fatal("cntwrit/fd");
    if (LSEEK64(fd,0L,SEEK_SET) != 0L)
        fatal("cntwrit/seek");

    for (treecase=0; treecase < 2; treecase++) {
#if TRCTRACE
printf("cntwrit - cnt[%d] \n",treecase);
#endif
#if CNV_PCFILES
	unin=CNBSIZ-CNBUNI;
#else
	unin=CNBSIZ;
#endif
#if CNV_PCBINUM
        memmove(cnv_pcbuff,(char *)&invp->cn[treecase],CNBSIZ);
        ConvertCNT_REC(cnv_pcbuff);
	unip=cnv_pcbuff;
#else
	unip=(char *)&invp->cn[treecase];
#endif
        if (CIWRITE(fd,unip,unin) != unin) fatal("cntwrit/writ");
    }
#if TRCTRACE
printf("cntwrit - ok \n");
#endif
    return(RCNORMAL);
}


#if CICPP
int nodewrit(DBXSTRU *dbxp,
             N0STRU  *n0p,
             int      level,
             int      isroot)
#else /*CICPP*/
int nodewrit(dbxp,n0p,level,isroot)                                   /*
------------
            grava No';
            grava .cnt se necessario
            retorna RCNORMAL ou abenda;
                                                                      */
DBXSTRU *dbxp;      /* ptr descritor da base de dados */
N0STRU *n0p;        /* ptr No' a gravar */
int level;          /* nivel da arvore a gravar */
int isroot;         /* root node indicator */
#endif /*CICPP*/
{
    INVMAP *invp;       /* ptr descritor do inverted file */
    int treecase,n,nodesize,fd,upcnt;
    off_t xbytes;
    PUNT punt;
#if CNV_PCFILES
    N0STRU *unp;
    N1STRU *n1p;
    N2STRU *n2p;
    char *up;
#if UNITRACE
    int i;
#endif
#endif

#if CICPP
    CISISX *cisisxp = dbxp->cisisxp;
#endif /* CICPP */

#if TRNTRACE
printf("nodewrit - dbxp=%p (%s)  n0p=%p  level=%d  isroot=%d\n",
 dbxp,DBXname,n0p,level,isroot);
#endif
    invp=DBXifmap;
    treecase = n0p->it - 1;
    punt = n0p->pos;
    if (invp->nybasep[treecase]) fatal("nodewrit/CNLI/nybasep");
    if (n0p->ock < 1 || n0p->ock > TWORDN) fatal("nodewrit/ock");
    if (punt < 1 || punt > invp->cn[treecase].nmaxpos)
        if (punt != invp->cn[treecase].nmaxpos + 1) fatal("nodewrit/pos");
    if (level < 0 || level > invp->cn[treecase].liv + 1)
        fatal("nodewrit/level");

#if LINDLUX
    if (invp->ltxthresh) {
        /* alloc */
        if (punt > invp->ltxpages[treecase]) {
            if (punt != invp->ltxpages[treecase]+1) fatal("nodewrit/ltx/punt");
            if (punt > LUXPAGES) fatal("nodewrit/ltx/overflow");
            if (invp->ltxvpagp[treecase][punt]) fatal("nodewrit/ltx/again");
            invp->ltxvpagp[treecase][punt]=
                                (N0STRU *)ALLOC((ALLOPARM)nxbsiz[treecase]);
            if (invp->ltxvpagp[treecase][punt] == (N0STRU *)ALLONULL)
                                                fatal("nodewrit/ltx/ALLOC");
            invp->ltxpages[treecase]++;
        }
        /* copy */
        memmove(invp->ltxvpagp[treecase][punt],(char *)n0p,nxbsiz[treecase]);
    }
    else { /* else if invp->ltxthresh */
#endif /* LINDLUX */

    if (treecase) {
        nodesize=N2BSIZ;
	if ((fd=invp->n2opn) <= 0 || !invp->n2opw) {
	    fd=dbxopenw(DBXname,DBXname,nx12extp[treecase],&invp->n2opn,&invp->n2opw,"nodewrit/n2opn/w");
	}
    }
    else {
        nodesize=N1BSIZ;
	if ((fd=invp->n1opn) <= 0 || !invp->n1opw) {
	    fd=dbxopenw(DBXname,DBXname,nx12extp[treecase],&invp->n1opn,&invp->n1opw,"nodewrit/n1opn/w");
	}
    }
    if (fd <= 0) fatal("nodewrit/fd");
#if TRNTRACE
printf("nodewrit - invp=%p  treecase=%d  punt=%"_LD_"  liv=%d\n",
    invp,treecase,(LONGX)punt,invp->cn[treecase].liv);
#endif

    for (n=0; n <= invp->cn[treecase].liv; n++) {
#if TRNTRACE
printf("nodewrit - left=%d por pages=%d total=%d\n",
    invp->nx[treecase][n].left,invp->nx[treecase][n].pages,
    invp->nx[treecase][n].pages);
#endif
#if BEFORE950308
        invp->nx[treecase][n].left = invp->nx[treecase][n].pages;
#else
	/* left=pages ==> 0 loaded */
        invp->nx[treecase][n].left = invp->nx[treecase][n].pages;
	/* but if level is ok, copy it to nx[] */
#endif
    }

#if CNV_PCFILES
    nodesize-=(treecase)?TWORDN*N2BUNI:TWORDN*N1BUNI;
#endif
    xbytes=((off_t)(punt-1L))*nodesize;
#if CNLI
    if (invp->cn_offset[treecase]) fatal("nodewrit/CNLI/cn_offset");
#endif
#if TRNTRACE
printf("nodewrit - xbytes=%"P_OFF_T" nodesize=%d \n",(LONG_LONG)xbytes,nodesize);
#endif
    if (LSEEK64(fd,xbytes,SEEK_SET) != xbytes) fatal("nodewrit/seek");

#if CNV_PCFILES
#if UNITRACE
 n1p=(N1STRU *)n0p;
 n2p=(N2STRU *)n0p;
 if (treecase) {
 printf("pos=%4"_LD_"  ock=%2d  it=%d  '%.30s'\n",
 n2p->pos,n2p->ock,n2p->it,n2p->idx[0].key);
     getchar();
 for (i=0;i<n2p->ock;i++)
  printf("i=%2d  ock=%2d  '%.30s'  punt=%"_LD_"\n",
    i+1,n2p->ock,n2p->idx[i].key,
    n2p->idx[i].punt);
 } else {
  printf("pos=%4"_LD_"  ock=%2d  it=%d  '%.10s'\n",
   n1p->pos,n1p->ock,n1p->it,n1p->idx[0].key);
      getchar();
  for (i=0;i<n1p->ock;i++)
   printf("i=%2d  ock=%2d  '%.10s'  punt=%"_LD_"\n",
    i+1,n1p->ock,n1p->idx[i].key,
    n1p->idx[i].punt);
 }
#endif /* UNITRACE */
    unp=(N0STRU *)nodeunibuff;
    unp->pos=n0p->pos;
    unp->ock=n0p->ock;
    unp->it=n0p->it;
    n1p=(N1STRU *)n0p;
    n2p=(N2STRU *)n0p;
    up=unp->idxchars;
    for (n=0; n < TWORDN; n++, up+=sizeof(PUNT))
        if (treecase) {
            memmove(up,n2p->idx[n].key,LE2); up+=LE2;
            memmove(up,&(n2p->idx[n].punt),sizeof(PUNT));
        }
        else {
            memmove(up,n1p->idx[n].key,LE1); up+=LE1;
            memmove(up,&(n1p->idx[n].punt),sizeof(PUNT));
        }
#if CNV_PCBINUM
    if (treecase) ConvertN02_REC(nodeunibuff);
             else ConvertN01_REC(nodeunibuff);
#endif
    if (CIWRITE(fd,nodeunibuff,nodesize) != nodesize)
        fatal("nodewrit/writ");
#else /* CNV_PCFILES */
#if CNV_PCBINUM
    memmove(cnv_pcbuff,(char *)n0p,nodesize);
    if (treecase) ConvertN02_REC(cnv_pcbuff);
             else ConvertN01_REC(cnv_pcbuff);
    if (CIWRITE(fd,cnv_pcbuff,nodesize) != nodesize)
        fatal("nodewrit/writ");
#else
    if (CIWRITE(fd,(char *)n0p,nodesize) != nodesize)
        fatal("nodewrit/writ");
#endif
#endif /* CNV_PCFILES */

#if LINDLUX
    } /* end if invp->ltxthresh */
#endif /* LINDLUX */
    /* Update .cnt in memory */
    upcnt=0;
    if (punt > invp->cn[treecase].nmaxpos) {
        invp->cn[treecase].nmaxpos = punt;
        upcnt=1;
    }
    if (level > invp->cn[treecase].liv) {
        invp->cn[treecase].liv = level;
        upcnt=1;
    }
    if (isroot) {
        if (invp->cn[treecase].abnormal == ABNORMAL)
#if BEFORE950627
            if (punt > 1) {
#else
	    /* gdb resets ABNORMAL when root has ORDN keys */
	    if (punt > 1 || (punt == 1 && n0p->ock >= ORDN)) {
#endif
                invp->cn[treecase].abnormal = ABNORMAL+1;   /* NORMAL */
                upcnt=1;
            }
        if (invp->cn[treecase].posrx != punt) {
            invp->cn[treecase].posrx = punt;
            upcnt=1;
        }
    }
    /* Update .cnt file */
    if (upcnt)
        if (trmifupd == IFUPDXUP) cntwrit(dbxp);
#if TRNTRACE
printf("nodewrit - ok \n");
#endif
    return(RCNORMAL);
}


#if CICPP
int leafwrit(DBXSTRU *dbxp,
             L0STRU  *l0p)
#else /*CICPP*/
int leafwrit(dbxp,l0p)                                                /*
------------
            grava Folha;
            grava .cnt se necessario
            retorna RCNORMAL ou abenda;
                                                                      */
DBXSTRU *dbxp;      /* ptr descritor da base de dados */
L0STRU *l0p;        /* ptr folha a gravar */
#endif /*CICPP*/
{
    INVMAP *invp;       /* ptr descritor do inverted file */
    int treecase;
    PUNT punt;
    int lbufsiz,fd;
    off_t xbytes;

#if CICPP
    CISISX *cisisxp = dbxp->cisisxp;
#endif /* CICPP */

#if TRLTRACE
printf("leafwrit - dbxp=%p (%s)  l0p=%p \n",dbxp,DBXname,l0p);
#endif
    invp=DBXifmap;
    treecase = l0p->it - 1;
    punt = l0p->pos;
    if (invp->lybasep[treecase]) fatal("leafwrit/CNLI");
    if (l0p->ock < 1 || l0p->ock > TWORDF) fatal("leafwrit/ock");
    if (punt < 1 || punt > invp->cn[treecase].fmaxpos)
        if (punt != invp->cn[treecase].fmaxpos + 1) fatal("leafwrit/pos");
#if LINDLUX
    if (invp->luxthresh) {
        /* alloc */
        if (punt > invp->luxpages[treecase]) {
            if (punt != invp->luxpages[treecase]+1) fatal("leafwrit/lux/punt");
            if (punt > LUXPAGES) fatal("leafwrit/lux/overflow");
            if (invp->luxvpagp[treecase][punt]) fatal("leafwrit/lux/again");
            invp->luxvpagp[treecase][punt]=
                                (L0STRU *)ALLOC((ALLOPARM)lxbsiz[treecase]);
            if (invp->luxvpagp[treecase][punt] == (L0STRU *)ALLONULL)
                                                fatal("leafwrit/lux/ALLOC");
            invp->luxpages[treecase]++;
        }
        /* copy */
        memmove(invp->luxvpagp[treecase][punt],(char *)l0p,lxbsiz[treecase]);
        /* Update .cnt */
        if (punt > invp->cn[treecase].fmaxpos) {
            invp->cn[treecase].fmaxpos = punt;
            if (trmifupd == IFUPDXUP) cntwrit(dbxp);
        }
        return(RCNORMAL);
    }
#endif /* LINDLUX */
    if (treecase) {
        lbufsiz=L2BSIZ;
	if ((fd=invp->l2opn) <= 0 || !invp->l2opw) {
	    fd=dbxopenw(DBXname,DBXname,lx12extp[treecase],&invp->l2opn,&invp->l2opw,"leafwrit/l2opn/w");
	}
    }
    else {
        lbufsiz=L1BSIZ;
	if ((fd=invp->l1opn) <= 0 || !invp->l1opw) {
	    fd=dbxopenw(DBXname,DBXname,lx12extp[treecase],&invp->l1opn,&invp->l1opw,"leafwrit/l1opn/w");
	}
    }
    if (fd <= 0) fatal("leafwrit/fd");
#if TRLTRACE
printf("leafwrit - invp=%p  treecase=%d  punt=%"_LD_"\n",
    invp,treecase,(LONGX)punt);
#endif
    xbytes=((off_t)(punt-1L))*lbufsiz;
#if CNLI
    if (invp->cl_offset[treecase]) fatal("leafwrit/CNLI/cl_offset");
#endif
#if TRLTRACE
printf("leafwrit - xbytes=%"P_OFF_T" lbufsiz=%d \n",(LONG_LONG)xbytes,lbufsiz);
#endif

    if (LSEEK64(fd,xbytes,SEEK_SET) != xbytes) {
        printf("leafread - xbytes=%"P_OFF_T" punt=%"_LD_" lbufsiz=%d \n",(LONG_LONG)xbytes,(LONGX)punt, lbufsiz);            
        printf("errno=%"_LD_"\n", (LONGX)errno);
        fatal("leafwrit/lseek");
    }
#if CNV_PCBINUM
    memmove(cnv_pcbuff,(char *)l0p,lbufsiz);
    if (treecase) ConvertL02_REC(cnv_pcbuff);
             else ConvertL01_REC(cnv_pcbuff);
    if (CIWRITE(fd,cnv_pcbuff,lbufsiz) != lbufsiz) fatal("leafwrit/write");
#else
    if (CIWRITE(fd,(char *)l0p,lbufsiz) != lbufsiz) fatal("leafwrit/write");
#endif
    /* Update .cnt */
    if (punt > invp->cn[treecase].fmaxpos) {
        invp->cn[treecase].fmaxpos = punt;
        if (trmifupd == IFUPDXUP) cntwrit(dbxp);
    }
#if TRLTRACE
printf("leafwrit - ok \n");
#endif
    return(RCNORMAL);
}


#if CNV_PCBINUM
#if CICPP
int ifpwrit(DBXSTRU *dbxp,
            char    *buffer,
            LONGX     nbytes,
            INFO     hdblk[],
            INFO     hdoff[],
            int      hdn)
#else /*CICPP*/
int ifpwrit(dbxp,buffer,nbytes,hdblk,hdoff,hdn)
DBXSTRU *dbxp;           /* ptr descritor da base de dados */
char *buffer;            /* ptr if post's a gravar */
LONGX nbytes;             /* bytes contiguos a gravar  */
INFO hdblk[];
INFO hdoff[];
int hdn;
  /* hdblk,hdoff,hdn - armazenam as ocorrencias de headers de                */
  /*                   chaves que estao em buff                              */
  /* hdblk[i],hdoff[i] ->posicao do header i (blk,off)
     hdn-> qtda de headers
  */
#endif /*CICPP*/
#else /* CNV_PCBINUM */
#if CICPP
int ifpwrit(DBXSTRU *dbxp,
            char    *buffer,
            LONGX     nbytes)
#else /*CICPP*/
int ifpwrit(dbxp,buffer,nbytes)                                         /*
------------
            grava buffer de "nbytes/size(IFPSTRU)" blocos de postings;
            retorna RCNORMAL ou abenda;
                                                                        */
DBXSTRU *dbxp;           /* ptr descritor da base de dados */
char *buffer;            /* ptr if post's a gravar */
LONGX nbytes;             /* bytes contiguos a gravar  */
#endif /*CICPP*/
#endif /* CNV_PCBINUM */
{
    INVMAP *invp;       /* ptr descritor do inverted file */
    int fd;
    unsigned int towrite;

#if CICPP
    CISISX *cisisxp = dbxp->cisisxp;
#endif /* CICPP */

#if !LIND
    off_t xbytes;
#endif
#if CNV_PCBINUM
    INFO thisblk;
    int blk1;
    int i;
#endif /* CNV_PCBINUM */
#if TRPTRACE || TRPTRAC2
 int nn;
 IFPSTRU *qq;
 qq=(IFPSTRU *)buffer;
 printf("\nifpwrit - dbxp=%p (%s)  buffer=%p  nbytes=%_LD_",
 dbxp,DBXname,buffer,nbytes);
 for (nn=nbytes;nn>0;nn=nn-IFBSIZ,qq++){
   printf("\nifpwrit blocos existentes buffer: blk=%_LD_",qq->ifpblk);
}
#endif
    invp=DBXifmap;
    if ((fd=invp->ifopn) <= 0 || !invp->ifopw) {
	fd=dbxopenw(DBXname,DBXname,ix1extp,&invp->ifopn,&invp->ifopw,"ifpwrit/ifopn/w");
    }
    if (fd <= 0) fatal("ifpwrit/fd");
#if LIND
    LSEEK64(fd,0L,SEEK_END); /* append */
#else
    xbytes=((off_t)(((IFPSTRU *)buffer)->ifpblk - 1L))*sizeof(IFPSTRU );
    //if (LSEEK64(fd,xbytes,SEEK_SET) != xbytes) fatal("ifpwrit/seek");
    if (LSEEK64(fd,xbytes,SEEK_SET) != xbytes) {
      printf("xbytes=%"P_OFF_T" lseek=%"P_OFF_T"\n", (LONG_LONG)xbytes, (LONG_LONG)LSEEK64(fd,xbytes,SEEK_SET));
      fatal("ifpwrit/seek");
    }
#endif
    if (nbytes >= USHRT_MAX) fatal("ifpwrit/nbytes");
    towrite=nbytes;
#if LIND
    if (CIWRITE(fd,buffer,towrite) != towrite)
#else
#if CNV_PCBINUM
    memmove(cnv_pcbuff,buffer,towrite);
    blk1=0;
    if (((IFPSTRU *)buffer)->ifpblk == 1) blk1=1;
    for (xbytes=0; xbytes < towrite; xbytes+=IFBSIZ, blk1=0) {
	thisblk=(INFO )((IFPSTRU *)(buffer+xbytes))->ifpblk;
	/* extrai os headers do regitro atual */
	for (i=0; i <= hdn; i++) { /* se 1 elem: hdn = 0 (init = -1) */
#if TRACEhh
      printf("this=%"_LD_" hdb[%d]=%"_LD_" hdo[%d]=%"_LD_" ", thisblk,i,hdblk[i],i,hdoff[i]);
#endif
	    if (hdblk[i] == thisblk) {
#if CONVERTE
		ConvertIFP_PSTHDR(cnv_pcbuff+xbytes+hdoff[i]);
#endif
#if TRACEhh
                print_head((IFPSTRU *)(cnv_pcbuff+xbytes),hdoff[i]);
#endif
                hdblk[i]=-hdblk[i]; /* Limpa */
	    }
#if TRACEhh
	    else { printf("\n");}
#endif
	}
#if CONVERTE
        ConvertIFP_BLKCTL(cnv_pcbuff+xbytes,blk1);
#endif /* CONVERTE */
    }
    if (CIWRITE(fd,cnv_pcbuff,towrite) != towrite)
#else /* CNV_PCBINUM */
    if (CIWRITE(fd,buffer,towrite) != towrite)
#endif /* CNV_PCBINUM */
#endif
        fatal("ifpwrit/write");

#if TRPTRACE
printf("ifpwrit - ok \n");
#endif
    return(RCNORMAL);
}


#if CICPP
int trmisis0(CISISX *cisisxp, char *dbnamp)
#else /*CICPP*/
int trmisis0(dbnamp)                                                  /*
------------
                    cria .cnt;
                    cria .n0x e .l0x vazios;
                    cria .ifp;
                    retorna CREAT()
                                                                      */
char *dbnamp;       /* ptr nome bases de dados */
#endif /*CICPP*/
{
    int fd;
    char area[CIMPL+1],*p;
    int treecase;
    CNSTRU cntbuff[2];
    int unin;
#if !LIND
    IFPSTRU ifpbuff;
#endif
#if MPE
    char dcb[80];
#endif

#if TR0TRACE
printf("trmisis0 - dbnamp=%s\n",dbnamp);
#endif

#if MULTI
    if (dbxstorp(dbnamp)->dbxnetws != MONONETS)
	fatal("trmisis0/MONONETS expected");
#endif

    /* .cnt */
    strcpy(area,dbnamp);
    strcat(area,cx1extp);
    strcpy(area,dbxcipar(dbnamp,area,'='));
#if TR0TRACE
printf("trmisis0 - filename='%s'  len=%d\n",area,strlen(area));
#endif
#if MPE
    sprintf(dcb,"b R%3d",sizeof(CNSTRU));
    if ((fd=OPEN(area,
                O_WRONLY|O_CREAT|O_TRUNC|O_MPEOPTS,PERMIS,dcb)) <= 0) {
#else
    if ((fd=CREAT(area,PERMIS)) <= 0) {
#endif
        fatal("trmisis0/cnt");
    }
    for (treecase=0; treecase < 2; treecase++) {
        if (trmtrace) printf("+++ trmisis0 - %s=%d [id=%d]\n",area,fd,treecase+1);
        cntbuff[treecase].idtype=treecase+1;
        cntbuff[treecase].ordn=ORDN;
        cntbuff[treecase].ordf=ORDF;
        cntbuff[treecase].n=CNTN;
        cntbuff[treecase].k=CNTK;
        cntbuff[treecase].liv=NEGLIV;
        cntbuff[treecase].posrx=0;
        cntbuff[treecase].nmaxpos=0;
        cntbuff[treecase].fmaxpos=0;
        cntbuff[treecase].abnormal=ABNORMAL;
        p=(char *)&cntbuff[treecase];
#if CNV_PCFILES
	unin=CNBSIZ-CNBUNI;
#else
	unin=CNBSIZ;
#endif
#if CNV_PCBINUM
#if CONVERTE
	ConvertCNT_REC(p); /* local */
#endif
#endif
	if (CIWRITE(fd,p,unin) != unin) fatal("trmisis0/write/cnt");
    }
    CLOSE(fd);

    /* .l01 */
    strcpy(area,dbnamp);
    strcat(area,lx12extp[0]);
    strcpy(area,dbxcipar(dbnamp,area,'='));
#if TR0TRACE
printf("trmisis0 - filename='%s'  len=%d\n",area,strlen(area));
#endif
#if MPE
    sprintf(dcb,"b R%3d",sizeof(L1STRU));
    if ((fd=OPEN(area,
                O_WRONLY|O_CREAT|O_TRUNC|O_MPEOPTS,PERMIS,dcb)) <= 0) {
#else
    if ((fd=CREAT(area,PERMIS)) <= 0) {
#endif
        fatal("trmisis0/.l01");
    }
    if (trmtrace) printf("+++ trmisis0 - %s=%d\n",area,fd);
    CLOSE(fd);

    /* .l02 */
    strcpy(area,dbnamp);
    strcat(area,lx12extp[1]);
    strcpy(area,dbxcipar(dbnamp,area,'='));
#if TR0TRACE
printf("trmisis0 - filename='%s'  len=%d\n",area,strlen(area));
#endif
#if MPE
    sprintf(dcb,"b R%3d",sizeof(L2STRU));
    if ((fd=OPEN(area,
                O_WRONLY|O_CREAT|O_TRUNC|O_MPEOPTS,PERMIS,dcb)) <= 0) {
#else
    if ((fd=CREAT(area,PERMIS)) <= 0) {
#endif
        fatal("trmisis0/.l02");
    }
    if (trmtrace) printf("+++ trmisis0 - %s=%d\n",area,fd);
    CLOSE(fd);

    /* .n01 */
    strcpy(area,dbnamp);
    strcat(area,nx12extp[0]);
    strcpy(area,dbxcipar(dbnamp,area,'='));
#if TR0TRACE
printf("trmisis0 - filename='%s'  len=%d\n",area,strlen(area));
#endif
#if MPE
    sprintf(dcb,"b R%3d",sizeof(N1STRU));
    if ((fd=OPEN(area,
                O_WRONLY|O_CREAT|O_TRUNC|O_MPEOPTS,PERMIS,dcb)) <= 0) {
#else
    if ((fd=CREAT(area,PERMIS)) <= 0) {
#endif
        fatal("trmisis0/.n01");
    }
    if (trmtrace) printf("+++ trmisis0 - %s=%d\n",area,fd);
    CLOSE(fd);

    /* .n02 */
    strcpy(area,dbnamp);
    strcat(area,nx12extp[1]);
    strcpy(area,dbxcipar(dbnamp,area,'='));
#if TR0TRACE
printf("trmisis0 - filename='%s'  len=%d\n",area,strlen(area));
#endif
#if MPE
    sprintf(dcb,"b R%3d",sizeof(N2STRU));
    if ((fd=OPEN(area,
                O_WRONLY|O_CREAT|O_TRUNC|O_MPEOPTS,PERMIS,dcb)) <= 0) {
#else
    if ((fd=CREAT(area,PERMIS)) <= 0) {
#endif
        fatal("trmisis0/.n02");
    }
    if (trmtrace) printf("+++ trmisis0 - %s=%d\n",area,fd);
    CLOSE(fd);

    /* .ifp */
    strcpy(area,dbnamp);
    strcat(area,ix1extp);
    strcpy(area,dbxcipar(dbnamp,area,'='));
#if TR0TRACE
printf("trmisis0 - filename='%s'  len=%d\n",area,strlen(area));
#endif
#if MPE
    sprintf(dcb,"b R%3d",sizeof(IFPSTRU));
    if ((fd=OPEN(area,
                O_WRONLY|O_CREAT|O_TRUNC|O_MPEOPTS,PERMIS,dcb)) <= 0) {
#else
    if ((fd=CREAT(area,PERMIS)) <= 0) {
#endif
	fatal("trmisis0/ifp");
    }
    if (trmtrace) printf("+++ trmisis0 - %s=%d\n",area,fd);
#if !LIND
    p=(char *)&ifpbuff;
    memset(p,0xFF,sizeof(IFPSTRU));
    ifpbuff.ifpblk=1L;
    ifpbuff.ifprec[0]=1L;
    ifpbuff.ifprec[1]=2L;
#if CNV_PCBINUM
#if CONVERTE
    ConvertIFP_BLKCTL(p,1); /* local */
#endif
#endif
    if (CIWRITE(fd,p,sizeof(IFPSTRU)) != sizeof(IFPSTRU)) {
	fatal("trmisis0/write/ifp");
    }
#endif
    CLOSE(fd);

#if TR0TRACE
printf("trmisis0 - ok \n");
#endif
    return(fd);
}


#if TRACEhh
#if CICPP
void print_head(IFPSTRU *p,
                INFO     off)
#else /* CICPP */
void print_head(p,off)
IFPSTRU *p;
INFO off;
#endif /* CICPP */
{
IFPHEAD  *ifhead;
if (off >=0) {
   ifhead = (IFPHEAD *) &(p->ifprec[off]);
   printf("->Nxtb=%lu Nxtp=%lu totp=%lu segp=%lu segc=%lu\n",
		  ifhead->ifpnxtb,ifhead->ifpnxtp,ifhead->ifptotp,
		  ifhead->ifpsegp,ifhead->ifpsegc);
}
}
#endif /*TRACEhh*/
#if DEBIFUPD
/*------------------------------------------------------------------------*/
#if CICPP
void lifp_print_cnt(INVMAP *invp)
#else /*CICPP*/
void lifp_print_cnt(invp)
INVMAP *invp;
#endif /*CICPP*/
{ int treecase;
printf(
"\n-------------------------------------------------------------------------");
printf(
"\nTree|Type |Ordn |Ordf |N   |k   |Liv |Posrx |Nmaxpos |Fmaxpos |Abnormal|");
printf(
"\n----|-----|-----|-----|----|----|----|------|--------|--------|---------|");
treecase=0;
printf("\n%4d|%5d|%5d|%5d|%4d|%4d|%4d|%6"_LD_"|%8"_LD_"|%8"_LD_"|%9d|",
treecase,
invp->cn[treecase].idtype ,
invp->cn[treecase].ordn ,
invp->cn[treecase].ordf ,
invp->cn[treecase].n ,
invp->cn[treecase].k ,
invp->cn[treecase].liv ,
invp->cn[treecase].posrx ,
invp->cn[treecase].nmaxpos ,
invp->cn[treecase].fmaxpos ,
invp->cn[treecase].abnormal);
printf(
"\n----|-----|-----|-----|----|----|----|------|--------|--------|---------|");
treecase=1;
printf("\n%4d|%5d|%5d|%5d|%4d|%4d|%4d|%6"_LD_"|%8"_LD_"|%8"_LD_"|%9d|",
treecase,
invp->cn[treecase].idtype ,
invp->cn[treecase].ordn ,
invp->cn[treecase].ordf ,
invp->cn[treecase].n ,
invp->cn[treecase].k ,
invp->cn[treecase].liv ,
invp->cn[treecase].posrx ,
invp->cn[treecase].nmaxpos ,
invp->cn[treecase].fmaxpos ,
invp->cn[treecase].abnormal);
printf(
"\n-------------------------------------------------------------------------");
}
/* ------------------------------------------------------------------------*/
#if CICPP
void upif_chgtostr(UCHR *v,
                   int   n,
                   UCHR *tkey)
#else /*CICPP*/
void upif_chgtostr(v,n,tkey)
UCHR *v;
int n;
UCHR *tkey;
#endif /*CICPP*/
{
 memmove((UCHR *)tkey,v,n);
 tkey[n]='\0';
}
/* ------------------------------------------------------------------------*/
#if CICPP
void upif_print_node(N0STRU *n0p)
#else /*CICPP*/
void upif_print_node(n0p)
N0STRU *n0p;
#endif /*CICPP*/
{
  int ock,i,keysize;
  int treecase;
  PUNT punt;
  N1STRU *n1p;
  N2STRU *n2p;
  UCHR  key[LE2+1];
  ock=n0p->ock;
  treecase=n0p->it-1;
  n1p=(N1STRU *)n0p;
  n2p=(N2STRU *)n0p;
  keysize=vlex[treecase];
  printf("\n  pos=%"_LD_", ock=%d, treecase=%d",n0p->pos,ock,treecase);
  printf("\n");
  for (i=0;i<ock;i++){
/*  if(i-i/4*4==0)printf("\n    "); */
    printf("\n");
    if (treecase == 0){
       memmove(key,n1p->idx[i].key,keysize);
       punt= n1p->idx[i].punt;
    }
    else {
       memmove(key,n2p->idx[i].key,keysize);
       punt= n2p->idx[i].punt;
    }
    key[keysize]='\0';
    printf(" +++ >%s %"_LD_" ;",(char *)key,punt);
  }
}
/*-------------------------------------------------------------------------*/
#if CICPP
void upif_print_leaf(L0STRU *l0p)
#else /*CICPP*/
void upif_print_leaf(l0p)
L0STRU *l0p;
#endif /*CICPP*/
{ int ock,i,treecase,keysize;
  UCHR  key[LE2+1];
  L1STRU *l1p;
  L2STRU *l2p;
  INFO info1,info2;
  ock=l0p->ock;
  treecase=l0p->it-1;
  keysize=vlex[treecase];
  printf("\n pos=%"_LD_",ock=%d,it=%d,ps=%"_LD_,
	l0p->pos,ock,treecase,l0p->ps );
  ock=l0p->ock;
  l1p=(L1STRU *)l0p;
  l2p=(L2STRU *)l0p;
  for (i=0;i<ock;i++){
     if (i-i/4*4==0)printf("\n  ");
     if (treecase == 0 ) {
     memmove((char *)key ,l1p->idx[i].key,keysize);
     info1=l1p->idx[i].info1;
     info2=l1p->idx[i].info2;
     }
     else {
     memmove((char *)key ,l2p->idx[i].key,keysize);
     info1=l2p->idx[i].info1;
     info2=l2p->idx[i].info2;
     }
     key[keysize]='\0';
     printf("%s, %"_LD_", %"_LD_" ;",
	key,info1,info2);
  }
}

/* -----------------------------------------------------------------------*/
#if CICPP
void upif_print_btree(INVMAP *invp,
		      int     ident,
		      int     level,
		      PUNT    punt,
		      int     treecase,
		      FILE   *fout)
#else /*CICPP*/
void upif_print_btree(invp,ident,level,punt,treecase,fout)
INVMAP *invp;
int ident;
int level;
PUNT punt;
int treecase;
FILE *fout;
#endif /*CICPP*/
{
  int ock,i;
  UCHR  *np;
  N2STRU *n2p, *keep_node2,area_node2;
  N1STRU *n1p, *keep_node1,area_node1;
  N0STRU *n0local;
  L0STRU *l0p;
  L1STRU *l1p;
  L2STRU *l2p;
  PUNT next_punt;
  UCHR lbufp[sizeof(L2STRU)+1];
  UCHR  key[LE2+1];
  INFO info1, info2;
  int keysize;
  keep_node1=&area_node1;
  keep_node2=&area_node2;
  keysize=vlex[treecase];
  if (punt==0) return;
  if (punt<0) {
	punt=-punt;
    l0p=(L0STRU *)leafread(lbufp,invp,treecase,punt,0);
    l1p=(L1STRU *)l0p;
    l2p=(L2STRU *)l0p;
    fprintf(fout,"\n     Leaf  punt=%"_LD_" ock=%d\n",-punt,l0p->ock);
    for (i=0;i<l0p->ock;i++){
      if(i-i/4*4==0) {
	fprintf(fout,"\n");
      }
      if (treecase == 0 ) {
	 memmove((char *)key ,l1p->idx[i].key,keysize);
	 info1=l1p->idx[i].info1;
	 info2=l1p->idx[i].info2;
      }
      else {
	  memmove((char *)key ,l2p->idx[i].key,keysize);
	  info1=l2p->idx[i].info1;
	  info2=l2p->idx[i].info2;
      }
      key[keysize]='\0';
      printf("%s, %"_LD_", %"_LD_" ;",
	      key,info1,info2);
    }
	return;
  }
  if (punt<=0 ) return;
  np=(UCHR *)noderead(invp,treecase,level,punt);
  n1p=(N1STRU *)np;
  n2p=(N2STRU *)np;
  if (treecase == 0) {
     memmove((UCHR *)keep_node1,(UCHR *)np,sizeof(N1STRU));
     n0local=(N0STRU *)keep_node1;
     n1p=(N1STRU *)n0local;
  }
  else {
     memmove((UCHR *)keep_node2,(UCHR *)np,sizeof(N2STRU));
     n0local=(N0STRU *)keep_node2;
     n2p=(N2STRU *)n0local;
  }
  fprintf(fout,"\n");
  for (i=0;i<ident;i++)fprintf(fout,"-");
  fprintf(fout,"pos=%"_LD_" ock=%d type=%d",n0local->pos,
	n0local->ock,n0local->it);
  ock=n0local->ock;
  upif_print_node(n0local);
  for (i=0;i<ock;i++){
      if (treecase == 0)
     next_punt = n1p->idx[i].punt;
      else
     next_punt = n2p->idx[i].punt;
	 upif_print_btree(/* trmp,*/ invp,ident+2,level+1,
	       next_punt,treecase,fout);
  }
}
/* -----------------------------------------------------------------------*/
#if CICPP
void upif_gera_keys(INVMAP *invp,
		    int     ident,
		    int     level,
		    PUNT    punt,
		    int     treecase,
		    FILE   *fkeys)
#else /*CICPP*/

void upif_gera_keys(invp,ident,level,punt,treecase,fkeys)
INVMAP *invp;
int ident;
int level;
PUNT punt;
int treecase;
FILE *fkeys;
#endif /*CICPP*/
{
  int ock,i,keysize;
  UCHR  *np;
  N2STRU *n2p, *keep_node2,area_node2;
  N1STRU *n1p, *keep_node1,area_node1;
  N0STRU *n0local;
  L0STRU *l0p;
  L1STRU *l1p;
  L2STRU *l2p;
  PUNT next_punt;
  PUNT info1,info2;
  UCHR lbufp[sizeof(L2STRU)+1];
  UCHR  key[LE2+1];
  keep_node1=&area_node1;
  keep_node2=&area_node2;
  keysize=vlex[treecase];
  if (punt==0) return;
  if (punt<0) {
	punt=-punt;
    l0p=(L0STRU *)leafread(lbufp,invp,treecase,punt,0);
    l1p=(L1STRU *)l0p;
    l2p=(L2STRU *)l0p;
    for (i=0;i<l0p->ock;i++){
      if (treecase == 0) {
	 memmove((char *)key,l1p->idx[i].key,keysize);
	 info1=l1p->idx[i].info1;
	 info2=l1p->idx[i].info2;
      }
      else{
	 memmove((char *)key,l2p->idx[i].key,keysize);
	 info1=l2p->idx[i].info1;
	 info2=l2p->idx[i].info2;
      }
      key[keysize]='\0';
      fprintf(fkeys,"%s , %"_LD_" * %"_LD_" \n",key,info1,info2);
    }/* FOR */
	return;
  } /* punt > 0 */
  np=(UCHR *)noderead(invp,treecase,level,punt);
  n1p=(N1STRU *)np;
  n2p=(N2STRU *)np;
  if (treecase == 0) {
     memmove((UCHR *)keep_node1,(UCHR *)np,sizeof(N1STRU));
     n0local=(N0STRU *)keep_node1;
     n1p=(N1STRU *)n0local;
  }
  else {
     memmove((UCHR *)keep_node2,(UCHR *)np,sizeof(N2STRU));
     n0local=(N0STRU *)keep_node2;
     n2p=(N2STRU *)n0local;
  }
  ock=n0local->ock;
  for (i=0;i<ock;i++){
      if (treecase == 0)
     next_punt = n1p->idx[i].punt;
      else
     next_punt = n2p->idx[i].punt;
	 upif_gera_keys(/* trmp,*/ invp,ident+2,level+1,
	       next_punt,treecase,fkeys);
  }
}
#endif /* DEBIFUPD */

#if CNV_PCBINUM
/*--------------------------------------------------------------------------*/
/* Armazena o par bloco/offset se ainda nao estiver armazenado              */
/*   Verifica se ha espaco nos vetores.                                     */
/*--------------------------------------------------------------------------*/
#if CICPP
void ifp_ins_new_hd(INFO blk,
                    INFO off,
                    INFO hdblk[],
                    INFO hdoff[],
                    int  hdmax,
                    int *hdn)
#else /* CICPP */
void ifp_ins_new_hd(blk,off,hdblk,hdoff,hdmax,hdn)
 INFO blk;
 INFO off;
 INFO hdblk[];
 INFO hdoff[];
 int  hdmax;
 int *hdn;
#endif /* CICPP */
 {
   int i;
   /* Verifica se  se ja existe uma entrada no vetor para o par */

   for (i=*hdn;i>=0;i--) {
     if (hdblk[i]==blk && hdoff[i]==off) break; /*existe */
   }

  if (i<0){ /* Nao esta */
   *hdn=*hdn+1;
   if (*hdn<hdmax){
      hdblk[*hdn]=blk;
      hdoff[*hdn]=off;
   } else fatal ("ifp_ins_new_hd");
  }
#if TRACEhh
     printf("\n===Ins===\n");
   for (i=*hdn;i>=0;i--) {
     printf("\n[InsHd] hdblk[%d]=%d hdoff[%d]=%d",i,hdblk[i],hdoff[i]);
   }

#endif
}

/*--------------------------------------------------------------------------*/
/* Inicializa as estruturas que guardam os headers de um ifp                */
/*                                                                          */
/*--------------------------------------------------------------------------*/
#if CICPP
void ifp_init_hd(int  *hdn)
#else /* CICPP */
void ifp_init_hd(hdn)
 int  *hdn;
#endif /* CICPP */
{
 *hdn=-1; /* ISTO E' SUFICIENTE */
#if TRACEhh
  printf("  Inicializou hdn\n");
#endif
}

#endif /* CNV_PCBINUM */

