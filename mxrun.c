/* ------------------------------ mxrun.c --------------------------------- */

#include <stdio.h>
#include <string.h>
/* #include <ctype.h> */

#if 1
#if CIAPI
#include "ciapi.h"   /* CISIS Interface API header file -
                  includes cisis, implements cirun.h and ci*.c using ci*.?pp */
#if !MXFUN
extern void *ciapip;
#endif /* MXFUN */

#else /* CIAPI */
#include "cisis.h"   /* CISIS Interface header file */
#endif /* CIAPI */
#include "mx.h"   /* CISIS Interface runtime declarations */
#else
#include "\cisis\include\cisis.h"   /* CISIS Interface header file */
#include "\cisis\mx\mx.h"           /* MX Program header file */
#endif


/* ------------------------------ mx.c ------------------------------------- */

#if MXFUN
int mxinit(xmxpp)
void **xmxpp;
{
    MXFUN_XMX *xmxp;
    *xmxpp=NULL;
    xmxp=(MXFUN_XMX *)ALLOC((ALLOPARM)sizeof(MXFUN_XMX));
    if (xmxp == (MXFUN_XMX *)ALLONULL) return(1);
    memset((char *)xmxp,sizeof(MXFUN_XMX),0x00);
    *xmxpp=(void *)xmxp;
#include "mxfun.c"
    return(0);
}
#else /* MXFUN */

#if OPEN30
union REGS      d33_regs;
struct SREGS    d33_sregs;
unsigned short  newax;
#endif

char *parmprologp=NULL;
char *parmepilogp=NULL;
LONGX  irecepilog=0L;
#if CICGI
LONGX wwrec1=0L,wwrec2=0L;
char *cgixpok=NULL;
#endif /* CICGI */

FMT_CODE *pgmp;
char *fmtarea=NULL;
LONGX fmtlw=79;
LONGX fmtlines;
LONGX fmtasize=MAXMFRL;
FILE *fp;

char *ptabtable=NULL;
int   ptabwidth=100; //LE2+LE2;
LONGX  ptabcategories=300000;
LONGX  ptabmaxprim;
LONGX *ptabCOUNT=NULL;  /* n(t) */

LONGX parmtell = 0;
LONGX parmload = 0;
int parmtrace = 0;
int parmcopy  = 0;

char *parmdupp = NULL;
int parmdupx = 0;

LONGX *tagfreq;

int parmiso1  = 0;
int parmiso2  = 0;
FILE *fpfix = NULL;    /*parmiso2=2 */

#if RECXPND
int parmxpn01 = 0;
int parmxpn02 = 0;
#endif
#if MXGIZP
char *parmgizp = NULL;
int parmgizhp = 0;
#endif

char *prompt1p = "..";
char *prompt2p = "->";

char promptxx[2];

char line[LINSIZE];              /* recfgets() + working */
char prmbuff[PRMSIZE];           /* in= */

FILE *fpseq = NULL;
FILE *fpprm = NULL;

FILE *vfpprm[MAXNFPRM];
int nfpprms=0;

int tmpseq=0;                   /* mxexit() */
char *mx_bup=NULL;/*free*/                   /* global */
LONGX mx_buplen=0L;              /* global */

LONGX upcrec=0L;                 /* global */
LONGX upirec=0L;                 /* global */

#if MXFULLINV
char *fullinv_ifnp=NULL;
char *fullinv_ln1p=NULL;
char *fullinv_ln2p=NULL;
char *fullinv_lk1p=NULL;
char *fullinv_lk2p=NULL;
int fullinv_pstflag=IFUPISIS;
int fullinv_reset=1;
int fullinv_balan=1;
int fullinv_keep=0;
#if LIND
LONGX fullinv_maxmfn=0L;
#endif
char *fullinv_master=NULL;
#endif

#if MXIFUPD
LONGX mstcrec=0L;/*free*/
LONGX mstirec=0L;/*free*/
char *ifnp=NULL;
int svdifupx=0;
int svdifupy=0;
#endif

#if MXIFUPD
int pstflag=IFUPISIS;
int endup=IFUPCORE;
#endif

#if MXIFUPD || MXFST
char *stwp=NULL;
FST_CODE *fstpgmp=NULL;
char *lnk1p=NULL;
char *lnk2p=NULL;
LONGX maxlk1=1000;
LONGX maxlk2=500;
#endif

PRMSTRU *vprmp[MAXNPRM];        /* pointers to the entries      */
LONGX nprms=0;                   /* actual #entries              */
LONGX maxnprm=MAXNPRM;           /* runtime value                */


