
/* --------------------------- lydump.c ---------------------------- */

/* ------------------------------ my.c ----------------------------- */

#include <stdio.h>
#include <string.h>

#include "cisis.h"      /* dbx routines */
#include "cirun.h"      /* runtime area and defines by AOT */


/* ----------------------------- tlc.h ------------------------------ */

typedef struct tlcleaf {
    PUNT        pos;                    /* gdb .l0x pos */
    UWORD         ock;                    /* gdb .l0x ock */
    UWORD         it;                     /* gdb .l0x it */
    PUNT        ps;                     /* gdb .l0x ps */
    PUNT        psb;                    /* tlc gdb .l0x ps backward */
    union {
        struct l1xe {
            UCHR        key[LE1] PACKED;       /* tlc .l01 keys */
            INFX        info1 PACKED;          /* tlc info1 - byte addr */
            INFO        info2 PACKED;          /* tlc info2 - tot no of postings */
            union {
                INFX    info3 PACKED;          /* tlc info3 - tot no of docs */
                INFX    info4 PACKED;          /* tlc info4 - posting type and etc */
            } info3info4;
        } l1x[TWORDF];
        struct l2xe {
            UCHR        key[LE2] PACKED;       /* tlc .l02 keys */
            INFX        info1 PACKED;          /* tlc info1 - byte addr */
            INFO        info2 PACKED;          /* tlc info2 - tot no of postings */
            union {
                INFX    info3 PACKED;          /* tlc info3 - tot no of docs */
                INFX    info4 PACKED;          /* tlc info4 - posting type and etc */
            } info3info4;
        } l2x[TWORDF];
    } lx;
} TLCLEAF;

#define BITSTRING       (-1)
#define MFNSTRING       0
#define CNTSTRING       1

/* ----------------------------------------------------------------------- */

TLCLEAF tlcleaf;
int sizeofleaf[2] = { sizeof(TLCLEAF)
                                -sizeof(struct l2xe)*TWORDF
                                +sizeof(struct l1xe)*TWORDF,
                      sizeof(TLCLEAF) };



void main(argc,argv)
int argc;
char *argv[];
{
    int fd,nread,i,n,treecase,szofleaf;

    TLCLEAF *lp;
    char *p;
    LONGX count;


    if (argc < 2)
        fatal("parm missing");

    if ((fd=OPEN(argv[1],O_RDONLY | O_BINARY)) <= 0)
        fatal(argv[1]);

    treecase=argv[1][strlen(argv[1])-1];
    if (treecase != '1' && treecase != '2') fatal("treecase 1/2");
    szofleaf=sizeofleaf[(treecase-='1')];


    count=0;
    lp= &tlcleaf;
    if (argc>2) {
       LONGX ld;
       INFX skipb;
       if (sscanf(argv[2],"%"_LD_,&ld)==1) {
              skipb=(INFX)ld*szofleaf;
              printf("ld,sizeofleaf,skipb=%"_LD_",%d,%lu\n",ld,szofleaf,skipb);
              printf("lseek=%lu\n",LSEEK64(fd,skipb,SEEK_SET));
       }
     }


    nread=CIREAD(fd,(char *)lp,szofleaf);
    printf("nread,sizeofleaf=%d,%d\n",nread,szofleaf);

    while (nread == szofleaf) {
        ++count;
        printf("%4"_LD_" pos=%4"_LD_" ock=%2d it=%d ps=%"_LD_"/%"_LD_" \'",
            count,(LONGX)lp->pos,lp->ock,lp->it,(LONGX)lp->ps,(LONGX)lp->psb);
                if (treecase)
        for (p=lp->lx.l2x[0].key, n=LE2; n--; p++) printf("%c",*p);
                else
        for (p=lp->lx.l1x[0].key, n=LE1; n--; p++) printf("%c",*p);
        printf("\'");
        if (argc > 2) {
            if (*argv[2] == '/') putchar('\n'); else getchar();
            for (i=0; i < lp->ock; i++) {
                printf(" %2d \'",i+1);
                if (treecase) {
                    for (p=lp->lx.l2x[i].key, n=LE2; n--; p++) printf("%c",*p);
                    printf("\' info=%"P_OFF_T",%"_LD_",%"_LD_"=%"_LD_"\n",
                        (LONG_LONG)lp->lx.l2x[i].info1,
                        lp->lx.l2x[i].info2,
                        lp->lx.l2x[i].info3info4.info3,
                        lp->lx.l2x[i].info3info4.info4);
                }
                else {
                    for (p=lp->lx.l1x[i].key, n=LE1; n--; p++) printf("%c",*p);
                    printf("\' info=%lu,%"_LD_",%"_LD_"=%"_LD_"\n",
                        lp->lx.l1x[i].info1,
                        lp->lx.l1x[i].info2,
                        lp->lx.l1x[i].info3info4.info3,
                        lp->lx.l1x[i].info3info4.info4);
                }
            }
        }
        else
            putchar('\n');
        nread=CIREAD(fd,(char *)lp,szofleaf);
    }

    printf("count,nread=%"_LD_",%d\n",count,nread);
    exit(0);
}
