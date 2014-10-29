/* Program MSRT:	sorts a master file in ascending order,
			according to a supplied format specification
			AOT, BRM 25/07/91.
*/
#include <stdio.h>
#include <string.h>
#include "cisis.h"   /* CISIS Interface header file */
#include "cirun.h"   /* CISIS Interface runtime declarations */

#define TRACE 0

#if BEFORE20010109
#define MAXFMTA (MAXMFRL)
#define MAXFMTS (MAXMFRL)
#endif

FMT_CODE *pgmp;
LONGX fmtkeylen;
LONGX fmtlines;
LONGX fmtasize;
LONGX parmkeylen;
char *fmtspec=NULL;
char *ibuf;
char *tbuf;
LONGX irec=0L;		/* indice vrecp, para leitura */
LONGX incount=0;		/* recs in */
int parmtrace=0;	/* trace */
LONGX parmtell=0;	/* tell */
int parmmfn=1;		/* update mfn */
int usetag=0;		/* do not use format */
int parmdel=0;		/* delete duplicate keys */
#if BEFORE20010109
LONGX recbyts=MAXMFRL;
#endif
/* prototypes */
#if ANSI
void getkey(UBYTE *buff, LONGX mfn);
XRPTR findptr(LONGX i);
void writptr(LONGX i, XRPTR ptr);
void exchptr(LONGX i, LONGX j);
void ordena(LONGX regsin);
#else
void getkey();
XRPTR findptr();
void writptr();
void exchptr();
void ordena();
#endif
void main(argc,argv)
int argc;
char *argv[];
{
    RECSTRU *recp;	/* mandatory for defines REC,MFR,DIR,FIELDP */
    int iarg;
    char *p,*p1,*p2;
    char *dbnp,*fmtp;
    int n,rc,x;
    LONGX nxtmfn;
    DBXSTRU *dbxp;	/* mandatory for defines DBX */
    LONGX mfn;
    off_t seek;
    int fd;
    LONGX wcomb;
    int wcomp;
#if CNV_PCBINUM
    char convmfn[sizeof(mfn)];
#endif
    LONGX loadxrf=0;
    char *xryyp;
    LONGX xpos;
    LONGX nbytes;

    if (argc < 4) {
        printf("%s",cicopyr("Utility MSRT"));
	printf(" \n");
	printf("msrt <dbname> <keylen> <keyfmt> [-mfn] [tell=<n>] \n");
	printf(" \n");
	printf(" <dbname> master file to be sorted in place");
	printf(" or <dbn>,<%"_LD_"> \n",loadxrf);
	printf(" <keylen> sort key length \n");
	printf(" <keyfmt> format specification to generate sort keys");
	printf(" or tag=<n> \n");
	printf(" \n");
	printf(" -mfn     keep the original MFNs \n");
	printf(" +del     delete duplicate tag=<n> keys \n");
	printf(" tell=<n> produce a message to the stderr every each <n>");
	printf(" keys \n");
	printf(" \n");
	exit(1);
    }

    dbnp=argv[1];
    p=strchr(dbnp,',');
    if (p) { 
        *p++='\0'; 
	if (sscanf(p,"%"_LD_,&loadxrf) != 1) fatal(dbnp);
    }

    if (sscanf(argv[2],"%"_LD_,&parmkeylen) != 1)	fatal(argv[2]);

    fmtp=argv[3];

    if (strncmp(fmtp,"tag=",4) == 0) {usetag=1; fmtp+=4;}

    for (iarg=4; iarg < argc; iarg++) {
	p=argv[iarg];
        if (strcmp(p,"trace") == 0) {
            parmtrace=1;
	    continue;
	}
	if (strcmp(p,"-mfn") == 0) {
	    parmmfn=0;
	    continue;
	}
	if (strncmp(p,"tell=",5) == 0) {
	    if (sscanf(p+5,"%"_LD_,&parmtell) != 1) fatal(p);
            continue;
	}
	if (usetag && strcmp(p,"+del") == 0) {
	    parmdel=1;
	    continue;
	}
    }

    /* get format */
    if (usetag) {
        if (sscanf(fmtp,"%d",&usetag) != 1) fatal(fmtp);
        if (usetag < 1) fatal(fmtp);
    }
    else {
#if BEFORE20010109
	fmtspec=loadfile(dbnp,'@',fmtp,fmtspec,MAXFMTS,'\0');
#else
	fmtspec=loadfile(dbnp,'@',fmtp,fmtspec,0L,'\0');
#endif
	if (!fmtspec) fatal(fmtp);
	if (parmtrace) {
	    showcore("+++fmt");
	    printf("+++fmt=®"); printf("%s",fmtspec); printf("¯\n");
	}
	if (fmt_gener(&pgmp,fmtspec) < 0L) fatal(fmtspec+fmt_errof);
    }

    /* set up processing */
#if CIWTF                                /* WTFUN support */              
    /* setup wtfnew
    */
    //p=NULL;
    //for (i=argnext; i < argc; i++) if (strncmp(argv[i],"cipar?",6) == 0 || strncmp(argv[i],"cipar=",6) == 0) { p=argv[i]+6; break; }
    //if (!ciawtfp) if (p) if (*(p+6)) if (dbxciset(p+6)) fatal(p);
    if (!ciawtfp) if (dbxciset("msrt.cip")) dbxciset(NULL); // optional msrt.cip
    if (!ciawtfp) fatal("msrt/ciawtfp");
#endif //CIWTF    

    dbxstorp(dbnp); /* dbxinit ok */
    recallok(irec,rec_maxmfrl);


    RECORD(irec,dbnp,0L);
    nxtmfn=MF0nxtmfn;

    fmtkeylen=parmkeylen+6; /* %6ld */
    if (fmtkeylen >= ALLOMAXV) fatal("msrt/keylen/overflow");

#if BEFORE20010109
    nbytes=MAXFMTA;
#else
    nbytes=RECnbytes;
#endif
    if (usetag) nbytes=fmtkeylen;

    if ((ibuf=(char *)ALLOC((ALLOPARM)(nbytes+1))) == NULL)
	fatal("msrt/ALLOC/ibuf");

    if ((tbuf=(char *)ALLOC((ALLOPARM)(nbytes+1))) == NULL)
	fatal("msrt/ALLOC/tbuf");

    if (!loadxrf) loadxrf = (MF0nxtmfn/XRMAXTIV + 1 ) * XRBSIZ;	/* 127 */
    mstflush(dbnp);
    n=rectrace; if (parmtrace) rectrace=1;
    mstsetup(dbnp,loadxrf,0L);
    rectrace=n;
    if (parmtrace) printf("+++ loadxrf=%"_LD_" \n",loadxrf);
    dbxp=RECdbxp;
    if (DBXxryyp == NULL) fatal("msrt/loadxrf/overflow");
    xryyp=DBXxryyp; /* keep */
    DBXxryyp=NULL;  /* avoid FREE */
    mstflush(dbnp);
    dbxopt_ordwr=O_RDWR;
    mstsetup(dbnp,0L,0L);
    DBXxryyp=xryyp; /* restore */

    /* processing */
    ordena(nxtmfn-1);
#if 0 /* 1 */
    writptr(0,0); /* breakout */
#else
    if (!DBXxropw) fatal("msrt/xropw/break");
    if (LSEEK64(DBXxropn,0L,SEEK_SET) != 0) fatal("msrt/seek/break");
    if (!DBXxryyp) fatal("msrt/xryyp/break");
    for (xryyp=DBXxryyp, xpos=1; ; xpos++, xryyp+=XRBSIZ) {
#if CNV_PCBINUM
	memmove(cnv_pcbuff,xryyp,XRBSIZ);
	ConvertXRF_REC(cnv_pcbuff);
	if (CIWRITE(DBXxropn,cnv_pcbuff,XRBSIZ) != XRBSIZ)
#else
	if (CIWRITE(DBXxropn,xryyp,XRBSIZ) != XRBSIZ)
#endif
	    fatal("msrt/write/break");
	if (((XRSTRU *)xryyp)->xrxrpos < 0) break;
    }
#endif /* 1 */


    /* rewrite mfn */
    if (parmmfn) {
	for (mfn=1L; ; mfn++) { /* not MFRmfn */
	    rc=recxref(recp,mfn,&wcomb,&wcomp);
	    if (parmtell)
		if (mfn % parmtell == 0)
		    printf("+++ mfn/mfn %"_LD_" \n",mfn);
	    if (rc == RCEOF) break;
	    if (rc == RCPDEL) continue;
            seek = (((off_t)(wcomb-1))<<MSBSHIFT);
	    //seek = (off_t)((wcomb-1)<<MSBSHIFT);
	    seek += wcomp;
	    fd=DBXmsopn;
	    if (LSEEK64(fd,seek,SEEK_SET) != seek) fatal("msrt/mfn/seek");
	    n=sizeof(mfn);
#if TRACEW
if (parmtrace)
printf("+++ mfn/fd,seek,write=%d,%"P_OFF_T",%d\n",fd,(LONG_LONG)seek,n);
#endif
	    p=(char *)&mfn;
#if CNV_PCBINUM
	    strcpy(convmfn,p);
	    ConvertBuffer(convmfn),n);
	    p=convmfn;
#endif
    	    if (CIWRITE(fd,p,n) < n) fatal("msrt/mfn/write");
	}
    }

    /* "delete" key */
    if (parmdel) {
        tbuf[0]='\0'; p1=tbuf; p2=ibuf;
	for (mfn=1L; ; mfn++) {
	    rc=recxref(recp,mfn,&wcomb,&wcomp);
	    if (rc == RCEOF) break;
	    if (rc == RCPDEL) continue;
	    if (parmtell)
		if (mfn % parmtell == 0)
		    printf("+++ del/mfn %"_LD_" \n",mfn);
	    getkey(p2,mfn);
	    if (memcmp(p1,p2,(unsigned)parmkeylen) == 0) {
	        x=fieldx(irec,usetag,1);
		if (x < 0) continue;
	        seek = (off_t)((wcomb-1)<<MSBSHIFT);
	        seek += wcomp;
		seek += LEADER + x*sizeof(DIRSTRU);
		fd=DBXmsopn;
#if TRACEW
if (parmtrace)
printf("+++ del/fd,seek,write=%d,%"P_OFF_T",%d\n",fd,(LONG_LONG)seek,n);
#endif
	        if (LSEEK64(fd,seek,SEEK_SET) != seek) fatal("msrt/del/seek");
		DIRtag(x)=0; 
		p=(char *)&DIRtag(x); n=sizeof(DIRtag(x));
		if (CIWRITE(fd,p,n) < n) fatal("msrt/del/write");
	    } 
	    else {
		p=p1; p1=p2; /* last key */
		p2=p; /* next key */
	    }
	}
    }

    exit(0);

} /* end of main */


