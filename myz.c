/* ----------------------------- myz.c -------------------------------- */

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
int napus;
LONGX *vapup[MAXNAPU];
LONGX *apup;
LONGX maxapu,offapu,divapu;

int parmtrace=0;

#if ANSI
LONGX leafaddr(PUNT pos,PUNT punt);
#else
LONGX leafaddr();
#endif

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

    int lzsize=0;
    int filler=0;
    LONGX nextaddr;

    LONGX tell,countell,rangtell;
    LONGX telln,countelln,rangtelln;
    int show;
    FILE *fpx;

    int origlsiz;
    LONGX origsize;

    char *p,*keyp,prevkey[LE2+1];
    int iarg,n,iock;
    int eq,keylen,ne;
    unsigned char *lzkp;

    N0STRU *n0p;
    N1IDXE *n1xp;
    N2IDXE *n2xp;
    int nodesize;


    if (argc < 3) {
        printf("%s",cicopyr("Utility MYZ"));
        printf("\n");
        printf("myz <dbn> <treecase> <zdbn> [<option> [...]] \n");
        printf("\n");
        printf("options: show[=<file>] \n");
        printf("         {tell|telln}=<times> \n");
        printf("\n");
        exit(1);
    }

    dbnp=argv[1];
    dbxopt_ordwr=O_RDONLY;
    invsetup(dbnp,0L,0L,0L);
    dbxopt_ordwr=O_RDWR;
    invp=dbxstorp(dbnp)->dbxifmap;

    if (sscanf(argv[2],"%d",&treecase) != 1) fatal(argv[3]);
    if (treecase != 1 && treecase != 2) fatal("treecase 1/2");
    treecase--;

    zdbnp=argv[3];

    punt1=1; punt2=invp->cn[treecase].fmaxpos;
    rangtell=punt2-punt1+1;
    if (rangtell <= 0) rangtell=1;
    fpx=stderr; show=0; tell=telln=0;

    for (iarg=4; iarg < argc; iarg++) {
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
        if (strcmp(dbnp,"rdonly") == 0) {
            dbxopt_ordwr=O_RDONLY;
            continue;
        }
        if (strcmp(p,"trace") == 0) {
            dbxtrace=trmtrace=parmtrace=1; continue;
        }
        if (strncmp(p,"tell=",5) == 0) {
            if (sscanf(p+5,"%"_LD_,&tell) != 1) fatal(p);
            if (tell < 0) fatal(p);
            continue;
        }
        if (strncmp(p,"telln=",6) == 0) {
            if (sscanf(p+6,"%"_LD_,&telln) != 1) fatal(p);
            if (telln < 0) fatal(p);
            continue;
        }
        fatal(p);
    }

    if (parmtrace) printf("+++ fmaxpos=%"_LD_" \n",punt2);

    if (tell) {
        tell=rangtell/tell;
        if (tell <= 0) tell=1;
    }

    origlsiz = (treecase) ? L2BSIZ : L1BSIZ;
    origsize=0;
    lex=vlex[treecase];
    countell=0;
    napus=0;
    maxapu=offapu=0;
    divapu=ALLOMAXV/sizeof(LONGX);
    nextaddr=0;

    if (parmtrace) printf("+++ coreleft=%"_LD_"\n",CORELEFT());

    fpccreat("",zdbnp,lx12extp[treecase],0);

    /*
        for each leaf
    */
    for (punt=punt1; punt <= punt2; punt++) {

        if (punt >= maxapu) {
            if (napus >= MAXNAPU) fatal("MAXNAPU");
            apup=(LONGX *)ALLOC((ALLOPARM)ALLOMAXV);
            if (apup == (LONGX *)ALLONULL) fatal("ALLOC/vapup");
            maxapu+=divapu;
            if (napus) offapu+=divapu;
            vapup[napus++]=apup;
        }
        apup[punt-offapu]=nextaddr+1; /* punt < 0 */

        /* read */
        leafread((char *)&tlcleaf,invp,treecase,punt,0);

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
        
        lzsize = LZHSIZE;
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
        }

        for (iock=tlcleaf.l0x.ock; iock < TWORDF; iock++) {
            lzxleaf.info1[iock] = 0;
            lzxleaf.info2[iock] = 0;
#if LIND
            lzxleaf.info3[iock] = 0;
#endif
        }

