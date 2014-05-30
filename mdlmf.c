/* ----------------------------------------------------------------------
   mdlmf.c    -   NLM Medlars II - Master file translation/reformatting
----------------------------------------------------------------------- */

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "cisis.h"      /* CISIS Interface */
#include "cirun.h"      /* runtime area and defines by AOT */

char *parmgizp = NULL;  /* gizp */

int argnext = 1;

int  parmcontrol=0,parmleader=0,parmfields=0;
LONGX parmfrom=1,parmto=0,parmloop=1;
int  parmwait=1;
int  parmtrace=0;
LONGX parmcount=999999999L, parmtell=0;

int parmzdecs=0;
int parmok=0;
char *zdbnp;
char *zdp;
unsigned char *z1p,*z2p,zkey[LE2+BUFSIZ+1];
LONGX itrm=0L;
TRMSTRU *trmp;
LONGX zsave=0L;
int zx0,zx1;
LONGX zdlookup=0L,zdnotf=0L;

int parmgizmo=0;
VGIZPSTRU *vgzrp=NULL;          /* gizmo processing */
LONGX grec;
UWORD gmfrl0;
LONGX gsave=0L;

LONGX totmfrl=0L,totleader=0L,totdir=0L,totdata=0L;
LONGX tot0,tot1,totz=0L,totg=0L;
FILE *fplog=NULL;
FILE *filog=NULL;

#define MINTAG           15 
#define MAXTAG          969

int nlmtag[MAXTAG+1];

#define NLMTAGAB        370     /* A830                      */
#define NLMTAGAA        371     /* ---- [370: (Au)]          */
#define NLMTAGAD        378     /* B100                      */
#define NLMTAGAU        372     /* A100                      */
#define NLMTAGRN        261     /* ----                      */
#define NLMTAGCY        307     /* ----                      */
#define NLMTAGDA        100     /* A910                      */
#define NLMTAGDP        354     /* A640 [aaaammdd: A650=650] */
#define NLMTAGEA        382     /* A410 [En]                 */
#define NLMTAGEM        383     /* E900                      */
#define NLMTAGID        640     /* ----                      */
#define NLMTAGPY        323     /* ----                      */
#define NLMTAGIS        301     /* A350                      */
#define NLMTAGIP        373     /* A320                      */
#define NLMTAGSB        324     /* ----                      */
#define NLMTAGJC        320     /* B300                      */
#define NLMTAGLA        306     /* A400 [ISO]                */
#define NLMTAGMR        368     /* ----                      */
#define NLMTAGMH        351     /* A770 [if: A870/A880]      */
#define NLMTAGZN        308     /* ----                      */
#define NLMTAGNI        376     /* ----                      */
#define NLMTAGNP        369     /* ----                      */
#define NLMTAGRF        377     /* A720                      */
#define NLMTAGPG        353     /* A140                      */
#define NLMTAGPS        379     /* A780                      */
#define NLMTAGRO        101     /* ----                      */
#define NLMTAGSI        606     /* ----                      */
#define NLMTAGLI        329     /* ----                      */
#define NLMTAGTI        352     /* A120 [380pres: A130=130]  */
#define NLMTAGTA        305     /* A300                      */
#define NLMTAGTT        380     /* A120                      */
#define NLMTAGUI        350     /* A020                      */
#define NLMTAGVI        381     /* S310                      */
#define NLMTAGCU        103     /* A900                      */
#define NLMTAGLR        102     /* M900                      */
#define NLMTAGCO        440     /* A440                      */
#define NLMTAGPT        360     /* Publication Type -1991 */

#define NEWTAGTI_130    130     /* when TT present */

#define ENAB650 0

#if ENAB650
#define NEWTAGDP_650    650     /* standardized date */
#endif

/* TOXLINE - AOT/MB/FJL 14/06/2000 */
/* Novos campos para display */
#define NLMTAGCN        529     /*                       */
#define NLMTAGSO        602     /*                       */
#define NLMTAGGS        328     /*                       */
#define NLMTAGOD        611     /*                       */
#define NLMTAGPR        547     /*                       */
#define NLMTAGKW        634     /*                       */
/* Campos em outros tags */
#define TOXTAGAD        410     /* x 378 -                       */
#define TOXTAGCY        642     /* x 307 -                       */
#define TOXTAGEM        100     /* x 383 - agora com 6 digitos   */
/* Campos que nao podem existir */
#define TOXTAGN1        378
#define TOXTAGN2        307
#define TOXTAGN3        383



unsigned char ebc2asc[256];

#define EBCBLANK        0x40
#define EBCPERIOD       0x4b
#define EBCOSQBR        0xad
#define EBCCSQBR        0xbd
#define EBCOPARE        0x4d
#define EBCCPARE        0x5d

#define SVCHR           0x51
#define EVCHR           0x52
#define SSCHR           0x53

#define FLDUPDLM        0x01

#define INVALID         0x02

#define CEDILLA         0x03    /* cedilla */
#define OPNQUOTE        0x04    /* '®' */
#define DOTLESSI        0x05    /* dotless i */
#define LPOLISHL        0x06    /* lower polish l */
#define CLSQUOTE        0x07    /* '¯' */
#define CIRCUMFLEX      0x08    /* circumflex */
#define TRADEMARK       0x09    /* trademark symbol */
#define ANGSTROM        0x0a    /* angstrom */
#define UMLAUT          0x0b    /* um laut */
#define TILDE           0x0c    /* tilde */
#define GRAVE           0x0d    /* grave */
#define LPOLISHO        0x0e    /* lower polish o */
#define BREVE           0x0f    /* breve */
#define ACUTE           0x10    /* acute */
#define MACRON          0x11    /* macron */


int MHnocc;

#define LATABSIZ        6
#define LANLMSIZ        3
#define LAISOSIZ        2
unsigned char LAin[128];
unsigned char LAtab[][LATABSIZ+1] = { "AFR Af", "ENG En", "" };

#if ANSI
unsigned char *DPconvert(unsigned char *errp, int n);
void mxexit(int rc);
#else
unsigned char *DPconvert();
void mxexit();
#endif

