/* ------------------------------ mxgw.c ----------------------------- */

#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>

#if 1
#include "cisis.h"   /* CISIS Interface header file */
#include "cirun.h"   /* CISIS Interface runtime declarations */
#else
#include "\cisis\include\cisis.h"   /* CISIS Interface header file */
#include "\cisis\include\cirun.h"   /* CISIS Interface runtime declarations */
#endif

#define MAXTAG  10000

/* Global */
UBYTE *buffup,line[CIMPL+1];
char *dbnamep,*dbnoutp;

LONGX count1=0;
LONGX count2=0;

#if CISISH
typedef struct gizmstru {
    short isize;
    short osize;
    unsigned char *ipatt;
    unsigned char *opatt;
    LONGX nused;
    struct gizmstru *nextp;
} GIZMSTRU;

#define MAXGIZR 8
typedef struct vgizpstru {
    GIZMSTRU *ghdrp[256];
    int grngs;
    UWORD grng1[MAXGIZR];
    UWORD grng2[MAXGIZR];
    unsigned char *gdbnp;
    struct vgizpstru *nextp;
} VGIZPSTRU;

typedef struct dbxstru {
....VGIZPSTRU *dbxvgzrp;          /* gizmo processing */
} DBXSTRU;
#endif

/* Prototypes */
#if ANSI
LONGX wecgizmo(LONGX irec, VGIZPSTRU *gizmap);
#else
LONGX wecgizmo();
#endif


