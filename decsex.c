/* ------------------------------ decsex.c ----------------------------- */

/* ========================================================================
        Link file generation for the MeSH Headings explotion

        Assumes DECSEX having 701/702/703 terms and 720 root
        Assumes DECS inverted as 1 0 mpl,(v720/)

        Author: decs9b.c/AOT, Oct 15 1991 (Prof-day)
======================================================================== */

#include <stdio.h>
#include <string.h>

#define TRACE   0
#define TRACEUC 0
#define TRACEAC 0

#include "cisis.h"      /* CISIS Interface */
#include "cirun.h"      /* runtime area and defines by AOT */


/*                     DECS & DECSEX                                 */

#define TOFFSET 700
#define TAGTING (1+TOFFSET)
#define TAGTESP (2+TOFFSET)
#define TAGTPOR (3+TOFFSET)
#define TAGCATN 720
#define TAGQUAL 14

char wxchr[] = { 'i','e','p' };         /* 1st = Author */

#define wx(tag) (tag-TOFFSET-1)

LONGX totpost;

/*unsigned char isisuctab[256];     agora ja' esta' em cirun.h */

FILE *fplst;
int fdout[3];

char *dexp,*dbnp;
char *uctp,*decs,*l1Ip,*l2Ip,*l1Ep,*l2Ep,*l1Pp,*l2Pp;

int vfd[3][2];
LONGX vc[3][2] = { 0,0,0,0,0,0 };

unsigned char line[BUFSIZ];
unsigned char explode[LE2+LE2];

int explen;
int lreclx;
LONGX count,ecount;

#if MPE
int vlrecl[2];
char dcb0[32],dcb1[32],dcbx[32],mpe='"';
#else
char mpe=' ';
#endif


LONGX dexirec;       /* indice registro decs */
LONGX irec;          /* indice registro decs */
LONGX itrm;          /* indice invertido decs */
LONGX itrmin;        /* indice invertido entrada */

int parmtrace=1;

#define PARMPFXL        1
#define PARMSFXL        3
#define PARMSFXC        '/'

#define PARMNPFX        1
char parmvpfx[PARMNPFX][LE2+1] = { "." };

#if PC && !DOS32BITS
#define OPEN30 1
#endif
#if OPEN30
union REGS      d33_regs;
struct SREGS    d33_sregs;
unsigned short  d33_newax,d33_n;
#endif


/* prototypes */
#if ANSI
void keepinfo(LONGX mfn, int tag, char pfx[], int plen, char sfx[], int slen);
int outlink(LONGX mfn, int tag, int occ, int cnt, char *kp, int klen, int sn);
int catlink(char *lnkp,char *pfx,int pl,char *midp,int ml,char *sfx,int sl);
LONGX copylnks(char pfxcall[]);
#else
void keepinfo();
int outlink();
int catlink();
LONGX copylnks();
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
    RECSTRU *recp;      /* RECORD */
    TRMSTRU *trmp;      /* TERM/NXTERM */
    LONGX mfn;

    int occ,ncats,ipfx;

    FILE *fp;

    LONGX xtell;
    int loop,n,expcatlen,found;
    unsigned char *p,expcat[LE2+1];


#if OPEN30
        d33_n=32;
        d33_regs.h.ah = 0x67;   /* Set handle count */
        d33_regs.x.bx = d33_n;  /* no of handles */
        d33_newax=intdosx(&d33_regs,&d33_regs,&d33_sregs);
        if (d33_regs.x.cflag)
            fprintf(stderr,"AX = %04x [%02d]\n",d33_newax,d33_n);