#if ENAB650
unsigned char DPin[128];
unsigned char DPok[128];
unsigned char *DPerrp;
#endif


int parmcopy = 1;

#if BEFORE20000320
int parmDP = 0;
#else
int parmDP = -1;
#endif
unsigned short int DP1,DP2;
int parmappd = 0;


RECSTRU *recp; /* mandatory for defines REC RDB MF0 MFR DIR, FIELDP */

unsigned char dbname[CIMPL+1],dbnout[CIMPL+1],buffup[MAXMFRL+MAXMFRL];

unsigned char *dbnamep,*dbnoutp,*batchp;

LONGX irec,upcrec,upirec,mfn;

int parmTAn = 0;
int parmTAy = 0;

int parmTOX = 0;

LONGX count = 0;

#define PROC 1
/* ========================== mp_scan/mp_edit ============================= */
#if PROC
#define MAXNPPP 1
FMT_CODE *mp_pgmp[MAXNPPP];
int mp_n = 0;
char *mp_areap=NULL;
LONGX fmtasize=MAXMFRL+MAXMFRL;
#if ANSI
void mp_scan(char *gidbnp, char *specp);
void mp_edit(LONGX ridx, int i1, int i2);
#else
void mp_scan();
void mp_edit();
#endif
#endif /* PROC */


void main(argc,argv)
int argc;
char *argv[];