void getkey(buff,mfn)
UBYTE *buff;
LONGX mfn;
{
    RECSTRU *recp;	/* mandatory for defines REC,MFR,DIR,FIELDP */
    char *fmtarea;
    LONGX fmtrc;
    int x;
    unsigned left,keysize,len;

    keysize=(unsigned)parmkeylen;
    
    RECORD(irec,VRDBname(irec),mfn);
    incount++;
    if (RECrc == RCEOF) {
	fprintf(stderr,"*** mfn=%"_LD_"\n",mfn);
	fatal("msrt/getkey/RCEOF");
    }
    fmtarea=buff;

    if (usetag) {
        x=fieldx(irec,usetag,1);
	if (x < 0) {
	    memset(fmtarea,(int)' ',keysize);
	    left=0;
	}
	else {
	    len=DIRlen(x); if (len > keysize) len=keysize;
	    memmove(fmtarea,FIELDP(x),len);
	    left=keysize-len;
	}
    }
    else {
#if BEFORE20010109
	fmtrc=fmt_inter(pgmp,irec,(LONGX)MAXFMTA,fmtarea,(LONGX)MAXFMTA);
#else
	fmtrc=fmt_inter(pgmp,irec,(LONGX)RECnbytes,fmtarea,(LONGX)RECnbytes);
#endif
	if (fmtrc < 0L) {
	    fprintf(stderr,"*** Format RC = %"_LD_,fmtrc);
	    fatal(fmtspec);
	}
	len=strlen(fmtarea); if (len > keysize) len=keysize;
	left=keysize-len;
    }

    if (left > 0) memset(fmtarea+keysize-left,' ',left);
    sprintf(fmtarea+keysize,"%06"_LD_,VMFRmfn(irec)); /* parmkeylen */
    
    if (parmtrace) if (!usetag) printf("%s\n",fmtarea);
    if (parmtell)
	if (incount % parmtell == 0)
	    printf("+++ getkey=%"_LD_"\n",incount);
}


