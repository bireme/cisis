#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "cisis.h"

#if CICPP
#include "cirec.hpp"
#include "citrm.hpp"
#include "cirun.hpp"
#endif /* CICPP */

#if !CICPP || CIAPI_SOURCE
#if GEN_UTL
/* ********************************************************************** */
/* * prtcontrol/prtleader/prtfields/prtxref/prtdir                      * */
/* ********************************************************************** */
/* ***************************** prtcontrol ***************************** */
#if ANSI
LONGX prtcontrol(RECSTRU *recp, char dbname[])                           /*
---------------
                                        display control
                                                                        */
#else
LONGX prtcontrol(recp,dbname)
RECSTRU *recp;
char dbname[];
#endif
{
    if (dbname)
        if (*dbname) printf("*%s\n",dbname);
    printf(" nxtmfn nxtmfb nxtmfp  t  reccnt mfcxx1 mfcxx2 mfcxx3    RC\n");
    printf("%6"_LD_" %6"_LD_" %6d   %d %6"_LD_" %6"_LD_" %6"_LD_" %6"_LD_" %6d\n",
        MF0nxtmfn,MF0nxtmfb,MF0nxtmfp,MF0mftype,
        MF0reccnt,MF0mfcxx1,MF0mfcxx2,MF0mfcxx3,RECrc);
    return(MF0nxtmfn);
}
/* ***************************** prtleader ****************************** */
#if ANSI
LONGX prtleader(RECSTRU *recp, LONGX mfn)                                 /*
--------------
                                        display leader
                                                                        */
#else
LONGX prtleader(recp,mfn)
RECSTRU *recp;
LONGX mfn;
#endif
{
    char x;
    if (mfn)
        printf("mfn=%6"_LD_"\n",mfn);
    printf(
       "mfn=%6"_LD_"|mfrl=%5"_LD_"|mfbwb=%6"_LD_"|mfbwp=%5d|base=%5"_LD_"|nvf=%3d|status=%2d|",
        MFRmfn,(LONGX)MFRmfrl,MFRmfbwb,MFRmfbwp,(LONGX)MFRbase,MFRnvf,MFRstatus);
    x=' ';
#if MULTI
    if (RECgdbw) x = (RECgdbl) ? '*' : '+';
#endif
    printf("%2d%c\n",RECrc,x);      /* tty line overflow */
    return(mfn);
}
/* ***************************** prtfields ****************************** */
#if ANSI
LONGX prtfields(RECSTRU *recp, LONGX mfn)                                 /*
--------------
                                        display fields
                                                                        */
#else
LONGX prtfields(recp,mfn)
RECSTRU *recp;
LONGX mfn;
#endif
{
    int xdir,loop1;
    FFI loop2;
    UBYTE *p;
    if (mfn)
        printf("mfn=%6"_LD_"%s\n",mfn,(MFRstatus != ACTIVE) ? " [DELETED]" : " ");
    for (xdir=0, loop1=MFRnvf; loop1--; xdir++) {
        printf("%3u  %c",DIRtag(xdir),OPENFCHR);
        for (p=(UBYTE*)FIELDP(xdir), loop2=DIRlen(xdir); loop2--; p++)
            printf("%c",*p);
        printf("%c\n",CLOSFCHR);
    }
    return(mfn);
}
#if CIAPI_SOURCE
LONGX prtxref  (CISISX *cisisxp, RECSTRU *recp, LONGX mfn)                         
#else /* CIAPI_SOURCE */
#if ANSI
LONGX prtxref  (RECSTRU *recp, LONGX mfn)                                 /*
------------
                                        display xref
                                                                        */
#else
LONGX prtxref(recp,mfn)
RECSTRU *recp;
LONGX mfn;
#endif
#endif /* CIAPI_SOURCE */
{
    LONGX xrftiv,comb;
    int xtiv,comp,flagnew,flagmod;
    DBXSTRU *dbxp;
    dbxp=RECdbxp;
    xrftiv=DBXxribp->xrmfptr[(xtiv=(MFRmfn-1)%XRMAXTIV)];
    comb = recxrefb;
    comp = recxrefp;
    flagnew=recxrefn;
    flagmod=recxrefm;
    if (mfn) printf("mfn=%6"_LD_"|",mfn);
    printf("comb=%5"_LD_"|comp=%3d|%c|%c|%5"_LD_"+%03d=%08lx\n",
            comb,comp,
	    flagnew?'N':' ',flagmod?'M':' ',
	    DBXxribp->xrxrpos,xtiv,xrftiv);
    return(mfn);
}
#if ANSI
LONGX prtdir   (RECSTRU *recp, LONGX mfn)                                 /*
-----------
                                        display dir
                                                                        */
