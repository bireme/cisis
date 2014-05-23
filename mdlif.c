/* ----------------------------------------------------------------------
   mdlif.c    -   NLM Medlars II - Access points translation / decoding
----------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "cisis.h"      /* CISIS Interface */
#include "cirun.h"      /* runtime area and defines by AOT */

int argnext = 1;

int  parmcontrol=0,parmleader=0,parmfields=0;
LONGX parmfrom=1,parmto=0,parmloop=1;
int  parmwait=0; /* =1; */
LONGX parmcount=LONGX_MAX, parmtell=0;
int  parmtest=0;
FILE *fptest=NULL;


#define ENBSSMH2        1
#define ENBSSMH3        1

#define MINTAG           15 
#define MAXTAG          969

int nlmtag[MAXTAG+1];


#define SVCHR           0x1e
#define EVCHR           0x1e
#define SSCHR           0x1d

#define QQCHR           '/'


#define NLMTAGAU        372
#define NLMTAGEM        383
#define NLMTAGLR        102
#define NLMTAGJC        320
#define NLMTAGTI        352
#define NLMTAGUI        350
#define NLMTAGCU        103
#define NLMTAGMH        351
#define NLMTAGTA        305
#define NLMTAGTT        380
#define NLMTAGAB        370
#define NLMTAGLA        306
#define NLMTAGPS        379
#define NLMTAGDP        354
#define NLMTAGRN        261             /* cdrom */
#define NLMTAGLI        329             /* cdrom */
#define NLMTAGSB        324             /* cdrom */
#define NLMTAGIS        301             /* cdrom */
#define NLMTAGCY        307             /* cdrom */
#define NLMTAGZN        308             /* cdrom */
#define NLMTAGPT        360             /* Publication Type -1991 */
#define NLMTAGAD        378             /* Address */
#define NLMTAGGS        328             /* Gene Symbol */
#define NLMTAGVI        381             /* Volume */
#define NLMTAGIP        373             /* Issue\Part\Supplement */

#define NEWTAGTI_130    130

/* TOXLINE - AOT/MB/FJL 14/06/2000 */
/* Novos campos para I/F */
#define NLMTAGCN        529     /*                       */
#define NLMTAGKW        634     /*                       */
#define NLMTAGSA        550     /*                       */
#define NLMTAGYR        504     /*                       */
/* Campos em outros tags */
#define TOXTAGEM        100     /* x 383 - agora com 6 digitos   */


unsigned char ebc2asc[256];
unsigned char ebc2dia[256];
int newlen;


int parmcopy = 1;

int parmascii= 0;

#if BEFORE20000320
int parmDP = 0;
#else
int parmDP = -1;
#endif
unsigned short int DP1,DP2;

int parmappd = 0;

int parmABy = 0;
int parmABn = 0;

int parmTOX = 0;


RECSTRU *recp; /* mandatory for defines REC RDB MF0 MFR DIR, FIELDP */

#define FLDUPDLM        0x01

unsigned char dbname[CIMPL+1],dbnout[CIMPL+1],buffup[MAXMFRL+MAXMFRL];

unsigned char *dbnamep,*dbnoutp,*batchp,*lastp;

LONGX count = 0;

#if ANSI
int qqlist(unsigned char *p,int loop2,int iocc);
#else
int qqlist();
#endif

