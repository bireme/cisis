/* ------------------------------ decs9b.c ----------------------------- */

/* ========================================================================
        Link file generation for the MeSH Headings field - STEP b

        Author: decs9a.c/AOT, Oct 31 1990 (Halloween)

	Alter:  AOT/RSP, 07/03/94
		1. Parametro -bug2 (para LILACS !!!)
======================================================================== */

#include <stdio.h>
#include <string.h>

#include "cisis.h"      /* CISIS Interface */
#include "cirun.h"      /* runtime area and defines by AOT */

#define TRACE   0
#define TRACEUC 0
#define TRACEAC 0

#if PC && !DOS32BITS
#define OPEN30 1
#endif
#if OPEN30
#include <dos.h>
#endif


/*                          DECS                                 */

#define TOFFSET 700
#define TAGTING (1+TOFFSET)
#define TAGTESP (2+TOFFSET)
#define TAGTPOR (3+TOFFSET)
#define TAGPROI (50+TOFFSET)
#define TAGCATN 720

#define TAGQUAL 14                      /* CHECK /xx */

char delim[] = { 'i','e','p' };         /* 1st = Author */

#define wx(tag) ((tag != TAGCATN) ? (tag-TOFFSET-1) : 3)

char wxchr[] = { 'i','e','p','c' };

LONGX totpost;
int totpmfnok[sizeof(wxchr)];

int optbug2=0;

FILE *fplst;
int fdout[4];

char *dbnp;
char *decs,*l1Ip,*l2Ip,*l1Ep,*l2Ep,*l1Pp,*l2Pp,*l1Cp,*l2Cp;

int vfd[4][2];
LONGX vc[4][2] = { 0,0,0,0,0,0,0,0 };

#if OUTONISIS
LONGX crecout;       /* indice registro controle saida */
LONGX irecout;       /* indice registro saida */
#endif

unsigned char buffup[MAXMFRL+MAXMFRL];
unsigned char line[BUFSIZ],meshkey[BUFSIZ],key2[BUFSIZ],keynew[BUFSIZ];

unsigned char *keyinp;
unsigned char prefkey[BUFSIZ],qualkey[BUFSIZ];
int n1,n2,n3,qualcase,prefcase,qualx;
int lreclx;

#if MPE
int vlrecl[2];
char dcb0[32],dcb1[32],dcbx[32],mpe='"';
#else
char mpe=' ';
#endif

#if OPEN30
union REGS      d33_regs;
struct SREGS    d33_sregs;
unsigned short  d33_newax,d33_n;
#endif


/* prototypes */
#if ANSI
int outlink(LONGX mfn, int tag, int occ, int cnt, char *kp, int klen, int sn);
int genlk1(int occ, int tag, RECSTRU *recp, int xdir, char dlm, int sn);
#else
int outlink();
int genlk1();
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
    LONGX irec;          /* indice registro decs */
    LONGX itrm;          /* indice invertido decs */
    LONGX itrmin;        /* indice invertido entrada */
    LONGX mfn;

    int xdir,occ,tag;

    FILE *fp;

    LONGX ecount,count,xtell;
    int loop,n,len;
    unsigned char *p;
    unsigned char *meshp,*startp;


#if OPEN30
        d33_n=32;
        d33_regs.h.ah = 0x67;   /* Set handle count */
        d33_regs.x.bx = d33_n;  /* no of handles */
        d33_newax=intdosx(&d33_regs,&d33_regs,&d33_sregs);
        if (d33_regs.x.cflag)
            fprintf(stderr,"AX = %04x [%02d]\n",d33_newax,d33_n);