{
    int rc;
    int minus1,plus1;
    unsigned char *p,*q;
    int i/*,j*/,xdir,loop1,loop2,tag,iocc;
    VGIZPSTRU *gizmap;

for (i=0; i<MAXTAG; )
  nlmtag[i++]=2;                /* health */
                                /* health */
nlmtag[NLMTAGAA]=0;
nlmtag[NLMTAGRN]=1;             /* cdrom */
nlmtag[NLMTAGCY]=0;
nlmtag[NLMTAGID]=0;
nlmtag[NLMTAGPY]=0;
nlmtag[NLMTAGSB]=1;             /* cdrom */
nlmtag[NLMTAGMR]=0;
nlmtag[NLMTAGZN]=0;
nlmtag[NLMTAGNI]=0;
nlmtag[NLMTAGNP]=0;
nlmtag[NLMTAGRO]=0;
nlmtag[NLMTAGSI]=0;
nlmtag[NLMTAGLI]=1;             /* cdrom */

                                /* health */

nlmtag[NLMTAGAB]=1;
nlmtag[NLMTAGAD]=1;
nlmtag[NLMTAGAU]=1;
nlmtag[NLMTAGDA]=1;
nlmtag[NLMTAGDP]=1;
nlmtag[NLMTAGEA]=1;
nlmtag[NLMTAGEM]=1;
nlmtag[NLMTAGIS]=0;
nlmtag[NLMTAGIP]=1;
nlmtag[NLMTAGJC]=1;
nlmtag[NLMTAGLA]=1;
nlmtag[NLMTAGMH]=1;
nlmtag[NLMTAGRF]=1;
nlmtag[NLMTAGPG]=1;
nlmtag[NLMTAGPS]=1;
nlmtag[NLMTAGTI]=1;
nlmtag[NLMTAGTA]=0;
nlmtag[NLMTAGTT]=1;
nlmtag[NLMTAGUI]=1;
nlmtag[NLMTAGVI]=1;
nlmtag[NLMTAGCU]=1;
nlmtag[NLMTAGLR]=1;
nlmtag[NLMTAGCO]=1;
nlmtag[NLMTAGPT]=1;


for (i=0; i<256; )
  ebc2asc[i++]=INVALID;


ebc2asc[0x40]=' ';
ebc2asc[0x4a]=CEDILLA;          /* cedilla */
ebc2asc[0x4b]='.';
ebc2asc[0x4c]='<';
ebc2asc[0x4d]='(';
ebc2asc[0x4f]='|';
ebc2asc[0x4e]='+';
ebc2asc[0x50]='&';
ebc2asc[0x51]=INVALID;          /* SVCHR */
ebc2asc[0x52]=INVALID;          /* EVCHR */
ebc2asc[0x53]=INVALID;          /* SSCHR */
ebc2asc[0x5a]='!';
ebc2asc[0x5b]='$';
ebc2asc[0x5c]='*';
ebc2asc[0x5d]=')';
ebc2asc[0x5e]=';';
ebc2asc[0x5f]=INVALID;          /* not sign */
ebc2asc[0x60]='-';
ebc2asc[0x61]='/';
ebc2asc[0x69]='|';
ebc2asc[0x6a]='|';
ebc2asc[0x6b]=',';
ebc2asc[0x6c]='%';
ebc2asc[0x6d]='_';
ebc2asc[0x6e]='>';
ebc2asc[0x6f]='?';
ebc2asc[0x7a]=':';
ebc2asc[0x7b]='#';
ebc2asc[0x7c]='@';
ebc2asc[0x7d]='\'';
ebc2asc[0x7e]='=';
ebc2asc[0x7f]=OPNQUOTE;         /* '®' */
ebc2asc[0x80]=INVALID;          /* afs delimiter */
ebc2asc[0x81]='a';
ebc2asc[0x82]='b';
ebc2asc[0x83]='c';
ebc2asc[0x84]='d';
ebc2asc[0x85]='e';
ebc2asc[0x86]='f';
ebc2asc[0x87]='g';
ebc2asc[0x88]='h';
ebc2asc[0x89]='i';
ebc2asc[0x8a]=DOTLESSI;         /* dotless i */
ebc2asc[0x8e]=LPOLISHL;         /* lower polish l */
ebc2asc[0x91]='j';
ebc2asc[0x92]='k';
ebc2asc[0x93]='l';
ebc2asc[0x94]='m';
ebc2asc[0x95]='n';
ebc2asc[0x96]='o';
ebc2asc[0x97]='p';
ebc2asc[0x98]='q';
ebc2asc[0x99]='r';
ebc2asc[0x9d]=CLSQUOTE;         /* '¯' */
ebc2asc[0x9f]=CIRCUMFLEX;       /* circumflex */
ebc2asc[0xa0]=TRADEMARK;        /* trademark symbol */
ebc2asc[0xa1]=ANGSTROM;         /* angstrom */
ebc2asc[0xa2]='s';
ebc2asc[0xa3]='t';
ebc2asc[0xa4]='u';
ebc2asc[0xa5]='v';
ebc2asc[0xa6]='w';
ebc2asc[0xa7]='x';
ebc2asc[0xa8]='y';
ebc2asc[0xa9]='z';
ebc2asc[0xab]=UMLAUT;           /* um laut */
ebc2asc[0xac]=TILDE;            /* tilde */
ebc2asc[0xad]='[';
ebc2asc[0xaf]=GRAVE;            /* grave */
ebc2asc[0xb9]=LPOLISHO;         /* lower polish o */
ebc2asc[0xbb]=BREVE;            /* breve */
ebc2asc[0xbc]=ACUTE;            /* acute */
ebc2asc[0xbd]=']';
ebc2asc[0xbf]=MACRON;           /* macron */
ebc2asc[0xc0]='{';
ebc2asc[0xc1]='A';
ebc2asc[0xc2]='B';
ebc2asc[0xc3]='C';
ebc2asc[0xc4]='D';
ebc2asc[0xc5]='E';
ebc2asc[0xc6]='F';
ebc2asc[0xc7]='G';
ebc2asc[0xc8]='H';
ebc2asc[0xc9]='I';
ebc2asc[0xd0]='}';
ebc2asc[0xd1]='J';
ebc2asc[0xd2]='K';
ebc2asc[0xd3]='L';
ebc2asc[0xd4]='M';
ebc2asc[0xd5]='N';
ebc2asc[0xd6]='O';
ebc2asc[0xd7]='P';
ebc2asc[0xd8]='Q';
ebc2asc[0xd9]='R';
ebc2asc[0xe0]='\\';
ebc2asc[0xe2]='S';
ebc2asc[0xe3]='T';
ebc2asc[0xe4]='U';
ebc2asc[0xe5]='V';
ebc2asc[0xe6]='W';
ebc2asc[0xe7]='X';
ebc2asc[0xe8]='Y';
ebc2asc[0xe9]='Z';
ebc2asc[0xf0]='0';
ebc2asc[0xf1]='1';
ebc2asc[0xf2]='2';
ebc2asc[0xf3]='3';
ebc2asc[0xf4]='4';
ebc2asc[0xf5]='5';
ebc2asc[0xf6]='6';
ebc2asc[0xf7]='7';
ebc2asc[0xf8]='8';
ebc2asc[0xf9]='9';


    argnext=1;
    if (argc < 3) {
        if (argc == 2 && strcmp(argv[1],"what") == 0) {
            printf("%s",cicopyr("?Utility MDL/MDLMF"));
            printf("\n");
            exit(1);
        }
        printf("%s",cicopyr("Utility MDL/MDLMF"));
        printf("\n");
        printf("mdlmf <dbname> [create=]<dbnout> [<option> [...]] \n");
        printf("\n");
        printf("options: {+/-}{control/leader/fields} \n");
        printf("         {from/to/loop/count/tell}=<n> \n");
        printf("         DP=<aa> \n");
        printf("         TA=NO \n");
        /*printf("         TA={YES|NO} \n");*/
        printf("         TOXLINE \n");
        printf("         APPEND  \n");
        printf("         zdecs=<zdecs> [ok]\n");
#if PROC
        printf("         proc={<fldupdat_fmt_spec>|@<file>} \n");
#endif
        printf("         gizmo=<gizmo_dbn>[,<taglist>] [gizp=<dbnx>] \n");
        printf("         now[ait] \n");
        printf("\n");
        printf("(version 14/06/2000)\n");
        printf("\n");
        exit(1);
    }
    printf("===> ISO Languages\n");

    dbnamep=argv[argnext++];
    dbnoutp=argv[argnext++];

    if ((fplog=fopen("mdlmf.log","w")) == NULL) fatal("mdlmf.log");
    if ((filog=fopen("mdlmf.chr","w")) == NULL) fatal("mdlmf.chr");
    
    printf("+++ input data base:  %s\n",dbnamep);


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

        if (strcmp(p,"trace") == 0) {
            parmtrace=1; 
            continue;
        }

        if (strncmp(p,"zdecs=",6) == 0) {
            parmzdecs=1; zdbnp=p+6;
            continue;
        }
        if (strcmp(p,"ok") == 0) {
            parmok=1; 
            continue;
        }

        if (strncmp(p,"proc?",5) == 0 || strncmp(p,"proc=",5) == 0) {
            q=p+5;
            if (!*q) fatal(p);
            mp_scan(NULL,q);
            continue;
        }

        if (strncmp(p,"gizp?",5) == 0 || strncmp(p,"gizp=",5) == 0) {
            parmgizp=p+5;
            continue;
        }

        if (strncmp(p,"gizmo?",6) == 0 || strncmp(p,"gizmo=",6) == 0) {
            q=p+6; if (!*q) fatal(p);
	    if (vgzrp) {        /* gizmo already defined */
		for (gizmap=vgzrp; gizmap->nextp; )
		    gizmap=gizmap->nextp;
                gizmread(q,&gizmap->nextp,(grec)?grec:(grec=nrecs));
                gizmap=gizmap->nextp;
            }
            else {              /* first gizmo definition */
		gizmread(q,&vgzrp,(grec)?grec:(grec=nrecs));
            }
            if (parmtrace) {
		for (gizmap=vgzrp; gizmap; gizmap=gizmap->nextp)
                    printf("+++ gizmo=%s \n",gizmap->gdbnp);
            }
	    parmgizmo=1;
            continue;
        }

        if (!strncmp(p,"count=",6)) {
            p=p+6;
            if ((sscanf(p,"%"_LD_,&parmcount)) != 1)
                fatal(p);
            continue;
        }
        if (!strncmp(p,"tell=",5)) {
            p=p+5;
            if ((sscanf(p,"%"_LD_,&parmtell)) != 1)
                fatal(p);
            continue;
        }

        if (!strncmp(p,"DP=",3)) {
            p=p+3;
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

        if (!strncmp(argv[i],"APPEND",6)) {
           parmappd=1;
           printf("+++ APPEND \n");
           continue;
        }

        if (!strncmp(argv[i],"TA=NO",5)) {
           parmTAn=1;
           nlmtag[NLMTAGTA] = 0;
           nlmtag[NLMTAGIS] = 0;
           nlmtag[NLMTAGCY] = 0;
           printf("+++ parmTAn=%d parmTAy=%d \n",parmTAn,parmTAy);
	   continue;
        }

        /*if (!strncmp(argv[i],"TA=YES",6)) {
           parmTAy=1;
           for (j=0; j<MAXTAG; ) nlmtag[j++]=0;
           nlmtag[NLMTAGJC] = 1;
           nlmtag[NLMTAGTA] = 1;
           nlmtag[NLMTAGIS] = 1;
           nlmtag[NLMTAGCY] = 1;
           printf("+++ parmTAn=%d parmTAy=%d \n",parmTAn,parmTAy);
           continue;
        } 
        */

        /* TOXLINE - AOT/MB/FJL 14/06/2000 */
        if (!strcmp(argv[i],"TOXLINE")) {
           parmTOX=1;
           nlmtag[NLMTAGCN] = 1;
           nlmtag[NLMTAGSO] = 1;
           nlmtag[NLMTAGGS] = 1;
           nlmtag[NLMTAGOD] = 1;
           nlmtag[NLMTAGPR] = 1;
           nlmtag[NLMTAGKW] = 1;
           nlmtag[TOXTAGAD] = 1;
           nlmtag[TOXTAGCY] = 1;
           nlmtag[TOXTAGEM] = 1;
           printf("+++ TOXLINE \n");
	   continue;
        }

        fatal(p);
    }


    printf("+++ output data base: %s\n",dbnoutp);


    /*
        inicializa bases de dados
    */
    if (strncmp(dbnoutp,"create=",7) == 0) {
        rc=recisis0((dbnoutp+=7));
        /* printf("+++ recisis0=%d\n",rc); */
    }

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

        /* printf("*** mfn=%"_LD_" \n",MFRmfn); */

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

        for (xdir=0, loop1=MFRnvf; loop1--; xdir++)
            for (p=FIELDP(xdir), loop2=DIRlen(xdir); loop2--; p++) {
                if (*p < EBCBLANK) {
                    printf("*** mfn=%"_LD_" tag=%d left=%d ebcdic_chr=%02x\n",
                        MFRmfn,DIRtag(xdir),loop2,*p);
                    *p = EBCBLANK;
                    continue;
                }
                if (ebc2asc[*p] == INVALID) {
                    if (*p == SVCHR)
                        continue;
                    if (*p == EVCHR)
                        continue;
                    if (*p == SSCHR)
                        continue;
                    printf("*** mfn=%"_LD_" tag=%d left=%d ebcdic_chr=%02x\n",
                        MFRmfn,DIRtag(xdir),loop2,*p);
                    *p = EBCBLANK;
                }
            }

        MHnocc=nocc(irec,NLMTAGMH);

        batchp=buffup; *batchp=NULL;

        for (xdir=tag=0, loop1=MFRnvf; loop1--; xdir++) {

            if (DIRtag(xdir) < MINTAG || DIRtag(xdir) > MAXTAG) {
                printf("*** mfn=%"_LD_" tag=%d \n",
                    MFRmfn,DIRtag(xdir));
                fatal("mdlmf/tag");
            }

            if (nlmtag[DIRtag(xdir)] == 0)
                continue;

            if (DIRtag(xdir) != tag) {
                tag=DIRtag(xdir);
                iocc=1;
            }
            else
                iocc++;

            if (DIRlen(xdir) < 1) {
                printf("*** mfn=%"_LD_" tag=%d occ=%d len=%d \n",
                    MFRmfn,DIRtag(xdir),iocc,DIRlen(xdir));
                if (DIRlen(xdir) == 0)
                    continue;
                fatal("mdlmf/length");
            }


            if (tag != NLMTAGMH) {

                switch (tag) {

                case NLMTAGAB:
                    sprintf(batchp,"A%3d%c",DIRtag(xdir),FLDUPDLM); batchp+=5;
                    p=FIELDP(xdir); loop2=DIRlen(xdir);
                    if (*(p+loop2-1) == EBCPERIOD)
                        loop2--;
                    for (; loop2--; )    { 
                        if ( ebc2asc[*p] != INVALID)  
                           *batchp++ = ebc2asc[*p++]; 
                        else    {
                           fprintf(filog,"+++ input data base: %s *** mfn=%"_LD_" tag=%d left=%d ebcdic_chr=%02x\n", dbnamep,MFRmfn,DIRtag(xdir),loop2,*p);
                           *p++; }
                                         }

                    if (fieldn(irec,NLMTAGAA,1)) {
                        sprintf(batchp," (Au)"); batchp+=5;
                    }
                    *batchp++ = FLDUPDLM;
                    break;


                case NLMTAGTI:
                    if (fieldn(irec,NLMTAGTT,1))
                        sprintf(batchp,"A%3d%c",NEWTAGTI_130,FLDUPDLM);
                    else
                        sprintf(batchp,"A%3d%c",DIRtag(xdir),FLDUPDLM);
                    batchp+=5;
                    p=FIELDP(xdir); loop2=DIRlen(xdir);
                    if (*(p+loop2-1) == EBCPERIOD)
                        loop2--;
                    if (*p == EBCOSQBR)
                        if (*(p+loop2-1) == EBCCSQBR) {
                            p++; loop2--;
                            loop2--;
                        }

                    for (; loop2--; )    { 
                        if ( ebc2asc[*p] != INVALID)  
                           *batchp++ = ebc2asc[*p++];  
                        else {
                         fprintf(filog, "+++ input data base: %s *** mfn=%"_LD_" tag=%d left=%d ebcdic_chr=%02x\n", dbnamep,MFRmfn,DIRtag(xdir),loop2,*p);
                        *p++;  }
                                         }

                    *batchp++ = FLDUPDLM;
                    break;

                case NLMTAGRN:
                    sprintf(batchp,"A%3d%c",DIRtag(xdir),FLDUPDLM);
                    batchp+=5;
                    *batchp++ = '^'; *batchp++ = 'a';
                    p=FIELDP(xdir); loop2=DIRlen(xdir);
                    if (*(p+loop2-1) == EBCCPARE)
                        loop2--;
                    for (; loop2; ) {
                        if (*p == EBCOPARE)
                            break;
                        loop2--;
                        *batchp++ = ebc2asc[*p++];
                    }
                    if (*(batchp-1) == ' ') { batchp--; }
                    if (*p == EBCOPARE) { loop2--; p++; }
                    *batchp++ = '^'; *batchp++ = 'b';
                    for (; loop2--; )
                        *batchp++ = ebc2asc[*p++];
                    *batchp++ = FLDUPDLM;
                    break;

                case NLMTAGTT:
                    sprintf(batchp,"A%3d%c",DIRtag(xdir),FLDUPDLM); batchp+=5;
                    p=FIELDP(xdir); loop2=DIRlen(xdir);
                    if (*(p+loop2-1) == EBCPERIOD)
                        loop2--;
                    for (; loop2--; )    { 
                        if ( ebc2asc[*p] != INVALID)  
                           *batchp++ = ebc2asc[*p++]; 
                        else    {
                           fprintf(filog,"+++ input data base: %s *** mfn=%"_LD_" tag=%d left=%d ebcdic_chr=%02x\n", dbnamep,MFRmfn,DIRtag(xdir),loop2,*p);
                           *p++; }
                                         }
                    *batchp++ = FLDUPDLM;
                    break;

                case NLMTAGAD:
                    sprintf(batchp,"A%3d%c",DIRtag(xdir),FLDUPDLM); batchp+=5;
                    p=FIELDP(xdir); loop2=DIRlen(xdir);
                    if (*(p+loop2-1) == EBCPERIOD)
                        loop2--;
                    for (; loop2--; )    { 
                        if ( ebc2asc[*p] != INVALID)  
                           *batchp++ = ebc2asc[*p++]; 
                        else    {
                           fprintf(filog,"+++ input data base: %s *** mfn=%"_LD_" tag=%d left=%d ebcdic_chr=%02x\n", dbnamep,MFRmfn,DIRtag(xdir),loop2,*p);
                           *p++; }
                                         }
                    *batchp++ = FLDUPDLM;
                    break;

                case NLMTAGCY:
                case NLMTAGAU:
                case NLMTAGPS:
                case NLMTAGPT:
                case NLMTAGCO:
                    sprintf(batchp,"A%3d%c",DIRtag(xdir),FLDUPDLM); batchp+=5;
                    for (p=FIELDP(xdir), loop2=DIRlen(xdir); loop2--; ) {
                        if (*p == SVCHR)
                            break;
                        if (*p == SSCHR) {
                            *batchp++ = '/'; p++;
                        }
                        else
                            *batchp++ = ebc2asc[*p++];
                    }
                    *batchp++ = FLDUPDLM;
                    break;

                case NLMTAGEA:
                    sprintf(batchp,"A%3d%c",DIRtag(xdir),FLDUPDLM); batchp+=5;
                    sprintf(batchp,"En"); batchp+=2;
                    *batchp++ = FLDUPDLM;
                    break;

                case NLMTAGDP:
                    sprintf(batchp,"A%3d%c",DIRtag(xdir),FLDUPDLM); batchp+=5;
                    for (p=FIELDP(xdir), loop2=DIRlen(xdir); loop2--; )
                        *batchp++ = ebc2asc[*p++];
                    *batchp++ = FLDUPDLM;
#if ENAB650
                    q=DPin;
                    for (p=FIELDP(xdir), loop2=DIRlen(xdir); loop2--; ) {
                        *q++ = toupper(ebc2asc[*p++]);
                    }
                    *q = '\0';
                    if (DPconvert(DPin,DIRlen(xdir))) {
                        sprintf(batchp,"A%3d%c",NEWTAGDP_650,FLDUPDLM);
                        batchp+=5;
                        strcpy(batchp,DPok); batchp+=strlen(batchp);
                        *batchp++ = FLDUPDLM;
                    }
                    if (*DPerrp != '\0') {
                        printf("*** mfn=%"_LD_" tag=%d DP = '",
                            MFRmfn,DIRtag(xdir));
                        for (p=FIELDP(xdir), loop2=DIRlen(xdir); loop2--; p++)
                            printf("%c",ebc2asc[*p]);
                        printf("' [%s]\n",DPerrp);
                    }
#endif
                    break;

#define ENABLA1 1
#define ENABLA2 0
#if ENABLA1
                case NLMTAGLA:
                    sprintf(batchp,"A%3d%c",DIRtag(xdir),FLDUPDLM); batchp+=5;
#if ENABLA2
                    q=LAin;
                    for (p=FIELDP(xdir), loop2=DIRlen(xdir); loop2--; )
                        *q++ = toupper(ebc2asc[*p++]);
                    for (p=LAtab[0]; *p; p+=(LATABSIZ+1)) {
                        if ((i=strncmp(LAin,p,LANLMSIZ)) == 0) {
                            for (p+=(LANLMSIZ+1), loop2=LAISOSIZ; loop2--; )
                                *batchp++ = *p++;
                            break;
                        }
                        if (i < 0) /* gt */
                            break;
                    }
                    if (*(batchp-1) == FLDUPDLM) {
                        printf("*** mfn=%"_LD_" tag=%d LA = '",
                            MFRmfn,DIRtag(xdir));
                        for (p=FIELDP(xdir), loop2=DIRlen(xdir); loop2--; ) {
                            printf("%c",ebc2asc[*p]);
                            *batchp++ = ebc2asc[*p++];
                        }
                        printf("'\n");
                    }
#else
                    for (p=FIELDP(xdir), loop2=DIRlen(xdir); loop2--; )
                        *batchp++ = ebc2asc[*p++];
#endif
                    *batchp++ = FLDUPDLM;
                    break;
#endif

                default:
                    if (nlmtag[tag] != 1 && nlmtag[tag] != 2) {
                        printf("*** mfn=%"_LD_" tag=%d \n",
                            MFRmfn,DIRtag(xdir));
                        printf(" '");                           /* health */
                        for (p=FIELDP(xdir), loop2=DIRlen(xdir); loop2--; )
                            printf("%c",ebc2asc[*p++]);
                        printf("' \n");                         /* health */
                        fatal("mdlmf/switch");
                    }
                    sprintf(batchp,"A%3d%c",DIRtag(xdir),FLDUPDLM); batchp+=5;
                    p=FIELDP(xdir); loop2=DIRlen(xdir);         /* health */
                    if (*(p+loop2-1) == EBCPERIOD)
                        loop2--;                                /* health */
                    for (; loop2--; )
                        *batchp++ = ebc2asc[*p++];
                    *batchp++ = FLDUPDLM;

                } /* end of switch */

                tag=0;                                 /* ????? */
                continue; /* next dir entry */
            }


#if TRACE
printf("iocc=%d/%d '",iocc,MHnocc);
for (p=FIELDP(xdir), i=DIRlen(xdir); i--; p++)
    putchar(ebc2asc[*p]);
printf("'\n");
#endif

            if (iocc == 1) {
                sprintf(batchp,"A%3d%c",DIRtag(xdir),FLDUPDLM); batchp+=5;
            }
            else {
                if (!parmzdecs) {
		    sprintf(batchp,"; "); batchp+=2;
		}
                else { 
		    sprintf(batchp,";"); batchp+=1;
		    zsave+=1;
		}
            }
	    
	    if (parmzdecs) zdp=batchp;

            for (zx0=0, p=FIELDP(xdir), loop2=DIRlen(xdir); loop2; ) {
                if (*p == SSCHR)
                    break;
                *batchp++ = ebc2asc[*p++]; 
                loop2--; zx0++;
            }
	    totz+=zx0;

	    if (parmzdecs) {
		*batchp='\0'; zdlookup++;
		if (parmtrace) printf("+++ %d: %s\n",DIRtag(xdir),zdp);
		for (z1p=zdp, z2p=zkey; *z1p; ) *z2p++ = isisuctab[*z1p++];
		*z2p='\0';
		TERM(itrm,zdbnp,zkey);
		if (TRMrc == RCNORMAL) {
		    if (posting(itrm,0L) > 1) {
		        fprintf(stderr,"*** key=%s\n",TRMkey);
			fatal("mdlmf/zdecs/more than 1 posting");
		    }
		    batchp=zdp;
		    sprintf(batchp,"%"_LD_,posting(itrm,1L));
		    batchp+=(zx1=strlen(batchp));
		    if (parmtrace) {
                       printf("+++ %d: %s -> %"_LD_"\n",DIRtag(xdir),zkey,TRMpmfn);
		    }
		    zsave+=(zx0-zx1);
		}
		else {
		    if (!parmok) {
		      printf("*** mfn=%"_LD_" tag=%d key=%s -> NOT FOUND \n",
			                             MFRmfn,DIRtag(xdir),zkey);
		      fprintf(fplog,"*** mfn=%"_LD_" tag=%d key=%s -> NOT FOUND \n",
			      MFRmfn,DIRtag(xdir),zkey);
		    }
		    zdnotf++;
		}
	    }


            if (*p != SSCHR) {
                /* fatal("1\\SS\\2"); */
                printf("1\\SS\\2 - mfn=%"_LD_" tag=%d occ=%d\n",
                    MFRmfn,DIRtag(xdir),iocc);
                if (iocc == MHnocc) *batchp++ = FLDUPDLM;
                continue;
            }
            p++; loop2--;
            p++; loop2--;


            if (*p != SSCHR) {
                /* fatal("2\\SS\\3"); */
                printf("2\\SS\\3 - mfn=%"_LD_" tag=%d occ=%d\n",
                    MFRmfn,DIRtag(xdir),iocc);
                if (iocc == MHnocc) *batchp++ = FLDUPDLM;
                continue;
            }
            p++; loop2--;
            p++; loop2--;


            if (loop2 < 0) {
                printf("loop2<0 - mfn=%"_LD_" tag=%d occ=%d loop2=%d \n",
                    MFRmfn,DIRtag(xdir),iocc,loop2);
                if (iocc == MHnocc) *batchp++ = FLDUPDLM;
                continue;
            }


            if (*p == SSCHR) {
                p++; loop2--;
                *batchp++ = '/';
                while (loop2--) {
                    if (*p == SVCHR)
                        break;
                    if (*p == EVCHR)
                        break;
                    *batchp++ = ebc2asc[*p++];
                }
            }

            if (iocc == MHnocc) *batchp++ = FLDUPDLM;

        } /* next dir entry */


        *batchp='\0';

#if TRACE
printf("buffup='%s'\n",buffup);
#endif

        RECORD(upirec,dbnoutp,VMF0nxtmfn(upcrec)); /* get null PDEL record */
        MFRstatus=ACTIVE;

        p=fldupdat(upirec,buffup);
        if (p) {
            printf("*** mfn=%"_LD_"  fldupdat='%s'\n",mfn,p);
            fatal("mdlmf/fldupdat");
        }

#if PROC
        if (mp_n) mp_edit(upirec,0,mp_n-1);
#endif /* PROC */

        gmfrl0=VMFRmfrl(upirec);
	totg+=VMFRmfrl(upirec)-VMFRbase(upirec);
        if (parmgizmo)
	    for (gizmap=vgzrp; gizmap; gizmap=gizmap->nextp)
	        recgizmo(upirec,gizmap);
        gsave+=gmfrl0-VMFRmfrl(upirec);


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
            for (p=FIELDP(xdir), loop2=DIRlen(xdir); loop2--; p++)
#if SHOWHEX
                if (*p < ' ' || *p > '}') printf("|x%02x|",*p); else
#endif
                printf("%c",*p);
            printf("|\n");
        }
}


        for (xdir=0, loop1=MFRnvf; loop1--; xdir++)
            for (p=FIELDP(xdir), loop2=DIRlen(xdir); loop2--; p++)
                if (*p == INVALID) {
                    printf("*** mfn=%"_LD_" tag=%d left=%d chr=%02x",
                        MFRmfn,DIRtag(xdir),loop2,*p);
                    printf(" '");                               /* health */
                    for (p=FIELDP(xdir), loop2=DIRlen(xdir); loop2--; p++)
                        printf("%c",(*p == INVALID) ? '?' : *p);
                    printf("' \n");                             /* health */
                    fatal("mdlmf/invalid");
                }

        /* TOXLINE - AOT/MB/FJL 14/06/2000 */
	if (parmTOX)
            for (xdir=0, loop1=MFRnvf; loop1--; xdir++) {
	        tag=DIRtag(xdir);
	        if (tag == TOXTAGN1) fatal("mdlmf/TOXLINE/TOXTAGN1");
	        if (tag == TOXTAGN2) fatal("mdlmf/TOXLINE/TOXTAGN2");
	        if (tag == TOXTAGN3) fatal("mdlmf/TOXLINE/TOXTAGN3");
	    }