#else
LONGX prtdir(recp,mfn)
RECSTRU *recp;
LONGX mfn;
#endif
{
    int xdir,loop1;
        for (xdir=0,loop1=MFRnvf; loop1--; xdir++) {
            if (mfn)
                printf("mfn=%6"_LD_"|",mfn);
            printf("dir=%3d|tag=%5u|pos=%5"_LD_"|len=%5"_LD_"\n",
                xdir+1,DIRtag(xdir),(LONGX)DIRpos(xdir),(LONGX)DIRlen(xdir));
        }
    return(mfn);
}

/* **************************************************************** */
/* fldxocc */
/* **************************************************************** */
/* *************************** fldxocc  ******************************** */
#if CIAPI_SOURCE
int fldocc(CISISX *cisisxp, RECSTRU *recp, int diridx)
#else /* CIAPI_SOURCE */
int fldocc(ridx,diridx)
LONGX ridx;
int diridx;
#endif /* CIAPI_SOURCE */
{
#if !CIAPI_SOURCE
    RECSTRU *recp;  /* mandatory for defines REC,MFR,DIR,FIELDP */
#endif /* !CIAPI_SOURCE */
    int xocc,xdir;
#if !CIAPI_SOURCE
    recp=vrecp[ridx];
#endif /* !CIAPI_SOURCE */

    for (xocc=0, xdir=0; xdir <= diridx; xdir++)
        if (DIRtag(xdir) == DIRtag(diridx))
            xocc++;
    if (xocc == 0) fatal("fldocc");
    return(xocc);
}
/* *************************** recfieldx **************************** */
#if CIAPI_SOURCE
char *recfield(CISISX *cisisxp, char *areap,RECSTRU *recp, int tag,int iocc,char *defaultp)
#else /* CIAPI_SOURCE */
char *recfield(areap,ridx,tag,iocc,defaultp)
char *areap;
LONGX ridx;
int tag;
int iocc;
char *defaultp;
#endif /* CIAPI_SOURCE */
{
    int xdir;
    FFI n;
    unsigned char *p;
#if CIAPI_SOURCE
    if ((xdir=recp->xfieldx(tag,iocc)) >= 0) {
        p=( unsigned char*)FIELDP(xdir); n=DIRlen(xdir);
    }
#else /* CIAPI_SOURCE */
    if ((xdir=fieldx(ridx,tag,iocc)) >= 0) {
        p=VFIELDP(ridx,xdir); n=VDIRlen(ridx,xdir);
    }
#endif /* CIAPI_SOURCE */
    else
        if (defaultp)
            n=strlen((char*)(p=(unsigned char*)defaultp));
        else
            n=strlen((char*)(p=(unsigned char*)"\0"));
    if (!areap)
#if CIAPI_SOURCE
        if ((areap=new char[(ALLOPARM)n+1]) == (char *)ALLONULL)
#else /* CIAPI_SOURCE */
        if ((areap=(char *)ALLOC((ALLOPARM)n+1)) == (char *)ALLONULL)
#endif /* CIAPI_SOURCE */
            fatal("recfield/ALLOC");
    memmove(areap,p,n); areap[n]='\0';
    return(areap);
}
#endif /* GEN_UTL */

/* *************************** showcore ***************************** */
#if CICPP
unsigned LONGX showcore(char *s)
#else /* CICPP */
unsigned LONGX showcore(s)
char *s;
#endif /* CICPP */
{
    char *sp;
    unsigned LONGX left;
    sp = "::";
    if (s)
        if (*s)
            sp = s;
#if CIAPI_SOURCE
    left=0;
#else /* CIAPI_SOURCE */
    left=CORELEFT();
#endif /* CIAPI_SOURCE */

    printf("%s: coreleft=%"_LD_"\n",sp,left);
    return(left);
}
#if GEN_UTL
/* ***************************** 12345678 ******************************* */
/* ***************************** prtifcnt   ***************************** */
#if ANSI
LONGX prtifcnt  (TRMSTRU *trmp, char dbname[])                           /*
-------------
                                        display .cnt
                                                                        */