void main(argc,argv)
int argc;
char *argv[];
{
    LONGX irec,upcrec,upirec,mfn;
    int rc;

    int minus1,plus1;

    unsigned char *p,*q,c;
    int i,j,xdir,loop1,loop2,tag,iocc;

for (i=0; i<MAXTAG; )
  nlmtag[i++]=0;
nlmtag[NLMTAGAU] = 1;
nlmtag[NLMTAGEM] = 1;
nlmtag[NLMTAGLR] = 1;
nlmtag[NLMTAGJC] = 1;
nlmtag[NLMTAGTI] = 1;
nlmtag[NLMTAGUI] = 1;
nlmtag[NLMTAGCU] = 1;
nlmtag[NLMTAGMH] = 1;
nlmtag[NLMTAGTA] = 1;
nlmtag[NLMTAGTT] = 1;
nlmtag[NLMTAGAB] = 1;
nlmtag[NLMTAGLA] = 1;
nlmtag[NLMTAGPS] = 1;
nlmtag[NLMTAGDP] = 1;
nlmtag[NLMTAGRN] = 1;
nlmtag[NLMTAGLI] = 1;
nlmtag[NLMTAGSB] = 1;
nlmtag[NLMTAGIS] = 1;
nlmtag[NLMTAGCY] = 1;
nlmtag[NLMTAGZN] = 1;
nlmtag[NLMTAGPT] = 1;
nlmtag[NLMTAGAD] = 1;
nlmtag[NLMTAGGS] = 1;
nlmtag[NLMTAGVI] = 1;
nlmtag[NLMTAGIP] = 1;

for (i=0; i<256; )
  ebc2asc[i++]=' ';
                                           for (i=0; i<256; )
                                             ebc2dia[i++]=0;

ebc2asc[0x40]=' ';                         ebc2dia[0x40]=' ';
ebc2asc[0x4a]=' '; /* cedilla */           ebc2dia[0x4a]=0;
ebc2asc[0x4b]='.';                         ebc2dia[0x4b]='.';
ebc2asc[0x4c]='<';                         ebc2dia[0x4c]='<';
ebc2asc[0x4d]='(';                         ebc2dia[0x4d]='(';
ebc2asc[0x4f]='|';                         ebc2dia[0x4f]='|';
ebc2asc[0x4e]='+';                         ebc2dia[0x4e]='+';
ebc2asc[0x50]='&';                         ebc2dia[0x50]='&';
ebc2asc[0x51]=SVCHR;                       ebc2dia[0x51]=SVCHR;
ebc2asc[0x52]=EVCHR;                       ebc2dia[0x52]=EVCHR;
ebc2asc[0x53]=SSCHR;                       ebc2dia[0x53]=SSCHR;
ebc2asc[0x5a]='!';                         ebc2dia[0x5a]='!';
ebc2asc[0x5b]='$';                         ebc2dia[0x5b]='$';
ebc2asc[0x5c]='*';                         ebc2dia[0x5c]='*';
ebc2asc[0x5d]=')';                         ebc2dia[0x5d]=')';
ebc2asc[0x5e]=';';                         ebc2dia[0x5e]=';';
ebc2asc[0x5f]='^'; /* not sign */          ebc2dia[0x5f]='^';
ebc2asc[0x60]='-';                         ebc2dia[0x60]='-';
ebc2asc[0x61]='/';                         ebc2dia[0x61]='/';
ebc2asc[0x69]='|';                         ebc2dia[0x69]='|';
ebc2asc[0x6a]='|';                         ebc2dia[0x6a]='|';
ebc2asc[0x6b]=',';                         ebc2dia[0x6b]=',';
ebc2asc[0x6c]='%';                         ebc2dia[0x6c]='%';
ebc2asc[0x6d]='_';                         ebc2dia[0x6d]='_';
ebc2asc[0x6e]='>';                         ebc2dia[0x6e]='>';
ebc2asc[0x6f]='?';                         ebc2dia[0x6f]='?';
ebc2asc[0x7a]=':';                         ebc2dia[0x7a]=':';
ebc2asc[0x7b]='#';                         ebc2dia[0x7b]='#';
ebc2asc[0x7c]='@';                         ebc2dia[0x7c]='@';
ebc2asc[0x7d]='\'';                        ebc2dia[0x7d]='\'';
ebc2asc[0x7e]='=';                         ebc2dia[0x7e]='=';
#if BEFORE990715
ebc2asc[0x7f]='®';                         ebc2dia[0x7f]='®';
#else
ebc2asc[0x7f]=' ';                         ebc2dia[0x7f]=' ';
#endif
ebc2asc[0x80]=' '; /* afs delimiter */     ebc2dia[0x80]=0;
ebc2asc[0x81]='a';                         ebc2dia[0x81]='a';
ebc2asc[0x82]='b';                         ebc2dia[0x82]='b';
ebc2asc[0x83]='c';                         ebc2dia[0x83]='c';
ebc2asc[0x84]='d';                         ebc2dia[0x84]='d';
ebc2asc[0x85]='e';                         ebc2dia[0x85]='e';
ebc2asc[0x86]='f';                         ebc2dia[0x86]='f';
ebc2asc[0x87]='g';                         ebc2dia[0x87]='g';
ebc2asc[0x88]='h';                         ebc2dia[0x88]='h';
ebc2asc[0x89]='i';                         ebc2dia[0x89]='i';
ebc2asc[0x8a]='i'; /* dotless i */         ebc2dia[0x8a]='i';
ebc2asc[0x8e]='l'; /* lower polish l */    ebc2dia[0x8e]='l';
ebc2asc[0x91]='j';                         ebc2dia[0x91]='j';
ebc2asc[0x92]='k';                         ebc2dia[0x92]='k';
ebc2asc[0x93]='l';                         ebc2dia[0x93]='l';
ebc2asc[0x94]='m';                         ebc2dia[0x94]='m';
ebc2asc[0x95]='n';                         ebc2dia[0x95]='n';
ebc2asc[0x96]='o';                         ebc2dia[0x96]='o';
ebc2asc[0x97]='p';                         ebc2dia[0x97]='p';
ebc2asc[0x98]='q';                         ebc2dia[0x98]='q';
ebc2asc[0x99]='r';                         ebc2dia[0x99]='r';
#if BEFORE990715
ebc2asc[0x9d]='¯';                         ebc2dia[0x9d]='¯';
#else
ebc2asc[0x9d]=' ';                         ebc2dia[0x9d]=' ';
#endif
ebc2asc[0x9f]=' '; /* circumflex */        ebc2dia[0x9f]=0;
#if BEFORE990715
ebc2asc[0xa0]='þ'; /* trademark symbol */  ebc2dia[0xa0]='þ';
#else
ebc2asc[0xa0]=' '; /* trademark symbol */  ebc2dia[0xa0]=' ';
#endif
ebc2asc[0xa1]=' '; /* angstrom */          ebc2dia[0xa1]=0;
ebc2asc[0xa2]='s';                         ebc2dia[0xa2]='s';
ebc2asc[0xa3]='t';                         ebc2dia[0xa3]='t';
ebc2asc[0xa4]='u';                         ebc2dia[0xa4]='u';
ebc2asc[0xa5]='v';                         ebc2dia[0xa5]='v';
ebc2asc[0xa6]='w';                         ebc2dia[0xa6]='w';
ebc2asc[0xa7]='x';                         ebc2dia[0xa7]='x';
ebc2asc[0xa8]='y';                         ebc2dia[0xa8]='y';
ebc2asc[0xa9]='z';                         ebc2dia[0xa9]='z';
ebc2asc[0xab]=' '; /* um laut */           ebc2dia[0xab]=0;
ebc2asc[0xac]=' '; /* tilde */             ebc2dia[0xac]=0;
ebc2asc[0xad]='[';                         ebc2dia[0xad]='[';
ebc2asc[0xaf]=' '; /* grave */             ebc2dia[0xaf]=0;
ebc2asc[0xb9]='o'; /* lower polish o */    ebc2dia[0xb9]='o';
ebc2asc[0xbb]=' '; /* breve */             ebc2dia[0xbb]=0;
ebc2asc[0xbc]=' '; /* acute */             ebc2dia[0xbc]=0;
ebc2asc[0xbd]=']';                         ebc2dia[0xbd]=']';
ebc2asc[0xbf]='-'; /* macron */            ebc2dia[0xbf]='-';
ebc2asc[0xc0]='{';                         ebc2dia[0xc0]='{';
ebc2asc[0xc1]='A';                         ebc2dia[0xc1]='A';
ebc2asc[0xc2]='B';                         ebc2dia[0xc2]='B';
ebc2asc[0xc3]='C';                         ebc2dia[0xc3]='C';
ebc2asc[0xc4]='D';                         ebc2dia[0xc4]='D';
ebc2asc[0xc5]='E';                         ebc2dia[0xc5]='E';
ebc2asc[0xc6]='F';                         ebc2dia[0xc6]='F';
ebc2asc[0xc7]='G';                         ebc2dia[0xc7]='G';
ebc2asc[0xc8]='H';                         ebc2dia[0xc8]='H';
ebc2asc[0xc9]='I';                         ebc2dia[0xc9]='I';
ebc2asc[0xd1]='J';                         ebc2dia[0xd1]='J';
ebc2asc[0xd2]='K';                         ebc2dia[0xd2]='K';
ebc2asc[0xd3]='L';                         ebc2dia[0xd3]='L';
ebc2asc[0xd4]='M';                         ebc2dia[0xd4]='M';
ebc2asc[0xd5]='N';                         ebc2dia[0xd5]='N';
ebc2asc[0xd6]='O';                         ebc2dia[0xd6]='O';
ebc2asc[0xd7]='P';                         ebc2dia[0xd7]='P';
ebc2asc[0xd8]='Q';                         ebc2dia[0xd8]='Q';
ebc2asc[0xd9]='R';                         ebc2dia[0xd9]='R';
ebc2asc[0xe0]='\\';                        ebc2dia[0xe0]='\\';
ebc2asc[0xe2]='S';                         ebc2dia[0xe2]='S';
ebc2asc[0xe3]='T';                         ebc2dia[0xe3]='T';
ebc2asc[0xe4]='U';                         ebc2dia[0xe4]='U';
ebc2asc[0xe5]='V';                         ebc2dia[0xe5]='V';
ebc2asc[0xe6]='W';                         ebc2dia[0xe6]='W';
ebc2asc[0xe7]='X';                         ebc2dia[0xe7]='X';
ebc2asc[0xe8]='Y';                         ebc2dia[0xe8]='Y';
ebc2asc[0xe9]='Z';                         ebc2dia[0xe9]='Z';
ebc2asc[0xf0]='0';                         ebc2dia[0xf0]='0';
ebc2asc[0xf1]='1';                         ebc2dia[0xf1]='1';
ebc2asc[0xf2]='2';                         ebc2dia[0xf2]='2';
ebc2asc[0xf3]='3';                         ebc2dia[0xf3]='3';
ebc2asc[0xf4]='4';                         ebc2dia[0xf4]='4';
ebc2asc[0xf5]='5';                         ebc2dia[0xf5]='5';
ebc2asc[0xf6]='6';                         ebc2dia[0xf6]='6';
ebc2asc[0xf7]='7';                         ebc2dia[0xf7]='7';
ebc2asc[0xf8]='8';                         ebc2dia[0xf8]='8';
ebc2asc[0xf9]='9';                         ebc2dia[0xf9]='9';


    argnext=1;
    if (argc < 3) {
        if (argc == 2 && strcmp(argv[1],"what") == 0) {
            printf("%s",cicopyr("?Utility MDL/MDLIF"));
            printf("\n");
            exit(1);
        }
        printf("%s",cicopyr("Utility MDL/MDLIF"));
        printf("\n");
        printf("mdlif <dbname> [create=]<dbnout> [<option> [...]] \n");
        printf("\n");
        printf("options: {+/-}{control/leader/fields} \n");
        printf("         {from/to/loop/count/tell}=<n> \n");
        printf("         DP=<aa> \n");
	printf("         AB={YES|NO} \n");
        printf("         TOXLINE \n");
        printf("         [APPEND] \n");
        printf("         now[ait] \n");
        printf("\n");
        printf("(version 14/06/2000)\n");
        printf("\n");
	exit(1);
    }

    dbnamep=argv[argnext++];
    dbnoutp=argv[argnext++];


    minus1=plus1=0;

    for (i=argnext; i<argc; i++) {
        p=argv[i];

        if (*p == '-') {
            minus1++;
            if (minus1 == 1)
                parmcontrol=parmleader=parmfields=1;
        }

        if (strcmp(p,"-control") == 0) {
            parmcontrol=0;
            continue;
        }
        if (strcmp(p,"-leader") == 0) {
            parmleader=0;
            continue;
        }
        if (strcmp(p,"-fields") == 0) {
            parmfields=0;
            continue;
        }

        if (*p == '+') {
            plus1++;
            if (plus1 == 1) {
                parmcontrol=parmleader=parmfields=0;
            }
        }

        if (strcmp(p,"+control") == 0) {
            parmcontrol=1;
            continue;
        }
        if (strcmp(p,"+leader") == 0) {
            parmleader=1;
            continue;
        }
        if (strcmp(p,"+fields") == 0) {
            parmfields=1;
            continue;
        }

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

        if (strcmp(p,"nowait") == 0 || strcmp(p,"now") == 0) {
            parmwait=0;
            continue;
        }

        if (!strncmp(argv[i],"count=",6)) {
            p=argv[i]+6;
            if ((sscanf(p,"%"_LD_,&parmcount)) != 1)
                fatal(p);
            continue;
        }
        if (!strncmp(argv[i],"tell=",5)) {
            p=argv[i]+5;
            if ((sscanf(p,"%"_LD_,&parmtell)) != 1)
                fatal(p);
            continue;
        }

        if (!strncmp(argv[i],"DP=",3)) {
            p=argv[i]+3;
            if ((sscanf(p,"%d",&parmDP)) != 1)
                fatal(p);
#if BEFORE20000310		
            if (parmDP < 48 || parmDP > 99)
                fatal("parmDP 48..99");
#endif
            DP1=parmDP/10; DP2=parmDP-DP1*10;
            printf("+++ parmDP=%d%d",DP1,DP2);
            DP1|=0xF0; /* EBCDIC '0' */
            DP2|=0xF0; /*            */
            printf("=%02x%02x \n",DP1,DP2);
            continue;
        }

        if (!strncmp(argv[i],"AB=NO",5)) {
           parmABn=1;
           nlmtag[NLMTAGAB] = 0;
           nlmtag[NLMTAGAU] = 0;
           printf("+++ parmABn=%d parmABy=%d \n",parmABn,parmABy);
	   continue;
        }

        if (!strncmp(argv[i],"AB=YES",6)) {
           parmABy=1;
           for (j=0; j<MAXTAG; ) nlmtag[j++]=0;
           nlmtag[NLMTAGUI] = 1;
           nlmtag[NLMTAGAB] = 1;
           nlmtag[NLMTAGAU] = 1;
           printf("+++ parmABn=%d parmABy=%d \n",parmABn,parmABy);
           continue;
        }

        if (!strncmp(argv[i],"APPEND",6)) {
           parmappd=1;
           printf("+++ APPEND \n");
           continue;
        }

        if (!strncmp(argv[i],"test=",5)) {
            p=argv[i]+5;
            if ((sscanf(p,"%d",&parmtest)) != 1)
                fatal(p);
            continue;
        }

        if (strcmp(p,"asciinomore") == 0) {
            parmascii=1;
            continue;
        }

        /* TOXLINE - AOT/MB/FJL 14/06/2000 */
        if (!strcmp(argv[i],"TOXLINE")) {
            parmTOX=1;
            nlmtag[NLMTAGCN] = 1;
            nlmtag[NLMTAGKW] = 1;
            nlmtag[NLMTAGSA] = 1;
            nlmtag[NLMTAGYR] = 1;
            nlmtag[TOXTAGEM] = 1;
            printf("+++ TOXLINE \n");
	    continue;
        }

        fatal(p);
    }


    printf("+++ input data base:  %s%s\n",dbnamep,(parmascii)?" ascii":" ");
    printf("+++ output data base: %s\n",dbnoutp);

    if (parmtest)
        if ((fptest=fopen("xmdlif","w")) == NULL)
            fatal("xmdlif");


    /*
        inicializa bases de dados
    */
    if (strncmp(dbnoutp,"create=",7) == 0) rc=recisis0((dbnoutp+=7));

    /*
        ve entrada
    */
    RECORD((irec=nrecs),dbnamep,0L);
    if (!parmto) parmto=MF0nxtmfn-1;
    if (parmcontrol) prtcontrol(recp,dbnamep);

    /*
        ve saida
    */
    RECORD((upcrec=nrecs),dbnoutp,0L);
    if (parmcontrol) prtcontrol(recp,dbnoutp);

    /*
        loop principal
    */
    for (upirec=nrecs, mfn=parmfrom; mfn<=parmto; mfn+=parmloop) {

        RECORD(irec,dbnamep,mfn);

        if (RECrc != RCNORMAL)
            continue;

#if BEFORE20000320
        if (parmDP) {
#else
        if (parmDP >= 0) {
#endif
            xdir=fieldx(irec,NLMTAGDP,1);
            if (xdir >= 0) {
                if (DIRlen(xdir) >= 4) {
                    p=FIELDP(xdir);
                    if (*(p+2) != DP1) continue;
                    if (*(p+3) != DP2) continue;
                }
            }
            else {          
                  if (!parmappd) continue;
            }
        }

        if (parmleader) prtleader(recp,0L);

        batchp=buffup; *batchp=NULL;

        for (xdir=tag=0, loop1=MFRnvf; loop1--; xdir++) {

            if (DIRtag(xdir) < MINTAG || DIRtag(xdir) > MAXTAG) {
                printf("*** mfn=%"_LD_" tag=%d \n",
                    MFRmfn,DIRtag(xdir));
                fatal("mdlif/tag");
            }

            if (nlmtag[DIRtag(xdir)] == 0)
                continue;

            if (DIRtag(xdir) != tag) {
                tag=DIRtag(xdir);
                iocc=1;
            }
            else
                iocc++;

            newlen=DIRlen(xdir);
            if (parmascii == 0) {
                p=q=FIELDP(xdir);
                for (loop2=DIRlen(xdir); loop2--; p++) {
                    c = *p;
                    *q = ebc2asc[c];
                    if (ebc2dia[c] == 0) {
                        newlen--;
if (parmtest >= 2) {
 fprintf(fptest,"ebc2dia - tag=%d  iocc=%d  in char=%c=%02x newlen=%d",
  DIRtag(xdir),iocc,*q,c,newlen);
 fprintf(fptest,"\n");
}
                    }
                    else {
                        q++;
                    }
                }
            }


#if Y
*batchp=NULL;
puts("for xdir++  "); getchar();
puts(buffup);
#endif

if (parmfields)
 if (parmtest >= 1) {
        printf("%3d %d >",DIRtag(xdir),iocc);
        for (q=FIELDP(xdir), loop2=newlen; loop2--; q++)
            printf("%c",*q);
        printf("<\n");
 }


            if (DIRtag(xdir) != NLMTAGMH && DIRtag(xdir) != NLMTAGRN) {

                if (DIRtag(xdir) == NLMTAGTI && fieldn(irec,NLMTAGTT,1))
                    sprintf(batchp,"A%3d%c",NEWTAGTI_130,FLDUPDLM);
                else
                    sprintf(batchp,"A%3d%c",DIRtag(xdir),FLDUPDLM);
                batchp+=5;

                for (p=FIELDP(xdir), loop2=newlen; loop2; ) {
                    if (*p == SVCHR || *p == EVCHR || *p == SSCHR) {
                        printf("*** mfn=%"_LD_" tag=%d occ=%d ...",
                            MFRmfn,DIRtag(xdir),iocc);
                        /* for (; loop2--; p++)
                            printf("%c",*p);
                        printf(".\n"); 
                        break;  */
                        p++;
                        loop2--;
                    }
		    if (loop2) {
                        *batchp++ = *p; p++;
                        loop2--;
		    }
                }
                *batchp++ = FLDUPDLM;
                iocc=0;
                continue; /* next dir entry */
            }

            if (DIRtag(xdir) == NLMTAGRN) {
                sprintf(batchp,"A%3d%c",DIRtag(xdir),FLDUPDLM);
                batchp+=5;
                *batchp++ = '^'; *batchp++ = 'a';
                p=FIELDP(xdir); loop2=newlen;
                if (*(p+loop2-1) == ')')
                    loop2--;
                for (; loop2; ) {
                    if (*p == '(')
                        break;
                    loop2--;
                    *batchp++ = *p; p++;
                }
                if (*(batchp-1) == ' ') { batchp--; }
                if (*p == '(') { loop2--; p++; }
                *batchp++ = '^'; *batchp++ = 'b';
                for (; loop2--; ) {
                     *batchp++ = *p; p++;
                }
                *batchp++ = FLDUPDLM;

                continue;
            }

            sprintf(lastp=batchp,"A%3d%c",880,FLDUPDLM); batchp+=5; /*secund.*/

            *batchp++ = '^'; *batchp++ = 'h';

            for (p=FIELDP(xdir), loop2=newlen; loop2; ) {
                if (*p == SSCHR) {
                    break;
                }
                *batchp++ = *p; p++;
                loop2--;
            }


            if (*p != SSCHR) {
                /* fatal("1\\SS\\2"); */
                printf("1\\SS\\2 - mfn=%"_LD_" tag=%d occ=%d\n",
                    MFRmfn,DIRtag(xdir),iocc);
                *batchp++ = FLDUPDLM;
                continue;
            }
            p++; loop2--;
#if ENBSSMH2
            *batchp++ = '^'; *batchp++ = 'c';
            *batchp++ = *p;
#endif
            p++; loop2--;


            if (*p != SSCHR) {
                /* fatal("2\\SS\\3"); */
                printf("2\\SS\\3 - mfn=%"_LD_" tag=%d occ=%d\n",
                    MFRmfn,DIRtag(xdir),iocc);
                *batchp++ = FLDUPDLM;
                continue;
            }
            p++; loop2--;
#if ENBSSMH3
            *batchp++ = '^'; *batchp++ = 'i';
            *batchp++ = *p;
#endif
            p++; loop2--;
            *batchp++ = FLDUPDLM;               /*end required subelements*/

            if (loop2 < 0) {
                printf("loop2<0 - mfn=%"_LD_" tag=%d occ=%d loop2=%d \n",
                    MFRmfn,DIRtag(xdir),iocc,loop2);
                continue;
            }

            if (*p == SSCHR) {
                p++; loop2--;
                batchp--;           /* erase FLDUPDLM required subelements */
                loop2=qqlist(p,loop2,iocc);
                *batchp++ = FLDUPDLM;
                continue;
            }

            if (loop2) {
if (parmtest >= 3) {
    fprintf(fptest,"ev/sv - mfn=%"_LD_" occ=%d loop2=%d ?",MFRmfn,iocc,loop2);
    for (q=p; loop2--; q++)
        fprintf(fptest,"%c",*q);
    fprintf(fptest,"?\n");
}
                if (*p != SVCHR && *p != EVCHR) {
                    printf("loop2 - mfn=%"_LD_" tag=%d occ=%d loop2=%d \n",
                        MFRmfn,DIRtag(xdir),iocc,loop2);
                }
            }
        }

#if Y
*batchp=NULL;
puts("end xdir++  "); getchar();
puts(buffup);
#endif

        if (parmABn)
	    if (fieldn(irec,NLMTAGAB,1)) {
                sprintf(batchp,"A%3d%c%s%c",NLMTAGAB,FLDUPDLM,"Y",FLDUPDLM);
                batchp+=strlen(batchp);
            }

        *batchp=NULL;

        RECORD(upirec,dbnoutp,VMF0nxtmfn(upcrec)); /* get null PDEL record */
        MFRstatus=ACTIVE;

        p=fldupdat(upirec,buffup);
        if (p) {
            printf("*** fldupdat='%s'\n",p);
            fatal("mdlif/fldupdat");
        }

if (parmfields) {
        tag=NULL; iocc=1;
        for (xdir=0, loop1=MFRnvf; loop1--; xdir++) {
            if (DIRtag(xdir) == tag) {
                iocc++;
            }
            else {
                tag=DIRtag(xdir);
                iocc=1;
            }
            printf("%6"_LD_"|%3d|%3d|",MFRmfn,DIRtag(xdir),iocc);
            for (q=FIELDP(xdir), loop2=DIRlen(xdir); loop2--; q++)
#if SHOWHEX
                if (*q < ' ' || *q > '}') printf("|x%02x|",*q); else
#endif
                printf("%c",*q);
            printf("|\n");
        }
}


if (parmcopy) {
        rc=recupdat(upcrec,upirec);
        if (rc) {
            printf("*** recupdat=%d\n",rc);
            fatal("mdlif/recupdat");
        }
}

        count++;
        if (parmtell)
            if (!(count % parmtell)) {
              if (parmtest >= 2)
                fprintf(fptest,
                  "%7"_LD_"|%6"_LD_"|%5d/%5d|%5d/%5d|%5d/%5d(%3d/%3d)|%5d/%5d|%6"_LD_"\n",
                        count,
                        VMFRmfn(irec),VMFRmfrl(irec),MFRmfrl,
                        LEADER,LEADER,
                        VMFRbase(irec)-LEADER,MFRbase-LEADER,
                        VMFRnvf(irec),MFRnvf,
                        VMFRmfrl(irec)-VMFRbase(irec),MFRmfrl-MFRbase,
                        MFRmfn);

              fprintf(stderr,
                    "+++ %"_LD_" recs mfn=%6"_LD_"  mfrl=%d  =  %d + %d (%3d) + %d\n",
                        count,
                        MFRmfn,MFRmfrl,
                        LEADER,MFRbase-LEADER,MFRnvf,MFRmfrl-MFRbase);
            }

        if (parmwait) {
            printf("..");
            if (getchar() != '\n')
                break;
        }

        if (count >= parmcount)
            break;
    }

    exit(0);
}


int qqlist(p,loop2,iocc)
unsigned char *p;
int loop2;
int iocc;
{
        unsigned char *thisp;
        unsigned char *q;
        int qlen,n;


        for (*batchp=NULL, qlen=strlen(lastp); loop2; ) {

#if X
printf("+++ p   : loop2=%d - '",loop2);
for (n=loop2, q=p; n--; q++)
 putchar(*q);
puts("'\n");
#endif

            if (*p == '*') {
                *(lastp+1)='8';
                *(lastp+2)='7'; /* principal */
                *(lastp+3)='0';
                p++; loop2--;           /*skip 1 input char*/
#if X
printf("+++ * on: loop2=%d - '",loop2);
for (n=loop2, q=p; n--; q++)
 putchar(*q);
puts("'\n");
#endif

            }
            else {
                *(lastp+1)='8';
                *(lastp+2)='8'; /* secundario */
                *(lastp+3)='0';
            }
#if X
printf("+++ * ok: loop2=%d - '",loop2);
for (n=loop2, q=p; n--; q++)
 putchar(*q);
puts("'\n");
#endif


            if (loop2) {
                if (*p != QQCHR && *p != SVCHR && *p != EVCHR) {
#if X
printf("+++ qq  : loop2=%d - '",loop2);
for (n=loop2, q=p; n--; q++)
 putchar(*q);
puts("'\n");
#endif

                    if (loop2 < 2) {
                        printf("loop<2 - mfn=%"_LD_"/%d loop2=%d chr=%02x=%c \n",
                            MFRmfn,iocc,loop2,*p,*p);
                        return(loop2);
                    }
                    q=p+1;
                    if (!isalpha(*p) || !isalpha(*q)) {
                        printf("letter - mfn=%"_LD_"/%d loop2=%d chr=%02x=%c \n",
                            MFRmfn,iocc,loop2,*p,*p);
                        return(loop2);
                    }
                    *batchp++ = '^'; *batchp++ = 'q';
                    *batchp++ = *p; p++;
                    *batchp++ = *p; p++;
                    loop2-=2;
#if X
printf("+++ ok  : loop2=%d - '",loop2);
for (n=loop2, q=p; n--; q++)
 putchar(*q);
puts("'\n");
#endif

                }
            }

            if (loop2 > 0)
                if (*p == QQCHR) {

                    *batchp++ = FLDUPDLM;

                    for (thisp=lastp, lastp=batchp, n=qlen; n-- ; )
                        *batchp++ = *thisp++;
                    p++; loop2--;   /* skip 1 input char */
#if X
printf("+++ more: loop2=%d - '",loop2);
for (n=loop2, q=p; n--; q++)
 putchar(*q);
puts("'\n");
#endif
                    continue;
                }

            break;
        }

#if Z
*batchp=NULL;
puts("end qqls++  "); getchar();
puts(buffup);
#endif

        return(loop2);
}