void main(argc,argv)
int argc;
char *argv[];
{
    RECSTRU *recp;      /* mandatory for defines REC RDB MF0 MFR DIR, FIELDP */

    LONGX irec;          /* current master file record */
    LONGX upcrec;        /* output control record */

    int argnext,parmtrace;
    LONGX parmfrom,parmto,parmloop,parmcount,parmtell;

    LONGX mfn;
    UBYTE *p,*q;
    UWORD mftype,tag,tag1,tag2;
    LONGX xtag1,xtag2;
    VGIZPSTRU *gizmap,*vgizmap = NULL;

    if (argc < 3) {
        printf("%s",cicopyr("Utility MXGW"));
        printf("\n");
        printf("mxgw <dbin> [create=]<dbout> [<option> [...]] \n");
        printf("\n");
        printf("options: {from|to|loop|count|tell}=<n> \n");
	printf("         gizmo=<dbgiz>[,tag_list>] \n");
        printf("\n");
        printf("\n");
	printf("Ex: mxgw in out gizmo=giz\n");
        printf("\n");
	printf("     giz = 1 %cagua%c\n",OPENFCHR,CLOSFCHR);
	printf("           2 %c gua%c\n",OPENFCHR,CLOSFCHR);
        printf(" \n");
	printf("      in = 9 %cagua%c\n",OPENFCHR,CLOSFCHR);
	printf("           9 %caguardente%c\n",OPENFCHR,CLOSFCHR);
        printf(" \n");
	printf("     out = 9 %c gua%c\n",OPENFCHR,CLOSFCHR);
	printf("           9 %caguardente%c\n",OPENFCHR,CLOSFCHR);
        printf("\n");
        exit(1);
    }


    argnext=1;

    dbnamep=argv[argnext++];
    dbnoutp=argv[argnext++];

    recp=vrecp[recallok(irec=nrecs,MAXMFRL)];
    parmtrace=0;
    parmfrom=1; parmto=0; parmloop=1; parmcount=0; parmtell=0;

    /*
        get optional parameters
    */
    for ( ; argnext < argc; argnext++) {
        p=argv[argnext];

        if (strncmp(p,"from=",5) == 0) {
            if (sscanf(p+5,"%"_LD_,&parmfrom) != 1)
                fatal(p);
            continue;
        }
        if (strncmp(p,"to=",3) == 0) {
            if (sscanf(p+3,"%"_LD_,&parmto) != 1)
                fatal(p);
            continue;
        }
        if (strncmp(p,"loop=",5) == 0) {
            if (sscanf(p+5,"%"_LD_,&parmloop) != 1)
                fatal(p);
            continue;
        }
        if (strncmp(p,"count?",6) == 0 || strncmp(p,"count=",6) == 0) {
            if (sscanf(p+6,"%"_LD_,&parmcount) != 1)
                fatal(p);
            continue;
        }
        if (strncmp(p,"tell?",5) == 0 || strncmp(p,"tell=",5) == 0) {
            if (sscanf(p+5,"%"_LD_,&parmtell) != 1)
                fatal(p);
            continue;
        }

        if (strncmp(p,"gizmo=",6) == 0) {
            for (line[0]='\0', q=p+6; *q; q++)
		if (*q == '=' || *q ==',') {
                    *q='\0'; strcpy(line,p+6);
                    if (!*++q) fatal(p);
                    break;
                }
            if (!line[0]) {
                strcpy(line,p+6);
                q=p+strlen(p);
            }
            if (vgizmap) {
                for (gizmap=vgizmap; gizmap->nextp; )
                    gizmap=gizmap->nextp;
                gizmread(line,&gizmap->nextp,irec);
                gizmap=gizmap->nextp;                           /* 11/03/92 */
            }
            else {
                gizmread(line,&vgizmap,irec);
                gizmap=vgizmap;
            }

            while (*q) {
                if (gizmap->grngs >= MAXGIZR)
                    fatal("Too many tags in gizmo tag_list");
                if (sscanf(q,"%"_LD_"/%"_LD_,&xtag1,&xtag2) == 2) {
		    tag1=(UWORD)xtag1;
		    tag2=(UWORD)xtag2;
                    if (tag1 > tag2) fatal(p);
                    while (*q != '/') q++; q++;
                    while (isdigit(*q)) q++;
                    gizmap->grng1[gizmap->grngs]=tag1;
                    gizmap->grng2[gizmap->grngs]=tag2;
                    if (*q == ',') if (*(q+1)) q++;
                    gizmap->grngs++;
                    continue;
                }
                if (sscanf(q,"%"_LD_,&xtag1) == 1) {
		    tag=(UWORD)xtag1;
                    while (isdigit(*q)) q++;
                    if (tag >= MAXTAG) fatal(p);
                    gizmap->grng1[gizmap->grngs]=tag;
                    gizmap->grng2[gizmap->grngs]=tag;
                    if (*q == ',') if (*(q+1)) q++;
                    gizmap->grngs++;
                    continue;
                }
                fatal(p);
            }
            if (parmtrace) {
                for (gizmap=vgizmap; gizmap; gizmap=gizmap->nextp)
                    printf("+++ gizmo %s \n",gizmap->gdbnp);
            }
            continue;
        }

        if (strcmp(p,"trace") == 0) {
            parmtrace=1;
            continue;
        }

        fatal(p);
    }

    if (parmtrace) {
        printf("+++ input data base:  %s\n",dbnamep);
        printf("+++ output data base: %s\n",dbnoutp);
    }

    /*
        allocate buffup
    */
    if ((buffup=(char *)ALLOC((ALLOPARM)(BUFFUPL))) == ALLONULL)
        fatal("ALLOC/buffup");

    /*
        setup input data base
    */
    RECORD(irec,dbnamep,0L);
    mftype=MF0mftype;
    if (parmtrace && !parmtell) {
        parmtell=(MF0nxtmfn-1)/10;
        if (parmtell < 1) parmtell=1;
    }


    /*
        setup output master file
    */
    if (strncmp(dbnoutp,"create=",7) == 0)
        recisis0((dbnoutp+=7));

    /*
        open output and load output control record
    */
    RECORD((upcrec=nrecs),dbnoutp,1L);
    RECORD(upcrec,dbnoutp,0L);
    MF0mftype=mftype;


    /*
        main loop
    */
    for (mfn=parmfrom; ; mfn+=parmloop) {

        if (parmtell)
            if (!(count1 % parmtell))
                if (count1)
                    fprintf(stderr,"+++ %"_LD_"/%"_LD_" recs\n",count1,count2);

        if (parmto)
            if (mfn > parmto)
                break;

        if (parmcount)
            if (count2 >= parmcount)
                break;

        RECORD(irec,dbnamep,mfn);

        if (RECrc == RCEOF)
            break;
        count1++;

        if (RECrc != RCNORMAL)
            continue;

        /*
            gizmo processing
        */
        for (gizmap=vgizmap; gizmap; gizmap=gizmap->nextp)
            wecgizmo(irec,gizmap);

        /*
            output the current master file record
        */
        if (MFRnvf) {
            MFRmfn=mfn+0;
            MFRstatus=ACTIVE; /* good */
            if (recupdat(upcrec,irec))
                fatal("recupdat");

            count2++;
        }

    } /* end of main loop */

    if (parmtrace)
        if (!(count1 % parmtell))
            ;
        else
            fprintf(stderr,"+++ %"_LD_"/%"_LD_" recs\n",count1,count2);

    exit(0);
}


/* ----------------------------- giz.c ------------------------------ */

#define TRACEG1         0
#define TRACEG2         0
#define TRACEG3         0

#define TRACEGT         00
#if TRACEGT
#define GETCHAR printf("\n");
#else
#define GETCHAR getchar()
#endif

