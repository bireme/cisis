/* ----------------------------- myzcru.c ----------------------------- */

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "cisis.h"
#include "cirun.h"

/* ----------------------------- tlc.h -------------------------------- */

typedef union {
    L0STRU l0x;
    L1STRU l1x;
    L2STRU l2x;
} LUSTRU;

LUSTRU tlcleaf;
LZSTRU lzxleaf;

#define MAXNAPU 256
int napus,apun,apuk;
LONGX *vapup[MAXNAPU];
LONGX *apup;
LONGX divapu;

int parmtrace=0;

#if ANSI
LONGX leafadda(int treecase,PUNT punt,LONGX addr);
LONGX leafaddr(int treecase,PUNT pos,int ock,PUNT punt);
#else
LONGX leafadda();
LONGX leafaddr();
#endif

/* ----------------------------- mov.h -------------------------------- */
#if INTELCPU /* PC */ /* AOT, 02/04/2002 */
#define SWAB 0
#define MOV21 0
#define MOV20 1
#define MOV43 0
#define MOV42 1
#define MOV41 2
#define MOV40 3
#else
#define SWAB 1
#define MOV21 1
#define MOV20 0
#define MOV43 3
#define MOV42 2
#define MOV41 1
#define MOV40 0
#endif

char *movp,*movtmp;
UWORD moved;
char *mov_record=NULL;
char *movps_p;
char *movpsb_p;

#if SWAB
#define mov2(source)                            \
        movtmp  = (char *)&(source);            \
        swab(movtmp,movp,2); movp+=2;           \
        moved+=2;
#else
#define mov2(source)                            \
        movtmp  = (char *)&(source);            \
        *movp++ = movtmp[MOV21];                \
        *movp++ = movtmp[MOV20];                \
        moved+=2;
#endif

#define mov4(source)                            \
        movtmp  = (char *)&(source);            \
        *movp++ = movtmp[MOV43];                \
        *movp++ = movtmp[MOV42];                \
        *movp++ = movtmp[MOV41];                \
        *movp++ = movtmp[MOV40];                \
        moved+=4;

#define movx(sourcep,len)                       \
        memmove(movp,sourcep,(size_t)len);       \
        movp+=len;                              \
        moved+=len;