#if LIND
        thisaddr=nextaddr;
        if (punt == punt1) lastaddr=0;
#endif
        nextaddr+=lzsize;
        origsize+=origlsiz;

        filler=0;
        if (punt == punt2) {
            n = (treecase) ? sizeof(LZSTRU) : LZHSIZE + LZKASZ1;
            filler = n-lzsize;
            p=(char *)&lzxleaf; memset(p+lzsize,0x00,filler);
            lzsize+=filler;
            nextaddr+=filler;
            filler = n;
            if (nextaddr % filler) {
                filler = filler - nextaddr % filler;
                nextaddr+=filler;
            } else filler=0;
        }

        /* fixup ps/psb header */
        lzxleaf.ps  = (punt == punt2) ? 0L : nextaddr+1; /* punt < 0 */
#if LIND
        lzxleaf.psb = (punt == punt1) ? 0L : lastaddr+1; /* punt < 0 */
        lastaddr=thisaddr;
#endif

        fpcwrite((char *)&lzxleaf,lzsize);

        ++countell;
        if (tell)
            if (countell % tell == 0 || filler) {
                fprintf(fpx,"+%5"_LD_"p %3"_LD_"%%    ",countell,
                    (10 * (countell*100/rangtell) + 5) / 10);
                fprintf(fpx,"orig=%7"_LD_" [%3d]    ",origsize,origlsiz);
                fprintf(fpx,"this=%7"_LD_" [%3d]    ",nextaddr,lzsize);
                fprintf(fpx,"save=%7"_LD_" (%2d%%)\n",origsize-nextaddr,
                    (10 * ((origsize-nextaddr)*100/origsize) + 5) / 10);
            }
    }

    if (filler) {
        memset((char *)&lzxleaf,0x00,filler);
        fpcwrite((char *)&lzxleaf,filler);
    }

    fpcclose();

    
    /*
        rewrite .n0x 
        (pointing to the absolute leaf address)
    */

    punt1=1; punt2=invp->cn[treecase].nmaxpos;
    rangtelln=punt2-punt1+1;
    if (rangtelln <= 0) rangtelln=1;
    countelln=0;

    if (parmtrace) printf("+++ nmaxpos=%"_LD_" \n",punt2);

    if (telln) {
        telln=rangtelln/telln;
        if (telln <= 0) telln=1;
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
                    n2xp->punt = -leafaddr(punt,-n2xp->punt);
            }
        else
            for (n1xp=((N1STRU *)n0p)->idx; n--; n1xp++) {
                if (n1xp->punt < 0)
                    n1xp->punt = -leafaddr(punt,-n1xp->punt);
            }
            
        /* write */
        fpcwrite((char *)n0p,nodesize);

        ++countelln;
        if (telln)
            if (countelln % telln == 0) {
                fprintf(fpx,"+%5"_LD_"p %3"_LD_"%%    ",countelln,
                    (10 * (countelln*100/rangtelln) + 5) / 10);
                fprintf(fpx,"orig=%7"_LD_" [%3d]  \n",punt*nodesize,nodesize);
            }
    }

    fpcclose();

    
    /*
        rewrite .cnt
    */

    if (parmtrace) printf("+++ ordf=%d \n",invp->cn[0].ordf);

    fpccreat("",zdbnp,cx1extp,0);

    invp->cn[0].ordf++;
    fpcwrite((char *)&(invp->cn[0]),CNBSIZ);
    fpcwrite((char *)&(invp->cn[1]),CNBSIZ);

    fpcclose();

   
    exit(0);
}


LONGX leafaddr(pos,punt)
PUNT pos;
PUNT punt;
{
    int n,k;
    
    if (punt < 1 || punt >= maxapu) {
        fprintf(stderr,"*** nodepos=%"_LD_" leafpunt=%"_LD_" \n",pos,punt);
        fatal("leafaddr/punt");
    }
    
    n=punt/divapu;
    k=punt%divapu;

    apup=vapup[n];
    
    return(apup[k]);
}