#endif

    if (argc < 10) {
        printf("%s",cicopyr("Utility MDL/DECSEX"));
        printf("\n");
        printf("decsex <decsex> <dbB> <decs> <I1> <I2> <E1> <E2> <P1> <P2> \n");
        printf("\n");
        printf("generate exploded I/E/P link files for authorized english mesh headings \n");
        printf("from mdlif <dbB>, according to decs <decs> \n");
        printf("\n");
        printf("\n");
        exit(1);
    }

    /* get parms */
    dexp=argv[1];
    dbnp=argv[2];
    decs=argv[3];
    l1Ip=argv[4];
    l2Ip=argv[5];
    l1Ep=argv[6];
    l2Ep=argv[7];
    l1Pp=argv[8];
    l2Pp=argv[9];

    /* ve master decsex */
    RECORD((dexirec=nrecs),dexp,0L);

    /* ve invertido de entrada */
    TERM((itrmin=ntrms),dbnp,"!");

    xtell=MF0nxtmfn;
    xtell/=10;
    if (xtell < 1)
        xtell=1;


    /* ve master decs */
    RECORD((irec=nrecs),decs,0L);

    /* ve invertido decs */
    TERM((itrm=ntrms),decs,"!");

    /* open link files */
#if PC || UNIX
    vfd[0][0]=creat(l1Ip,0644);
    vfd[0][1]=creat(l2Ip,0644);
    vfd[1][0]=creat(l1Ep,0644);
    vfd[1][1]=creat(l2Ep,0644);
    vfd[2][0]=creat(l1Pp,0644);
    vfd[2][1]=creat(l2Pp,0644);
#endif
#if MPE
    vlrecl[0]=25+LE1;
    vlrecl[1]=25+LE2;
    sprintf(dcb0,"R%d Bl%d",vlrecl[0],8192/vlrecl[0]);
    sprintf(dcb1,"R%d Bl%d",vlrecl[1],8192/vlrecl[1]);
    vfd[0][0]=open(l1Ip,O_WRONLY|O_CREAT|O_TRUNC|O_MPEOPTS,0644,dcb0);
    vfd[0][1]=open(l2Ip,O_WRONLY|O_CREAT|O_TRUNC|O_MPEOPTS,0644,dcb1);
    vfd[1][0]=open(l1Ep,O_WRONLY|O_CREAT|O_TRUNC|O_MPEOPTS,0644,dcb0);
    vfd[1][1]=open(l2Ep,O_WRONLY|O_CREAT|O_TRUNC|O_MPEOPTS,0644,dcb1);
    vfd[2][0]=open(l1Pp,O_WRONLY|O_CREAT|O_TRUNC|O_MPEOPTS,0644,dcb0);
    vfd[2][1]=open(l2Pp,O_WRONLY|O_CREAT|O_TRUNC|O_MPEOPTS,0644,dcb1);
#endif
    if (vfd[0][0] == EOF) fatal(l1Ip);
    if (vfd[0][1] == EOF) fatal(l2Ip);
    if (vfd[1][0] == EOF) fatal(l1Ep);
    if (vfd[1][1] == EOF) fatal(l2Ep);
    if (vfd[2][0] == EOF) fatal(l1Pp);
    if (vfd[2][1] == EOF) fatal(l2Pp);

    /* open lst and output files */
    if ((fplst   =fopen("decsex.lst", "w")) == NULL) fatal("decsex.lst");

    lreclx = 6+1 + LE2+1 + 1+1+1+1 + LE2+1 + 2+1+3+1 + 2+1+2+1+1+1+1+1 + 9+1;

#if PC || UNIX
    if ((fdout[0]=creat("decsexI.lst",0644)) == EOF) fatal("decsexI.lst");
    if ((fdout[1]=creat("decsexE.lst",0644)) == EOF) fatal("decsexE.lst");
    if ((fdout[2]=creat("decsexP.lst",0644)) == EOF) fatal("decsexP.lst");
#endif
#if MPE
    sprintf(dcbx,"R%d Bl%d",lreclx,8192/lreclx);
    fdout[0]=open("decsexI.lst",O_WRONLY|O_CREAT|O_TRUNC|O_MPEOPTS,0644,dcbx);
    fdout[1]=open("decsexE.lst",O_WRONLY|O_CREAT|O_TRUNC|O_MPEOPTS,0644,dcbx);
    fdout[2]=open("decsexP.lst",O_WRONLY|O_CREAT|O_TRUNC|O_MPEOPTS,0644,dcbx);
