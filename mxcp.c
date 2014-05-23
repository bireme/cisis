/* ------------------------------ mxcp.c ----------------------------- */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#if 1
#include "cisis.h"   /* CISIS Interface header file */
#include "cirun.h"   /* CISIS Interface runtime declarations */
#else
#include "\cisis\include\cisis.h"   /* CISIS Interface header file */
#include "\cisis\include\cirun.h"   /* CISIS Interface runtime declarations */
#endif

#define MAXTAG  9999

#define PERIOD  '.'
#define REPEAT  '%'

/* Global */
UBYTE line[CIMPL+1],*buffup;
char *dbnamep,*dbnoutp,*batchp;
LONGX needup=0L;
LONGX totalup=0L;
UWORD mintag = 1;
UWORD maxtag = MAXTAG;
int parmtrace = 0;

LONGX count1=0;
LONGX count2=0;

FILE *lfp = NULL; /* stdout */
FILE *xfp = NULL;

UBYTE *vperiod = NULL;
UBYTE *vrepeat = NULL;
int tagperiod = 0;                                              /* 17/03/92 */
int tagrepeat = 0;                                              /* 17/03/92 */

UBYTE spacechr = ' ';                                           /* 17/03/92 */

#define MAXARGC	128
char *inargv[MAXARGC+1];					/* 25/05/98 */
char *inargvp=NULL;
int inargc=0;

/* Prototypes */
#if ANSI
LONGX clean(LONGX irec);
LONGX store(LONGX mfn, UWORD tag, int xocc, UWORD len, char *fldp);
#else
LONGX clean();
LONGX store();
#endif


