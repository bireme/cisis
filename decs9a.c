/* ------------------------------ decs9a.c ----------------------------- */

/* ========================================================================
        Link file generation for the MeSH Headings field

        Author: decs8b.c/AOT, Oct 23 1990
======================================================================== */

#include <stdio.h>
#include <string.h>

#include "cisis.h"      /* CISIS Interface */
#include "cirun.h"      /* runtime area and defines by AOT */

#define TRACE   0
#define TRACEUC 0


/*                          DECS                                 */

FILE *fplst;


char *dbnp;
char *uctp,*decs,*l1Sp,*l2Sp,*l1Np,*l2Np;
int parmmyb=0;
int vfd[2][2];
LONGX vc[2][2] = { 0,0,0,0 };
char line[BUFSIZ],key1[BUFSIZ],key2[BUFSIZ];
char key0[BUFSIZ];
char vetflag[BUFSIZ];
#define NOTFLAGGED 0
LONGX countx=0;
LONGX itrmw = MAXNTRM-3;

#if MPE
int vlrecl[2];
char dcb[32];
#endif


/* prototypes */
#if ANSI
int outlink(LONGX mfn, int tag, int occ, int cnt, char *kp, int klen, int sn);
int genlk1(int occ, int tag, RECSTRU *recp, int xdir);
int genlk2(int occ, int tag, RECSTRU *recp, int xdir);
#else
int outlink();
int genlk1();
int genlk2();
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
    RECSTRU *recp;      /* mandatory for defines REC RDB MF0 MFR DIR, FIELDP */
    LONGX irec;          /* indice registro de entrada */
    LONGX mfn;

    int xdir,occ,dirloop;

    LONGX count,xtell;
    int loop,n;
    unsigned char *p;


    if (argc < 7 || argc > 8) {
        printf("%s",cicopyr("Utility MDL/DECS9A"));
        printf("\n");
        printf("decs9a <dbB> <decs> <lkfil1> <lkfil2> <lkfil1> <lkfil2> [+myb] \n");
        printf("\n");
        printf("generate link files for mesh headings/qualifiers \n");
        printf("from mdlmf <dbB>, according to decs <decs> \n");
        printf("\n");
        printf("\n");
        exit(1);
    }

    /* get parms */
    dbnp=argv[1];
    decs=argv[2];
    l1Sp=argv[3];
    l2Sp=argv[4];
    l1Np=argv[5];
    l2Np=argv[6];
    if (argc > 7) if (strcmp(argv[7],"+myb")) fatal(argv[7]); else parmmyb=1;


    /*
        ve entrada
    */
    RECORD((irec=nrecs),dbnp,0L);
    if ((xtell=MF0nxtmfn/10) < 1) xtell=1;



    /* open link files */
#if MPE
    vlrecl[0]=25+LE1;
    vlrecl[1]=25+LE2;
#endif
#if PC || UNIX
    vfd[0][0]=creat(l1Sp,0644);
    vfd[1][0]=creat(l2Sp,0644);
    vfd[0][1]=creat(l1Np,0644);
    vfd[1][1]=creat(l2Np,0644);
#endif
#if MPE
    sprintf(dcb,"R%d Bl%d",vlrecl[0],8192/vlrecl[0]);
    vfd[0][0]=open(l1Sp,O_WRONLY|O_CREAT|O_TRUNC|O_MPEOPTS,0644,dcb);
    sprintf(dcb,"R%d Bl%d",vlrecl[1],8192/vlrecl[1]);
    vfd[1][0]=open(l2Sp,O_WRONLY|O_CREAT|O_TRUNC|O_MPEOPTS,0644,dcb);
    sprintf(dcb,"R%d Bl%d",vlrecl[0],8192/vlrecl[0]);
    vfd[0][1]=open(l1Np,O_WRONLY|O_CREAT|O_TRUNC|O_MPEOPTS,0644,dcb);
    sprintf(dcb,"R%d Bl%d",vlrecl[1],8192/vlrecl[1]);
    vfd[1][1]=open(l2Np,O_WRONLY|O_CREAT|O_TRUNC|O_MPEOPTS,0644,dcb);