XRPTR findptr(i)
LONGX i;
{
    XRPTR ptr;
    DBXSTRU *dbxp;
    RECSTRU *recp;
    LONGX mfn;
    LONGX wcomb;
    int wcomp;
    recp=vrecp[irec];
    dbxp=RECdbxp;
    mfn=i;
    if (recxref(recp,mfn,&wcomb,&wcomp) == RCEOF)
	return(0L);
    ptr=DBXxribp->xrmfptr[(mfn-1)%XRMAXTIV];
    return(ptr);
}


void writptr(i,ptr)
LONGX i;
XRPTR ptr;
{
    RECSTRU *recp;      /* elemento de vrecp, para info */
    DBXSTRU *dbxp;
    LONGX mfn;
    off_t xbyte;
#if 1
    char *xryyp;
#else
    char *p;
    XRPOS xpos;
#endif
    XRPOS thispos;
    int thisidx;
    LONGX wcomb;
    int  wcomp;
    static int changed=0;

    recp=vrecp[irec];
    dbxp=RECdbxp;
    mfn=i;

    thispos=    (mfn+XRMAXTV1)/XRMAXTIV;

#if 0 /* 1 */
    if (changed && (mfn <= 0 || labs(DBXxribp->xrxrpos) != thispos)) {
	xpos=labs(DBXxribp->xrxrpos);
	xbyte=(LONGX)((xpos-1)<<XRSHIFT);
	if (LSEEK64(DBXxropn,xbyte,SEEK_SET) != xbyte)
	    fatal("msrt/writptr/seek/break");
#if CNV_PCBINUM
	memmove(cnv_pcbuff,DBXxribp,XRBSIZ);
	ConvertXRF_REC(cnv_pcbuff);
	if (CIWRITE(DBXxropn,cnv_pcbuff,XRBSIZ) != XRBSIZ)
#else
	if (CIWRITE(DBXxropn,(char *)DBXxribp,XRBSIZ) != XRBSIZ)
#endif
	    fatal("msrt/writptr/write/break");
	if (mfn <= 0) return;
    }
#endif /* 1 */

    recxref(recp,mfn,&wcomb,&wcomp); /* already buffered */
    if (labs(DBXxribp->xrxrpos) != thispos) fatal("msrt/writptr/thispos");
    thisidx=    (mfn-1)%XRMAXTIV;
    DBXxribp->xrmfptr[thisidx] = ptr;
    changed=1;

    if (!DBXxryyp) fatal("msrt/writptr/xryyp");
    xbyte=(LONGX)((thispos-1)<<XRSHIFT);
    xryyp=DBXxryyp+xbyte;
    ((XRSTRU *)xryyp)->xrmfptr[thisidx] = ptr;

#if 1
    return;
#else
    xbyte=(LONGX)( ((thispos-1)<<XRSHIFT)+XRPOSSIZ+thisidx*XRPTRSIZ );
    if (LSEEK64(DBXxropn,xbyte,0) != xbyte)
	fatal("msrt/writptr/seek");
    p=(char *)&DBXxribp->xrmfptr[thisidx];
#if CNV_PCBINUM
    ConvertXRF_PTR(p);
#endif
    if (CIWRITE(DBXxropn,p,XRPTRSIZ) != XRPTRSIZ)
	fatal("msrt/writptr/write");
#if TRACE
printf("+++writptr - now %"_LD_" = %"_LD_"\n",mfn,(LONGX)ptr);
#endif
#endif /* 1 */
}