#endif

    if (argc < 11 || argc > 12) {
        printf("%s",cicopyr("Utility MDL/DECS9B"));
        printf("\n");
        printf("decs9b <dbB> <decs> <I1> <I2> <E1> <E2> <P1> <P2> <C1> <C2> [key1/-bug2]\n");
        printf("\n");
        printf("generate I/E/P link files for authorized english mesh headings \n");
        printf("from mdlmf <dbB>, according to decs <decs> \n");
        printf("\n");
        printf("\n");
        exit(1);
    }

    /* get parms */
    dbnp=argv[1];
    decs=argv[2];
    l1Ip=argv[3];
    l2Ip=argv[4];
    l1Ep=argv[5];
    l2Ep=argv[6];
    l1Pp=argv[7];
    l2Pp=argv[8];
    l1Cp=argv[9];
    l2Cp=argv[10];
    startp="!";
    if (argc > 11) {
        if (strcmp(argv[11],"-bug2") == 0) optbug2=1;
	else startp=argv[11];
    }


    /* ve invertido de entrada */
    TERM((itrmin=ntrms),dbnp,startp);

    xtell=(TDBifmap->cn[0].fmaxpos+TDBifmap->cn[1].fmaxpos)*TWORDF;
    xtell/=10;
    if (xtell < 1)
        xtell=1;
    if (*startp != '!')
        xtell=1;



    /* ve master e invertido decs */
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
    vfd[3][0]=creat(l1Cp,0644);
    vfd[3][1]=creat(l2Cp,0644);
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
    vfd[3][0]=open(l1Cp,O_WRONLY|O_CREAT|O_TRUNC|O_MPEOPTS,0644,dcb0);
    vfd[3][1]=open(l2Cp,O_WRONLY|O_CREAT|O_TRUNC|O_MPEOPTS,0644,dcb1);
#endif
    if (vfd[0][0] == EOF) fatal(l1Ip);
    if (vfd[0][1] == EOF) fatal(l2Ip);
    if (vfd[1][0] == EOF) fatal(l1Ep);
    if (vfd[1][1] == EOF) fatal(l2Ep);
    if (vfd[2][0] == EOF) fatal(l1Pp);
    if (vfd[2][1] == EOF) fatal(l2Pp);
    if (vfd[3][0] == EOF) fatal(l1Cp);
    if (vfd[3][1] == EOF) fatal(l2Cp);

    /* open lst and output files */
    if ((fplst   =fopen("decs9b.lst", "w")) == NULL) fatal("decs9b.lst");

    lreclx = 6+1 + LE2+1 + 1+1+1+1 + LE2+1 + 2+1+3+1 + 2+1+2+1+1+1+1+1 + 9+1;

#if PC || UNIX
    if ((fdout[0]=creat("decs9bI.lst",0644)) == EOF) fatal("decs9bI.lst");
    if ((fdout[1]=creat("decs9bE.lst",0644)) == EOF) fatal("decs9bE.lst");
    if ((fdout[2]=creat("decs9bP.lst",0644)) == EOF) fatal("decs9bP.lst");
    if ((fdout[3]=creat("decs9bC.lst",0644)) == EOF) fatal("decs9bC.lst");
#endif
#if MPE
    sprintf(dcbx,"R%d Bl%d",lreclx,8192/lreclx);
    fdout[0]=open("decs9bI.lst",O_WRONLY|O_CREAT|O_TRUNC|O_MPEOPTS,0644,dcbx);
    fdout[1]=open("decs9bE.lst",O_WRONLY|O_CREAT|O_TRUNC|O_MPEOPTS,0644,dcbx);
    fdout[2]=open("decs9bP.lst",O_WRONLY|O_CREAT|O_TRUNC|O_MPEOPTS,0644,dcbx);
    fdout[3]=open("decs9bC.lst",O_WRONLY|O_CREAT|O_TRUNC|O_MPEOPTS,0644,dcbx);
#endif
    if (fdout[0] == EOF) fatal("decs9bI.lst");
    if (fdout[1] == EOF) fatal("decs9bE.lst");
    if (fdout[2] == EOF) fatal("decs9bP.lst");
    if (fdout[3] == EOF) fatal("decs9bC.lst");

#if OUTONISIS
    /* create decs9b */
    recisis0("decs9b");
    record((crecout=nrecs),"decs9b",0L);
    record((irecout=nrecs),"decs9b",1L);