#endif
    if (vfd[0][0] == EOF)
        fatal(l1Sp);
    if (vfd[1][0] == EOF)
        fatal(l2Sp);
    if (vfd[0][1] == EOF)
        fatal(l1Np);
    if (vfd[1][1] == EOF)
        fatal(l2Np);

    /* open log */
    if ((fplst=fopen("decs9a.lst","w")) == NULL)
        fatal("decs9a.lst");

    count=0;

    /*
        processing loop
    */

    for (mfn=1; (RECORD(irec,dbnp,mfn))->recrc != RCEOF; mfn++) {

        if (RECrc != RCNORMAL)
            continue;

        for (xdir=0; xdir < MFRnvf; xdir++)
            if (DIRtag(xdir) == 870 || DIRtag(xdir) == 880)
                for (p=FIELDP(xdir), loop=DIRlen(xdir); loop--; )
                    *p++ = isisuctab[*p];

        for (xdir=0; xdir < MFRnvf; xdir++) vetflag[xdir]=NOTFLAGGED;
        for (occ=0, xdir=0, dirloop=MFRnvf; dirloop--; xdir++)
            if (DIRtag(xdir) == 870 || DIRtag(xdir) == 880) {
                if (vetflag[xdir] == NOTFLAGGED)
                    genlk1(occ+1,DIRtag(xdir),recp,xdir);
                occ++;
            }

        for (xdir=0; xdir < MFRnvf; xdir++) vetflag[xdir]=NOTFLAGGED;
        for (occ=0, xdir=0, dirloop=MFRnvf; dirloop--; xdir++)
            if (DIRtag(xdir) == 870 || DIRtag(xdir) == 880) {
                if (vetflag[xdir] == NOTFLAGGED)
                    genlk2(occ+1,DIRtag(xdir),recp,xdir);
                occ++;
            }

        if ((++count % xtell) == 0)
            printf(
           "+++ mfn / recs / links:  %"_LD_" / %"_LD_" / S=%"_LD_"+%"_LD_" N=%"_LD_"+%"_LD_" (%"_LD_") \n",
                mfn,count,vc[0][0],vc[0][1],vc[1][0],vc[1][1],countx);
    }

    printf("+++ recs / links:  %"_LD_"/%"_LD_" / S=%"_LD_"+%"_LD_" N=%"_LD_"+%"_LD_" (%"_LD_") \n",
                mfn,count,vc[0][0],vc[0][1],vc[1][0],vc[1][1],countx);

    exit(0);
}




#if ANSI
int outlink(LONGX mfn, int tag, int occ, int cnt, char *keyp, int keylen,
                                                                        int sn)
#else
int outlink(mfn,tag,occ,cnt,keyp,keylen,sn)
LONGX mfn;
int tag;
int occ;
int cnt;
char *keyp;
int keylen;
int sn;
#endif
{
    unsigned char *p,*q;
    int loop,rc,lrecl;

#if TRACE
    printf("+++ outlink - %8"_LD_"/%5d/%4d/%4d/%d ",mfn,tag,occ,cnt,sn);
    for (q=keyp, loop=keylen; loop--; q++) printf("%c",*q);
    printf("\n");
#endif

    /* setup link record */
    lrecl=0;
    if (keylen > LE2) keylen=LE2;
    rc = (keylen <= LE1) ? 0 : 1;

#if UNIX
/*
1234567890123456 1234567 12345 1234 1234
IS 0262-0898           1   300    1    1
JC DFC                 1   300    1    3
IS 0262-0898           2   300    1    1
*/
    for (p=&line[lrecl], q=keyp, loop=keylen; loop--; ) *p++ = *q++;
    lrecl+=keylen;
    loop=(rc)?LE2-keylen:LE1-keylen;
    memset(p,' ',loop); p+=loop; lrecl+=loop;
    sprintf(&line[lrecl]," %8"_LD_" %5d %4d %4d",mfn,tag,occ,cnt);
    p+=25; lrecl+=25;
    *p='\n';
    lrecl++;
#endif /* UNIX */

#if MPE || PC
#if MPE
    line[lrecl++]='\0';
    sprintf(&line[lrecl],"%8"_LD_" %5d %4d %4d ",mfn,tag,occ,cnt);
    lrecl+=25;
#endif
#if PC
    sprintf(&line[lrecl],"%1"_LD_" %1d %1d %1d ",mfn,tag,occ,cnt);
    lrecl+=strlen(&line[lrecl]);
#endif
    for (p=&line[lrecl], q=keyp, loop=keylen; loop--; ) *p++ = *q++;
    lrecl+=keylen;
#if MPE
    for (loop=vlrecl[rc]-lrecl; loop--; ) {
        *p++ = ' ';
        lrecl++;
    }
    *p='\n';
    lrecl++;
#endif
#endif /* MPE || PC */

    /* write it */
    if (write(vfd[rc][sn],line,lrecl) != lrecl) {
        printf("mfn/tag/occ/cnt: %"_LD_"/%d/%d/%d \n",mfn,tag,occ,cnt);
        fatal((rc)?l2Sp:l1Sp);
    }

    vc[rc][sn]++;

    return(rc);
}




