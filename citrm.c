#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "cisis.h"

#if CICPP
#include <citrm.hpp>
#define CITRM_SOURCE
#include <cirun.hpp>
/* ------------------------- CITRM.HPP --------------------------------- */
#define trminit                         xtrminit                  /* internal use */
#define trmalloc(itrm,nbytes)           xtrmalloc(nbytes)         /* internal use */
#define term(itrm,dbnamp,keyp)          xterm(dbnamp,keyp)        /* internal use */
#define trmread(trmp,treecase,retkeyp)  xtrmread(treecase,retkeyp)                   /* internal use */
/*#define cntread                         xcntread                   moved to CISISX */
/*#define invsetup                        xinvsetup                  moved to CISISX */
/*#define noderead                        xnoderead                  moved to CISISX */
/*#define leafread                        xleafread                  moved to CISISX */
#define nxterm                          xnxterm                   /* internal use */
#define posthdr1(trmp)                  xposthdr1()               /* internal use */
#define posting(itrm,nord)              xposting(nord)            /* internal use */
#define postpost(trmp)                  xpostpost()               /* internal use */
/*#define postread                        xpostread                  moved to CISISX */
/* --------------------------------------------------------------------- */
#endif /* CICPP */

/* ----------------------------- trm.c ------------------------------ */
#define TRXTRACE    00
#define TRMTRACE    00
#define TRATRACE    0
#define TRITRACE    0
#define TRRTRACE    0
#define TRSTRACE    00007
#define TRSXRACE    00007
#define TRNTRACE    000
#define TRLTRACE    00
#define TRPTRACE    00
#define TRETRACE    0
#define TRYTRACE    00
#define TRCTRACE    0

#define SAMTRACx    0
#define SAMTRACE    00          /* last */

#define IFPDUMMY    1           /* .ifp may be absent for validation */

#if !CICPP
/* global */
int trmifupd=0;                         /* trm IFUPDATE operation */

off_t lrlrseek=0L;
int lrlrsize=0;

char lasttrmread[CIMPL+1] = "";             /* 950724 */

#if PC
UCHR highv[LE2+1] = { 0xFF, '\0' };     /* manca controllo.. if signed */
#else
UCHR highv[LE2+1] = { 0x7F, '\0' };     /* manca controllo.. if signed */
#endif

#endif /* CICPP */

#if !CICPP
#define CITRM_CISISX_SOURCE 1
#include "cisisx.c"
#endif

#if CICPP
#define NO_ITRM   -1L
#endif /* CICPP */

#if CICPP
TRMSTRU :: TRMSTRU (CISISX *cxp)
{
    TRMSTRU *trmp;
    memset (this, 0x00, sizeof(*this));         // sizeof X member functions ?
    trmp = this;
    TRMnbytes = -1;
    cisisxp = cxp;
    if (cisisxp == NULL)
     fatal("TRMSTRU/cisisxp==NULL");
}
#endif /* CICPP */


#if CICPP
void TRMSTRU :: xtrminit(void)
#else /* CICPP */
void trminit()                                                        /*
------------
		    inicializa vtrmp;
		    expande highv;
		    checa defines;
								      */
#endif /* CICPP */
{
#if !LIND
    int n;
#endif

    memset(&highv[1],highv[0],LE2-1); highv[LE2]='\0';

#if !CICPP
    for (ntrms=maxntrm;ntrms--; )
	vtrmp[ntrms]=NULL;
    ntrms=0;

#if !LIND
    n=IFHDRINFOS*INFOSIZE;
    if (n != IFHDRSIZ)
	fatal("trminit/check2");
    n=IFPSTINFOS*INFOSIZE;
    if (n != IFPSTSIZ)
	fatal("trminit/check3");
#endif

#if TRITRACE
printf("trminit - ntrms=%"_LD_" maxntrm=%"_LD_"\n",ntrms,maxntrm);
#endif

#endif /* CICPP */
}


#if CICPP
LONGX TRMSTRU :: xtrmalloc(LONGX nbytes)
#else /* CICPP */
LONGX trmalloc(itrm,nbytes)                                             /*
-------------
		    abenda se nbytes != IFBSIZ, se diferente de zero;
		    abenda se ja' alocado;
		    aloca TRMHSIZE+nbytes bytes;
		    aponta por vtrmp[itrm];
		    garante inicializacao com zeros;
		    armazena vtrmp[itrm]->trmnbytes;
		    retorna itrm
								      */
LONGX itrm;          /* indice de vtrmp, para ALLOC */
LONGX nbytes;        /* tamanho da area em bytes */
#endif /* CICPP */
{
    TRMSTRU *trmp;

#if CICPP
    trminit();
    LONGX itrm = NO_ITRM;
#else /* CICPP */
    if (!ntrms)
	trminit();

    if (itrm < 0 || itrm >= maxntrm)
	fatal("trmalloc/index");
    if (ntrms >= maxntrm)
	fatal("trmalloc/overflow");
#endif /* CICPP */

    if (nbytes)
	if (nbytes != IFBSIZ)
	    fatal("trmalloc/nbytes");

#if !CICPP
    if (vtrmp[itrm])
	fatal("trmalloc/again");
    if ((TRMHSIZE+nbytes) > ALLOMAXV)
	fatal("trmalloc/ALLOMAXV/TRMHSIZE+nbytes");
    trmp=(TRMSTRU *)ALLOC((ALLOPARM)(TRMHSIZE+nbytes));
    if (trmp == (TRMSTRU *)ALLONULL)
	fatal("trmalloc/ALLOC");
    vtrmp[itrm]=trmp;
    ntrms++;
    memset(trmp,0x00,(size_t)(TRMHSIZE+nbytes));
#else /* CICPP */
    trmp = this;
#endif /* CICPP */
    TRMnbytes=nbytes;
    TRMtype=TYPETRM;
#if TRATRACE
#if !CICPP
printf("trmalloc - vtrmp[%"_LD_"]=%p ntrms=%"_LD_"\n",itrm,vtrmp[itrm],ntrms);
#endif /* CICPP */
printf("trmalloc - TRMtype=%d TRMnbytes=%"_LD_"\n",TRMtype,TRMnbytes);
#endif
    return(itrm);
}
#if CICPP
LONGX TRMSTRU :: xterm(UCHR *dbnamp,
		      UCHR *keyp)
