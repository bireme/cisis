/* ------------------------------ mxbol.c --------------------------------- */

#include <stdio.h>
#include <string.h>

#if 1
#if CIAPI
#include "ciapi.h"   /* CISIS Interface API header file -
                  includes cisis, implements cirun.h and ci*.c using ci*.?pp */

#if !MXFUN
extern void *ciapip;
#endif /* MXFUN */

#else /* CIAPI */
#include "cisis.h"                  /* CISIS Interface */
#endif /* CIAPI */
#include "mx.h"                     /* MX Program header file */
#else
#include "\cisis\include\cisis.h"   /* CISIS Interface header file */
#include "\cisis\mx\mx.h"           /* MX Program header file */
#endif


#if CIB71

int mxbool(xmxp,parmp,dbnp)
void *xmxp;
char *parmp;
char *dbnp;
{
    RECSTRU *recp;
    char *p,*q;
    LONGX mfn;
    LONGX crec=0L; /* b5_exp usa crec condicionado a booldbnp */
    int errno,xfd;
#if !MXFUN
    char mx_bup[512];
#endif
    LONGX asize;

    cib7p=b7_cib7(cib7p, ciapip);

    if (strncmp(parmp,"bool?",5) == 0 || strncmp(parmp,"bool=",5) == 0)
        q=parmp+5;
    else
        q=parmp;

    boolqryp=loadfile(dbnp,'@',q,boolqryp,MAXBOOL,'\0');
    if (!boolqryp) fatal(q);

    for (boolridx=nrecs, mfn=1; ; mfn++) { /* get actual maxmfn */
        RECORD(boolridx,dbnp,mfn);
	if (RECrc == RCEOF) fatal(dbnp);
	if (RECrc == RCNORMAL) break;
    }

    if (booldbnp) {
        dbxopt_fatal=0; xfd=dbxopen("",booldbnp,xx1extp);
        if (xfd > 0) CLOSE(xfd); else recisis0(booldbnp);
        RECORD(crec=nrecs,booldbnp,0L);
        b7setno=MF0nxtmfn-1L;
/*
<R>
        vrecp[boolridx]->recdbxp=RECdbxp;
*/
	VRECdbxp(boolridx)=RECdbxp;
    }
    else {
        VRECdbxp(boolridx)=NULL; /* nao usa base de log */
        VMFRmfn(boolridx)=0L;
    }

    /* allocate buffup
    */
    asize=     strlen(dbnp)     +                 /* field MFQTDBN0=11 (may be longer than CIMPL+1) */
               strlen(boolqryp) +                 /* the expression */
               2*strlen(boolqryp) +               /* the terms + pseudo operators _1termX+11X */
               BUFSIZ+strlen(dbnp)     +          /* field MFQSWDBN=20 + a diferent db */
               24*(strlen(B7DELADD)+3*B7UPLEN) +  /* 2 duzias de control fields */  /* DxxAxx|xxx| */ 
               strlen(boolqryp)+4 +               /* tmpfile .%03x */
               BUFSIZ;                            /* por si las moscas */

    if (!mx_bup || mx_buplen < asize) {   
        if (mx_bup) {
#if CICPP
	        delete [] mx_bup;
#else /* CICPP */
            FREE(mx_bup);
#endif /* CICPP */
        }
        mx_buplen=asize;
        mx_bup=loadfile(NULL,'@',"",NULL,mx_buplen,'\0');
    }
    
    p=b7_exp(cib7p,boolridx,dbnp,boolqryp,mx_bup,booldbnp,crec,&errno);
    if (p || errno) printf("Expression syntax error %d: '%s'\n",errno,p);
    else {
        /* set hit file name to "null=don't write" or as provided by "b70x=" */
        if (!booldbnp || booltmpnam[0]) {
            char *hitfnamp="null";
            if (booltmpnam[0]) hitfnamp=booltmpnam;
    	    sprintf(mx_bup,"D%dA%d|%s|",MFQTHMEM,MFQTHMEM,hitfnamp);
    	    if (fldupdat(boolridx,mx_bup) != NULL) fatal("mxbol/fldupdat");
    	    if (b70trace) prtfields(vrecp[boolridx],VMFRmfn(boolridx));
    	}
        /* b7_run faz "trirecp=vrep[tlirec]" */
        if (!booltlir) record(booltlir=nrecs,"null",1L);

    	p=b7_run(cib7p,boolridx,dbnp,mx_bup,mx_buplen,booltell,isisuctab,"",booltlir,crec,&errno);
    	if (p || errno) printf("Execution error %d: '%s'\n",errno,p);
    	else {
    	    boolhits=b7_hit(cib7p,boolridx,boolh=0L,b7setno);
    	    if (booltell) printf("Hits=%"_LD_"\n",boolhits);
    	}
    }
    if (p || errno) return(1);
    return(0);
}