if (parmcopy) {
        rc=recupdat(upcrec,upirec);
        if (rc) {
            printf("*** recupdat=%d\n",rc);
            fatal("mdlmf/recupdat");
        }
}

        count++;
	totmfrl+=VMFRmfrl(upirec);
        totleader+=LEADER;
	totdir+=VMFRbase(upirec)-LEADER;
	totdata+=VMFRmfrl(upirec)-VMFRbase(upirec);
	
        if (parmtell)
            if (!(count % parmtell)) {
              tot0=totmfrl;
	      tot1=zsave+gsave;
              fprintf(stderr,
		/*
                    "+++ %"_LD_" mfn=%"_LD_" mfrl=%d=%d+%d(%d)+%d ",
                        count,MFRmfn,MFRmfrl,
                        LEADER,MFRbase-LEADER,MFRnvf,MFRmfrl-MFRbase);
		*/
                    "+++ %"_LD_"  w=%"_LD_" ",
                        count,totmfrl);
              if (parmzdecs) {
	          fprintf(stderr," z=%"_LD_"=%d%% ",
	                zsave,(int)(10*(zsave*100/totz)+5)/10);
                  if (zdnotf) fprintf(stderr,"%"_LD_"! ",zdnotf);
	      }
              if (parmgizmo) fprintf(stderr," g=%"_LD_"=%d%% ",
	      		gsave,(int)(10*(gsave*100/totg)+5)/10);
              fprintf(stderr," s=%"_LD_"=%d%% ",
	      		tot1,(int)(10*(tot1*100/tot0)+5)/10);
	      fprintf(stderr,"\n");
              fprintf(fplog,
                    "+++ %"_LD_"  w=%"_LD_" [%"_LD_"+%"_LD_"+%"_LD_"] ",
                        count,totmfrl,
			totleader,totdir,totdata);
              if (parmzdecs) fprintf(fplog," z=%"_LD_"=%d%% [%"_LD_"-%"_LD_"] ",
	                zsave,(int)(10*(zsave*100/totz)+5)/10,
			zdlookup,zdnotf);
              if (parmgizmo) fprintf(fplog," g=%"_LD_"=%d%% ",
	      		gsave,(int)(10*(gsave*100/totg)+5)/10);
              fprintf(fplog," s=%"_LD_"=%d%% ",
	      		tot1,(int)(10*(tot1*100/tot0)+5)/10);
	      fprintf(fplog,"\n");
            }

        if (parmwait) {
            printf("..");
            if (getchar() != '\n')
                break;
        }

        if (count >= parmcount)
            break;
    }

    if (parmgizp) dbxstorp(tmpnam(NULL))->dbxvgzrp=vgzrp;
    
    mxexit(0);
}


