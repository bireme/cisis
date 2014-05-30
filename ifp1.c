/* ------------------------------ ifp1.c ----------------------------- */
/* ===========================================================================
        Link file generation for the MeSH Headings field - STEP c
	Author: decs9a.c/AOT, Nov 2 1990
	Alter:  AOT, Oct 08 1991
		1. Scan the total no of posting, as the last input field
		2. Scan an additional trailing space
	Alter:  AOT, Oct 17 1991
		1. CNLI support
		2. Key1 = "**DELETE**" (move 0 to the total no of postings)
=========================================================================== */
#include <stdio.h>
#include <string.h>
#define TRACEMOV 0
#define TRACEHI  0
#include "cisis.h"       /* CISIS Interface */
#include "cirun.h"       /* runtime area and defines by AOT */
unsigned char keydel[LE2+1] = { "**DELETE**" };
extern UCHR highv[];
int parmtrace=0;
/* prototypes */
#if ANSI
int moveinfo(LONGX itrm1, LONGX itrm2);
int outleaf(int fd, char *buffp, int lbufsiz, PUNT punt, LONGX offset);
#else
int moveinfo();
int outleaf();
#endif
/* main */
#if ANSI
void main (int argc, char *argv[])
#else
main(argc,argv)
int argc;
char *argv[];
#endif
{
    unsigned char *ifnautp,*ifnioxp,*listfnp;
    INVMAP *invp; /* tell */
    FILE *fplst;
    FILE *fp;
    int loop,n;
#if 0
    unsigned char listfmt[80];
#endif
    unsigned char key1[LE2+1],key2[LE2+1],prefkey[LE2+1],qualkey[LE2+1];
    LONGX umb;
    int key1l,key2l,prefl,quall;
    unsigned char lang,type;
    TRMSTRU *trmp;
    LONGX itrmaut,itrmiox;
    unsigned char keyiox[LE2+1],keyaut[LE2+1];
    unsigned char *p;
    LONGX ecount,count,xtell;
    extern int trmtrace;
    if (argc < 4) {
        printf("%s",cicopyr("Utility IFP1"));
        printf("\n");
	printf("ifp1 <ifnaut> <ifniox> <crosslst> [<tell> [trace]]\n");
        printf("\n");
	printf(" <ifnaut>      -> primary inverted file       \n");
        printf(" <ifniox>      -> dictionary to be updated    \n");
        printf(" <crosslst>    -> correspondence table        \n");
        printf("\n");
	printf(" trace         -> trace execution \n");
        printf(" tell=<n>      -> tell <n> keys has been processed \n");
        printf(" \n");
        exit(1);
    }

    /* get parms */
    xtell=0;
    ifnautp=argv[1];
    ifnioxp=argv[2];
    listfnp=argv[3];
    if (argc > 4)
	if (sscanf(argv[4],"%"_LD_,&xtell) != 1) fatal(argv[4]);
    if (argc > 5) {
	dbxtrace=trmtrace=parmtrace=1;
    }
    /* aloca trm sem area p/ posting - invertido autorizado */
    trmalloc((itrmaut=ntrms),0L);      			    	/* tidx #1 */
    /* aloca trm sem area p/ posting - invertido ifnioxp */
    trmalloc((itrmiox=ntrms),0L);          			/* tidx #2 */
    trmp=vtrmp[itrmiox];
    invsetup(ifnioxp,0L,0L,0L);
    invp=dbxstorp(ifnioxp)->dbxifmap;
    /* === open leaf's for CIWRITE */
    dbxopenw("",ifnioxp,lx12extp[0],&invp->l1opn,&invp->l1opw,"ifp1/l1opn/w");
    dbxopenw("",ifnioxp,lx12extp[1],&invp->l2opn,&invp->l2opw,"ifp1/l2opn/w");
    /* === */
    if (xtell == 0) {
	xtell=(invp->cn[0].fmaxpos+invp->cn[1].fmaxpos)*TWORDF;
	xtell/=10;
	if (xtell < 1)
	    xtell=1;
    }
    /* open lst and output files */
    if ((fplst=fopen(listfnp, "r")) == NULL) fatal(listfnp);
    ecount=count=0;
    /* setup keydel */
    for (p=keydel, n=sizeof(keydel); *p; n--) p++;
    for (; --n > 0; ) *p++ = ' ';
    *p = '\0';
#if 0
000001|/PD                           |I|a|/PD EXTENSO                   |
                                                        . |   | 3|11|1|0
6|LE2|1|1|LE2|2|3|2|2|1|1\n
%"_LD_"|%.30s|%c|%c|%.30s|%.2s|%.3s|%2d|%2d|%d|%d
/* #endif */
    sprintf(listfmt,
        "%%"_LD_"|%%.%ds|%%c|%%c|%%.%ds|%%.2s|%%.3s|%%2d|%%2d|%%d|%%d",LE2,LE2);
    /* loop principal */
    while (fscanf(fplst,listfmt,&umb,key1,&lang,&type,key2,prefkey,qualkey,
                                        &key1l,&key2l,&prefl,&quall) == 11) {
#else
    while (fscanf(fplst,"%"_LD_"|",&umb) == 1) {
        for (p=key1, n=LE2; n--; ) *p++ = fgetc(fplst); *p='\0'; fgetc(fplst);
        lang = fgetc(fplst); fgetc(fplst);
        type = fgetc(fplst); fgetc(fplst);
        for (p=key2, n=LE2; n--; ) *p++ = fgetc(fplst);  *p='\0'; 
fgetc(fplst);
        for (p=prefkey, n=2; n--; ) *p++ = fgetc(fplst); *p='\0'; 
fgetc(fplst);
        for (p=qualkey, n=3; n--; ) *p++ = fgetc(fplst); *p='\0'; 
fgetc(fplst);
	n=fscanf(fplst,"%2d|%2d|%d|%d",
                                        &key1l,&key2l,&prefl,&quall);
	fscanf(fplst,"|%*"_LD_" \n");
        if (n != 4) {
            printf("n=%d\n%"_LD_"|%s|%c|%c|%s|%s|%s|%2d|%2d|%d|%d\n",
            
n,umb,key1,lang,type,key2,prefkey,qualkey,key1l,key2l,prefl,quall);
            fatal("!= 4");
	}
        key1[key1l]='\0'; key2[key2l]='\0';
        prefkey[prefl]='\0'; qualkey[quall]='\0';
#endif NOW
        memmove(keyaut,            prefkey,prefl);
        memmove(keyaut+prefl,      key1,key1l);
        memmove(keyaut+prefl+key1l,qualkey,quall);
               keyaut[prefl+key1l+quall]='\0';
        memmove(keyiox,            prefkey,prefl);
        memmove(keyiox+prefl,      key2,key2l);
        memmove(keyiox+prefl+key2l,qualkey,quall);
               keyiox[prefl+key2l+quall]='\0';
	if ((++count % xtell) == 0)
	    fprintf(stderr,"+++ %"_LD_" %s%s%s %d,%d,%d [%c,%c]\n",
                count,prefkey,key1,qualkey,prefl,key1l,quall,lang,type);
	for (p=keyaut; *p; p++) *p=isisuctab[*p];
	if (memcmp(key1,keydel,key1l) == 0) {
	    printf("+++ %s keyaut=%s (%"_LD_")\n",key1,keyiox,umb);
	    TERM(itrmaut,ifnautp,highv);
	    if (TRMrc != RCEOF) {
		printf("*** ifnaut=%s keyaut=%s (%"_LD_")\n",ifnautp,keyaut,umb);
		fatal(keyaut);
	    }
	}
	else {
	    TERM(itrmaut,ifnautp,keyaut);
	    if (TRMrc != RCNORMAL) {
		printf("*** ifnaut=%s keyaut=%s (%"_LD_")\n",ifnautp,keyaut,umb);
		fatal("TRMrc");
	    }
	}
	for (p=keyiox; *p; p++) *p=isisuctab[*p];
        TERM(itrmiox,ifnioxp,keyiox);
        if (TRMrc != RCNORMAL) {
            printf("*** ifniox=%s keyiox=%s (%"_LD_")\n",ifnioxp,keyiox,umb);
            fatal("TRMrc");
        }
        moveinfo(itrmaut,itrmiox);
    }
    printf("==> %"_LD_"/%"_LD_" \n",count,ecount);
    exit(0);
}

#if ANSI
int moveinfo(LONGX itrmaut, LONGX itrmiox)
#else
int moveinfo(itrmaut,itrmiox)
LONGX itrmaut;
LONGX itrmiox;
#endif
{
    TRMSTRU *trmp;
    PUNT punt;
    char *buffp;
    int lbufsiz,fd;
    INFO info1,info2;
#if LIND
    INFO info3;
#endif
    LONGX offset;
#if TRACEMOV
    char *p;
    int n;
#endif
    trmp=vtrmp[itrmaut];                                /* ifnaut */
    if (TRMrc == RCEOF) {				/* **DELETE** */
#if LIND
	info1=0L;	/* MFNSTRING */
	info2=0L;	/* TRMydocs */
	info3=0L;	/* info2*PMFNSIZ */
#else
	fatal(keydel);	/* Don't know what to do with ifphdr */
	/* info1=1L; */
#endif
	if (parmtrace)
	    printf("+++ moveinfo - TRMrc=%d TRMkey=%s \n",TRMrc,TRMkey);
    }
    else {
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
    }
#if TRACEMOV
printf("key=%s (%d) info=%"_LD_"/%"_LD_"\n",TRMkey,TRMlcase,info1,info2);
#endif
    trmp=vtrmp[itrmiox];                                /* ifniox */
    if (TRMlcase) {
        ((L2IDXE *)TRMlidxp(1))->info1 = info1;
        ((L2IDXE *)TRMlidxp(1))->info2 = info2;
#if LIND
        ((L2IDXE *)TRMlidxp(1))->info3info4.info3 = info3;
#endif
#if TRACEMOV
        p=TRMlidxp(1); n=LE2;
#endif
        punt=TRMl2buf.pos;
        buffp=(char *)&TRMl2buf;
        lbufsiz=L2BSIZ;
        fd=TDBifmap->l2opn;
    }
    else {
        ((L1IDXE *)TRMlidxp(0))->info1 = info1;
        ((L1IDXE *)TRMlidxp(0))->info2 = info2;
#if LIND
        ((L1IDXE *)TRMlidxp(0))->info3info4.info3 = info3;
#endif
#if TRACEMOV
        p=TRMlidxp(0); n=LE1;
#endif
        punt=TRMl1buf.pos;
        buffp=(char *)&TRMl1buf;
        lbufsiz=L1BSIZ;
        fd=TDBifmap->l1opn;
    }
#if CNLI
    offset=TDBifmap->cl_offset[TRMlcase];
    if (parmtrace) {
	printf("offset=%"_LD_" ..",offset);
	if (getchar() != '\n') exit(1);
    }
#else
    offset=0;
#endif
#if TRACEMOV
printf(" -> '");
for (; n--; p++) printf("%c",*p);
printf("' +%"_LD_,offset); getchar();
#endif
    if (parmtrace)
	printf("+++ moveinfo -  %s. %"_LD_"/%"_LD_"\n",TRMkey,(LONGX)info1,(LONGX)info2);
    if (outleaf(fd,buffp,lbufsiz,punt,offset)) {
        printf("*** %s. %"_LD_"/%"_LD_"\n",TRMkey,(LONGX)info1,(LONGX)info2);
        fatal("CIWRITE");
    }
    return(RCNORMAL);
}

#if ANSI
int outleaf(int fd, char *buffp, int lbufsiz, PUNT punt, LONGX offset)
#else
int outleaf(fd,buffp,lbufsiz,punt,offset)
int fd;
char *buffp;
int lbufsiz;
PUNT punt;
LONGX offset;
#endif
{
    off_t bytes;
    int n;
    if (fd <= 0)
        fatal("fd");
    if (punt < 1)
        fatal("punt");
    bytes=(punt-1L)*lbufsiz+offset;
    if (LSEEK64(fd,bytes,SEEK_SET) != bytes)
        fatal("lseek");
    n=CIWRITE(fd,buffp,lbufsiz) - lbufsiz;
    ((L0STRU *)buffp)->pos=0;   /*
                                        This is to force a read on disk:
                                        TRMl1buf/TRMl2buf->pos in leafread()
                                        is checked to avoid reading the disk
                                */
    return(n);
}