#if ANSI
int genlk1(int occ, int tag, RECSTRU *recp, int xdir)
#else
int genlk1(occ,tag,recp,xdir)
int occ;
int tag;
RECSTRU *recp;
int xdir;
#endif
{
    unsigned char *fldp;
    FFI len,xlen;
    int hn,qn,xloop,xn,cnt,n,sn,cn,chktag,n2,prim870;
    unsigned char *hp,*qp,*xp,*p,*cp;
    TRMSTRU *trmp;


    fldp=FIELDP(xdir); cnt=1;

    len=DIRlen(xdir);
    hp=subfldp(fldp,'h',&len);
    hn=(hp)?subfldn(hp,len):0;

    len=DIRlen(xdir);
    cp=subfldp(fldp,'c',&len);
    cn=(cp)?subfldn(cp,len):0;

    len=DIRlen(xdir);
    qp=subfldp(fldp,'q',&len);
    qn=(qp)?subfldn(qp,len):0;

    if (hn == 0) {
        fprintf(stderr,"mfn/tag: %"_LD_"/%d\n",MFRmfn,tag);
if (parmmyb) return(occ); /* myb */
        fatal("^h missing");
    }
    if (cn != 0 && cn != 1) {
        fprintf(stderr,"mfn/tag: %"_LD_"/%d\n",MFRmfn,tag);
if (parmmyb) return(occ); /* myb - 28.04.92 */
        fatal("^c invalid");
    }
    if (qn != 0 && qn != 2) {
        fprintf(stderr,"mfn/tag: %"_LD_"/%d\n",MFRmfn,tag);
if (parmmyb) return(occ); /* myb - 28.04.92 */
        fatal("^q invalid");
    }

    if (*hp <= ' ') {
        printf("+++ Adjusting key %"_LD_"/%d/%d/%d \n",MFRmfn,tag,occ,cnt);
        while (*hp <= ' ')
            if (hn > 1) {
                hp++; hn--;
            }
            else {
                printf("*** Rejected key -  mfn/tag/occ/cnt: %"_LD_"/%d/%d/%d \n",
                                MFRmfn,tag,occ,cnt);
                return(occ);
           }
    }
    if (qn > 0 && *qp <= ' ') {
        printf("+++ Adjusting sub: %"_LD_"/%d/%d/%d \n",MFRmfn,tag,occ,cnt);
        while (*qp <= ' ')
            if (qn > 1) {
                qp++; qn--;
            }
            else break;
    }


    if (cn == 1 && (*cp == '3' || *cp == '2')) {
        memmove(key1,"CT?",3);
        memmove(key1+3,hp,hn);
        p=key1; n=hn+3;
        chktag=1;
    }
    else {
        p=hp; n=hn;
        chktag=0;
    }
    if (chktag)
        return(occ);            /* NLM checks duplication */

    sn = 0;
    memmove(line,p,n); line[n]='\0';
    if ((TERM(itrmw,decs,line))->trmrc != RCNORMAL) {
        printf("+++ mfn=%"_LD_" tag=%d key=%s -> Not found\n",
                                MFRmfn,DIRtag(xdir),line);
        fprintf(fplst,"mfn=%06"_LD_" tag=%03d key=%s -> Not found\n",
                                MFRmfn,DIRtag(xdir),line);
    }

    if (n > LE2-1-(1+2)) {
        printf("+++ mfn=%"_LD_" tag=%d key=%s -> %d chars\n",
                                MFRmfn,DIRtag(xdir),line,n);
        fprintf(fplst,"mfn=%06"_LD_" tag=%03d key=%s -> %d chars\n",
                                MFRmfn,DIRtag(xdir),line,n);
        n = LE2-1-(1+2);
    }


    outlink(MFRmfn,0,occ,cnt,p,n,sn);
    prim870=0;

    if (DIRtag(xdir) == 870) {
        memmove(key2,".",1); n2=1;
        memmove(key2+1,p,n); n2+=n;
        outlink(MFRmfn,1,occ,cnt,key2,n2,sn);
        prim870=1;
    }
    else
        memmove(key2,p,n2=n);

    memmove(key2+n2,"/",1);
    if (qn)
        memmove(key2+n2+1,qp,qn);
    if (DIRtag(xdir) == 870)
        outlink(MFRmfn,20,occ,cnt,key2+1,n2+1+qn-1,sn);
    outlink(MFRmfn,21,occ,cnt,key2,n2+1+qn,sn);


    for (xloop=xdir+1; xloop < MFRnvf; xloop++) {

        occ++;
        if (DIRtag(xloop) != 870 && DIRtag(xloop) != 880)
            continue;

        if (vetflag[xloop] == NOTFLAGGED) {     /* if not yet flagged */

            fldp=FIELDP(xloop);
            xlen=DIRlen(xloop);
            xp=subfldp(fldp,'h',&xlen);
            xn=(xp)?subfldn(xp,xlen):0;
            if (xn == 0) {
                fprintf(stderr,"mfn/tag: %"_LD_"/%d\n",MFRmfn,tag);
if (parmmyb) return(occ); /* myb */
                fatal("^h missing");
            }

            if (*xp <= ' ') {
                printf("+++ Adjusting key %"_LD_"/%d/%d/%d (compare) \n",
                                                        MFRmfn,tag,occ,cnt);
                while (*xp <= ' ')
                    if (xn > 1) {
                        xp++; xn--;
                    }
                    else break;
            }
            if (xn != hn)
                continue;
            if (memcmp(hp,xp,xn) != 0)
                continue;

            xlen=DIRlen(xloop);
            qp=subfldp(fldp,'q',&xlen);
            qn=(qp)?subfldn(qp,xlen):0;
            if (qn != 0 && qn != 2) {
                fprintf(stderr,"mfn/tag: %"_LD_"/%d\n",MFRmfn,tag);
if (parmmyb) return(occ); /* myb */
                fatal("^q invalid");
            }

            if (qn > 0 && *qp <= ' ') {
                printf("+++ Adjusting sub: %"_LD_"/%d/%d/%d \n",
                                                        MFRmfn,tag,occ,cnt);
                while (*qp <= ' ')
                    if (qn > 1) {
                        qp++; qn--;
                    }
                    else break;
            }

    if (DIRtag(xloop) == 870) {
        memmove(key2,".",1); n2=1;
        memmove(key2+1,p,n); n2+=n;
        if (!prim870) {
            outlink(MFRmfn,1,occ,cnt,key2,n2,sn);
            prim870=1;
        }
    }
    else
        memmove(key2,p,n2=n);

    memmove(key2+n2,"/",1);
    if (qn)
        memmove(key2+n2+1,qp,qn);
    if (DIRtag(xloop) == 870)
        outlink(MFRmfn,20,occ,cnt,key2+1,n2+1+qn-1,sn);
    outlink(MFRmfn,21,occ,cnt,key2,n2+1+qn,sn);


#if 0
            memmove(key2+n2,"/",1);
            if (qn)
                memmove(key2+n2+1,qp,qn);
            outlink(MFRmfn,20,occ,cnt,key2+1,n2+1+qn-1,sn);
            if (DIRtag(xloop) == 870)
                outlink(MFRmfn,21,occ,cnt,key2,n2+1+qn,sn);
#endif
            vetflag[xloop] = !NOTFLAGGED;    /* flag it */

        }
    }

    return(occ);
}