LONGX wecgizmo(irec,gizmap)
LONGX irec;                      /* indice de vrecp, para info/store */
VGIZPSTRU *gizmap;              /* estrutura gizmo a considerar */
{
    RECSTRU *recp;

    GIZMSTRU *gizmhp,*gizmp;

    short match;
    unsigned char *p,*q;
    unsigned char *batchp,*fuplenp;
    LONGX nmatchs;
    int xdir,dirloop,fldloop,fuplen,irng,rngok,wl;
    UWORD tag;
#if TRACEG3
    unsigned char *xp;
    int n;
#endif



    if (rectrace)
        printf("wecgizmo - irec=%"_LD_" %s=%p\n",irec,gizmap->gdbnp,gizmap);

    if (!nrecs)
        fatal("wecgizmo/nrecs");

    recp=vrecp[irec];
    if (!recp)
        fatal("wecgizmo/irec");

    if (RECtype != TYPEMFR) {
        printf("*** irec=%"_LD_"  RECtype=%d\n",irec,RECtype);
        fatal("wecgizmo/check");
    }


    nmatchs=0;
    if (gizmap->ghdrp == NULL) fatal("wecgizmo/ghdrp");

    for (batchp=buffup, xdir=0, dirloop=MFRnvf; dirloop--; xdir++) {
        tag=DIRtag(xdir); rngok=1;
#if TRACEG1
printf("\\begin tag %d..",tag); GETCHAR;
#endif

        if (gizmap->grngs)
            for (rngok=0, irng=0; irng < gizmap->grngs; irng++) {
                if (tag < gizmap->grng1[irng]) continue;
                if (tag > gizmap->grng2[irng]) continue;
                rngok=1; break;
            }

        if (rngok) {
#if TRACEG1
printf("\\gizmo tag %d..",tag); GETCHAR;
#endif
            sprintf(batchp,"H%u 000000 ",tag); batchp+=strlen(batchp);
            fuplenp=batchp-1-6; fuplen=0; /* fuplenp points to 000000 */
#if TRACEG2
printf("\\buffup=%s",buffup); GETCHAR;
printf("%3d '",DIRtag(xdir));
for (p=FIELDP(xdir), fldloop=DIRlen(xdir); fldloop-- > 0; p++) printf("%c",*p);
printf("'\n");
#endif
            for (p=FIELDP(xdir), fldloop=DIRlen(xdir); fldloop > 0; ) {
#if TRACEG3
printf("\\fldloop=%d *p=%c",fldloop,*p); GETCHAR;
#endif
                if (!isiswctab[*p]) {
#if TRACEG3
printf("\\no word - copying %c",*p); GETCHAR;
#endif
                    *batchp++ = *p++;
                    fldloop--; fuplen++;
                    continue;
                }
                for (q=p, wl=1, fldloop--; fldloop > 0; )
                    if (isiswctab[*++p]) {
                        wl++;
                        fldloop--;
                    } else break;
#if TRACEG3
printf("\\word - wl=%d '",wl);
for (xp=q, n=wl; n--; xp++) printf("%c",*xp);
printf("'\n"); GETCHAR;
#endif

                match=0;
                gizmhp=gizmap->ghdrp[*q];
                for (gizmp=gizmhp; gizmp; gizmp=gizmp->nextp) {
#if TRACEG3
printf("\\need %d chars - ",wl);
for (xp=gizmp->ipatt, n=gizmp->isize; n--; xp++) printf("%c",*xp); GETCHAR;
#endif
                    if (gizmp->isize == wl)
                        if (memcmp(gizmp->ipatt,q,wl) == 0) {
                            match=1;
                            break;
                        }
                }

                if (match) {
#if TRACEG3
printf("\\match %d -> %d chars",gizmp->isize,gizmp->osize); GETCHAR;
#endif
                    memmove(batchp,gizmp->opatt,gizmp->osize);
                    batchp+=gizmp->osize;
                    fuplen+=gizmp->osize;
                    gizmp->nused++;
                    nmatchs++;
                }
                else {
#if TRACEG3
printf("\\no match - copying %c",*p); GETCHAR;
#endif
                    memmove(batchp,q,wl);
                    batchp+=wl;
                    fuplen+=wl;
                }

            } /* end field chrs */

            sprintf(fuplenp,"%06d",fuplen); *(fuplenp+6)=' ';

        }

        else {  /* just copy it */
            sprintf(batchp,"H%u %u ",tag,DIRlen(xdir)); batchp+=strlen(batchp);
            memmove(batchp,FIELDP(xdir),DIRlen(xdir)); batchp+=DIRlen(xdir);
        }

    } /* end dir entries */

    *batchp=NULL;

    if (nmatchs) {

        MFRmfrl=MFRbase=LEADER; MFRnvf=0;

        p=fldupdat(irec,buffup);
        if (p) {
            printf("*** fldupdat='%s'\n",p);
            fatal("wecgizmo/fldupdat");
        }
    }

    if (rectrace)
        printf("wecgizmo - %s %"_LD_" match(s)\n",gizmap->gdbnp,nmatchs);

    return(nmatchs);
}


#undef GETCHAR