void exchptr(i,j)
LONGX i;
LONGX j;
{
    XRPTR ptr;
    ptr=findptr(i);
    writptr(i,findptr(j));
    writptr(j,ptr);
}


void ordena(regsin)
LONGX regsin;
/* Comm ACM, v12(3) march, 1969 pp185-187 - Algorithm 347 */
{
    LONGX parmi;
    LONGX ii,ij,k,l,m;
    LONGX il[64],iu[64];
    LONGX i,j;
    unsigned keysize;
    XRPTR ptr;

    keysize=(unsigned)fmtkeylen;

    m=0; il[m]=(parmi=1L); iu[m]=regsin;
    while (m >= 0){
        i = il[m]; j = iu[m]; m = m - 1;
#if TRACE
printf("+++desempilhou %"_LD_" - %"_LD_"\n",i,j);
#endif
        while ((j - i > 10) || (i == parmi && i < j)){
	    ij = (i + j) / 2; /* le tbuf */
#if TRACE
printf("+++particionando %"_LD_" - %"_LD_" - %"_LD_"\n",i,ij,j);
#endif
	    getkey(tbuf,ij);
#if TRACE
printf("+++1. leu ij %"_LD_": %s",ij,tbuf);
#endif
            k = i;  l = j;
	    getkey(ibuf,i);
#if TRACE
printf("+++2. leu i  %"_LD_": %s",i,ibuf);
#endif
	    if (memcmp(ibuf,tbuf,keysize) > 0) {
		exchptr(ij,i);
                memmove(tbuf,ibuf,keysize); /* t=a[ij] */
#if TRACE
printf("+++2. copiou: %s",tbuf);
#endif
            }
	    getkey(ibuf,j);
#if TRACE
printf("+++3. leu j  %"_LD_": %s",j,ibuf);
#endif
	    if(memcmp(ibuf,tbuf,keysize) < 0) {
		exchptr(ij,j);
                memmove(tbuf,ibuf,keysize); /* t=a[ij] */
#if TRACE
printf("+++3. copiou: %s",tbuf);
#endif
		getkey(ibuf,i);
#if TRACE
printf("+++4. leu i  %"_LD_": %s",i,ibuf);
#endif
		if (memcmp(ibuf,tbuf,keysize) > 0) {
		    exchptr(ij,i);
                    memmove(tbuf,ibuf,keysize); /* t=a[ij] */
#if TRACE
printf("+++4. copiou: %s",tbuf);
#endif
                }
            }
            while(1){
#if TRACE
printf("+++central: %s",tbuf);
#endif
                do{
		    l = l - 1;
		    getkey(ibuf,l);
#if TRACE
printf("+++l=%4"_LD_": %s",l,ibuf);
#endif
                } while (memcmp(ibuf,tbuf,keysize) > 0);
                do{
                    k = k + 1;
		    getkey(ibuf,k);
#if TRACE
printf("+++k=%4"_LD_": %s",k,ibuf);
#endif
                } while (memcmp(ibuf,tbuf,keysize) < 0);
		if (k <= l) {
		    exchptr(l,k);
                }
                else
                    break;
            }
	    if ( (j - i) <= 10)
		break;
            m = m + 1;
            if ( l - i > j - k){
#if TRACE
printf("+++vai empilhar %"_LD_",%"_LD_"\n",i,l);
#endif
                il[m] = i; iu[m] = l; i = k;
            }
            else{
#if TRACE
printf("+++vai empilhar %"_LD_",%"_LD_"\n",k,j);
#endif
                il[m] = k; iu[m] = j; j = l;
            }
        }
#if TRACE
printf("+++vai ordenar %"_LD_" - %"_LD_"\n",i,j);
#endif
        for(ii=i, i = i + 1; i <= j; i++){
	    ptr=findptr(i);
	    getkey(tbuf,i);
#if TRACE
printf("+++     i=%4"_LD_": %s",i,tbuf);
#endif
	    k = i - 1;
	    getkey(ibuf,k);
#if TRACE
printf("+++     k=%4"_LD_": %s",k,ibuf);
#endif
            if (memcmp(ibuf,tbuf,keysize) > 0){
                do {
		    writptr(k+1,findptr(k)); k = k - 1;
		    if (k < ii)
			break;
		    getkey(ibuf,k);
#if TRACE
printf("+++     k %4"_LD_": %s",k,ibuf);
#endif
		} while (memcmp(ibuf,tbuf,keysize) > 0);
		writptr(k+1,ptr);
            }
        }
    }
    return;
}