#else /* CICPP */
LONGX term(itrm,dbnamp,keyp)                                           /*
---------
		    inicializa vtrmp, se necessario;
		    toma valor absoluto de itrm;
		    aloca o registro itrm, se ainda nao alocado;
		    seta dbxp com o resultado de dbxstorp(dbnamp);
		    chama invsetup, se necessario;
		    guarda chave preenchida com brancos em trmp->trmkey;
		    procura termo exato;
		    se nao achou,
		      a) se itrm_in < 0 fica com primeiro resultado, ou
		      b) procura na outra arvore e fica com a menor;
		    move termo para trmp->trmkey;
		    seta trmp->trmrc com retorno de trmread();
		    seta controles para nxterm();
		    chama posthdr1;
		    retorna itrm_in
								      */
LONGX itrm;          /* indice de vtrmp, para store */
UCHR *dbnamp;       /* ptr nome base de dados a ler */
UCHR *keyp;         /* termo a recuperar */
#endif /* CICPP */
{

#if CICPP
  LONGX itrm = NO_ITRM;
#endif /* CICPP */

    TRMSTRU *trmp;
    DBXSTRU *dbxp;
    LONGX itrm_in;
    int treecase;
    int len,n,n1,tc2,n2,rc2,kn;
    UCHR *p,*k1p,*k2p,*dbnp;
#if TRXTRACE
    UCHR area[LE2+1];
#endif
#if SAMEL
    char dbxname[sizeof(DBXname)],*dbp;
    int lxx;
#endif
    if (!keyp) keyp=(unsigned char *)"";
    dbnp=dbnamp;
#if SAMEL
    lxx=0;
    strcpy(dbxname,dbp=dbnamp);
    for (p=dbnamp=dbxname; *p; p++)
	if (*p == '#') {
	    *p++ = '\0';
	    for (; isdigit(*p); p++) lxx=10*lxx+(int)*p-(int)'0';
	    if (*p) fatal(dbp);
	    if (lxx < 1 || lxx > MAXSAMEL) {
		printf("*** dbname=%s\n",dbp);
		fatal("term/dbname");
	    }
	    break;
	}
    if (trmtrace) printf("term - %s,%s,lxx=%d\n",dbp,dbnamp,lxx);
#endif
#if CICPP
    trminit();
    itrm_in = itrm;
#else /* CICPP */
    if (!ntrms)
	trminit();
    itrm_in = labs(itrm);
#endif /* CICPP */
#if CICPP
    trmp = this;
#else /* CICPP */
    if (itrm_in < 0 || itrm_in >= maxntrm)
	fatal("term/index");
    trmp=vtrmp[itrm_in];
#endif /* CICPP */
#if CICPP
    if (TRMnbytes < 0)
	xtrmalloc((LONGX)IFBSIZ);
#else /* CICPP */
    if (!trmp)
	trmp=vtrmp[trmalloc(itrm_in,(LONGX)IFBSIZ)];
#endif /* CICPP */

    dbxp=dbxstorp((char *)dbnamp);
#if SAMEL
    DBXiflxx=lxx;
#endif

#if BEFORE950724
#else
    if (strcmp((CONST char *)dbnamp,lasttrmread)) {
        if (lasttrmread[0]) {
	    DBXSTRU *lastdbxp=dbxsrchp(lasttrmread);
	    if (lastdbxp) if (lastdbxp->dbxiflush) invflush(lasttrmread);
	}
        strcpy(lasttrmread,(CONST char *)dbnamp);
    }
#endif

#if TRMTRACE
printf("term - itrm=%"_LD_"  trmp=%p  dbxp=%p  keyp=%s\n",itrm,trmp,dbxp,keyp);
#endif

    if (TRMdbxp) {
/* ver MULTI */
#if SAMEL
	if (strcmp(TDBname,dbnamp) || TDBiflxx != lxx) {        /* v3.2 */
#else
	if (strcmp(TDBname,(CONST char *)dbnamp)) {             /* v3.2 */
/*
   Para nao referenciar TDBname quando TRMdbxp liberado por dbxflush:
   if ((TRMdbxp!=dbxp) || (strcmp(TDBname,(CONST char *)dbnamp)))
   (HB 23/11/98)
*/

#endif
	    TRMl1buf.pos=0;
	    TRMl2buf.pos=0;
#if 0
printf("now:\n");
printf("TRMl1buf.pos=%d TRMl2buf.pos=%d\n",TRMl1buf.pos,TRMl2buf.pos);
#endif
#if !LIND
	    if (TRMnbytes) ((IFPSTRU *)TRMifbuf)->ifpblk=0;     /* 26/02/92 */
#endif
	}
    }

    TRMdbxp=dbxp;

/* ver MULTI */
    if (!TDBifmap)
	invsetup(dbnp,0L,0L,0L);

    len=strlen((CONST char *)keyp);
    for (p=keyp+len-1; len; )
	if (isspace(*p)) {
	    p--; len--;
	}
	else
	    break;
    if (len > LE2) len=LE2;
    memmove(TRMkey,keyp,len);
    memset(TRMkey+len,' ',LE2-len); TRMkey[LE2]='\0';
#if TRMTRACE
printf("term - len=%d  TRMkey='%s'(%d)\n",len,TRMkey,strlen(TRMkey));
#endif
    if (len <= LE1) {
	treecase=0; n1=LE1;
	tc2=1; n2=LE2;
    }
    else {
	treecase=1; n1=LE2;
	tc2=0; n2=LE1;
    }
#if TRMTRACE
printf("term - treecase=%d  n1=%d  tc2=%d  n2=%d\n",treecase,n1,tc2,n2);
#endif

    TRMrc=trmread(trmp,treecase,&k1p); TRMlidxp(treecase)=k1p;

#if TRXTRACE
memmove(area,k1p,n1); area[n1]='\0';
printf("term - TRMrc=%d (em %d)  k1p=%p=%s\n",TRMrc,treecase,k1p,area);
#endif

#if CICPP
    if (TRMrc == RCNORMAL) {
#else
    if (TRMrc == RCNORMAL || itrm < 0) {
#endif /* CICPP */
	kn=n1;
	TRMlcase=treecase; TRMlidxp(tc2)=NULL;
    }
    else {
	if (tc2 == 0)
	    TRMkey[LE1-1]++;                    /* OLD: nothing */
            
	rc2=trmread(trmp,tc2,&k2p); TRMlidxp(tc2)=k2p;

#if TRXTRACE
memmove(area,k2p,n2); area[n2]='\0';
printf("term - TRMrc=%d (em %d)  k2p=%p=%s\n",TRMrc,tc2,k2p,area);
#endif
	if (rc2 == RCNORMAL) {
	    TRMrc=RCNEXT; kn=n2;                /* OLD: TRMrc=rc2 */
	    TRMlcase=tc2;
	}
	else {
            n=strncmp((CONST char *)k1p,(CONST char *)k2p,LE1);             /* min */
#if TRXTRACE
memmove(area,k1p,n1); area[n1]='\0';
printf("term - n=%d[%d-%d] p/ strncmp(%s,",n,*k1p,*k2p,area);
memmove(area,k2p,n2); area[n2]='\0';
printf("%s,%d)\n",area,LE1);
#endif
	    if (n == 0) {		/* AOT/HB - 08/04/97 */
	       if (treecase) n=1;	/* get LE1 key */
	    }

	    if (n <= 0) {
		memmove(TRMkey,k1p,n1);         /* k1p is less than k2p */
		TRMlcase=treecase;
		kn=n1;
	    }
	    else {
		memmove(TRMkey,k2p,n2);         /* k2p is less than k1p */
		TRMlcase=tc2;
		TRMrc=rc2; kn=n2;
	    }
	}
    }
    *(TRMkey+kn)='\0';
#if SAMTRACE
printf("term - TRMrc=%d  TRMlcase=%d,%p,%p  TRMkey=%s(%d) TRMliock=%d/%d \n",
 TRMrc,TRMlcase,TRMlidxp(0),TRMlidxp(1),TRMkey,strlen(TRMkey),
  TRMliock(0),TRMliock(1));
#endif
    posthdr1(trmp);                                        /* v3.2 */
#if TRXTRACE
printf("term - TRMrc=%d  TRMlcase=%d,%p,%p  TRMkey=%s(%d)\n",
TRMrc,TRMlcase,TRMlidxp(0),TRMlidxp(1),TRMkey,strlen(TRMkey));
#endif
    if (trmtrace) printf("+++ term(%"_LD_",%s,%s) = %s (%d) [%d:%"_LD_",%d]\n",
				       itrm,dbnamp,keyp,TRMkey,TRMrc,
				       TRMlcase,
				       (TRMlcase)?TRMl2buf.pos:TRMl1buf.pos,
				       TRMliock(TRMlcase));

#define TRMINVFLUSH 0
#if TRMINVFLUSH /* 950724 */
    invflush(DBXname);
#endif

    return(itrm_in);
}


#if CICPP
int TRMSTRU :: xtrmread(int      treecase,
			UCHR   **retkeyp)
#else /* CICPP */
int trmread(trmp,treecase,retkeyp)                                    /*
-----------
		    //seta dbxp;
		    seta invp;
		    seta parametros de acordo com treecase;
		    retorna RCEOF se liv = NEGLIV;               3.1 
		    se trmp->key < primeiro termo, fica com primeiro termo;
		    busca nos Nos o punt da Folha onde deve estar trmp->key;
		    procura trmp->key nessa Folha, apontando por retkeyp;
		    se trmp->key > ultimo termo, aponta para high-value;
		    retorna RCEOF, RCNORMAL ou RCNEXT ou aborta
								      */
TRMSTRU *trmp;      /* elemento de vtrmp, com chave ja' setada */
int treecase;       /* indice da B*Tree a pesquisar */
UCHR **retkeyp;     /* endereco do apontador para a chave encontrada */
#endif /* CICPP */
{
    INVMAP  *invp;
    N0STRU  *np;
    L0STRU  *lp;
    UCHR *wp,*w0p;
    int idxesiz,idxksiz;
    UCHR *kp;
    PUNT punt;
    int rc,n,n0;
    UCHR *lbufp;
    int lidxesiz,level,liv,kk;
    UCHR *rp,*p;
    int lxx;
#if TRRTRACE /* 1 */
    UCHR area[LE2+1];
#endif
#if SAMTRACx
    int loop;
    L1STRU *l1p;
    L2STRU *l2p;
    UCHR *ap;
#endif

#if CICPP
    TRMSTRU *trmp = this;
#endif /* CICPP */

    invp=TDBifmap;
#if TRRTRACE
printf("trmread - trmp=%p  invp=%p\n",trmp,invp);
#if SAMEL
printf("trmread - name=%s,iflxx=%d,iflxn=%d \n",TDBname,TDBiflxx,invp->iflxn);
#endif
#endif
    if (TRMrcase) if ((TRMrcase-1) != treecase) {
#if TRETRACE
printf("trmread - forcing highv for treecase=%d\n",treecase);
#endif
	*retkeyp=highv;
	return(RCEOF);
    }
    if (invp->cn[treecase].liv == NEGLIV) {                     /* v3.1 */
#if TRETRACE
printf("trmread - highv for treecase=%d\n",treecase);
#endif
	*retkeyp=highv;
	return(RCEOF);
    }
    if (treecase) {
	idxksiz=LE2;
	idxesiz=sizeof(N2IDXE);
	lbufp=(UCHR *)(&TRMl2buf);
	lidxesiz=sizeof(L2IDXE);
    }
    else {
	idxksiz=LE1;
	idxesiz=sizeof(N1IDXE);
	lbufp=(UCHR *)(&TRMl1buf);
	lidxesiz=sizeof(L1IDXE);
    }
    np=noderead(invp,treecase,0,invp->cn[treecase].posrx);
    kp=TRMkey; wp=w0p=(unsigned char *)np->idxchars;     /* root node buffer */
#if TRRTRACE
printf("trmread - np->pos=%"_LD_"  np=%p  wp=%p\n",(LONGX)np->pos,np,wp);
#endif
    for (n=n0=np->ock; --n; ) {
	wp+=idxesiz;                            /* next entry */
        if (strncmp((CONST char *)kp,(CONST char *)wp,idxksiz) < 0) {
	    wp-=idxesiz;                        /* move back */
	    break;
	}
#if TRRTRACE
memmove(area,wp,idxksiz); area[idxksiz]='\0';
printf("trmread - key > %s\n",area);
#endif
    }
    punt= (treecase) ? ((N2IDXE *)wp)->punt : ((N1IDXE *)wp)->punt;
    liv=invp->cn[treecase].liv;
#if TRRTRACE
memmove(area,wp,idxksiz); area[idxksiz]='\0';
printf("trmread - key em %s\n",area);
printf("trmread - treecase=%d  em level=%d  punt=%"_LD_"  liv=%d\n",
    treecase,0,(LONGX)punt,liv);
#endif
    for (level=1; level<=liv; level++) {
	np=noderead(invp,treecase,level,punt);
	wp=(unsigned char *)np->idxchars;
	for (n=np->ock; --n; ) {
	    wp+=idxesiz;
            if (strncmp((CONST char *)kp,(CONST char *)wp,idxksiz) < 0) {
		wp-=idxesiz;
		break;
	    }
#if TRRTRACE
memmove(area,wp,idxksiz); area[idxksiz]='\0';
printf("trmread - key > %s\n",area);
#endif
	}
	punt= (treecase) ? ((N2IDXE *)wp)->punt : ((N1IDXE *)wp)->punt;
#if TRRTRACE
memmove(area,wp,idxksiz); area[idxksiz]='\0';
printf("trmread - key em %s\n",area);
printf("trmread - treecase=%d  em level=%d  punt=%"_LD_"\n",
    treecase,level,(LONGX)punt);
#endif
    }
    if (punt >= 0)
	fatal("trmread/punt");
    punt= -punt;
#if SAMEL
    lxx=TDBiflxx;
#else
    lxx=0;
#endif
    lp=leafread(lbufp,invp,treecase,punt,lxx);
    if (lrlrseek) TRMrseek(treecase)=lrlrseek;
    if (lrlrsize) TRMrsize(treecase)=lrlrsize;
    if ((UCHR *)lp != lbufp) fatal("trmread/leafread/bug/1");
    TRMliock(treecase)=0;                                       /* vl.l */
    wp=(unsigned char *)lp->idxchars; rc=RCNORMAL;
#if SAMTRACx
if (treecase) {
 l2p=(L2STRU *)lbufp; ap=wp;
 for (loop=0; loop < l2p->ock; loop++, ap+=lidxesiz)
  printf("%d/%d=%.30s=%"_LD_"/%"_LD_"/%"_LD_"=%.30s=%"_LD_"/%"_LD_"/%"_LD_" \n",
   loop+1,l2p->ock,
   l2p->idx[loop].key,
   l2p->idx[loop].info1,
   l2p->idx[loop].info2,
#if LIND
   l2p->idx[loop].info3info4.info3,
#else
   0,
#endif
   ap,
   ((L2IDXE *)ap)->info1,
   ((L2IDXE *)ap)->info2,
#if LIND
   ((L2IDXE *)ap)->info3info4.info3);
#else
   0L);
#endif
 getchar();
}
else {
 l1p=(L1STRU *)lbufp; ap=wp;
 for (loop=0; loop < l1p->ock; loop++, ap+=lidxesiz)
  printf("%d/%d=%.10s=%"_LD_"/%"_LD_"/%"_LD_"=%.10s=%"_LD_"/%"_LD_"/%"_LD_" \n",
   loop+1,l1p->ock,
   l1p->idx[loop].key,
   l1p->idx[loop].info1,
   l1p->idx[loop].info2,
#if LIND
   l1p->idx[loop].info3info4.info3,
#endif
   ap,
   ((L1IDXE *)ap)->info1,
   ((L1IDXE *)ap)->info2,
#if LIND
   ((L1IDXE *)ap)->info3info4.info3);
#else
   0L);
#endif
 getchar();
}
#endif
    kk=strncmp((CONST char *)kp,(CONST char *)wp,idxksiz);
    if (kk < 0) {
/*      if (strncmp(wp,w0p+idxesiz,idxksiz) < 0) {   <== bug == */
        if (n0 == 1 || strncmp((CONST char *)wp,(CONST char *)w0p+idxesiz,idxksiz) <= 0) {
	    rc=RCNEXT;
	    rp=wp;
	}
	else {
if (trmtrace) {
 printf("trmread - wp='");
 for (p=wp,n=idxksiz; n--; p++) printf("%c(%02x)",*p,*p);
 printf("'\n");
 printf("trmread - w0='");
 for (p=w0p+idxesiz,n=idxksiz; n--; p++) printf("%c(%02x)",*p,*p);
 printf("'\n");
}
	    fatal("trmread/leaf/less");
	}
    }
#if TRRTRACE /* 1 */
memmove(area,wp,idxksiz); area[idxksiz]='\0';
printf("trmread - folha=%s  kk=%d  l/k=%d/%d\n",area,kk,lidxesiz,idxksiz);
#endif
    if (kk > 0) {
	TRMliock(treecase)++;	/* 18/12/95 */
	rp=NULL;
	for (n=lp->ock; --n; TRMliock(treecase)++) {            /* vl.l */
#if ALPTRACE
memmove(area,wp,idxksiz); area[idxksiz]='\0';
printf("trmread - folxx=%s  kk=%d  l/k=%d/%d\n",area,kk,lidxesiz,idxksiz);
#endif
	    wp+=lidxesiz;
#if ALPTRACE
memmove(area,wp,idxksiz); area[idxksiz]='\0';
printf("trmread - folyy=%s  kk=%d  l/k=%d/%d\n",area,kk,lidxesiz,idxksiz);
#endif
#if SAMTRACE
memmove(area,wp,idxksiz); area[idxksiz]='\0';
printf("trmread - wp=%s  kk=%d [%d]\n",area,kk,TRMliock(treecase));
#endif
	    if (*wp <= MINLCHR) {                               /* vm.m */
		wp[idxksiz]='\0';
		printf("*** wp=%s\n",wp);
		fatal("trmread/check/wp");
	    }
            kk=strncmp((CONST char *)kp,(CONST char *)wp,idxksiz);
#if TRRTRACE /* 1 */
memmove(area,wp,idxksiz); area[idxksiz]='\0';
printf("trmread - wp=%s  kk=%d [%d]\n",area,kk,TRMliock(treecase));
#endif
	    if (kk == 0) {
		rp=wp;
		rc=RCNORMAL;
		break;
	    }
	    else
	       if (kk < 0) {
		     rp=wp;
		     rc=RCNEXT;
		     break;
		}
	}
	if (!rp)
	    if (lp->ps) {
		lp=leafread(lbufp,invp,treecase,lp->ps,lxx);
		if (lrlrseek) TRMrseek(treecase)=lrlrseek;
		if (lrlrsize) TRMrsize(treecase)=lrlrsize;
		if ((UCHR *)lp != lbufp) fatal("trmread/leafread/bug/2");
#if SAMTRACE
if (treecase) {
 l2p=(L2STRU *)lbufp;
 for (loop=0; loop < l2p->ock; loop++)
  printf("%d/%d=%.30s=%"_LD_"/%"_LD_"/%"_LD_"\n",loop+1,l2p->ock,
   l2p->idx[loop].key,
   l2p->idx[loop].info1,
   l2p->idx[loop].info2,
#if SAMTRACE
   l2p->idx[loop].info3info4.info3);
#endif
 getchar();
}
else {
 l1p=(L1STRU *)lbufp;
 for (loop=0; loop < l1p->ock; loop++)
  printf("%d/%d=%.10s=%"_LD_"/%"_LD_"/%"_LD_"\n",loop+1,l1p->ock,
   l1p->idx[loop].key,
   l1p->idx[loop].info1,
   l1p->idx[loop].info2,
#if SAMTRACE
   l1p->idx[loop].info3info4.info3);
#endif
 getchar();
}
#endif
		TRMliock(treecase)=0;                           /* vl.l */
		rp=(unsigned char *)lp->idxchars;
		rc=RCNEXT;
	    }
	    else {
		rp=highv;
		rc=RCEOF;
	    }
    }
    else
	rp=wp;
    *retkeyp=rp;
#if TRRTRACE
memmove(area,*retkeyp,idxksiz); area[idxksiz]='\0';
printf("trmread - rc=%d  *retkeyp=%s[%d]\n",rc,area,TRMliock(treecase));
#endif
    return(rc);
}