void main(argc,argv)
int argc;
char *argv[];
{
    extern int trmtrace;
    char *dbnp,*zdbnp;
    int treecase,lex;
    INVMAP *invp;

    PUNT punt;
    LONGX punt1,punt2;
    INFO info1,info2;
#if LIND
    INFO info3;
    LONGX lastaddr,thisaddr;
#endif

    int lzsize,mzxrecl;
    int filler=0;
    LONGX nextaddr;

    LONGX tell,countell,rangtell;
    LONGX parmtell=0L;
    LONGX parmtelln=0L;
    int show;
    FILE *fpx;

    int origlsiz;
    UWORD x2;
    LONGX origsize,x4;

    char *p,*keyp,prevkey[LE2+1];
    int iarg,n,iock,loop;
    int tree1=1,tree2=2;
    int eq,keylen,ne;
    unsigned char *lzkp;

    N0STRU *n0p;
    N1IDXE *n1xp;
    N2IDXE *n2xp;
    int nodesize;


    if (argc < 3) {
        printf("%s",cicopyr("Utility MYZCRU"));
        printf("\n");
        printf("myzcru <dbn> <pczdbn> [options..] \n");
        printf("\n");
        printf("options: show[=<file>] \n");
        printf("         {tell|telln}=<times> \n");
        printf("         {tree1|tree2}=<tree> \n");
        printf(" \n");
        exit(1);
    }

    dbnp=argv[1];
    dbxopt_ordwr=O_RDONLY;
    invsetup(dbnp,0L,0L,0L);

    dbxopt_ordwr=O_RDWR;
    invp=dbxstorp(dbnp)->dbxifmap;

    zdbnp=argv[2];

    fpx=stderr; show=0; 

    for (iarg=3; iarg < argc; iarg++) {
        p=argv[iarg];
        if (strcmp(p,"show") == 0 || strncmp(p,"show=",5) == 0) {
            if (strncmp(p,"show=",5) == 0) {
                if (!*(p+5)) fatal(p);
                if ((fpx=fopen(p+5,"w")) == NULL) fatal(p+5);
            }
            else fpx=stdout;
            show=1; 
            continue;
        }
        if (strcmp(p,"trace") == 0) {
            dbxtrace=trmtrace=parmtrace=1; continue;
        }
        if (strncmp(p,"tell=",5) == 0) {
            if (sscanf(p+5,"%"_LD_,&parmtell) != 1) fatal(p);
            if (parmtell < 0) fatal(p);
            continue;
        }
	if (strncmp(p,"telln=",6) == 0) {
            if (sscanf(p+6,"%"_LD_,&parmtelln) != 1) fatal(p);
            if (parmtelln < 0) fatal(p);
            continue;
        }
	if (strncmp(p,"tree1=",6) == 0) {
	    if (sscanf(p+6,"%d",&tree1) != 1) fatal(p);
	    if (tree1 < 1 || tree1 > tree2) fatal(p);
	    continue;
        }
	if (strncmp(p,"tree2=",6) == 0) {
	    if (sscanf(p+6,"%d",&tree2) != 1) fatal(p);
	    if (tree2 < tree1 || tree2 > 2) fatal(p);
	    continue;
        }
        fatal(p);
    }

    tree1--; tree2--;

    /* allocate crunching buffer */
    if (parmtrace) printf("+++ sizeof(LZSTRU)=%d\n",sizeof(LZSTRU));
    if (parmtrace) printf("+++          1: coreleft=%"_LD_"\n",CORELEFT());
    mov_record=(char *)ALLOC((ALLOPARM)(sizeof(LZSTRU)+1));
    if (mov_record == (char *)ALLONULL) fatal("ALLOC/mov_record");
    if (parmtrace) printf("+++ mov_record: coreleft=%"_LD_"\n",CORELEFT());

    /* init leaf addresses */
    napus=0; divapu=ALLOMAXV/sizeof(LONGX);

/*
    for each tree
*/

for (treecase=tree1; treecase <= tree2; treecase++) {

    punt1=1; punt2=invp->cn[treecase].fmaxpos;
    rangtell=punt2-punt1+1;
    if (rangtell <= 0) rangtell=1;
    if (parmtrace) printf("+++ fmaxpos=%"_LD_" \n",punt2);
    if (parmtell) {
        tell=rangtell/parmtell;
        if (tell <= 0) tell=1;
    }

    nextaddr=0;
    origlsiz = (treecase) ? L2BSIZ : L1BSIZ;
    origsize=0;
    lex=vlex[treecase];
    countell=0;

    fpccreat("",zdbnp,lx12extp[treecase],0);

    /*
        for each leaf
    */
    for (punt=punt1; punt <= punt2; punt++) {

	leafadda(treecase,punt,nextaddr+1); /* punt < 0 */

	/* read */
	if (leafread((char *)&tlcleaf,invp,treecase,punt,0) != &tlcleaf.l0x)
	    fatal("myzcru/bug1");

        /* setup header */
        lzxleaf.pos = tlcleaf.l0x.pos;
        lzxleaf.ock = tlcleaf.l0x.ock;
        lzxleaf.it  = tlcleaf.l0x.it;
        lzxleaf.ps  = tlcleaf.l0x.ps;
        if (punt == punt2 && lzxleaf.ps != 0) {
            fatal("lzxleaf.ps/last");
        }
        if (punt != punt2 && lzxleaf.ps != punt+1) {
            fatal("lzxleaf.ps/next");
        }
#if LIND
        lzxleaf.psb = tlcleaf.l0x.psb;
        if (lzxleaf.psb != punt-1) {
            fatal("lzxleaf.psb");
        }
#endif

        lzsize = 0; /* LZHSIZE; */
        lzkp = (unsigned char *)lzxleaf.keys;

        /*
            for each term
        */
        for (iock=0; iock < tlcleaf.l0x.ock; iock++) {

            if (treecase) {
                keyp  = tlcleaf.l2x.idx[iock].key;
                info1 = tlcleaf.l2x.idx[iock].info1;
                info2 = tlcleaf.l2x.idx[iock].info2;
#if LIND
                info3 = tlcleaf.l2x.idx[iock].info3info4.info3;
#endif
            }
            else {
                keyp  = tlcleaf.l1x.idx[iock].key;
                info1 = tlcleaf.l1x.idx[iock].info1;
                info2 = tlcleaf.l1x.idx[iock].info2;
#if LIND
                info3 = tlcleaf.l1x.idx[iock].info3info4.info3;
#endif
            }

            /* move existing info */
            lzxleaf.info1[iock] = info1; /* yaddr */
            lzxleaf.info2[iock] = info2;
#if LIND
            lzxleaf.info3[iock] = info3;
#endif

            /* compress existing key */
            for (keylen=lex, p=keyp+keylen-1; keylen; ) {
                if (*p != ' ') break;
                p--; keylen--;
            }
            if (!keylen) {
                fprintf(stderr,"*** punt=%"_LD_"/%d keylen=%d",punt,iock,keylen);
                fatal("keylen");
            }

            if (iock == 0) eq=0;
            else {
                for (eq=0; eq < keylen; eq++)
                    if (keyp[eq] != prevkey[eq]) break;
            }
            memmove(prevkey,keyp,lex);

            if (show) {
                fprintf(fpx,"%3d|%3d|",eq,keylen);
                for (p=keyp, n=keylen; n--; p++) fprintf(fpx,"%c",*p);
                fprintf(fpx,"|");
                for (n=lex-keylen; n--; ) fprintf(fpx," ");
                fprintf(fpx,"|\n");
            }

            *lzkp++ = (unsigned char)eq;
            *lzkp++ = (unsigned char)keylen;

            memmove(lzkp,keyp+eq,ne=keylen-eq);
            lzkp+=ne;

            lzsize+=2;          /* eq+keylen */
            lzsize+=ne;
	    
        } /* end for each term */

        for (iock=tlcleaf.l0x.ock; iock < TWORDF; iock++) {
            lzxleaf.info1[iock] = 0;
            lzxleaf.info2[iock] = 0;
#if LIND
            lzxleaf.info3[iock] = 0;
#endif
        }

	/* crunch .lx0 */
	movp=mov_record; moved=0;
	x4=lzxleaf.pos; mov4(x4);
	x2=lzxleaf.ock; mov2(x2); 
	x2=lzxleaf.it; mov2(x2); 
	movps_p=movp; x4=lzxleaf.ps; mov4(x4);
#if LIND
	movpsb_p=movp; x4=lzxleaf.psb; mov4(x4);
#endif
	for (iock=0; iock < TWORDF; iock++) {
	    info1=lzxleaf.info1[iock]; mov4(info1);
	}
	for (iock=0; iock < TWORDF; iock++) {
	    info2=lzxleaf.info2[iock]; mov4(info2);
	}
#if LIND
	for (iock=0; iock < TWORDF; iock++) {
	    info3=lzxleaf.info3[iock]; mov4(info3);
	}
#endif
	mzxrecl=moved+TWORDF*(2+lex); /* n */
	lzkp=(char *)lzxleaf.keys; movx(lzkp,lzsize);

	lzsize=moved;

#if LIND
	thisaddr=nextaddr;
	if (punt == punt1) lastaddr=0;
#endif
        nextaddr+=lzsize;
	origsize+=/* moved; */ origlsiz;

        filler=0;
        if (punt == punt2) {
	    filler = mzxrecl-lzsize; nextaddr+=filler;
	    if (filler >= sizeof(LZSTRU)) fatal("myzcru/bug2");
        }

        /* fixup ps/psb header */
        lzxleaf.ps  = (punt == punt2) ? 0L : nextaddr+1; /* punt < 0 */
	movp=movps_p; x4=lzxleaf.ps; mov4(x4); moved-=4;
#if LIND
        lzxleaf.psb = (punt == punt1) ? 0L : lastaddr+1; /* punt < 0 */
	movp=movpsb_p; x4=lzxleaf.psb; mov4(x4); moved-=4;
        lastaddr=thisaddr;
#endif

	/* write */
        fpcwrite(mov_record,moved);

        ++countell;
        if (parmtell)
            if (countell % tell == 0 || filler) {
                fprintf(fpx,"%s +%"_LD_"p=%3"_LD_"%%  ",lx12extp[treecase],countell,
                    (10 * (countell*100/rangtell) + 5) / 10);
		fprintf(fpx,"i=%3d/%"_LD_"  ",origlsiz,origsize);
		fprintf(fpx,"o=%3d/%"_LD_"  ",lzsize,nextaddr);
		fprintf(fpx,"save=%"_LD_"=%2d%%  ",origsize-nextaddr,
                    (10 * ((origsize-nextaddr)*100/origsize) + 5) / 10);
		if (filler) fprintf(fpx,"filler=%d",filler);
		fprintf(fpx,"\n");
	    }

    } /* end for punt */

    if (filler) {
        memset(mov_record,0x00,filler);
        fpcwrite(mov_record,filler);
    }
    fpcclose();
    
    /*
        rewrite .n0x
        (pointing to the absolute leaf address)
    */

    punt1=1; punt2=invp->cn[treecase].nmaxpos;
    rangtell=punt2-punt1+1;
    if (rangtell <= 0) rangtell=1;
    countell=0;

    if (parmtrace) printf("+++ nmaxpos=%"_LD_" \n",punt2);

    if (parmtelln) {
        tell=rangtell/parmtelln;
        if (tell <= 0) tell=1;
    }

    fpccreat("",zdbnp,nx12extp[treecase],0);
    nodesize=(treecase)?N2BSIZ:N1BSIZ;

    /* for each node */

    for (punt=punt1; punt <= punt2; punt++) {

        /* read */
        n0p=noderead(invp,treecase,0,punt);

        /* update leaf punt's */
        n=n0p->ock;
	if (treecase)
	    for (n2xp=((N2STRU *)n0p)->idx; n--; n2xp++) {
                if (n2xp->punt < 0)
		    n2xp->punt = -leafaddr(treecase,punt,TWORDN-n,-n2xp->punt);
            }
	else
	    for (n1xp=((N1STRU *)n0p)->idx; n--; n1xp++) {
                if (n1xp->punt < 0)
		    n1xp->punt = -leafaddr(treecase,punt,TWORDN-n,-n1xp->punt);
            }

        /* crunch .n0x */
	movp=mov_record; moved=0;
	mov4(n0p->pos);
	mov2(n0p->ock);
	mov2(n0p->it);
	loop=TWORDN;
	if (treecase)
	    for (n2xp=((N2STRU *)n0p)->idx; loop--; n2xp++) {
	        movx(n2xp->key,LE2);
	        mov4(n2xp->punt);
	    }
	else
	    for (n1xp=((N1STRU *)n0p)->idx; loop--; n1xp++) {
	        movx(n1xp->key,LE1);
	        mov4(n1xp->punt);
	    }

        /* write */
	fpcwrite(mov_record,moved);

        ++countell;
        if (parmtelln)
	    if (countell % tell == 0) {
		fprintf(fpx,"%s +%"_LD_"p=%3"_LD_"%%  ",nx12extp[treecase],countell,
		    (10 * (countell*100/rangtell) + 5) / 10);
		fprintf(fpx,"i=%3d/%"_LD_"\n",moved,punt*nodesize);
            }

    } /* end for punt */

    fpcclose();

} /* end for treecase */   

    
    /*
        rewrite .cnt
    */

    fpccreat("",zdbnp,cx1extp,0);
    invp->cn[0].ordf++;
    for (treecase=0; treecase < 2; treecase++) {
	if (parmtrace) printf("+++ ordf=%d \n",invp->cn[treecase].ordf);
#if PCCTRACE
 memmove(mov_record,&(invp->cn[treecase]),moved=CNBSIZ);
 putpcinv(moved);
#endif
        movp=mov_record; moved=0;
        mov2(invp->cn[treecase].idtype);
        mov2(invp->cn[treecase].ordn);
        mov2(invp->cn[treecase].ordf);
        mov2(invp->cn[treecase].n);
        mov2(invp->cn[treecase].k);
        mov2(invp->cn[treecase].liv);
        mov4(invp->cn[treecase].posrx);
        mov4(invp->cn[treecase].nmaxpos);
        mov4(invp->cn[treecase].fmaxpos);
        mov2(invp->cn[treecase].abnormal);
	fpcwrite(mov_record,moved);
    }
    fpcclose();


    exit(0);
}