/* ------------------------------ mx.h ------------------------------------- */

#if RECXPND
#endif

#if RECGIZM
#endif




/* ------------------------------ mxbol.c --------------------------------- */


char *boolqryp=NULL;
LONGX boolridx=0;
LONGX boolhits=0;
LONGX boolh=0;

char *booldbnp = NULL;          /* b4x query dbn */
LONGX booltell=1L;               /* 0L = no msg */
LONGX booltlir=0L;               /* b40 invx */

#if CIB71
b7_CIB7 *cib7p=NULL;
char booltmpnam[CIMPL+1]={'\0'};    /* b72 MFQTHMEM tmp file */
#else /* CIB71 */
#if LIND || CIB64
LONGX boolnby1=0L;               /* b62 nbytes1 */
LONGX boolnbyb=0L;               /* b62 b6bufsiz */
LONGX boolnby0=0L;               /* b62 b5irxbyts0 */
char booltmpnam[CIMPL+1]={'\0'};    /* b62 MFQTHMEM tmp file */
int boolerrno=0;                /* b62 errno */
#else
LONGX boolnby2=MAXMFRL;          /* b42 nbytes2 */
#endif
#endif /* CIB71 */

/* ------------------------------ mxrel.c --------------------------------- */

/* ========================== mp_scan/mp_edit ============================= */

FMT_CODE *mp_pgmp[MAXNPPP];
int mp_n = 0;
char *mp_areap=NULL;

/* ====================== mj_scan/mj_edit/mj_join ========================= */

MJ_TAGS *mj_vtagp[MAXNJJJ];
MJ_STWS *mj_vstwp[MAXNJJJ];
FMT_CODE *mj_pgmp[MAXNJJJ];
LONGX mj_plustag[MAXNJJJ]; /* AOT 23/04/2005 */
LONGX mj_ridx[MAXNJJJ];
LONGX mj_tidx=0;
char *mj_areap=NULL;
char *mj_buffup=NULL;
int mj_pjchk[MAXNJJJ];
int mj_n=0;
LONGX mj_jmax=LONGX_MAX;
UWORD mj_jtag=32000;

/* ------------------------------ mxtxt.c --------------------------------- */

int  mxtext_found=0;
int  mxtext_fdir;
int  mxtext_foff;
int  mxtext_pshow=0;

#endif /* MXFUN */




/* ============================== mxexit ================================== */