#endif

    ecount=count=0;

    /*
        processing loop
    */


    keyinp=VTRMkey(itrmin);
    if (*startp != '!')
        printf("term . = %s\n",keyinp);

    for (TERM(itrmin,dbnp,keyinp); TRMrc != RCEOF; NXTERM(itrmin)) {

        totpost=TRMxytotp;
        memset(totpmfnok,0x00,sizeof(totpmfnok));


        keyinp=VTRMkey(itrmin);
        if (*startp != '!')
            printf("term %"_LD_" = %s\n",count,keyinp);

        meshp=keyinp; n1=n3=0; len=n2=(TRMlcase)?LE2:LE1;

        prefcase=0;
        if (*meshp == '.') {
            prefkey[n1++]= *meshp; meshp++; n2--; prefcase+=1;
        }
#if STRIP1STSLASH
        if (*meshp == '/') {
            prefkey[n1++]= *meshp; meshp++; n2--; prefcase+=2;
        }
#endif
        prefkey[n1]='\0';

        for (p=keyinp+len; n2; ) if (*--p == ' ') n2--; else break;

        qualkey[0]='\0'; qualcase=0;
        if (n2 <= 2) {     /* tem so' ID no mfn 7242 */ /* => /xx */
            if (!prefcase)
                if (*meshp != '/')
                    if (n2 <= 1) {                      /* => ... */
                        printf(       "+++ %"_LD_" %s -> Bug1\n",count,keyinp);
                        fprintf(fplst,"+++ %"_LD_" %s -> Bug1\n",count,keyinp);
                        fatal(keyinp);
                    }
        }
        else
            if (*p == '/') {                            /* => .../ */
                qualcase=1; memmove(qualkey,p,n3=1); qualkey[n3]='\0';
                n2-=n3;
            }
            else if (n2 >= 5 && *(p-2) == '/') {        /* => .../xx */
                qualcase=2; memmove(qualkey,p-2,n3=3); qualkey[n3]='\0';
                n2-=n3;
            }


        memmove(meshkey,meshp,n2); meshkey[n2]='\0';
        TERM(itrm,decs,meshkey);

        if ((++count % xtell) == 0)
            printf("+++ %"_LD_" %s%s%s %d,%d,%d [%d,%d]\n",
                count,prefkey,meshkey,qualkey,n1,n2,n3,prefcase,qualcase);

        if (TRMrc == RCNORMAL && TRMxytotp != 1) {
            printf("+++ %"_LD_" %s -> %"_LD_" postings !!\n",
                                        count,keyinp,TRMxytotp);
            fprintf(fplst,
                   "+++ %"_LD_" %s -> %"_LD_" postings !!\n",
                                        count,keyinp,TRMxytotp);
            continue ; /* fatal("aborted"); */
        }

        if (TRMrc == RCNORMAL)
            mfn=posting(itrm,1L);
        else {
            mfn=999999L;
            printf(       "+++ %"_LD_" %s -> Not found\n",count,keyinp);
            fprintf(fplst,"+++ %"_LD_" %s -> Not found\n",count,keyinp);
            continue;    /*myb - 11.05.93 */
        }

        RECORD(irec,decs,mfn);

        if (RECrc != RCNORMAL) {
            printf(       "+++ %"_LD_" %s -> Not translated\n",count,keyinp);
            fprintf(fplst,"+++ %"_LD_" %s -> Not translated\n",count,keyinp);
            ecount++;
            sprintf(buffup,"D*H%d %d %s",TAGTING,n2,meshkey);
            if ((p=fldupdat(irec,buffup)) != NULL) {
                printf("+++ %"_LD_" key=%s mfn=%"_LD_" \nfldupdat=%s \n",
                                                count,keyinp,mfn,buffup);
                fprintf(fplst,
                       "+++ %"_LD_" key=%s mfn=%"_LD_" \nfldupdat=%s \n",
                                                count,keyinp,mfn,buffup);
                fatal(p);
            }
        }

        if (*meshkey == '/') {                          /* duplicate /xx */
            qualx=0;
            if (n2 != 3) qualx+=1;
            else if ((xdir=fieldx(irec,TAGQUAL,1)) < 0) qualx+=2;
            else if (DIRlen(xdir) != 2) qualx+=4;       /* only xx */
            if (qualx) {
                printf(       "+++ %"_LD_" %s %d -> Bug2\n",count,keyinp,qualx);
                fprintf(fplst,"+++ %"_LD_" %s %d -> Bug2\n",count,keyinp,qualx);
                if (optbug2) continue; else fatal(keyinp);
            }
            sprintf(buffup,"H%d 3 %sH%d 3 %sH%d 3 %s",
                TAGTING,meshkey,TAGTESP,meshkey,TAGTPOR,meshkey); /* do it */
            if ((p=fldupdat(irec,buffup)) != NULL) {
                printf("+++ %"_LD_" key=%s mfn=%"_LD_" \nfldupdat=%s \n",
                                                count,keyinp,mfn,buffup);
                fprintf(fplst,
                       "+++ %"_LD_" key=%s mfn=%"_LD_" \nfldupdat=%s \n",
                                                count,keyinp,mfn,buffup);
                fatal(p);
            }
        }

        for (occ=1, xdir=0; xdir < MFRnvf; xdir++) {
            tag=DIRtag(xdir);
            if (tag == TAGTING || tag == TAGTESP || tag == TAGTPOR ||
                                         tag == TAGCATN || tag == TAGPROI) {
                for (p=FIELDP(xdir), loop=DIRlen(xdir); loop--; )
                    *p++ = isisuctab[*p];
                if (tag == TAGPROI) {
                    occ+=genlk1(occ,tag,recp,xdir,delim[0],0);
                    occ+=genlk1(occ,tag,recp,xdir,delim[1],1);
                    occ+=genlk1(occ,tag,recp,xdir,delim[2],2);
                }
                else {
                    occ+=genlk1(occ,tag,recp,xdir,'\0',    wx(tag));

                }
            }
        }

    } /* end for each term */


    printf(
      "+++ terms=%"_LD_" err=%"_LD_" I=%"_LD_"+%"_LD_" E=%"_LD_"+%"_LD_" P=%"_LD_"+%"_LD_" C=%"_LD_"+%"_LD_"\n",
        count,ecount,
        vc[0][0],vc[0][1],vc[1][0],vc[1][1],vc[2][0],vc[2][1],
        vc[3][0],vc[3][1]);

    fprintf(fplst,
      "+++ terms=%"_LD_" err=%"_LD_" I=%"_LD_"+%"_LD_" E=%"_LD_"+%"_LD_" P=%"_LD_"+%"_LD_" C=%"_LD_"+%"_LD_"\n",
        count,ecount,
        vc[0][0],vc[0][1],vc[1][0],vc[1][1],vc[2][0],vc[2][1],
        vc[3][0],vc[3][1]);

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
    if (CIWRITE(vfd[sn][rc],line,lrecl) != lrecl) {
        printf("mfn/tag/occ/cnt: %"_LD_"/%d/%d/%d \n",mfn,tag,occ,cnt);
        fatal("outlink/write");
    }

    vc[sn][rc]++;

    return(rc);
}