#endif
    if (fdout[0] == EOF) fatal("decsexI.lst");
    if (fdout[1] == EOF) fatal("decsexE.lst");
    if (fdout[2] == EOF) fatal("decsexP.lst");


    ecount=count=0;

    /*
        processing loop
    */

    RECORD(dexirec,dexp,mfn=1L);
    for(; RECrc != RCEOF; RECORD(dexirec,dexp,++mfn)) {

        if ((++count % xtell) == 0)
            fprintf(stderr,"+++ %"_LD_" \n",count);

        if (RECrc != RCNORMAL) continue;

        ncats=nocc(dexirec,TAGCATN);
        if (ncats <= 0) fatal("CAT not found");
        if (nocc(dexirec,TAGTING) != 1) fatal("ING invalid");
        if (nocc(dexirec,TAGTESP) != 1) fatal("ESP invalid");
        if (nocc(dexirec,TAGTPOR) != 1) fatal("POR invalid");

        recfield(explode,dexirec,TAGTING,1,NULL);
        explen=strlen(explode);
        for (p=explode; *p; p++) *p = isisuctab[*p];

        for (occ=1; occ <= ncats; occ++) {

            recfield(expcat,dexirec,TAGCATN,occ,NULL);
            expcatlen=strlen(expcat);
            for (p=expcat; *p; p++) *p = isisuctab[*p];

            TERM(itrm,decs,expcat); found=0;
            while (memcmp(TRMkey,expcat,expcatlen) == 0) {
                if (TRMxytotp != 1) {
                    printf("+++ %"_LD_" %s -> %"_LD_" postings !!\n",
                                        count,TRMkey,TRMxytotp);
                    fprintf(fplst,
                           "+++ %"_LD_" %s -> %"_LD_" postings !!\n",
                                        count,TRMkey,TRMxytotp);
                    fatal("aborted");
                }

                RECORD(irec,decs,posting(itrm,1L));
                if (RECrc != RCNORMAL) fatal("DECS/RECrc");

                copylnks("");
                for (ipfx=0; ipfx < PARMNPFX; ipfx++)
                    copylnks(parmvpfx[ipfx]);

                found++;
                NXTERM(itrm);
            }

            if (!found) {
                printf(       "+++ %"_LD_" %s -> Not found\n",count,expcat);
                fprintf(fplst,"+++ %"_LD_" %s -> Not found\n",count,expcat);
                fatal("aborted");
            }

        } /* end occ */

    } /* end main loop */

    printf(
      "+++ terms=%"_LD_" err=%"_LD_" I=%"_LD_"+%"_LD_" E=%"_LD_"+%"_LD_" P=%"_LD_"+%"_LD_" \n",
        count,ecount,
        vc[0][0],vc[0][1],vc[1][0],vc[1][1],vc[2][0],vc[2][1]);

    fprintf(fplst,
      "+++ terms=%"_LD_" err=%"_LD_" I=%"_LD_"+%"_LD_" E=%"_LD_"+%"_LD_" P=%"_LD_"+%"_LD_" \n",
        count,ecount,
        vc[0][0],vc[0][1],vc[1][0],vc[1][1],vc[2][0],vc[2][1]);

    exit(0);
}