#else /* CIB71 */

#if LIND || CIB64

int mxbool(xmxp,parmp,dbnp)
void *xmxp;
char *parmp;
char *dbnp;
{
    RECSTRU *recp;
    char *p,*q;
    LONGX mfn;
    LONGX crec=0L; /* b5_exp usa crec condicionado a booldbnp */
    int errno,xfd;
#if !MXFUN
    char mx_bup[512];
#endif
    extern LONGX b5maxbyts0; /* <== mover para cisis.h (rever estrut. b7) */

#if 0
    if (argc < 5) {
        /* fatal("b6 <dbn> <bytes1> <bufsiz> <expr> [<hits> [x] [y]]"); */
        printf("%s",cicopyr("Utility B6"));
        printf("\n");
        printf("b6 <dbn> <bytes1> <bufsiz> <expr> [options]");
        printf("\n");
        printf("options: <hits>          -> max hits to show \n");
        printf("         <x>             -> don't show hits \n");
        printf("         <y>             -> max bitstring (in bytes) \n");
        printf("\n");
        exit(1);
    }
#endif
    
    if (strncmp(parmp,"bool?",5) == 0 || strncmp(parmp,"bool=",5) == 0)
        q=parmp+5;
    else
        q=parmp;

    boolqryp=loadfile(dbnp,'@',q,boolqryp,MAXBOOL,'\0');
    if (!boolqryp) fatal(q);

    if (parmtrace) {
        showcore("+++ bool");
        printf("+++ bool=%c",OPENFCHR); printf("%s",boolqryp);
        printf("%c ..",CLOSFCHR); if (getchar() != '\n') return(1);
    }

    if (boolnby0) b5maxbyts0=boolnby0;

    for (boolridx=nrecs, mfn=1; ; mfn++) { /* get actual maxmfn */
        RECORD(boolridx,dbnp,mfn);
	if (RECrc == RCEOF) fatal(dbnp);
	if (RECrc == RCNORMAL) break;
    }
    if (!boolnby1) boolnby1=dbxstorp(dbnp)->dbxmsmfn/8+1;


#if BIGREC
#else
    if (boolnbyb) b6_hballoc(boolnbyb,&b6bufferp);
#endif

    if (booldbnp) {
        dbxopt_fatal=0; xfd=dbxopen("",booldbnp,xx1extp);
        if (xfd > 0) CLOSE(xfd); else recisis0(booldbnp);
        RECORD(crec=nrecs,booldbnp,0L);
        b5setno=MF0nxtmfn-1L;
        vrecp[boolridx]->recdbxp=RECdbxp;
    }
    else {
        VRECdbxp(boolridx)=NULL; /* nao usa base de log */
        VMFRmfn(boolridx)=0L;
    }

    p=b5_exp(boolridx,dbnp,boolqryp,mx_bup,booldbnp,crec,&errno);
    if (p || errno) printf("Expression syntax error %d: '%s'\n",errno,p);
    else {
	if (!booldbnp) {
	    if (!booltmpnam[0]) 
#if BEFORE991117 || 1
	        tmpnam(booltmpnam);
#else
/*strcpy(booltmpnam,"c:\\TMP1.$$$");
*/
	        if (!dbxtmpnm("CI_TEMPDIR", 0, booltmpnam))
		    fatal("mxbol/dbxtmpnm");
#endif
	    sprintf(mx_bup,"D%dA%d|%s|",MFQTHMEM,MFQTHMEM,booltmpnam);
	    if (fldupdat(boolridx,mx_bup) != NULL) fatal("mxbol/fldupdat");
	    if (b50trace) prtfields(vrecp[boolridx],VMFRmfn(boolridx));
	}
#if BIGREC
	p=b5_run(boolridx,dbnp,mx_bup,
		boolnby1,booltell,isisuctab,"",booltlir,crec,&errno);
#else
	p=b6_run(boolridx,dbnp,mx_bup,mx_buplen,
		boolnby1,booltell,isisuctab,"",booltlir,crec,&errno);
#endif
	if (p || errno) printf("Execution error %d: '%s'\n",errno,p);
	else {
#if BIGREC
	    boolhits=b5_hit(boolridx,boolh,mx_bup);
#else /* BIGREC */
	    boolhits=b6_hit(boolridx,boolh,b5setno,b6bufferp);
#endif /* BIGREC */
	    if (booltell) printf("Hits=%"_LD_"\n",boolhits);
	}
    }
    if (p || errno) return(2);
    return(0);
}