void main(argc,argv)
int argc;
char *argv[];
{
    RECSTRU *recp;      /* mandatory for defines REC RDB MF0 MFR DIR, FIELDP */

    LONGX irec;          /* current master file record */
    LONGX upcrec;        /* output control record */
#if RECGIZM
    LONGX xrec;          /* gizmread() work record */
#endif

    int argnext,parmclean,parmundel;
    LONGX parmfrom,parmto,parmloop,parmcount;
    LONGX parmoffset,parmtell;

    LONGX mfn;
    UBYTE *p,*q,period,repeat;
    UWORD mftype,tag,tag1,tag2,create=0,n;
    LONGX xtag,xtag1,xtag2;                                      /* 17/03/92 */
#if RECGIZM
    VGIZPSTRU *gizmap,*vgizmap = NULL;
#endif


    /* processa in= como primeiro parm */
    
    if (argc > 1 && strncmp(p=argv[1],"in=",3) == 0) {
        inargc=1; inargv[inargc-1]=argv[0];
	inargvp=loadfile(NULL,'\0',p+3,NULL,0L,'\n');
	if (!inargvp) fatal(p);
        for (inargv[inargc]=p=inargvp; *p; p++) {
	    if (*p != '\n') continue;
	    *p='\0';
	    inargc++; inargv[inargc]=p+1;
	}
	inargv[inargc]=NULL;
    }
    else {
        for (inargc=0; inargc < argc; inargc++)
            inargv[inargc]=argv[inargc];
	inargv[inargc]=NULL;
    }


    if (inargc < 3) {
        printf("%s",cicopyr("Utility MXCP"));
        printf("\n");
        printf("mxcp {in=<file>|<dbin>} [create=]<dbout> [<option> [...]] \n");
        printf("\n");
        printf("options: {from|to|loop|count|tell|offset}=<n> \n");
#if RECGIZM
        printf("         gizmo=<dbgiz>[,tag_list>] \n");
#endif
	printf("         undelete \n");
        printf("         clean [mintag=%"_LD_"] ",(LONGX)mintag);
        printf("[maxtag=%"_LD_"] \n",(LONGX)maxtag);
        printf("         period=%c[,<tag_list>] \n",PERIOD);
        printf("         repeat=%c[,<tag_list>] \n",REPEAT);
        printf("         log=<filename> \n",(LONGX)maxtag);
        printf("\n");
        printf("\n");
        printf(" Ex: mxcp in create=out clean period=.,3 repeat=;,7 \n");
        printf(" \n");
        printf("      in = 3 %c   Field 3 occ 1. %c\n",OPENFCHR,CLOSFCHR);
        printf("           3 %cField 3 occ 2 . %c\n",OPENFCHR,CLOSFCHR);
        printf("           7 %c%s%c\n",OPENFCHR,
                                " Field 7/1;Field 7/2 ;Field 7/3.",
                                CLOSFCHR);
        printf(" \n");
        printf("     out = 3 %cField 3 occ 1%c\n",OPENFCHR,CLOSFCHR);
        printf("           3 %cField 3 occ 2%c\n",OPENFCHR,CLOSFCHR);
        printf("           7 %cField 7/1%c\n",OPENFCHR,CLOSFCHR);
        printf("           7 %cField 7/2%c\n",OPENFCHR,CLOSFCHR);
        printf("           7 %cField 7/3.%c\n",OPENFCHR,CLOSFCHR);
        printf("\n");
        exit(1);
    }

    argnext=1;
    
    dbnamep=inargv[argnext++];
    dbnoutp=inargv[argnext++];

    parmclean=0; parmundel=0;
    parmfrom=1; parmto=0; parmloop=1; parmcount=LONGX_MAX;
    parmoffset=0; parmtell=0;

#if RECGIZM
    xrec=nrecs++;
#endif

    /*
        get optional parameters
    */
    for ( ; argnext < inargc; argnext++) {
        p=inargv[argnext];

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
        if (strncmp(p,"count=",6) == 0) {
            if (sscanf(p+6,"%"_LD_,&parmcount) != 1)
                fatal(p);
            continue;
        }
        if (strncmp(p,"tell=",5) == 0) {
            if (sscanf(p+5,"%"_LD_,&parmtell) != 1)
                fatal(p);
            continue;
        }
        if (strncmp(p,"offset=",7) == 0) {
            if (sscanf(p+7,"%"_LD_,&parmoffset) != 1)
                fatal(p);
            continue;
        }

        if (strncmp(p,"mintag=",7) == 0) {
            if (sscanf(p+7,"%"_LD_,&tag) != 1) fatal(p);
            if (tag < 1 || tag > maxtag) fatal(p);
            mintag=tag;
            continue;
        }
        if (strncmp(p,"maxtag=",7) == 0) {
            if (sscanf(p+7,"%"_LD_,&tag) != 1) fatal(p);
            if (tag < mintag || tag > /* ALLOMAXV*/ USHRT_MAX) fatal(p);  /* AOT, 05/04/2001 */
            if (vperiod || vrepeat) fatal(p);
            maxtag=tag;
            continue;
        }

        if (strncmp(p,"log=",4) == 0) {
            if ((lfp=fopen(p+4,"w")) == NULL)
                fatal(p);
            continue;
        }
        if (strncmp(p,"xct=",4) == 0) {
            if ((xfp=fopen(p+4,"w")) == NULL)
                fatal(p);
            continue;
        }

        if (strncmp(p,"period=",7) == 0) {
            q=p+7; if (!*q) fatal(p);
            period = *q++;
            if (*q == '=' || *q == ',') q++;
            if (!vperiod) {
                vperiod=(UBYTE *)ALLOC((ALLOPARM)(maxtag+1));
                if (vperiod == ALLONULL)
                    fatal("Not enough memory for period");
                memset(vperiod,0x00,(unsigned)(maxtag+1));
            }
            else fatal(p);
            while (*q) {
                if (tagperiod) fatal(p);
                tagperiod=1;
                if (sscanf(q,"%"_LD_"/%"_LD_,&xtag1,&xtag2) == 2) {
                    tag1=(UWORD)xtag1; tag2=(UWORD)xtag2;
                    if (tag1 < 1 || tag2 > maxtag) fatal(p);
                    if (tag1 > tag2) fatal(p);
                    while (*q != '/') q++; q++;
                    while (isdigit(*q)) q++;
                    for (tag=tag1; tag <= tag2; tag++) {
                        if (vperiod[tag]) fatal(p);
                        vperiod[tag]=period;
                    }
                    if (*q == ',') if (*(q+1)) q++;
                    continue;
                }
                if (sscanf(q,"%"_LD_,&xtag) == 1) {
                    tag=(UWORD)xtag;
                    if (tag > maxtag) fatal(p);
                    while (isdigit(*q)) q++;
                    if (vperiod[tag]) fatal(p);
                    vperiod[tag]=period;
                    if (*q == ',') if (*(q+1)) q++;
                    continue;
                }
                fatal(p);
            }
            if (!tagperiod)
                memset(vperiod,period,(unsigned)(maxtag+1));
            if (parmtrace)
                for (tag=0; tag <= maxtag; tag++)
                    if (vperiod)
                        if (vperiod[tag])
                            printf("+++ period[%d]=%c\n",tag,vperiod[tag]);
            continue;
        }

        if (strncmp(p,"repeat=",6) == 0) {
            q=p+7; if (!*q) fatal(p);
            repeat = *q++;
            if (*q == '=' || *q == ',') q++;
            if (!vrepeat) {
                vrepeat=(UBYTE *)ALLOC((ALLOPARM)(maxtag+1));
                if (vrepeat == ALLONULL)
                    fatal("Not enough memory for repeat");
                memset(vrepeat,0x00,(unsigned)(maxtag+1));
            }
            else fatal(p);
            while (*q) {
                if (tagrepeat) fatal(p);
                tagrepeat=1;
                if (sscanf(q,"%"_LD_"/%"_LD_,&xtag1,&xtag2) == 2) {
                    tag1=(UWORD)xtag1; tag2=(UWORD)xtag2;
                    if (tag1 < 1 || tag2 > maxtag) fatal(p);
                    if (tag1 > tag2) fatal(p);
                    while (*q != '/') q++; q++;
                    while (isdigit(*q)) q++;
                    for (tag=tag1; tag <= tag2; tag++) {
                        if (vrepeat[tag]) fatal(p);
                        vrepeat[tag]=repeat;
                    }
                    if (*q == ',') if (*(q+1)) q++;
                    continue;
                }
                if (sscanf(q,"%"_LD_,&xtag) == 1) {
                    tag=(UWORD)xtag;
                    if (tag > maxtag) fatal(p);
                    while (isdigit(*q)) q++;
                    if (vrepeat[tag]) fatal(p);
                    vrepeat[tag]=repeat;
                    if (*q == ',') if (*(q+1)) q++;
                    continue;
                }
                fatal(p);
            }
            if (!tagrepeat)
                memset(vrepeat,repeat,(unsigned)(maxtag+1));
            if (parmtrace)
                for (tag=0; tag <= maxtag; tag++)
                    if (vrepeat)
                        if (vrepeat[tag])
                            printf("+++ repeat[%d]=%c\n",tag,vrepeat[tag]);
            continue;
        }

#if RECGIZM
        if (strncmp(p,"gizmo=",6) == 0) {
            q=p+6;

            if (!*q) fatal(p);

            if (vgizmap) {      /* gizmo already defined */
                for (gizmap=vgizmap; gizmap->nextp; )
                    gizmap=gizmap->nextp;
                gizmread(q,&gizmap->nextp,xrec);
                gizmap=gizmap->nextp;
            }
            else {              /* first gizmo definition */
                gizmread(q,&vgizmap,xrec);
            }

            if (parmtrace) {
                for (gizmap=vgizmap; gizmap; gizmap=gizmap->nextp)
                    printf("+++ gizmo=%s \n",gizmap->gdbnp);
            }
            continue;
        }
#endif

        if (strcmp(p,"clean") == 0) {
            parmclean=1;
            continue;
        }

        if (strcmp(p,"undelete") == 0) {
            parmundel=1;
            continue;
        }

        if (strcmp(p,"trace") == 0) {
            parmtrace=1;
            printf("+++ input data base:  %s\n",dbnamep);
            printf("+++ output data base: %s\n",dbnoutp);
            continue;
        }

        fatal(p);
    }

    if (!lfp) lfp=stdout;

    /*
        setup input data base
    */
    RECORD((irec=nrecs),dbnamep,0L);
    mftype=MF0mftype;
#if RECGIZM
    RDBvgzrp=vgizmap;                                           /* 17/03/92 */
#endif
    if (parmtrace && !parmtell) {
        parmtell=(MF0nxtmfn-1)/10;
        if (parmtell < 1) parmtell=1;
    }


    /*
        setup output master file
    */
    if (strncmp(dbnoutp,"create=",7) == 0) {
        if (strcmp(RDBname,dbnoutp+7) == 0) fatal(dbnoutp);
        recisis0((dbnoutp+=7));
        create=1;
    }

    /*
        open output and load its output control record
    */
    RECORD((upcrec=nrecs),dbnoutp,1L);
    RECORD(upcrec,dbnoutp,0L);
    if (create) MF0mftype=mftype;


    /*
        set spacechr to 0x00 [clean() is to be called for period/repeat]
    */
    if (!parmclean) if (vperiod || vrepeat) parmclean=1;

    /*
        allocate buffup
    */
    if (parmclean) {
        buffup=(UBYTE *)ALLOC((ALLOPARM)(BUFFUPL));
        if (buffup == ALLONULL)
            fatal("Not enough memory for buffup");
    }


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
            if (parmundel)
                if (RECrc == RCLDEL)
                    MFRstatus=ACTIVE;

        if (MFRstatus == DELETED) continue;

        /*
            cleaning procedure
        */
        if (parmclean) totalup+=clean(irec);

        /*
            output the current master file record
        */
        n=1;
        if (needup == 0) if (strcmp(RDBname,dbnoutp) == 0) n=0;
        if (create) n=1;
        if (!n) if (parmundel) if (RECrc == RCLDEL) n=1;
        if (n)
            if (MFRnvf > 0) {
                MFRmfn=mfn+parmoffset;
                MFRstatus=ACTIVE; /* good */
                if (recupdat(upcrec,irec))
                    fatal("recupdat");

                count2++;
            }
            else
                fprintf(lfp,"*** mfn %"_LD_" -> rejected record\n",mfn);

    } /* end of main loop */

    if (parmtrace)
        if (!(count1 % parmtell))
            ;
        else
            fprintf(stderr,"+++ %"_LD_"/%"_LD_" recs\n",count1,count2);

    if (xfp) fprintf(xfp,"%"_LD_"|%"_LD_"|%"_LD_"\n",count1,count2,totalup);
    
    exit(0);
}