#if ANSI
LONGX copylnks(char pfxcall[])
#else
LONGX copylnks(pfxcall)
char pfxcall[];
#endif
{
    int xdir,len,hn,n,x;
    LONGX nok;
    unsigned char *hp,*p;
    RECSTRU *recp;
    TRMSTRU *trmp;

    int pfxlen,n2,sfxlen;
    unsigned char inpkey[LE2+LE2],linkkey[LE2+LE2],sfxkey[LE2+LE2];

#if TRACE
    int tracn;
    unsigned char *tracp;
#endif

    char areaqq[LE2+1];


    if (nocc(dexirec,TAGTING) != 1) fatal("DECSEX/TAGTING");
    if (nocc(dexirec,TAGTESP) != 1) fatal("DECSEX/TAGTESP");
    if (nocc(dexirec,TAGTPOR) != 1) fatal("DECSEX/TAGTPOR");

    if (nocc(irec,TAGTING) != 1) fatal("DECS/TAGTING");

    recp=vrecp[irec];

    if (nocc(irec,TAGQUAL) == 1) {
        areaqq[0]='/';
        recfield(areaqq+1,irec,TAGQUAL,1,NULL);
        hp=areaqq;
        hn=strlen(hp);
        if (hn != 3) fatal("DECSEX/TAGQUAL");
    }
    else {
        xdir=fieldx(irec,TAGTING,1);
        hp=FIELDP(xdir);
        hn=DIRlen(xdir);
    }

    if (*hp <= ' ') {
        printf("+++ Adjusting key %"_LD_" \n",MFRmfn);
#if TRACE
for (tracp=hp, tracn=hn; tracn--; tracp++)
 printf("%c(%02x)",*tracp,*tracp);
getchar();
#endif
        while (*hp <= ' ')
            if (hn > 1) {
                hp++; hn--;
            }
            else {
                printf("*** Rejected key -  mfn: %"_LD_" \n",MFRmfn);
                fatal("aborted");
           }
    }

    pfxlen=0;
    if (*pfxcall) {
        strcpy(inpkey,pfxcall);
        pfxlen=strlen(pfxcall);
    }
    memmove(inpkey+pfxlen,hp,hn);
    inpkey[pfxlen+hn]='\0';
    for (p=inpkey; *p; p++) *p = isisuctab[*p];

    nok=0;
    for (TERM(itrmin,dbnp,inpkey); TRMrc != RCEOF; NXTERM(itrmin)) {

        if (memcmp(TRMkey,inpkey,pfxlen+hn)) break;

        totpost=TRMxytotp;

        len=n2=(TRMlcase)?LE2:LE1;

        if (pfxlen) n2-=pfxlen;

        for (p=TRMkey+len; n2; ) if (*--p == ' ') n2--; else break;

        sfxkey[0]='\0'; sfxlen=0;
        for (n=n2; n > 1; n--, p--) {
            if (*p == PARMSFXC) {
                sfxlen = n2 - n + 1;
                memmove(sfxkey,p,sfxlen);
                sfxkey[sfxlen]='\0';
                n2 = n - 1;
                break;
            }
        }

        if (n2 != hn) continue;
        nok++;

        if (parmtrace == 1) {
            printf("+++ %s%s = %s",pfxcall,explode,pfxcall);
            for (p=TRMkey+pfxlen, n=n2; n--; p++) printf("%c",*p);
            printf("%s psts=%"_LD_" [%"_LD_"]\n",sfxkey,totpost,nok);
        }

        if (parmtrace == 2) {
            printf("+++ pfx=%d=%s trm=%d=",pfxlen,pfxcall,n2);
            for (p=TRMkey+pfxlen, n=n2; n--; p++) printf("%c",*p);
            printf(" sfx=%d=%s psts=%"_LD_" [%"_LD_"]\n",sfxlen,sfxkey,totpost,nok);
        }

        n=0;
        if (memcmp(TRMkey+pfxlen,hp,hn)) n++;

        if (n && parmtrace != 2) {
            printf("+++ pfx=%d=%s trm=%d=",pfxlen,pfxcall,n2);
            for (p=TRMkey+pfxlen, n=n2; n--; p++) printf("%c",*p);
            printf(" sfx=%d=%s psts=%"_LD_" [%"_LD_"]\n",sfxlen,sfxkey,totpost,nok);
        }
        if (n) {
            printf("+++ key=%d=",hn);
            for (; hn--; hp++) printf("%c",*hp);
            printf(" cat=%d=%s \n",explen,explode);
            fatal("copylnks/bug1");
        }

        len=catlink(linkkey,pfxcall,pfxlen,explode,explen,sfxkey,sfxlen);

        while (posting(itrmin,TRMpost+1) > 0)
            outlink(TRMpmfn,TRMptag,TRMpocc,TRMpcnt,linkkey,len,wx(TAGTING));

        recp=vrecp[dexirec];

        x=fieldx(dexirec,TAGTESP,1);
        len=catlink(linkkey,pfxcall,pfxlen,FIELDP(x),DIRlen(x),sfxkey,sfxlen);
        for (p=linkkey; *p; p++) *p = isisuctab[*p];
        outlink(VMFRmfn(dexirec),TAGTESP,1,0,linkkey,len,wx(TAGTESP));

        x=fieldx(dexirec,TAGTPOR,1);
        len=catlink(linkkey,pfxcall,pfxlen,FIELDP(x),DIRlen(x),sfxkey,sfxlen);
        for (p=linkkey; *p; p++) *p = isisuctab[*p];
        outlink(VMFRmfn(dexirec),TAGTPOR,1,0,linkkey,len,wx(TAGTPOR));

        keepinfo(VMFRmfn(dexirec),TAGTESP,pfxcall,pfxlen,sfxkey,sfxlen);
        keepinfo(VMFRmfn(dexirec),TAGTPOR,pfxcall,pfxlen,sfxkey,sfxlen);

    } /* end NXTERM */


    if (!nok) {
        /* fatal("copylnks/bug2"); */
        printf(       "+++ %s%s = %s -> Not used\n",pfxcall,explode,inpkey);
        fprintf(fplst,"+++ %s%s = %s -> Not used\n",pfxcall,explode,inpkey);
        ecount++;
    }

    return(nok);

}