#else /* LIND || CIB64 */

int mxbool(xmxp,parmp,dbnp)
void *xmxp;
char *parmp;
char *dbnp;
{
#if !MXFUN
    extern char *mx_bup;
#endif

    char *p,*q,*fldp;
    int errno,xdir;
    FFI fldl;
    LONGX nby1,mfn,cc;
    char area[B4UPLEN+1];


    if (strncmp(parmp,"bool?",5) == 0 || strncmp(parmp,"bool=",5) == 0)
        q=parmp+5;
    else
        q=parmp;

    boolqryp=loadfile(dbnp,'@',q,boolqryp,MAXBOOL,'\0');
    if (!boolqryp) fatal(q);

    if (parmtrace) {
        showcore("+++ bool");
        printf("+++ bool=%c",OPENFCHR); printf("%s",boolqryp);
        printf("%c ..",CLOSFCHR); if (getchar() != '\n') return(1);
    }

    nby1=dbxstorp(dbnp)->dbxmsmfn/8+1;
    if (!boolridx) {
        boolridx=nrecs;
        recallok(boolridx,BUFSIZ+MAXBOOL+nby1);
        if (!booltlir)
            invsetup(dbnp,parmload,parmload,parmload);/* allocate areas */
        else
            for (xdir=0; xdir <= VMFRnvf(booltlir); xdir++)
                if (VDIRtag(booltlir,xdir) == MFQTXTAG) {
                    fldl=VDIRlen(booltlir,xdir);
                    fldp=subfldp(VFIELDP(booltlir,xdir),'y',&fldl);
                    fldl=subfldn(fldp,fldl);
                    if (fldl) {
                        memmove(mx_bup,fldp,fldl);
                        mx_bup[fldl]='\0';
                        invsetup(mx_bup,parmload,parmload,parmload);
                    }
                }
    }

    if (strcmp(boolqryp,"#") == 0) {
        if (booldbnp) {
            if (b4setno > 0) {
                for (mfn=1; mfn <= b4setno; mfn++) {
                    record(boolridx,VRDBname(boolridx),mfn);
                    if (VRECrc(boolridx) == RCNORMAL) {
                        sprintf(mx_bup,"D%dD%dD%dD%d",
                            MFQTHCUR,MFQTHMFN,MFQTHSIZ,MFQTHMEM);
                        if (fldupdat(boolridx,mx_bup)) fatal("mxbool");
                        sprintf(mx_bup,"D%dD%dD%dD%dD%d",
                            MFQTDBN0,MFQTOBJ0,MFQTXCTS,MFQTASET,MFQTALCC);
                        if (fldupdat(boolridx,mx_bup)) fatal("mxbool");
                        prtfields(vrecp[boolridx],mfn);
                    }
                }
                if (booltlir)
                    prtfields(vrecp[booltlir],VMFRmfn(booltlir));
                record(boolridx,VRDBname(boolridx),b4setno);
                return(0);
            }
        }
        if (booltlir) {
            prtfields(vrecp[boolridx],b4setno);
            prtfields(vrecp[booltlir],VMFRmfn(booltlir));
            return(0);
        }
    }

    p=b4_exp(boolridx,dbnp,boolqryp,booldbnp,&errno);
    if (p || errno) {
        printf("Expression syntax error %d: '%s'\n",errno,p);
        if (b40trace)
            if (vrecp[boolridx])
                prtfields(vrecp[boolridx],VMFRmfn(boolridx));
    }
    else {
        p=b4_run(boolridx,dbnp,nby1,boolnby2,booltell,
                                        isisuctab,booltlir,&errno);
        if (p || errno) {
            printf("Execution error %d: '%s'\n",errno,p);
            if (b40trace)
                if (vrecp[boolridx])
                    prtfields(vrecp[boolridx],VMFRmfn(boolridx));
        }
        else {
            boolhits=b4_hit(boolridx,boolh=0L);
            if (boolhits >= 0) {
                recfield(area,boolridx,MFQTALCC,1,NULL);
                if ((cc=atol(area)) != 0) sprintf(area," [cc=%"_LD_"]",cc);
                else strcpy(area," ");
                if (booltell) printf("Hits=%"_LD_" %s\n",boolhits,area);
            }
            else
                if (vrecp[boolridx])
                    prtfields(vrecp[boolridx],VMFRmfn(boolridx));
        }
    }
    if (p || errno) return(2);
    return(0);
}

#endif /* LIND || CIB64 */
#endif /* CIB71 */