LONGX clean(irec)
LONGX irec;
{
    RECSTRU *recp;
    UBYTE *p,*q,repeat;
    int dirindex,dirsleft;
    LONGX chrsleft;
    UWORD tag,pos,len;
    int xocc,xdir;

    needup=0;
    recp=vrecp[(unsigned)irec];

    /*
        set up the update buffer
    */
    batchp=buffup;
    for (dirindex=0, dirsleft=MFRnvf; dirsleft--; dirindex++) {

        for (xocc=0, xdir=0; xdir <= dirindex; xdir++)
            if (DIRtag(xdir) == DIRtag(dirindex)) xocc++;

        if ((tag=DIRtag(dirindex)) < 1 || tag > maxtag || tag < mintag) {
            fprintf(lfp,"*** mfn %"_LD_" tag=%u/%d -> rejected tag\n",
                MFRmfn,tag,xocc);
            needup++;
            continue;
        }
        if ((len=DIRlen(dirindex)) < 1) {
            fprintf(lfp,"*** mfn %"_LD_" tag=%u/%d len=%u -> rejected length\n",
                MFRmfn,tag,xocc,len);
            needup++;
            continue;
        }
        if ((pos=DIRpos(dirindex)) < 0 || pos+len+MFRbase > MFRmfrl) {
            fprintf(lfp,"*** mfn %"_LD_" tag=%u/%d pos=%u -> rejected position\n",
                MFRmfn,tag,xocc,pos);
            needup++;
            continue;
        }

        p=FIELDP(dirindex);
        chrsleft=len;

        repeat='\0';
        if (vrepeat)
            if (!tagrepeat)
                repeat=vrepeat[0];
            else
                if (tag <= maxtag)
                    repeat=vrepeat[tag];

        if (repeat)
            for (q=p, len=0; chrsleft--; ) {
                if (*q++ == repeat) {
                    store(MFRmfn,tag,xocc,len,p);
                    p=q; len=0;
                    needup++;
                }
                else
                    len++;
            }

        if (len > 0)
            store(MFRmfn,tag,xocc,len,p);

    } /* end of directory entries */

    *batchp='\0';

    /*
        update the current master file record
    */
    if (needup) {
        if (parmtrace) printf("buffup='%s'\n",buffup);
        if (fldupdat(irec,"D*")) fatal(fldupdat(irec,"D*"));
        if (fldupdat(irec,buffup)) fatal(fldupdat(irec,buffup));
    }

    return(needup);
}