#else
LONGX prtifcnt  (trmp,dbname)
TRMSTRU *trmp;
char dbname[];
#endif
{
    CNSTRU *cp;
    int treecase;
    if (*dbname)
        printf("*%s\n",dbname);
    printf("idtype ordn ordf   n   k  liv    ");
    printf("posrx nmaxpos  fmaxpos  abnormal\n");
    for (treecase=0; treecase < 2; treecase++) {
        cp= &TDBifmap->cn[treecase];
	printf("%4d  %4d %4d %4d%4d",cp->idtype,cp->ordn,cp->ordf,cp->n,cp->k);
        printf("%4d %8"_LD_,cp->liv,cp->posrx);
        printf("%8"_LD_" %8"_LD_"    %4d\n",cp->nmaxpos,cp->fmaxpos,cp->abnormal);
    }
    return(0L);
}
/* *************************** chkxref  ******************************** */
#if ANSI
LONGX chkxref  (RECSTRU *recp, LONGX mfn, int delta)                      /*
------------
                                        check xref from MFRmfn on
                                                                        */
#else
LONGX chkxref(recp,mfn,delta)
RECSTRU *recp;
LONGX mfn;
int delta;
#endif
{
    LONGX xrftiv,xrftiva,wcomb;
    int flagnew,flagmod,wcomp;
    DBXSTRU *dbxp;
    dbxp=RECdbxp;
    for ( ; ; mfn+=delta) {
        if (mfn <= 0L) return(0L);
        if (recxref(recp,mfn,&wcomb,&wcomp) == RCEOF) return(0L);
        xrftiv=DBXxribp->xrmfptr[(MFRmfn-1)%XRMAXTIV];
        xrftiva=labs(xrftiv);
        flagnew=(xrftiva & XRXMASKN);
        flagmod=(xrftiva & XRXMASKU);
        if(flagnew || flagmod)
            return(mfn);
    }
}