#if ENAB650
unsigned char *DPconvert(errp,n)
unsigned char *errp;
int n;
{
    int loop,month;
    unsigned char *p,*q,*mop,edit[3];

    static unsigned char motab[] =
                                { "JANFEBMARAPRMAYJUNJULAUGSEPOCTNOVDEC" };

    sprintf(DPok,"00000000");
    DPerrp=errp;

    if (n < 4)
        return(NULL);

    for (p=DPin, q=DPok, loop=4; loop--; )
        if (isdigit(*p))
            *q++ = *p++;
        else
            return(NULL);
    n-=4;                                       /* space/month/space/day */
    DPerrp+=4;

    if (n >= 3)
        if (*p == '-')
            if (isdigit(*(p+1)))
                if (isdigit(*(p+2))) {
                    *(q-2) = *(p+1);
                    *(q-1) = *(p+2);
                    p+=3;
                    n-=3;                       /* space/month/space/day */
                    DPerrp+=3;
                }

    if (n == 0)
        return(DPok);

    if (*p != ' ')
        return(NULL);

    p++; n--;                                   /* month/space/day */
    DPerrp++;

    if (n < 3)
        return(DPok);

    for (mop=motab, month=1; *mop; month++, mop+=3)
        if (*mop == *p)
            if (*(mop+1) == *(p+1))
                if (*(mop+2) == *(p+2)) {
                    sprintf(edit,"%02d",month);
                    *q++ = edit[0];
                    *q++ = edit[1];
                    p+=3;
                    n-=3;                       /* space/day */
                    DPerrp+=3;
                    break;
                }

    if (!*mop)
        return(DPok);

    if (*p == '-') {
        p++;
        n--;
        DPerrp++;
        for (mop=motab, month=1; *mop; month++, mop+=3)
            if (*mop == *p)
                if (*(mop+1) == *(p+1))
                    if (*(mop+2) == *(p+2)) {
                        sprintf(edit,"%02d",month);
                        *(q-2) = edit[0];
                        *(q-1) = edit[1];
                        p+=3;
                        n-=3;                       /* space/day */
                        DPerrp+=3;
                        break;
                    }
        if (!*mop)
            return(NULL);
    }

    if (n == 0)
        return(DPok);

    if (*p != ' ')
        return(NULL);

    p++; n--;                                   /* day */
    DPerrp++;

    if (!isdigit(*p))
        return(DPok);

    if (*(p+1) == '-') {
        p+=2; n-=2;
        DPerrp+=2;
    }
    else
        if (isdigit(*(p+1)))
            if (*(p+2) == '-') {
                p+=3; n-=3;
                DPerrp+=3;
            }

    if (!isdigit(*p))
        return(DPok);

    if (isdigit(*(p+1))) {
        *q++ = *p++;
        *q++ = *p++;
        *q = '\0';
        n-=2;
        DPerrp+=2;
    }
    else {
        *q++ = '0';
        *q++ = *p++;
        *q = '\0';
        n--;
        DPerrp++;
    }

    return(DPok);
}
#endif