LONGX store(mfn, tag, xocc, len, fldp)
LONGX mfn;
UWORD tag;
int xocc;
UWORD len;
char *fldp;
{
    UBYTE *p,*q,period,chrhex[4];
    LONGX chrsleft;

    p=fldp;
    q=p+len-1;

    chrsleft=len;

    while (chrsleft && *p <= spacechr) {
        sprintf(chrhex, isprint(*p) ? "%c  " : "%03d", *p);
        fprintf(lfp,"*** mfn %"_LD_" tag=%u/%d %s -> rejected char\n",
            mfn,tag,xocc,chrhex);
        p++; chrsleft--;
        needup++;
    }

    period='\0';
    if (vperiod)
        if (!tagperiod)
            period=vperiod[0];
        else
            if (tag <= maxtag)
                period=vperiod[tag];

    while (chrsleft > 0 && (*q <= spacechr || *q == period)) {
        sprintf(chrhex, isprint(*q) ? "%c  " : "%03d", *q);
        fprintf(lfp,"*** mfn %"_LD_" tag=%u/%d %s -> rejected char\n",
            mfn,tag,xocc,chrhex);
        q--; chrsleft--;
        needup++;
    }

    if (chrsleft < 1) {
        fprintf(lfp,"*** mfn %"_LD_" tag=%u/%d len=%u/%"_LD_" -> rejected length\n",
            mfn,tag,xocc,len,chrsleft);
        needup++;
        return(0);
    }

    sprintf(batchp,"H%u %"_LD_" ",tag,chrsleft);
    batchp+=strlen(batchp);

    for (; chrsleft--; )
        if (*p < spacechr) {
            *batchp++ = spacechr;
            fprintf(lfp,"*** mfn %"_LD_" tag=%u/%d %03d -> blanked\n",
                mfn,tag,xocc,*p);
            p++;
            needup++;
        }
        else
            *batchp++ = *p++;

    *batchp = '\0';

    return(chrsleft);
}