LONGX leafadda(treecase,punt,addr)
int treecase;
PUNT punt;
LONGX addr;
{
    if (punt < 1) fatal("leafadda/punt");

    apun=punt/divapu;

    if (apun >= napus) {
	if (napus >= MAXNAPU) fatal("MAXNAPU");
	apup=(LONGX *)ALLOC((ALLOPARM)ALLOMAXV);
	if (apup == (LONGX *)ALLONULL) fatal("ALLOC/vapup");
	apun=napus;
	vapup[napus++]=apup;
    }

    apuk=punt%divapu;
    apup=vapup[apun];

    if (parmtrace)
	printf("+++ leafadda - punt=%d/%"_LD_" addr=%"_LD_"  [%d/%d +%d/%d]\n",
	    treecase,punt,addr,apun,napus,apuk,divapu);

    apup[apuk]=addr;

    return(addr);
}


LONGX leafaddr(treecase,pos,ock,punt)
int treecase;
PUNT pos;
int ock;
PUNT punt;
{
    apun=punt/divapu;

    if (punt < 1 || apun >= napus) fatal("leafaddr/punt");

    apuk=punt%divapu;
    apup=vapup[apun];

    if (parmtrace)
	printf("+++ leafaddr - punt=%d/%"_LD_" node=%"_LD_"/%d addr=%"_LD_" \n",
	    treecase,punt,pos,ock,apup[apuk]);

    return(apup[apuk]);
}