/* ============================== mxexit ================================== */

void mxexit(rc)
int rc;
{
    int i,n,igiz;
    LONGX idbx,nused;
    char *p,*q,usedbn[CIMPL+1],line[BUFSIZ+1];
    VGIZPSTRU *gizmap;
    GIZMSTRU *gizmhp,*gizmp;

    if ((p=parmgizp) != NULL) {

        if (!upcrec) recallok((upcrec=nrecs),(LONGX)sizeof(M0STRU));
        if (!upirec) recallok((upirec=nrecs),MAXMFRL);

        for (idbx=0L; idbx < ndbxs; idbx++) {
	    for (gizmap=VDBXvgzrp(idbx), igiz=0; gizmap; igiz++,
                                                gizmap=gizmap->nextp) {
                sprintf(usedbn,"%s%d",parmgizp,igiz);
                recisis0(usedbn); record(upcrec,usedbn,0L);
                nused=0;
		if (parmtrace) printf("+++ %s/giz%d=%s\n",
					VDBXname(idbx),igiz,gizmap->gdbnp);
                for (i=0; i <= 255; i++) {
                    gizmhp=gizmap->ghdrp[i];
                    if (!gizmhp) continue;
                    for (gizmp=gizmhp; gizmp; gizmp=gizmp->nextp) {
                        if (gizmp->nused == 0) continue;
                        nused++;
                        record(upirec,usedbn,VMF0nxtmfn(upcrec));
                        VMFRstatus(upirec)=ACTIVE; q=line;
                        sprintf(q,"A10 #%"_LD_"#",gizmp->nused);    q+=strlen(q);
                        sprintf(q,"H1   %d ",gizmp->isize);     q+=strlen(q);
                        memmove(q,gizmp->ipatt,n=gizmp->isize);  q+=n;
                        sprintf(q,"H2   %d ",gizmp->osize);     q+=strlen(q);
                        memmove(q,gizmp->opatt,n=gizmp->osize);  q+=n;
                        sprintf(q,"A31 #%d#",gizmp->isize);     q+=strlen(q);
                        sprintf(q,"A32 #%d#",gizmp->osize);     q+=strlen(q);
                        *q = '\0';
                        if ((p=fldupdat(upirec,line)) != NULL) fatal(p);
                        recupdat(upcrec,upirec);
                    }
                }
                if (parmtrace) printf("+++ %s = %"_LD_" entries\n",usedbn,nused);
                dbxflush(usedbn);
            }
        }
    }

    exit(rc);
}