int mxexit(xmxp,rc)
void *xmxp;
int rc;
{
#if MXIFUPD
    DBXSTRU *dbxp;
#endif
#if RECGIZM
#if MXGIZP
    int i,n,igiz;
    LONGX idbx,nused;
    char *p,*q,usedbn[CIMPL+1];
    VGIZPSTRU *gizmap;
    GIZMSTRU *gizmp;
#endif
#endif

#if CICGI
//    if (parmepilogp && wwrec2) /* then wwrec1 was alloced */ {
//        if (recfmt(wwrec1,rec_maxmfrl,parmepilogp,mx_bup,mx_buplen) < 0) {
    if (parmepilogp && irecepilog) /* then wwrec1 was alloced */ {
        if (recfmt(irecepilog,rec_maxmfrl,parmepilogp,mx_bup,mx_buplen) < 0) {
            printf("Content-type: text/plain\n\nMX fatal: %s",parmepilogp);
        }
        if (parmpftoutfp && 0) fprintf(parmpftoutfp,"%s",mx_bup); else
        printf("%s",mx_bup);
    }
    else if (cgixpok) printf("\n</body></html>\n");
#else /* CICGI */
    if (parmepilogp) if (irecepilog) {
        if (recfmt(irecepilog,rec_maxmfrl,parmepilogp,mx_bup,mx_buplen) < 0) {
            printf("Content-type: text/plain\n\nMX fatal: %s",parmepilogp);
        }
        if (parmpftoutfp && 0) fprintf(parmpftoutfp,"%s",mx_bup); else
        printf("%s",mx_bup);
    }
#endif /* CICGI */

#if FREEptabjdi
#if CICPP
     if (ptabjditrmp) {
         delete ptabjditrmp;
         ptabjditrmp=NULL;
         ptabtable=NULL;
     if (ptabjdirecp) {
         delete ptabjdirecp;
         ptabjdirecp=NULL;
         ptabtable=NULL;
     }
#else /* CICPP */
     if (ptabjditrm_prat != (-1L)) if (vtrmp[ptabjditrm_prat]!=NULL) {
         FREE(vtrmp[ptabjditrm_prat]); vtrmp[ptabjditrm_prat]=NULL; ntrms--;
         ptabjditrm_prat=(-1L);
         ptabtable=NULL;
     if (ptabjdirec_prat != (-1L)) if (vrecp[ptabjdirec_prat]!=NULL) {
         FREE(vrecp[ptabjdirec_prat]); vrecp[ptabjdirec_prat]=NULL; nrecs--;
         ptabjdirec_prat=(-1L);
         ptabtable=NULL;
     }
#endif /* CICPP */
#endif //FREEptabjdi

    if (ptabtable) {
        LONGX hidx=0;
        char *h=ptabtable;
        int hwidth=(ptabwidth+1);       
        LONGX *n=ptabCOUNT;
        LONGX rev;
        for (; hidx < ptabmaxprim; hidx++, h+=hwidth, n++) {
            if (!*h) continue;
            if (!*n) continue;
            rev=999999999L - *n;
            printf("%09"_LD_"|%"_LD_"|%s\n",rev,*n,h); //table+hidx*hwidth
            if (parmtrace) printf("[%9"_LD_"] %9"_LD_" %s\n",hidx,n,h); //table+hidx*hwidth
        }
    }
    
#if RECGIZM
#if MXGIZP
    if ((p=parmgizp) != NULL) {

        if (!upcrec)
            recallok((upcrec=nrecs),(LONGX)sizeof(M0STRU));
        if (!upirec)
            recallok((upirec=nrecs),MAXMFRL);

        for (idbx=0L; idbx < ndbxs; idbx++) {
            if (idbx > 0)
                break;
            for (gizmap=VDBXvgzrp(idbx), igiz=0; gizmap; igiz++,
                                                gizmap=gizmap->nextp) {
                sprintf(usedbn,"%s%d",parmgizp,igiz);
                recisis0(usedbn); record(upcrec,usedbn,0L);
                nused=0;
                fprintf(stderr,"+++ %s/giz%d=%s\n",VDBXname(idbx),igiz,gizmap->gdbnp);
                for (i=0; i <= 255; i++) {
                    gizmp=gizmap->ghdrp[i];
                    if (!gizmp)
                        continue;
                    for (; gizmp; gizmp=gizmp->nextp) {
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
#if MXGIZHP
                        if (parmgizhp) {
                        sprintf(q,"A102#");                     q+=strlen(q);
                        for (p=gizmp->ipatt, n=gizmp->isize; n--; p++, q+=2)
                            sprintf(q,"%02x",*p);
                        *q++ = '#';
                        sprintf(q,"A202#");                     q+=strlen(q);
                        for (p=gizmp->opatt, n=gizmp->osize; n--; p++, q+=2)
                            sprintf(q,"%02x",*p);
                        *q++ = '#';
                        sprintf(q,"A103#");                     q+=strlen(q);
                        for (p=gizmp->ipatt, n=gizmp->isize; n--; p++, q+=3)
                            sprintf(q,"%03d",(int)*p);
                        *q++ = '#';
                        sprintf(q,"A203#");                     q+=strlen(q);
                        for (p=gizmp->opatt, n=gizmp->osize; n--; p++, q+=3)
                            sprintf(q,"%03d",(int)*p);
                        *q++ = '#';
                        }
#endif
                        *q = '\0';
                        if ((p=fldupdat(upirec,line)) != NULL) fatal(p);
                        recupdat(upcrec,upirec);
                    }
                }
                fprintf(stderr,"+++ %s = %"_LD_" entries\n",usedbn,nused);
                dbxflush(usedbn);
            }
        }
    }
#endif
#endif

    if (parmiso2 == 1) iso_close();

    if (tmpseq) {
#if DELETETMPSEQ
        mstflush("tmpseq");
#if PC
        remove("tmpseq.xrf");
        remove("tmpseq.mst");
#endif
#if MPE
        system("purge tmpseq.xrf");
        system("purge tmpseq.mst");
#endif
#if VAX
        system("delete/noconfirm tmpseq.xrf;*");
        system("delete/noconfirm tmpseq.mst;*");
#endif
#if UNIX
        remove("tmpseq.xrf");
        remove("tmpseq.mst");
#endif
#endif /* DELETETMPSEQ */
    }

#if CIB71
    cib7p=b7_cib7_delete(cib7p);
#endif
#if LIND || CIB64
    if (booltmpnam[0])           /* b62 MFQTHMEM tmp file */
        remove(booltmpnam);
#endif

#if MXIFUPD
    if (ifnp) {
        if (svdifupx == 2) {
            if (!svdifupy) upif_end(ifnp,pstflag);
        }
        else {
            if (endup == IFUPCORE)
                ifupdat(VRDBname(mstcrec),0L,0L,
                        ifnp,fstpgmp,stwp,maxlk1,maxlk2,pstflag,IFUPWRIT);
        }
        dbxp=dbxstorp(ifnp); /* inverted file ! */
        if (parmtell && DBXirecs) {
            fprintf(stderr,"+++ %"_LD_" records updated      \n",DBXirecs);
            fprintf(stderr,"+++ %"_LD_"+%"_LD_" postings added   \n",DBXipadd[0],DBXipadd[1]);
            fprintf(stderr,"+++ %"_LD_"+%"_LD_" postings deleted \n",DBXipdel[0],DBXipdel[1]);
    }
}
#endif

#if MXFST
    if (fstpgmp) {
        if (lnk1p) FREE(lnk1p);
        if (lnk2p) FREE(lnk2p);
        lnk1p=lnk2p=NULL;
        fst_clos(0); fst_clos(1); fst_fd[0]=fst_fd[1]=0;
        if (parmtell)
	     if (fst_fx[0]+fst_fx[1])
            fprintf(stderr,"+++ %"_LD_"+%"_LD_" links \n",fst_fx[0],fst_fx[1]);
        fst_free(fstpgmp);
    }
#endif

#if MULTI
    /* process Data Entry Lock and Exclusive Write Lock */
    if (parmcopy && VRDBdelxx(upcrec)) {
        if (parmtrace) printf("DEL - %"_LD_",%s\n",upcrec,VRDBname(upcrec));
        VREClock(upcrec)=NOLOCK; record(upcrec,VRDBname(upcrec),0L);
	     recunlck(upcrec,DELOCK);
	     if (parmtrace) prtcontrol(vrecp[upcrec],VRDBname(upcrec));
    }
    if (parmcopy && VRDBewlxx(upcrec)) {
        if (parmtrace) printf("EWL - %"_LD_",%s\n",upcrec,VRDBname(upcrec));
	     VREClock(upcrec)=NOLOCK; record(upcrec,VRDBname(upcrec),0L);
	     recunlck(upcrec,EWLOCK);
	     if (parmtrace) prtcontrol(vrecp[upcrec],VRDBname(upcrec));
    }
#endif /* MULTI */

#if MXFUN

#if MXFULLINV
    if (fullinv_ifnp) {
      char *errp=NULL;
      while (!errp) {
        /* ifupdat(VRDBname(mstcrec),0L,0L,
                        ifnp,fstpgmp,stwp,maxlk1,maxlk2,pstflag,IFUPWRIT); */
        char cmdline[CIMPL+CIMPL+BUFSIZ+1];
        char nameln1[CIMPL+1],nameln2[CIMPL+1];  /* dbname + .ext + NULL */
        char namelk1[CIMPL+1],namelk2[CIMPL+1];  /* dbname + .ext + NULL */
        int link1len,link2len;
        int rc;

#if PC || MYSFUN
#else /* PC || MYSFUN */
        char *setposix="";
        char *restposix="";
        char *envp,*p;
        char envarea[BUFSIZ];

        envp=getenv("OSTYPE");
        if (parmtrace) printf("+++ mxrun envp: '%s'\n",envp?envp:"");
#if UNIX /* AOT, 03/12/2002 */
	if (!envp) { 
	    envp="LINUX"; 
            if (parmtrace) printf("+++ mxrun envp: '%s'\n",envp);
	}
#endif
        if (envp) {
	    strcpy(envarea,envp);
            /* for (p=envarea; *p; p++) if (isalpha(*p)) { *p = toupper(*p); p++; }/* AOT/HB 02/08/2002 */
			for (p=envarea; *p; p++) if (isalpha(*p)) { *p = toupper(*p); }/* AOT/HB 19/02/2003 */
            if (parmtrace) printf("+++ mxrun envarea: '%s'\n",envarea);
            if (strstr(envarea,"LINUX")) {                                               /* AOT/HB 02/08/2002 */
                setposix="BAK_LC_ALL=$LC_ALL;export LC_ALL=POSIX;";
                restposix=";export LC_ALL=$BAK_LC_ALL";
                if (parmtrace) printf("+++ mxrun setposix: '%s'\n",setposix);
                if (parmtrace) printf("+++ mxrun restposix: '%s'\n",restposix);
            }
        }
#endif /* PC || MYSFUN */

        strcpy(nameln1,fullinv_ifnp); strcat(nameln1,".ln1");
        strcpy(nameln2,fullinv_ifnp); strcat(nameln2,".ln2");
        strcpy(namelk1,fullinv_ifnp); strcat(namelk1,".lk1");
        strcpy(namelk2,fullinv_ifnp); strcat(namelk2,".lk2");

        link1len=LE1+1+8+(ciiflfim?0:1+5+1+4+1+4)+(CRLF?2:1); /* 27: key %8"_LD_" %5d %4d %4d */
        link2len=LE2+1+8+(ciiflfim?0:1+5+1+4+1+4)+(CRLF?2:1); /* 57: key %8"_LD_" %5d %4d %4d */

#if PC || MYSFUN
        rc=cisis_mysfunc(fullinv_ifnp,link1len,nameln1,namelk1,parmtell|parmtrace);
#else /* PC || MYSFUN */
        sprintf(cmdline,"%ssort -o %s %s%s",setposix,namelk1,nameln1,restposix);
        if (parmtrace) printf("+++ mxrun sort 1: '%s'\n",cmdline);
        rc=system(cmdline);
#endif /* PC || MYSFUN */
        if (rc) { errp="fullinv/sort/1"; break; }
        if (!fullinv_keep) remove(nameln1);

#if PC || MYSFUN
        rc=cisis_mysfunc(fullinv_ifnp,link2len,nameln2,namelk2,parmtell|parmtrace);
#else /* PC || MYSFUN */
        sprintf(cmdline,"%ssort -o %s %s%s",setposix,namelk2,nameln2,restposix);
        if (parmtrace) printf("+++ mxrun sort 2: '%s'\n",cmdline);
        rc=system(cmdline);
#endif /* PC || MYSFUN */
        if (rc) { errp="fullinv/sort/2"; break; }
        if (!fullinv_keep) remove(nameln2);

        p=cmdline;
        sprintf(cmdline,"%s\n%s\n%s\n+fix%s\ntell=%"_LD_"\n",
                fullinv_ifnp,namelk1,namelk2,ciiflfim ? "/m" : "",parmtell);
        p+=strlen(p);
#if LIND
        if (fullinv_maxmfn) {
            sprintf(p,"maxmfn=%"_LD_"\n",fullinv_maxmfn); p+=strlen(p);
        }
        else if (fullinv_master) {
            sprintf(p,"master=%s\n",fullinv_master); p+=strlen(p);
        }
        else {errp="mxrun/fullinv/maxmfn"; break; }
#else /* LIND */
        if (fullinv_reset) {
            if (fullinv_master) {
                sprintf(p,"master=%s\n",fullinv_master); p+=strlen(p);
            }
        }
        else strcat(p,"-reset\n");
#endif /* LIND */
        if (fullinv_pstflag == IFUPDICT) strcat(p,"-posts\n");
        if (!fullinv_balan)              strcat(p,"-balan\n");
        if (parmtrace) printf("+++ mxrun cisis_ifload: '%s'\n",cmdline);
        rc=cisis_ifload(cmdline);
        if (rc) errp="fullinv/ifload";
        if (!fullinv_keep) remove(namelk1);
        if (!fullinv_keep) remove(namelk2);
        break;
      }
      if (parmtrace) printf("+++ mxrun \n");
      if (errp) fatal(errp);
    }
#endif /* MXFULLINV */

    if (mx_bup) FREE(mx_bup);
#if MXIFUPD
#if CIAPI
    if (mstcrec) { FREE(vrecp[mstcrec]); vrecp[mstcrec]=NULL; nrecs-=1; }
    if (mstirec) { FREE(vrecp[mstirec]); vrecp[mstirec]=NULL; nrecs-=1; }
#else /* CIAPI */
    if (mstcrec) { FREE(vrecp[mstcrec]); vrecp[mstcrec]=NULL; nrecs--; }
    if (mstirec) { FREE(vrecp[mstirec]); vrecp[mstirec]=NULL; nrecs--; }
#endif /* CIAPI */
#endif /* MXIFUPD */

    if (rc) if (parmtrace) fprintf(stderr,"*** mx: end of execution [%d]\n",rc);

#if CIAPI
    cisisApplicationDelete( ciapip );
#endif /* CIAPI */

    FREE(xmxp);
    return(0);

#else /* MXFUN */
    exit(rc);
#endif /* MXFUN */
}