#if ANSI
int genlk1(int occ, int tag, RECSTRU *recp, int xdir, char dlm, int sn)
#else
int genlk1(occ,tag,recp,xdir,dlm,sn)
int occ;
int tag;
RECSTRU *recp;
int xdir;
char dlm;
int sn;
#endif
{
    unsigned char *fldp;
    FFI hn,qn;
    int len,cnt,loop;
    unsigned char *hp,*qp,c,*batchp;
    LONGX totpostx;

#if TRACE
    int tracn;
    unsigned char *tracp;
#endif

    hp=fldp=FIELDP(xdir); cnt=1;
    hn=len=DIRlen(xdir);

    if (dlm) {
        hp=subfldp(fldp,dlm,&hn);
        if (hp == NULL)
            return(0);
        hn=(hp)?subfldn(hp,hn):0;

        /* eliminate homeo (^qp) and repidisca (^qr) */
        qp=fldp; qn=len;
        qp=subfldp(fldp,'q',&qn);
        if (qp != NULL) {
            qn=(qp)?subfldn(qp,qn):0;
            if (qn != 0 && qn != 1) {
                printf("*** mfn/tag: %"_LD_"/%d\n",MFRmfn,tag);
                fatal("^q invalid");
            }
            if (qn) {
                c=isisuctab[*qp];
                if (c != 'P' && c != 'R' && c != 'H' && c != 'V' && c != 'S') {
                    printf("*** mfn/tag: %"_LD_"/%d\n",MFRmfn,tag);
                    fatal("^q not P/R/H/V/S");
                }
                if (c == 'P' || c == 'R')
                    return(0);                              /* eliminate */
            }
        }
    }

    if (*hp <= ' ') {
        printf("+++ Adjusting key %"_LD_"/%d/%d/%d \n",MFRmfn,tag,occ,cnt);
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
                printf("*** Rejected key -  mfn/tag/occ/cnt: %"_LD_"/%d/%d/%d \n",
                                MFRmfn,tag,occ,cnt);
                return(0);
           }
    }


    if (prefcase)
        memmove(keynew,prefkey,n1);

    memmove(keynew+n1,hp,hn);

    if (qualcase)
        memmove(keynew+n1+hn,qualkey,n3);

    outlink(MFRmfn,1,occ,cnt,keynew,n1+hn+n3,sn);

    /* keep info */
    batchp=buffup;
    sprintf(batchp,"%06"_LD_"|",MFRmfn); batchp+=strlen(batchp);

    if (n2 >= LE2) {
        memmove(batchp,meshkey,LE2); batchp+=LE2;
    }
    else {
        memmove(batchp,meshkey,n2); batchp+=n2;
        for (loop=LE2-n2; loop--; ) *batchp++ = ' ';
    }
    *batchp++ = '|';

    if (dlm) {
        sprintf(batchp,"%c|x|",isisuctab[dlm]);
        totpostx=0; /* mfn count is ok */
    }
    else {
        sprintf(batchp,"%c|a|",isisuctab[wxchr[wx(tag)]]);
        if (totpmfnok[wx(tag)])
            totpostx=0;
        else {
            totpostx=totpost;
            totpmfnok[wx(tag)]=1;
        }
    }
    batchp+=strlen(batchp);

    if (hn >= LE2) {
        memmove(batchp,hp,LE2); batchp+=LE2;
    }
    else {
        memmove(batchp,hp,hn); batchp+=hn;
        for (loop=LE2-hn; loop--; ) *batchp++ = ' ';
    }
    *batchp++ = '|';

    if (n1 >= 2) {
        memmove(batchp,prefkey,2); batchp+=2;
    }
    else {
        memmove(batchp,prefkey,n1); batchp+=n1;
        for (loop=2-n1; loop--; ) *batchp++ = ' ';
    }
    *batchp++ = '|';

    if (n3 >= 3) {
        memmove(batchp,qualkey,3); batchp+=3;
    }
    else {
        memmove(batchp,qualkey,n3); batchp+=n3;
        for (loop=3-n3; loop--; ) *batchp++ = ' ';
    }
    *batchp++ = '|';

    sprintf(batchp,"%2d|%2d|%d|%d|%9"_LD_"\n",n2,hn,n1,n3,totpostx);
#if TRACE
*batchp='\0';
printf("sn=%d strlen=%d lreclx=%d\n%s.",sn,strlen(buffup),lreclx,buffup);
#endif
    if (CIWRITE(fdout[sn],buffup,lreclx) != lreclx)
        fatal("genlk1/write");

#if OUTONISIS
    memmove(key2,hp,hn); key2[hn]='\0';
    if (fldupdat(irecout,"D*") != NULL) fatal("fldupdat/init");
    VMFRmfn(irecout)=MFRmfn; etc;
#endif

    return(1);
}