/* ========================== mp_scan/mp_edit ============================= */
#if PROC

void mp_scan(gidbnp,specp)
char *gidbnp;
char *specp;
{

    if (mp_n >= MAXNPPP) fatal("MAXNPPP");

    mp_areap=loadfile(gidbnp,'@',specp,mp_areap,fmtasize+1,'\0');
    if (!mp_areap) fatal(specp);

    if (parmtrace) {
        printf("+++ proc=%c",OPENFCHR); printf("%s",mp_areap);
        printf("%c ..",CLOSFCHR); if (getchar() != '\n') mxexit(0);
    }

    if (fmt_gener(&mp_pgmp[mp_n],mp_areap)) fatal(specp+fmt_errof);

    mp_n++;
}

void mp_edit(irec,i1,i2)
LONGX irec;
int i1;
int i2;
{
    int ippp;
    LONGX lns;
    UBYTE *p;
    RECSTRU *recp;
#define MXRELPROC 1
#if MXRELPROC
    UBYTE c;
    LONGX ld,x,xnocc;
    UWORD tag1,tag2;
#endif

    recp=vrecp[irec];

    for (ippp=i1; ippp <= i2; ippp++) {

        *mp_areap = '\0';                       /* call format */
        lns=fmt_inter(mp_pgmp[ippp],irec,fmtasize,mp_areap,fmtasize);

        if (parmtrace)
            printf("+++ proc=%"_LD_"=%s.\n",lns,mp_areap);

#if MXRELPROC
        if (mp_areap[0] == '.' && toupper(mp_areap[1]) == 'A') {
            for (p=mp_areap+2, ld=0; *p; p++)
                if (isdigit(*p)) ld=10*ld+(int)*p-(int)'0';
                else break;
            tag1=(UWORD)ld;
            if (!tag1) fatal(mp_areap);
            c= *p;
            switch (c) {
            case '+':
            case '-':
                for (p++, ld=0; *p; p++)
                    if (isdigit(*p)) ld=10*ld+(int)*p-(int)'0';
                    else break;
                tag2=(UWORD)ld;
                if (!tag2) fatal(mp_areap);
                if (*p) fatal(mp_areap);
                p=mp_areap;
                sprintf(p,"D%"_LD_,(LONGX)tag1); p+=strlen(p);
                for (xnocc=nocc(irec,tag2), x=1; x <= xnocc; x++) {
                    sprintf(p,"A%"_LD_"/%"_LD_"/",(LONGX)tag1,(c == '+')?x:xnocc-x+1);
                    p+=strlen(p);
                }
                break;
            default:
                fatal(mp_areap);
            }
        }
#endif
        if ((p=fldupdat(irec,mp_areap)) != NULL)
            fatal(p);

        if (parmtrace)
            prtfields(recp,MFRmfn);
    }
}

#endif /* PROC */