#if ANSI
int genlk2(int occ, int tag, RECSTRU *recp, int xdir)
#else
int genlk2(occ,tag,recp,xdir)
int occ;
int tag;
RECSTRU *recp;
int xdir;
#endif
{
    unsigned char *fldp;
    FFI len,xlen;
    int hn,qn,xloop,xn,cnt,sn,cn,n2,primok;
    unsigned char *hp,*qp,*xp,*cp;


    fldp=FIELDP(xdir); cnt=1;

    len=DIRlen(xdir);
    hp=subfldp(fldp,'h',&len);
    hn=(hp)?subfldn(hp,len):0;

    len=DIRlen(xdir);
    cp=subfldp(fldp,'c',&len);
    cn=(cp)?subfldn(cp,len):0;

    len=DIRlen(xdir);
    qp=subfldp(fldp,'q',&len);
    qn=(qp)?subfldn(qp,len):0;

    if (hn == 0) {
        fprintf(stderr,"mfn/tag: %"_LD_"/%d\n",MFRmfn,tag);
if (parmmyb) return(occ); /* myb */
        fatal("^h missing");
    }
    if (cn != 0 && cn != 1) {
        fprintf(stderr,"mfn/tag: %"_LD_"/%d\n",MFRmfn,tag);
if (parmmyb) return(occ); /* myb */
        fatal("^c invalid");
    }
    if (qn != 0 && qn != 2) {
        fprintf(stderr,"mfn/tag: %"_LD_"/%d\n",MFRmfn,tag);
if (parmmyb) return(occ); /* myb */
        fatal("^q invalid");
    }

    if (*hp <= ' ') {
        printf("+++ Adjusting2 key %"_LD_"/%d/%d/%d \n",MFRmfn,tag,occ,cnt);
        while (*hp <= ' ')
            if (hn > 1) {
                hp++; hn--;
            }
            else {
                printf("*** Rejected key -  mfn/tag/occ/cnt: %"_LD_"/%d/%d/%d \n",
                                MFRmfn,tag,occ,cnt);
                return(occ);
           }
    }
    if (qn > 0 && *qp <= ' ') {
        printf("+++ Adjusting2 sub: %"_LD_"/%d/%d/%d \n",MFRmfn,tag,occ,cnt);
        while (*qp <= ' ')
            if (qn > 1) {
                qp++; qn--;
            }
            else break;
    }


    if (cn == 1 && (*cp == '3' || *cp == '2'))
        return(occ);            /* NLM checks duplication */


    if (qn == 0)
        return(occ);

    sn=1;

    primok=0; n2=0;
    if (DIRtag(xdir) == 870) {
        memmove(key2,".",n2=1);
        primok=1;
    }

    memmove(key2+n2,"/",1);
    if (qn)
        memmove(key2+n2+1,qp,qn);
    if (DIRtag(xdir) == 870)
        outlink(MFRmfn,30,occ,cnt,key2+1,n2+1+qn-1,sn);
    outlink(MFRmfn,31,occ,cnt,key2,n2+1+qn,sn);


    for (xloop=xdir+1; xloop < MFRnvf; xloop++) {

        occ++;
        if (DIRtag(xloop) != 870 && DIRtag(xloop) != 880)
            continue;

        if (vetflag[xloop] == NOTFLAGGED) {     /* if not yet flagged */

            fldp=FIELDP(xloop);

            xlen=DIRlen(xloop);
            xp=subfldp(fldp,'q',&xlen);
            xn=(xp)?subfldn(xp,xlen):0;
            if (xn != 0 && xn != 2) {
                fprintf(stderr,"mfn/tag: %"_LD_"/%d\n",MFRmfn,tag);
if (parmmyb) return(occ); /* myb */
                fatal("^q invalid");
            }

            if (xn > 0 && *xp <= ' ') {
                printf("+++ Adjusting2 sub: %"_LD_"/%d/%d/%d \n",
                                                        MFRmfn,tag,occ,cnt);
                while (*xp <= ' ')
                    if (xn > 1) {
                        xp++; xn--;
                    }
                    else break;
            }
            if (xn != qn)
                continue;
            if (memcmp(qp,xp,xn) != 0)
                continue;


            if (DIRtag(xloop) == 870)
                if (!primok) {
                    memmove(key1,".",1);
                    memmove(key1+1,key2,n2+1+qn);
                    outlink(MFRmfn,31,occ,cnt,key1,n2+1+qn+1,sn);
                    primok=1;
                }

            vetflag[xloop] = !NOTFLAGGED;    /* flag it */

        }
    }

    return(occ);
}