/* ----------------------------- nxt.c ------------------------------ */
#if CICPP
LONGX TRMSTRU :: xnxterm(void)
#else /* CICPP */
LONGX nxterm(itrm)                                                     /*
-----------
		    abenda se vtrmp ou registro itrm nao inicializado;
		    abenda se nenhuma folha foi carregada em term() anterior;
		    se necessario, "incrementa" chave e carrega a da outra;
		    compara e fica com a menor;
		    move termo para trmp->trmkey;
		    seta trmp->trmrc com RCEOF ou RCNORMAL;
		    seta controles para chamadas subsequentes;
		    chama posthdr1();
		    retorna itrm
								      */
LONGX itrm;          /* indice de vtrmp, para store */
#endif /* CICPP */
{
    TRMSTRU *trmp;
    UCHR *keyp;
    int rc,lcase,len;
    UCHR *l1idxp,*l2idxp;
    int n,kn;
    UCHR *p,*k1p,*k2p,*kp;
    PUNT ps;
    int lxx;
#if CICPP
    trmp = this;
    LONGX itrm = NO_ITRM;
#else /* CICPP */
    if (!ntrms)
	fatal("nxterm/ntrms");
    trmp=vtrmp[itrm];
    if (!trmp)
	fatal("nxterm/trmp");
#endif /* CICPP */
    rc=TRMrc; keyp=TRMkey;
    if (!*keyp)
	fatal("nxterm/key");
#if TRMTRACE
printf("nxterm - itrm=%"_LD_"  trmp=%p  TRMrc=%d  TRMkey=%s\n",itrm,trmp,rc,keyp);
#endif
    if (rc == RCEOF)
	return(itrm);
    l1idxp=TRMlidxp(0); l2idxp=TRMlidxp(1);
    lcase=TRMlcase; len=strlen((CONST char *)keyp);
    if (len)                                            /* 06/04/89 */
	for (p=keyp+len-1; len; --p)
	    if (*p != ' ')
		break;
	    else
		len--;
    else
	fatal("nxterm/len");
    if (!l2idxp) {              /* 2nd tree not used in last term() call */
	if (!l1idxp)
	    fatal("nxterm/l1idxp");
	if (lcase != 0)
	    fatal("nxterm/l1case");
	if (len < LE2) {
	    p=keyp+len; *p=0x01;
	    for (n=LE2-len; n--; )
		*++p=' ';
	}
	else {
	    p=keyp+len-1; (*p)++;
	}
#if TRXTRACE
printf("nxterm - calling trmread(1) - len=%d key='",len);
for (p=keyp,n=LE2+1; n--; p++) printf("%c(%02x)",*p,*p);
printf("'\n");
#endif
	trmread(trmp,1,&k2p);
    }
    else
	k2p=l2idxp;
    if (!l1idxp) {              /* 1st tree not used in last term() call */
	if (!l2idxp)
	    fatal("nxterm/l2idxp");
	if (lcase != 1)
	    fatal("nxterm/l2case");
/*      if (len < LE1) {
	    p=keyp+len; *p=0x01;
	    for (n=LE1-len; n--; )
		*++p=' ';
	}
	else {
	    p=keyp+len-1; (*p)++;
	}                               */
	if (len > LE1)                                  /* 06/04/89 */
	    len=LE1;
	p=keyp+len-1; (*p)++; *++p='\0';
#if TRXTRACE
printf("nxterm - calling trmread(0) - len=%d key='",len);
for (p=keyp,n=LE1+1; n--; p++) printf("%c(%02x)",*p,*p);
printf("'\n");
#endif
	trmread(trmp,0,&k1p);
    }
    else
	k1p=l1idxp;
#if TRXTRACE /* 1 */
printf("nxterm - 1. lcase=%d  k1p=",lcase);
for (p=k1p,n=LE1; n--; p++) printf("%c",*p);
printf("[%d]  k2p=",TRMliock(0));
for (p=k2p,n=LE2; n--; p++) printf("%c",*p);
printf("[%d]\n",TRMliock(1));
#endif
#if SAMEL
    lxx=TDBiflxx;
#else
    lxx=0;
#endif
    switch (lcase) {
    case 0: if (*k1p == *highv)                 /* eof */
		break;
	    if (*k1p) {                         /* active */
		k1p+=sizeof(L1IDXE);
		TRMliock(0)++;                          /* vl.l */
	    }
	    if (!*k1p) {                        /* last or inactive */
		ps=TRMl1buf.ps;
		if (ps) {                       /* next page ptr */
		    leafread((UCHR *)&TRMl1buf,TDBifmap,0,ps,lxx);
		    if (lrlrseek) TRMrseek(0)=lrlrseek;
		    if (lrlrsize) TRMrsize(0)=lrlrsize;
		    k1p=TRMl1buf.idx[0].key;
		    TRMliock(0)=0;                              /* vl.l */
		}
	    }
	    if (!*k1p)                          /* wood never del */
		k1p=highv;
	    break;
    case 1: if (*k2p == *highv)                 /* eof */
		break;
	    if (*k2p) {                         /* active */
		k2p+=sizeof(L2IDXE);
		TRMliock(1)++;                          /* vl.l */
	    }
	    if (!*k2p) {                        /* last or inactive */
		ps=TRMl2buf.ps;
		if (ps) {                       /* next page ptr */
		    leafread((UCHR *)&TRMl2buf,TDBifmap,1,ps,lxx);
		    if (lrlrseek) TRMrseek(1)=lrlrseek;
		    if (lrlrsize) TRMrsize(1)=lrlrsize;
		    k2p=TRMl2buf.idx[0].key;
		    TRMliock(1)=0;                              /* vl.l */
		}
	    }
	    if (!*k2p)                          /* wood never del */
		k2p=highv;
	    break;
    default: fatal("nxterm/lcase");
    }
#if TRXTRACE /* 1 */
printf("nxterm - 2. lcase=%d  k1p=",lcase);
for (p=k1p,n=LE1; n--; p++) printf("%c",*p);
printf("[%d]  k2p=",TRMliock(0));
for (p=k2p,n=LE2; n--; p++) printf("%c",*p);
printf("[%d]\n",TRMliock(1));
#endif
    n=strncmp((CONST char *)k1p,(CONST char *)k2p,LE1);             /* min */
    if (n <= 0) {
	kp=k1p;                         /* k1p is less than k2p */
	kn=LE1; lcase=0;
    }
    else {
	kp=k2p;                         /* k2p is less than k1p */
	kn=LE2; lcase=1;
    }
    memmove(TRMkey,kp,kn); *(TRMkey+kn)='\0';
    TRMrc = (*TRMkey == *highv) ? RCEOF : RCNORMAL;
    TRMlcase=lcase;
    TRMlidxp(0)=k1p;
    TRMlidxp(1)=k2p;
    posthdr1(trmp);                                             /* v3.2 */
#if TRXTRACE
printf("nxterm - TRMrc=%d  TRMlcase=%d,%p,%p  TRMkey=%s(%d)\n",
TRMrc,TRMlcase,TRMlidxp(0),TRMlidxp(1),TRMkey,strlen(TRMkey));
#endif
    return(itrm);
}