#if ANSI
int catlink(char *lnkp,char *pfx,int pl,char *midp,int ml,char *sfx,int sl)
#else
int catlink(lnkp,pfx,pl,midp,ml,sfx,sl)
char *lnkp;
char *pfx;
int pl;
char *midp;
int ml;
char *sfx;
int sl;
#endif
{
    char *p;
    int len;

    p=lnkp;
    *p='\0';

    if (pl) memmove(p,pfx,pl);
    len=pl;

    memmove(p+len,midp,ml);
    len+=ml;

    if (sl) memmove(p+len,sfx,sl);
    len+=sl;

    lnkp[len]='\0';

    return(len);
}




#if ANSI
void keepinfo(LONGX mfn, int tag, char pfx[], int plen, char sfx[], int slen)
#else
void keepinfo(mfn,tag,pfx,plen,sfx,slen)
LONGX mfn;
int tag;
char pfx[];
int plen;
char sfx[];
int slen;
#endif
{
    unsigned char *linp,*p;
    int hn,loop;

    linp=line;
    sprintf(linp,"%06"_LD_"|",mfn); linp+=strlen(linp);

    if (PARMPFXL+explen+PARMSFXL > LE2) fatal(explode);
    strcpy(linp,explode); linp+=explen;
    for (loop=LE2-explen; loop--; ) *linp++ = ' ';
    *linp++ = '|';

    sprintf(linp,"%c|a|",isisuctab[wxchr[wx(tag)]]);
    linp+=strlen(linp);

    recfield(linp,dexirec,tag,1,NULL);
    for (hn=0, p=linp; *p; p++) {
        *p = isisuctab[*p]; hn++;
    }
    if (PARMPFXL+hn+PARMSFXL > LE2) fatal(linp);
    linp+=hn;
    for (loop=LE2-hn; loop--; ) *linp++ = ' ';
    *linp++ = '|';

    if (plen > PARMPFXL) fatal("keepinfo/pfx/length");
    memmove(linp,pfx,plen); linp+=plen;
    for (loop=PARMPFXL-plen+1; loop--; ) *linp++ = ' '; /* one for ifp1 */
    *linp++ = '|';

    if (slen > PARMSFXL) fatal("keepinfo/sfx/length");
    memmove(linp,sfx,slen); linp+=slen;
    for (loop=PARMSFXL-slen; loop--; ) *linp++ = ' ';
    *linp++ = '|';

    sprintf(linp,"%2d|%2d|%d|%d|%9"_LD_"\n",explen,hn,plen,slen,totpost);
#if TRACE
*linp='\0';
printf("strlen=%d lreclx=%d\n%s.",strlen(line),lreclx,line);
#endif
    if (CIWRITE(fdout[wx(tag)],line,lreclx) != lreclx)
        fatal("copylnks/write");
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
    if (CIWRITE(vfd[sn][rc],line,lrecl) != lrecl) {
        printf("mfn/tag/occ/cnt: %"_LD_"/%d/%d/%d \n",mfn,tag,occ,cnt);
        fatal("outlink/write");
    }

    vc[sn][rc]++;

    return(rc);
}