/* ********************************************************************** */
/* * reccopy/recfgets/loaductb/loadactb                                 * */
/* ********************************************************************** */
/* ***************************** reccopy ******************************** */
#if CIAPI_SOURCE
int reccopy(CISISX *cisisxp, RECSTRU *upirec,RECSTRU *upcrec,RECSTRU *recp,LONGX mfn)
{
    upirec->xrecord(upcrec->recdbxp->dbxname,upcrec->recmfp->m0.m0nxtmfn);         /* dbnp,etc */
    if (MFRmfrl > upirec->recnbytes ) fatal("reccopy/check/mfrl");
    memmove(upirec->recmfp->mx,MFX,MFRmfrl); /* mfn included */
    upirec->recmfp->m1.m1mfn=mfn;
    return(0);
}
#else /* CIAPI_SOURCE */
int reccopy(upirec, upcrec, recp, mfn)
LONGX upirec;
LONGX upcrec;
RECSTRU *recp;
LONGX mfn;
{
    record(upirec,VRDBname(upcrec),VMF0nxtmfn(upcrec));         /* dbnp,etc */
    if (MFRmfrl > VRECnbytes(upirec)) fatal("reccopy/check/mfrl");
    memmove(VMFX(upirec),MFX,MFRmfrl); /* mfn included */
    VMFRmfn(upirec)=mfn;
    return(0);
}
#endif /* CIAPI_SOURCE */
/* ********************* recfgets ********************************* */
#if MFUPDATE
#if CIAPI_SOURCE
int recfgets(CISISX *cisisxp, RECSTRU *crec, RECSTRU *irec, char line[], FFI linsiz, FILE *fpconv)
#else /* CIAPI_SOURCE */
#if ANSI
int recfgets(LONGX crec, LONGX irec, char line[], FFI linsiz, FILE *fpconv)
#else
int recfgets(crec,irec,line,linsiz,fpconv)
LONGX crec;
LONGX irec;
char line[];
FFI linsiz;
FILE *fpconv;
#endif
#endif /* CIAPI_SOURCE */
{
    char *p,*p0,*batchp,c;
    int tag,rc;
    FFI len,/*left*,*/n;
    char *buffup;
    LONGX nbytes,left;

    /* truncate linsiz to fgets() maximum */
    if (linsiz > (FFI)INT_MAX) linsiz=(FFI)INT_MAX;

    /* allocate working area */
    nbytes=linsiz+BUFSIZ;
    if (nbytes > ALLOMAXV) nbytes=ALLOMAXV;
#if CICPP
    if ((buffup=new char[nbytes]) == (char *)ALLONULL)
#else /* CICPP */
    if ((buffup=(char *)ALLOC((ALLOPARM)nbytes)) == (char *)ALLONULL)
#endif /* CICPP */
        fatal("recfgets/ALLOC");
    left=nbytes-1;
    rc= ~EOF;
    *buffup='\0';
    c = (line[0]) ? line[0] : '|';
    if (fgets(line,linsiz-1,fpconv)) {
        for (batchp=buffup, p=line, tag=1; *p; tag++) {
            for (len=0, p0=p; *p; p++, len++)
                if (*p == c || *p == '\n')
                    break;
                if (len) {
                    sprintf(batchp,"H%d %d ",tag,len);
		    batchp+=(n=strlen(batchp));
		    if (left <= (n+len))
			fatal("recfgets/buffup");
		    for (left-=(n+len); len--; )
			*batchp++ = *p0++;
		    *batchp = '\0';
		}
		p++;
	    }
	}
    else
	rc=EOF;
#if CIAPI_SOURCE
    irec->xrecord(crec->recdbxp->dbxname,crec->recmfp->m0.m0nxtmfn);
    irec->recrc=RCNORMAL;
    irec->recmfp->m1.m1status=ACTIVE;    /* PDEL status is DELETED */
    if (*buffup) {
		p=irec->xfldupdat(buffup);
#else /* CIAPI_SOURCE */
    record(irec,VRDBname(crec),VMF0nxtmfn(crec));
    VRECrc(irec)=RCNORMAL;
    VMFRstatus(irec)=ACTIVE;    /* PDEL status is DELETED */
    if (*buffup) {
	p=fldupdat(irec,buffup);
#endif /* CIAPI_SOURCE */
	if (p) {
	    printf("+++fldupdat='%s'\n",p);
	    fatal("recfgets/fldupdat");
	}
    }
    /* deallocate working area */
#if CICPP
    delete(buffup);
#else /* CICPP */
    free(buffup);
#endif /* CICPP */
    return(rc);
}
#endif /* MFUPDATE */

#endif /* GEN_UTL */

#endif /* !CICPP || CIAPI_SOURCE */



/* ******************** subfield routines ************************** */
#if !CICPP
UBYTE sfldchr = SFLDCHR;
#endif /* CICPP */

#if CICPP
UBYTE *subfldp(UBYTE *fldp,
	       UBYTE  dlm,
	       FFI   *lenp)
#else /*CICPP*/
UBYTE *subfldp(fldp,dlm,lenp)
UBYTE *fldp;
UBYTE dlm;
FFI *lenp;
#endif /*CICPP*/
{
    UBYTE *p,*retp,ucdlm,ucdata;
    FFI n,len;
    ucdlm = toupper(dlm);
    for (retp=NULL, p=fldp, n=0, len= *lenp;  n < len; p++) {
	n++;
	if (*p == SFLDCHR)
	    if (n < len) {
		ucdata=toupper(*(p+1));
		if (ucdata == ucdlm) {
		    retp=p+2;
		    n++;
		    break;
		}
	    }
	    else {
		n--;
		break;
	    }
    }
    if (retp)
	*lenp = *lenp - n;
    else
	*lenp = 0;
    return(retp);
}

#if CICPP
FFI subfldn(UBYTE *sfldp,
	    FFI    len)
#else /*CICPP*/
FFI subfldn(sfldp,len)
UBYTE *sfldp;
FFI len;
#endif /*CICPP*/
{
    UBYTE *p;
    FFI n;
    if (!sfldp)
	return(0);
    for (n=0, p=sfldp; n < len; p++)
	if (*p == SFLDCHR)
	    break;
	else
	    n++;
    return(n);
}

#if CICPP
FFI subfield(UBYTE *fldp,
	     FFI    fldl,
	     UBYTE  dlm,
	     UBYTE *areap)
#else /*CICPP*/
FFI subfield(fldp,fldl,dlm,areap)
UBYTE *fldp;
FFI fldl;
UBYTE dlm;
UBYTE *areap;
#endif /*CICPP*/
{
    UBYTE *p;
    FFI n;
    static FFI l;
    l=fldl;
    p=subfldp(fldp,dlm,&l);
    n=subfldn(p,l);
    if (areap) {
	memmove(areap,p,n);
	areap[n]='\0';
    }
    return(n);
}


/* *************************** loaductb  ******************************** */
#define TRACEUC 0
#if VAX
#define UCTABCOLS 16
#else
#define UCTABCOLS 32
#endif

#if !CICPP
int loaduct0 = 0;               /* last char (zero) is not loaded */
#endif /* CICPP */

#if CICPP
char *loaductb(DBXSTRU *dbxp,
               char *gidbnp,			/* dbn.par */
	       char *uctbp,
	       char *ucfilp)
#else /*CICPP*/
char *loaductb(gidbnp,uctbp,ucfilp)
char *gidbnp;			/* dbn.par */
char *uctbp;
char *ucfilp;
#endif /*CICPP*/
{
    int fd,loop,lrecl,n,loop2;
    char line[BUFSIZ];
    unsigned char *p,*tabp;
    tabp=(unsigned char *)uctbp;

#if CICPP
    CISISX * cisisxp = dbxp->cisisxp;
#endif /* CICPP */

#if 0
000 001 002 003 004 005 006 007 008 009 010 011 012 013 014 015 ... 030 031
032 ....
224 225 226 227 228 229 230 231 232 233 234 235 236 237 238 239 ... 254 255
#endif
    lrecl=UCTABCOLS*4;
#if CRLF
    lrecl++;    /* 0D 0A for CIREAD with _read */
#endif
    ucfilp=dbxcipar(gidbnp,ucfilp,'=');
#if TRACEUC
printf("loaductb - %s lrecl=%d\n",ucfilp,lrecl);
#endif
    if ((fd=OPEN(ucfilp,O_RDONLY)) <= 0)
	return(NULL);
    for (loop=0; CIREAD(fd,line,lrecl) == lrecl; ) {
	line[lrecl]='\0';
#if TRACEUC
printf("%s",line);
#endif
	if (line[lrecl-1] != '\n')
	    return(NULL);
	for (p=(unsigned char *)line, loop2=UCTABCOLS; loop2--; p+=4) {
            sscanf((CONST char *)p,"%d",&n);
	    if (n < 0 || n > 255 || loop > 255)
		return(NULL);
	    if (loaduct0 == 0) {
		if (loop) if (n == 0) return(NULL);
	    }
	    tabp[loop]=n;
#if TRACEUC
printf("(%3d) %c -> %c (%3d)\n",loop,loop,tabp[loop],tabp[loop]);
#endif
	    loop++;
	}
    }
    CLOSE(fd);
    if (loop != 256)
	return(NULL);
    return(uctbp);
}

/* *************************** loadactb  ******************************** */
#if CICPP
char *loadactb(DBXSTRU *dbxp,
               char *gidbnp,			/* dbn.par */
	       char *actbp,
	       char *acfilp)
#else /*CICPP*/
char *loadactb(gidbnp,actbp,acfilp)
char *gidbnp;			/* dbn.par */
char *actbp;
char *acfilp;
#endif /*CICPP*/
{
    int fd,ne,k;
    char chr;

#if CICPP
    CISISX *cisisxp = dbxp->cisisxp;
#endif /* CICPP */

#if 0
...
140 141 142 143 144 145 146 147 148 149 150 151 152 153 154 160 ... 164 165
#endif

    actbp[ne=0]='\0'; /* empty table */

    acfilp=dbxcipar(gidbnp,acfilp,'=');
    if ((fd=OPEN(acfilp,O_RDONLY)) <= 0) return(NULL);

    for (k=0; CIREAD(fd,&chr,1) == 1; ) {

	if (chr < '0' || chr > '9') {
	    if (k > 255) return(NULL);
	    if (k) actbp[ne++]=(unsigned char)k;
	    k=0;
	}
	else k=k*10+((int)chr-(int)'0');
    }

    if (k) {
	if (k > 255) return(NULL);
	actbp[ne++]=(unsigned char)k;
    }

    actbp[ne++]='\0';	/* end of table = isisactab */

    CLOSE(fd);
    return(actbp);
}

/*-----------------------------------------------------------------------*/
/* encodepst                                                             */
/*-----------------------------------------------------------------------*/
#if CICPP
void encodepst(POSTSTRU *pstp,
	       LONGX mfn,
	       unsigned short tag,
	       unsigned short occ,
	       unsigned short cnt)
#else /*CICPP*/
void encodepst( pstp, mfn, tag, occ, cnt )
POSTSTRU *pstp;
LONGX mfn;
unsigned short tag,occ,cnt;
#endif /*CICPP*/
{
  unsigned char vc[sizeof(POSTSTRU)],*q;
  int i=0;
#define TRACENCODEPST 0
#if TRACENCODEPST
  unsigned char *p;
#endif
  q=(unsigned char *)&mfn;
  vc[i++] = q[M1];
  vc[i++] = q[M2];
  vc[i++] = q[M3];
#if LIND4
  vc[i++] = q[M4];
#else
  //vc[i++] = (unsigned char)0;
#endif 
  q=(unsigned char *)&tag;
  vc[i++] = q[T1];
  vc[i++] = q[T2];
  q=(unsigned char *)&occ;
  vc[i++] = q[O1];
  q=(unsigned char *)&cnt;
  vc[i++] = q[C1];
  vc[i++] = q[C2];
  memmove(pstp,vc,sizeof(POSTSTRU));
#if TRACENCODEPST
  printf("+++ encodepst: %"_LD_"/%u/%u/%u =",mfn,tag,occ,cnt);
  for (p=(unsigned char *)pstp, i=0; i<sizeof(POSTSTRU); i++) printf(" %02x",p[i]);
  printf("\n");
#endif
}

/*-----------------------------------------------------------------------*/
/* decodepst                                                             */
/*-----------------------------------------------------------------------*/
#if CICPP
void decodepst(POSTSTRU *pstp,
	       LONGX     *mfnp,
	       unsigned short *tagp,
	       unsigned short *occp,
	       unsigned short *cntp)
#else /*CICPP*/
void decodepst( pstp, mfnp, tagp, occp, cntp )
POSTSTRU *pstp;
LONGX *mfnp;
unsigned short *tagp,*occp,*cntp;
#endif /*CICPP*/
{
  unsigned char *p,*q;
  p=(unsigned char *)pstp;
  q=(unsigned char *)mfnp;
  q[M1] = *p++;
  q[M2] = *p++;
  q[M3] = *p++;
#if LIND4
  q[M4] = *p++;
#else
  q[M4] = (unsigned char)0;
#endif /* LIND4 */
  q=(unsigned char *)tagp;
  q[T1] = *p++;
  q[T2] = *p++;
  q=(unsigned char *)occp;
  q[O1] = *p++;
  q[O2]=(unsigned char)0;
  q=(unsigned char *)cntp;
  q[C1] = *p++;
  q[C2] = *p++;
}


/* ***************************** bobkey ***************************** */

#if CICPP
FFI bobkey(char *keyp,
	   FFI   klen,
	   char *areap,
	   FFI   maxlen,
	   int   leading,
	   int   middle,
	   int   trailing)
#else /*CICPP*/
FFI bobkey(keyp,klen,areap,maxlen,leading,middle,trailing)
char *keyp;
FFI klen;
char *areap;
FFI maxlen;
int leading;
int middle;
int trailing;
#endif /*CICPP*/
{
    FFI len,hn;
    char *p;

    len=klen;
    if (!klen) len=strlen(keyp);

    if (trailing) {
	/* BEFORE BOB - elimina brancos/ctlchr's finais */
	while (len) if ((UCHR)(keyp[len-1]) <= MINLCHR) len--; else break;
    }

    hn=0;
    if (leading) {
	/* BOB - elimina brancos/ctlchr's iniciais */
	for (p=keyp; hn < len; hn++) if ((UCHR)(*p++) > MINLCHR) break;
	len-=hn;
    }

    if (len > maxlen) {
	len=maxlen;
	if (trailing) {
	    /* BEFORE BOB - elimina brancos/ctlchr's finais - 13/08/96 */
	    p=keyp+hn;
	    while (len) if ((UCHR)(p[len-1]) <= MINLCHR) len--; else break;
	}
    }
    memmove(areap,keyp+hn,len); areap[len]='\0';

    if (middle) {
	/* BOB - corrige ctlchr's centrais  */
	for (hn=0, p=areap; hn < len; hn++, p++) if ((UCHR)(*p) < MINLCHR) *p=MINLCHR;
    }

    return(len);
}