/* ----------------------------- citrmh.c --------------------------- */
/* ----------------------------- pst.c ------------------------------ */
#if CICPP
void TRMSTRU :: xposthdr1(void)
#else /* CICPP */
void posthdr1(trmp)                                                   /*
-------------
		    zera trmp->trmifhdr/ifseg;
		    zera trmp->trmpost/pmfn/ptag/pocc/pcnt;
		    inicializa trmp->pcurr/pchar/pleft/pcblk/pcpos;
		    retorna se trmp->trmifbuf nao estiver alocado;
		    retorna se RCEOF;
		    le.ifp block, se necessario;
		    carrega trmp->trmifhdr;
		    duplica em trmp->trmifseg;
		    aponta primeiro posting por trmp->trmpchar;
		    seta trmp->trmpcblk com o .ifp blk do posting1;
		    seta trmp->trmpcpos com o offset do posting1;
		    sempre zera trmp->trmpost;
		    sempre seta trmp->trmpleft com trmp->trmifhdr.ifsegp
								      */
TRMSTRU *trmp;      /* elemento de vtrmp, apos nx/term(), com IFBSIZ */
#endif /* CICPP */
{
    INFX info1;
    INFO info2;
#if LIND
    INFX info3;
    INFX bytes;
#endif
    UCHR *p,*inp;
#if !LIND
    int n;
    INFO *hdrp,*segp,*hp;
#endif

#if CICPP
    TRMSTRU *trmp = this;
#endif /* CICPP */

#if TRPTRACE
printf("posthdr1 - trmp=%p  %s(%d)  TRMrc=%d\n",trmp,TRMkey,TRMlcase,TRMrc);
#endif
#if !LIND
    hdrp=(INFO *)&TRMifhdr;
    segp=(INFO *)&TRMifseg;
#if TRPTRACE
printf("posthdr1 - hdrp=%p  segp=%p  IFHDRINFOS=%d\n",hdrp,segp,IFHDRINFOS);
#endif
    for (n=IFHDRINFOS; n--; )
	*hdrp++= *segp++=0;
#endif
    TRMpost=TRMpmfn=0;
    TRMptag=TRMpocc=TRMpcnt=0;
    TRMpcurr=TRMpchar=NULL; TRMpleft=0;
#if !LIND
    TRMpcblk=NULL; TRMpcpos=IFMAXTIV;
#endif
    if (TRMrc == RCEOF)
	return;
#if SAMTRACE
printf("hdr1 - TRMrc=%d  TRMlcase=%d,%p,%p  TRMkey=%s(%d) lxx=%d\n",
TRMrc,TRMlcase,TRMlidxp(0),TRMlidxp(1),TRMkey,strlen(TRMkey),TDBiflxx);
#endif
    if (TRMlcase) {            /* {} yes */
	info1=((L2IDXE *)TRMlidxp(1))->info1;
	info2=((L2IDXE *)TRMlidxp(1))->info2;
#if LIND
	info3=((L2IDXE *)TRMlidxp(1))->info3info4.info3;
#endif
    }
    else {
	info1=((L1IDXE *)TRMlidxp(0))->info1;
	info2=((L1IDXE *)TRMlidxp(0))->info2;
#if LIND
	info3=((L1IDXE *)TRMlidxp(0))->info3info4.info3;
#endif
    }
#if SAMTRACE
printf("posthdr1 - info1=%"P_OFF_T"  info2=%"_LD_"  info3=%"P_OFF_T"\n",(LONG_LONG)info1,info2,(LONG_LONG)info3);
#endif
#if LIND
    TRMyaddr=info1;
    TRMypsts=info2;
    if (info3 < 0) {
	TRMycase=BITSTRING;
	TRMysize=1; /* one byte */
	TRMydocs=info2;
	TRMydisk= -info3;
    }
    if (info3 == 0) {
	TRMycase=MFNSTRING;
	TRMysize=PMFNSIZ;
	TRMydocs=info2;
	TRMydisk=info2*PMFNSIZ;
    }
    if (info3 > 0) {
	TRMycase=CNTSTRING;
	TRMysize=PMFNSIZ+PCNTSIZ;
	TRMydocs=info3;
	TRMydisk=info2*(PMFNSIZ+PCNTSIZ);
    }
#endif
    if (TRMnbytes < IFBSIZ)                                     /* v3.2 */
	return;
    inp=TRMifbuf;

#if !LIND
#if IFUPDATE
    if (trmifupd || *(INFO *)(inp) != info1) {
#else
    if (*(INFO *)(inp) != info1) {
#endif
#if TRPTRACE
printf("posthdr1 - info1=%"_LD_"  info2=%"_LD_"\n",(LONGX)info1,(LONGX)info2);
#endif
	postread(inp,TDBifmap,info1,TDBiflxx);                  /* v3.3 */
    }
    p=inp+INFOSIZE+(info2*INFOSIZE);  /* INFOSIZE = ifpblk */
    hdrp=(INFO *)&TRMifhdr;
    segp=(INFO *)&TRMifseg;
    for (hp=(INFO *)(p), n=IFHDRINFOS; n--; )
	*hdrp++= *segp++= *hp++;
#if CNV_PCBINUM
    ConvertIFP_PSTHDR((char *)&TRMifhdr);
    ConvertIFP_PSTHDR((char *)&TRMifseg);
#endif
#if TRPTRACE
printf("posthdr1/hdr - ifpnxtb=%"_LD_"  nxtp=%"_LD_"  totp=%"_LD_"  segp=%"_LD_"  segc=%"_LD_"\n",
    (LONGX)TRMifhdr.ifpnxtb,(LONGX)TRMifhdr.ifpnxtp,
    TRMifhdr.ifptotp,TRMifhdr.ifpsegp,TRMifhdr.ifpsegc);
#endif
    TRMpchar=p+IFHDRSIZ;
    TRMpcblk=info1; TRMpcpos=info2+IFHDRINFOS;
    TRMpost=0; TRMpleft=TRMifhdr.ifpsegp;
#if TRPTRACE
printf("posthdr1 - post=%"_LD_"  pchar=%p  pcblk=%"_LD_" pcpos=%"_LD_"  pleft=%"_LD_"\n",
    (LONGX)TRMpost,TRMpchar,(LONGX)TRMpcblk,(LONGX)TRMpcpos,TRMpleft);
#endif
#else //LIND
#if TRPTRACE
printf("posthdr1 - info1=%"P_OFF_T"  info2=%"_LD_"  info3=%"P_OFF_T"\n",(LONG_LONG)info1,info2,(LONG_LONG)info3);
#endif
    if (TRMydisk > IFBSIZ)
	bytes=IFBSIZ;
    else 
        bytes=TRMydisk;
    postread(inp,TDBifmap,TRMyaddr,(unsigned int)bytes,TDBiflxx);
    TRMyread=bytes;
    TRMyleft=bytes;
    p=inp;
    TRMpchar=p;
    TRMpost=0; TRMpleft=TRMypsts;
#if TRPTRACE
printf("posthdr1 - post=%"_LD_"  pchar=%p  yread=%"P_OFF_T" ydisk=%"P_OFF_T"  pleft=%"_LD_"\n",
    (LONGX)TRMpost,TRMpchar,(LONG_LONG)TRMyread,(LONG_LONG)TRMydisk,(LONGX)TRMpleft);
#endif
#endif //LIND
}
#if !LIND
#if CICPP
LONGX TRMSTRU :: xposting(LONGX nord)
#else /* CICPP */
LONGX posting(itrm,nord)                                               /*
------------
		    seta trmp;
		    retorna trmp->trmifhdr.ifptotp se nord = 0;
		    se nord > trmp->trmifhdr.ifptotp
			zera trmp->trmpxxx e retorna um valor negativo;
		    incrementa trmp->trmpost e abenda se nord diferente;
		    le proximo segmento/bloco, se necessario;
		    decodifica posting;
		    mantem controles para chamadas subsequentes;
		    retorna trmp->trmpmfn
								      */
LONGX itrm;          /* indice de vtrmp, para store */
LONGX nord;          /* numero de ordem do posting desejado ou 0L */
#endif /* CICPP */
{
    TRMSTRU *trmp;
    LONGX total;
    INFO info1,info2,*hp,*sp;
    int n;
    UCHR *p,*inp;
#if CICPP
    trmp = this;
    LONGX itrm = NO_ITRM;
#else /* CICPP */
    if (!ntrms)
	fatal("posting/ntrms");
    trmp=vtrmp[itrm];
    if (!trmp)
	fatal("posting/trmp");
#endif /* CICPP */
    if (TRMnbytes < IFBSIZ)                                     /* 05/07/91 */
	fatal("posting/TRMnbytes");
    total=TRMifhdr.ifptotp;
#if TRPTRACE
printf("posting - itrm=%"_LD_"  trmp=%p  rc=%d  nord=%"_LD_"  post=%"_LD_"/%"_LD_"\n",
    itrm,trmp,TRMrc,nord,TRMpost,total);
#endif
    if (nord == 0)
	return(total);
    if (nord > total) {
	TRMpost=TRMpmfn=NULL;
	TRMptag=TRMpocc=TRMpcnt=NULL;
	TRMpcurr=TRMpchar=NULL; TRMpleft=NULL;
	return(-1L);
    }
    TRMpost++;
    if (nord != TRMpost)
	fatal("posting/nord");
    while (!TRMpleft) {
	info1=TRMifseg.ifpnxtb;
	if (!info1)
	    fatal("posting/ifpnxtb");
	info2=TRMifseg.ifpnxtp;
#if TRPTRACE
printf("posting - pleft=%"_LD_"  info1=%"_LD_"  info2=%"_LD_"\n",
    TRMpleft,info1,info2);
#endif
	inp=TRMifbuf;
#if IFUPDATE
	if (trmifupd || *(PUNT *)(inp) != info1) {
#else
	if (*(PUNT *)(inp) != info1) {
#endif
#if TRPTRACE
printf("posting - new seg - info1=%"_LD_"  info2=%"_LD_" \n",
    (LONGX)info1,(LONGX)info2);
#endif
	    postread(inp,TDBifmap,info1,TDBiflxx);              /* v3.3 */
	}
	p=inp+INFOSIZE+(info2*INFOSIZE);      /* INFOSIZE = ifpblk */
	for (sp=(INFO *)&TRMifseg, hp=(INFO *)p, n=IFHDRINFOS; n--; )
	    *sp++= *hp++;
#if CNV_PCBINUM
	ConvertIFP_PSTHDR((char *)&TRMifseg);
#endif
	TRMpleft=TRMifseg.ifpsegp; TRMpchar=p+IFHDRSIZ;
	TRMpcblk=info1;
	TRMpcpos=info2+IFHDRINFOS;
#if TRPTRACE
printf("posting - post=%"_LD_"  pchar=%p  pcblk=%"_LD_" pcpos=%"_LD_"  pleft=%"_LD_"\n",
    (LONGX)TRMpost,TRMpchar,(LONGX)TRMpcblk,(LONGX)TRMpcpos,TRMpleft);
#endif
    }
    if (TRMpcpos >= (IFMAXTIV-1)) {
	info1=TRMpcblk+1; info2=0;              /* 22/02/89 */
	inp=TRMifbuf;
	postread(inp,TDBifmap,info1,TDBiflxx);                  /* v3.3 */
#if TRPTRACE                                    /* 22/02/89 */
printf("posting - new blk -  info1=%"_LD_"  info2=%"_LD_" \n",
    (LONGX)info1,(LONGX)info2);
#endif
	TRMpchar=inp+INFOSIZE;                  /* INFOSIZE = ifpblk */
	TRMpcblk=info1;
	TRMpcpos=info2;                         /* 22/02/89 */
#if TRPTRACE                                    /* 22/02/89 */
printf("posting - post=%"_LD_"  pchar=%p  pcblk=%"_LD_" pcpos=%"_LD_"  pleft=%"_LD_"\n",
    (LONGX)TRMpost,TRMpchar,(LONGX)TRMpcblk,(LONGX)TRMpcpos,TRMpleft);
#endif
    }
    postpost(trmp);
    TRMpleft--; TRMpcurr=TRMpchar;
    TRMpchar+=IFPSTSIZ; TRMpcpos+=IFPSTINFOS;
    return(TRMpmfn);
}
#else /* LIND posting() */
#if CICPP
LONGX TRMSTRU :: xposting(LONGX nord)
#else /* CICPP */
LONGX posting(itrm,nord)                                               /*
------------                                                          */
LONGX itrm;          /* indice de vtrmp, para store */
LONGX nord;          /* numero de ordem do posting desejado ou 0L */
#endif /* CICPP */
{
    TRMSTRU *trmp;
    LONGX total,mfn;
    off_t bytes,xbyte;
    int loop2,n,blshift; 
    UCHR *inp;
#if CICPP
    trmp = this;
    LONGX itrm = NO_ITRM;
#else /* CICPP */
    if (!ntrms)
	fatal("posting/ntrms");
    trmp=vtrmp[itrm];
    if (!trmp)
	fatal("posting/trmp");
#endif /* CICPP */
    if (TRMnbytes < IFBSIZ)                                     /* 05/07/91 */
	fatal("posting/TRMnbytes");
    total=TRMypsts;
#if TRPTRACE
printf("posting - itrm=%"_LD_"  trmp=%p  rc=%d  nord=%"_LD_"  post=%"_LD_"/%"_LD_"\n",
    itrm,trmp,TRMrc,nord,TRMpost,total);
#endif
    if (nord == 0)
	return(total);
    mfn=(TRMpmfn == 0)?0:TRMpmfn+1;
    TRMpmfn=TRMptag=TRMpocc=TRMpcnt=0;
    if (nord > total) {
	TRMpost=0;
	TRMpcurr=TRMpchar=NULL; TRMpleft=0;
	return(-1L);
    }
    TRMpost++;
    if (nord != TRMpost)
	fatal("posting/nord");
    for (; !TRMpmfn; ) {
	if (TRMyleft < TRMysize) {
	    if (TRMyleft) {
		if (TRMycase == BITSTRING) fatal("posting/align/BITSTRING");
		blshift=TRMyleft;
		TRMyread-=blshift;
	    }
	    else blshift=0;
	    inp=TRMifbuf;
	    bytes=TRMydisk-TRMyread;
	    if (bytes <= 0)
		fatal("posting/bytes");
	    if (bytes > IFBSIZ)
		bytes=IFBSIZ;
	    xbyte=TRMyaddr+TRMyread;
	    postread(inp,TDBifmap,xbyte,(unsigned int)bytes,TDBiflxx);
	    TRMyread+=bytes;
	    TRMyleft=bytes;
#if TRPTRACE
printf("posting - new blk  \n");
#endif
	    TRMpchar=inp;
	}
	if (TRMycase == BITSTRING) {
	    if (!*TRMpchar) {
		TRMpchar++; TRMyleft--;
		mfn+=8;
		continue;
	    }
	    loop2 = 8 - (mfn&BY8RMASK);
#if TRPTRACE
printf("posting - TRMpchar=%p=%02x TRMyleft=%"_LD_"      mfn=%"_LD_"/%d\n",
 TRMpchar,*TRMpchar,TRMyleft,mfn,loop2);
#endif
	    for (; loop2--; mfn++) {
		n=mfn&BY8RMASK;
#if TRPTRACE
printf("posting - TRMpchar=%p=%02x TRMyleft=%"_LD_"  now mfn=%"_LD_"/%d %d/%02x\n",
 TRMpchar,*TRMpchar,TRMyleft,mfn,loop2,n,bitmask[n]);
#endif
		if (*TRMpchar & bitmask[n]) {
		    TRMpmfn=mfn;
		    break;
		}
	    }
	    if (!TRMpmfn) {
		TRMpchar++; TRMyleft--;
		continue;
	    }
	    TRMpleft--; TRMpcurr=TRMpchar;
	    if (n == BY8RMASK) {
		TRMpchar++; TRMyleft-=1;
	    }
	}
	else {
	    postpost(trmp);
	    TRMpleft--; TRMpcurr=TRMpchar;
	    TRMpchar+=TRMysize; TRMyleft-=TRMysize;
	}
    }
    if (!TRMpmfn)
	fatal("posting/TRMpmfn");
    return(TRMpmfn);
}
#endif /* LIND */

#if CICPP
void TRMSTRU :: xpostpost(void)
#else /* CICPP */
void postpost(trmp)                                                   /*
-------------
		    aponta origem para trmp->trmpchar;
		    decodifica em trmp->trmpmfn...trmp->trmpcnt
								      */
TRMSTRU *trmp;      /* elemento de vtrmp, posting() */
#endif /* CICPP */
{
    unsigned char *p,*q;

#if CICPP
    TRMSTRU *trmp = this;
#endif /* CICPP */

#if TRPTRACE
char *pp;
int pn;
printf("postpost - post=%"_LD_"  ifbuf=%p  char=%p=%02x left=%"_LD_"\n",
   TRMpost,&TRMifbuf,TRMpchar,*TRMpchar,TRMpleft);
for (pp=TRMpchar, pn=8; pn--; pp++) printf("%02x ",*pp); printf("\n");
#endif
#if LIND
    if (TRMycase != MFNSTRING && TRMycase != CNTSTRING)
	fatal("postpost/TRMycase");
#endif

  p=TRMpchar;
  q=(unsigned char *)&TRMpmfn;
  q[M1] = *p++;
  q[M2] = *p++;
  q[M3] = *p++;
#if LIND4
  q[M4] = *p++;
#else
  q[M4]=(unsigned char)0;
#endif
#if !LIND
  q=(unsigned char *)&TRMptag;
  q[T1] = *p++;
  q[T2] = *p++;
  q=(unsigned char *)&TRMpocc;
  q[O1] = *p++;
  q[O2]=(unsigned char)0;
#endif
#if LIND
if (TRMycase == CNTSTRING) {
#endif
  q=(unsigned char *)&TRMpcnt;
  q[C1] = *p++;
  q[C2] = *p++;
#if LIND
 }
#endif

#if TRPTRACE
printf("postpost - mfn=%"_LD_"  tag=%d  occ=%d  cnt=%d\n",
   TRMpmfn,TRMptag,TRMpocc,TRMpcnt);
#endif
}


