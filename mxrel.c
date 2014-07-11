/* ------------------------------ mxrel.c --------------------------------- */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

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

/* ========================== mp_scan/mp_edit ============================= */

int mp_scan(xmxp,gidbnp,specp)
void *xmxp;
char *gidbnp;
char *specp;
{

    if (mp_n >= MAXNPPP) fatal("MAXNPPP");

    mp_pgmp[mp_n]=(FMT_CODE *)NULL;
    
#if 1 //BEFORE20060309
    mp_areap=loadfile(gidbnp,'@',specp,mp_areap,fmtasize+1,'\0'); //alloc 1 vez
    if (!mp_areap) fatal(specp);                      // perde mp_areap se erro
#else
    if (!mj_n) {
        mp_areap=loadfile(NULL,'@',"",NULL,fmtasize+1,'\0');     //alloc
        if (!mp_areap) fatal("mp_scan/ALLOC");
    }
    if (!loadfile(gidbnp,'@',specp,mp_areap,fmtasize+1,'\0')) fatal(specp);
#endif

    if (parmtrace) {
        printf("+++ proc=%c",OPENFCHR); printf("%s",mp_areap);
        printf("%c ..",CLOSFCHR); if (getchar() != '\n') return(1);
    }

    if (fmt_gener(&mp_pgmp[mp_n],mp_areap)) fatal(specp+fmt_errof);

    mp_n++;
    return(0);
}

int mp_edit(xmxp,irec,i1,i2)
void *xmxp;
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
#if BEFORE20010213
#else
        if (lns < 0) fatal("mx/proc/format execution error");
#endif

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
    return(0);
}


int mp_free(xmxp)
void *xmxp;
{
    while (mp_n--) {
        if (mp_pgmp[mp_n]) 
	    { fmt_free(mp_pgmp[mp_n]); mp_pgmp[mp_n]=(FMT_CODE *)NULL; }
    }
    if (mp_areap) { FREE(mp_areap); mp_areap=(char *)NULL; }
    mp_n=0;
    return(0);
}


/* ====================== mj_scan/mj_edit/mj_join ========================= */

int mj_scan(xmxp,gidbnp,dbnamp,specp,parmjchk)
void *xmxp;
char *gidbnp;
char *dbnamp;
char *specp;
int parmjchk;
{
    char *p;
    UWORD tag,tag1,tag2;
    LONGX n1,n2;
    MJ_TAGS *mj_tagp;
    MJ_STWS *mj_stwp;
    char dbname[CIMPL+1];
    int nstws;

    if (isiswctot <= 0) fatal("mj_scan/isiswctot");

    strcpy(dbname,dbnamp);

    if (mj_n >= MAXNJJJ)
        fatal("MAXNJJJ");

    mj_vtagp[mj_n]=(MJ_TAGS *)NULL;
    mj_vstwp[mj_n]=(MJ_STWS *)NULL;
    mj_pgmp[mj_n]=(FMT_CODE *)NULL;
    mj_ridx[mj_n]=0;
    mj_plustag[mj_n]=0; /* AOT 23/04/2005 */

    mj_pjchk[mj_n]=parmjchk;

    for (p=dbname; *p; p++) { /* for p=dbname */
        if (*p == '+') {
            *p++ = '\0';
            mj_stwp=(MJ_STWS *)loadstw(gidbnp,p,(char *)NULL,0L,&nstws);
            if (!mj_stwp) fatal(p);
            if (parmtrace) printf("+++ nstws=%d\n",nstws);
            mj_vstwp[mj_n]=mj_stwp;
            break;
        }
    } /* end for p=dbname */

    mj_tagp=NULL;
    mj_vtagp[mj_n]=NULL;
    for (p=dbname; *p; p++) { /* for p=dbname */
        if (*p == ':') { /* AOT 23/04/2005 */
            *p++ = '\0';
            while ( isdigit(*p) ) {
                mj_plustag[mj_n]=mj_plustag[mj_n]*10+((int)(*p)-(int)'0');
                *p++ = '\0';
            }
            if (parmtrace && mj_plustag[mj_n]) {
                printf("+++ plustag=%"_LD_"\n",mj_plustag[mj_n]);
            }
        } /* end AOT 23/04/2005 */
        if (*p == ',') {
            *p++ = '\0';
            mj_tagp=(MJ_TAGS *)ALLOC((ALLOPARM)sizeof(MJ_TAGS));
            if (mj_tagp == (MJ_TAGS *)ALLONULL)
                fatal("mj_scan/ALLOC");
            mj_vtagp[mj_n]=mj_tagp;
            memset(mj_tagp->tagv,0x00,sizeof(MJ_TAGS));
            while (*p) {
                if (sscanf(p,"%"_LD_"/%"_LD_,&n1,&n2) == 2) {
                    tag1=n1; tag2=n2; /* UWORD */
                    if (tag1 > tag2) fatal(p);
                    while (*p != '/') p++; p++;
                    while (isdigit(*p)) p++;
                    for (tag=tag1; tag <= tag2; tag++)
                        if (tag < MAXVTAG) mj_tagp->tagv[tag]=tag;
                    if (*p == ',') if (*(p+1)) p++;
                    continue;
                }
                if (sscanf(p,"%"_LD_":%"_LD_,&n1,&n2) == 2) {
                    tag1=n1; tag2=n2; /* UWORD */
                    while (*p != ':') p++; p++;
                    while (isdigit(*p)) p++;
                    if (tag2 < MAXVTAG) mj_tagp->tagv[tag2]=tag1;
                    if (*p == ',') if (*(p+1)) p++;
                    continue;
                }
                if (sscanf(p,"%"_LD_,&n1) == 1) {
                    tag=n1; /* UWORD */
                    while (isdigit(*p)) p++;
                    if (tag < MAXVTAG) mj_tagp->tagv[tag]=tag;
                    if (*p == ',') if (*(p+1)) p++;
                    continue;
                }
                fatal(p);
            }
            break;
        }
    } /* end for p=dbname */
    if (parmtrace && mj_tagp) {
        for (tag=0; tag < MAXVTAG; tag++)
            if (mj_tagp->tagv[tag])
                printf("+++ tag=%d tagv=%d\n",tag,mj_tagp->tagv[tag]);
    }

    mj_ridx[mj_n]=nrecs;
    if (parmjchk) {
#if BEFORE20010109
        recallok(mj_ridx[mj_n],MAXMFRL);   /* alloc */
#else
        recallok(mj_ridx[mj_n],rec_maxmfrl);   /* alloc */
#endif
        VRECdbxp(mj_ridx[mj_n]) = dbxstorp(dbname);
        VRECtype(mj_ridx[mj_n]) = TYPEMFR;
    }
    else {
        mstsetup(dbname,parmload,parmload);
        record(mj_ridx[mj_n],dbname,1L);   /* alloc and setup */
    }

    if (!mj_n) {
        if (parmjchk)
            trmalloc(mj_tidx=ntrms,0L);                         /* posting() */
        else
            trmalloc(mj_tidx=ntrms,IFBSIZ);
        if ((mj_areap=(char *)ALLOC((ALLOPARM)(MJASIZE+1))) 
                        == (char *)ALLONULL) fatal("mj_areap");
#if BEFORE20010109
        if ((mj_buffup=(char *)ALLOC((ALLOPARM)(BUFFUPL))) == (char *)ALLONULL)
#else
        if ((mj_buffup=(char *)ALLOC((ALLOPARM)mx_buplen)) == (char *)ALLONULL)
#endif
            fatal("mj_buffup");
    }

    if (!loadfile(gidbnp,'@',specp,mj_areap,MJASIZE,'\0')) fatal(specp); /* tmp */

    if (fmt_gener(&mj_pgmp[mj_n],mj_areap)) fatal(specp+fmt_errof);

    if (parmtrace) {
        printf("+++ join=%c",OPENFCHR);
        printf("%s",mj_areap); printf("%c ..",CLOSFCHR);
        if (getchar() != '\n') return(2);
    }

    mj_n++;
    return(0);
}

int mj_edit(xmxp,ridx,i1,i2,tagoff,areapp)
void *xmxp;
LONGX ridx;
int i1;
int i2;
int tagoff;
char **areapp;
{
    int ijjj;
    LONGX lns;
    char *p,*keyp,*q,word[LE2+1],*sp;
    int wn,k,isstw,wocc,wcnt,wood;
    MJ_STWS *mj_stwp;

    for (ijjj=i1; ijjj <= i2; ijjj++) {
        mj_stwp=mj_vstwp[ijjj];
        *mj_areap = '\0';                       /* call format */
        lns=fmt_inter(mj_pgmp[ijjj],ridx,(LONGX)MJASIZE,mj_areap,(LONGX)MJASIZE);
#if BEFORE20010213
#else
        if (lns < 0) fatal("mx/join/format execution error");
#endif

        if (parmtrace)
            printf("+++ join=%"_LD_"=%s.\n",lns,mj_areap);

        if (areapp) {
            *areapp=mj_areap;
            return(0);
        }

        for (wocc=0, p=keyp=mj_areap; *keyp; ) {
            if (*p != '\r')
                if (*p != '\n')
                    if (*p) {
                        p++;
                        continue;
                    }

            wocc++;

            while (*p == '\r' || *p == '\n') {
                *p++ = '\0';
            }

            if (mj_stwp) {
                for (wcnt=0, q=keyp, wn=0; ; ) {
                    if (wn >= LE2) for(; isiswctab[*q]; ) q++;
                    if (isiswctab[*q]) {
                        word[wn++] = *q++;
                        if (*q)
                            continue;
                    }
                    word[wn] = '\0';
                    while (!isiswctab[*q]) if (!*q) break; else q++;
                    wood = (wn > LE1) ? 2 : 1;
                    wcnt++;
                    if (parmtrace) {
                        printf("+++ word[%d,%d]=%s\n",wocc,wcnt,word);
                    }
                    isstw=0;
                    if (wood == 1)
                        for (sp=mj_stwp; *sp; sp+=LE1+1) {
                            k=memcmp(sp,word,wn+1);
                            if (k > 0) break;
                            if (k < 0) continue;
                            isstw=1; break;
                        }
                    if (!isstw) mj_join(xmxp,ridx,ijjj,word,tagoff,wood,wocc,wcnt);
                    if (!*q) break;
                    wn=0;
                }
            }
            else {
                wood = (strlen(keyp) > LE1) ? 2 : 1;
                mj_join(xmxp,ridx,ijjj,keyp,tagoff,wood,wocc,0);
            }
            keyp=p;
        }

    }
    return(0);
}


#define FDLM '\n'

int mj_join(xmxp,ridx,ijjj,keyp,tagoff,wood,wocc,wcnt)
void *xmxp;
LONGX ridx;
int ijjj;
char *keyp;
int tagoff;
int wood;
int wocc;
int wcnt;
{
    RECSTRU *recp;
    TRMSTRU *trmp;

    int dirloop,xdir,rc;
    LONGX pmfn,left,x;
    char *p,*batchp;
    UWORD tag;
    MJ_TAGS *mj_tagp;
    int parmjchk;
#if JOINALL
    LONGX joined=0L;
#endif

    parmjchk=mj_pjchk[ijjj];

    recp=vrecp[mj_ridx[ijjj]]; rc=RCEOF;
    trmp=vtrmp[mj_tidx]; pmfn=0L;

    if (*keyp) {
        p=keyp;
        if (toupper(*p++) == 'M')
            if (toupper(*p++) == 'F')
                if (toupper(*p++) == 'N') {
                    pmfn=atol(p+1);     /* MFN=val or MFN val or etc */
                }
        if (pmfn == 0L) {
            if (parmjchk)
                invsetup(RDBname,parmload,parmload,-1L); /* jchk= (don't open .ifp) */
            else
                invsetup(RDBname,parmload,parmload,parmload); /* join= */

            term(mj_tidx,RDBname,keyp);

            if (TRMrc == RCNORMAL) {
                if (parmjchk)
                    pmfn=1L;                                    /* posting() */
                else
                    pmfn=posting(mj_tidx,1L);
            }
        }
        if (parmtrace)
            printf("+++ term=%s=%s: mfn=%"_LD_"\n",RDBname,keyp,pmfn);
    }

    batchp=mj_buffup; left=mx_buplen;   /* single fldupdat() */

#if JOINALL
LOOPMFN:
#endif
    if (pmfn > 0L) {
        if (parmjchk) {
            rc=RCNORMAL;
            if (fldupdat(mj_ridx[ijjj],"D*") != NULL)
                fatal("parmjchk/fldupdat");
            VMFRmfn(mj_ridx[ijjj])=pmfn;
        }
        else 
            rc=VRECrc(record(mj_ridx[ijjj],RDBname,pmfn));
	if (parmtrace) {
            printf("mj_ridx=%"_LD_" RDBname=%s pmfn=%"_LD_" rc=%d\n",
	        mj_ridx[ijjj],RDBname,pmfn,rc);
	    prtfields(vrecp[mj_ridx[ijjj]],pmfn);
	}
    }

    if (tagoff) {
        if (left <= MINBJJJ) fatal(keyp);
        sprintf(batchp,"A%6"_LD_"%c%s^l%d^k",
            (LONGX)(tagoff+ijjj+1),FDLM,RDBname,wood);
        batchp+=(x=strlen(batchp)); left-=x;
        x=strlen(keyp);
        if (left <= (x+MINBJJJ)) fatal(keyp);
        memmove(batchp,keyp,x); batchp+=x; left-=x;
        if (wocc) {
            if (left <= MINBJJJ) fatal(keyp);
            sprintf(batchp,"^o%d",wocc);
            batchp+=(x=strlen(batchp)); left-=x;
        }
        if (wcnt) {
            if (left <= MINBJJJ) fatal(keyp);
            sprintf(batchp,"^c%d",wcnt);
            batchp+=(x=strlen(batchp)); left-=x;
        }
        *batchp++ = FDLM; left--;
    }

    if (parmjchk < 0) {
        if (tagoff) {
            if (parmjchk == -1 && rc != RCNORMAL) {
                *(batchp-1)='\0';
                printf("%"_LD_"^j%s\n",VMFRmfn(ridx),mj_buffup+1+6+1);
            }
            if (parmjchk == -2 && rc == RCNORMAL) {
                *(batchp-1)='\0';
                printf("%"_LD_"^j%s\n",VMFRmfn(ridx),mj_buffup+1+6+1);
            }
        }
        return(0);
    }

    if (rc == RCNORMAL) {

        if (tagoff) {
            batchp--; left++;
            if (left <= MINBJJJ) fatal(keyp);
            sprintf(batchp,"^m%"_LD_"%c",MFRmfn,FDLM);
            batchp+=(x=strlen(batchp)); left-=x;
        }

        mj_tagp=mj_vtagp[ijjj];

        for (xdir=0, dirloop=MFRnvf; dirloop--; xdir++) {

            tag=DIRtag(xdir);

            if (mj_tagp) {
                if (tag >= MAXVTAG) continue;
                tag=mj_tagp->tagv[tag];
                if (tag <= 0) continue;
                if (parmtrace)
                    printf("+++ %s %"_LD_" (%d) - tag=%d tagv=%d\n",
                        RDBname,MFRmfn,MFRnvf,tag,mj_tagp->tagv[tag]);
            }
            if (left <= MINBJJJ) fatal(keyp);
            sprintf(batchp,"H%"_LD_" %"_LD_" ",(LONGX)tag+mj_plustag[ijjj],(LONGX)DIRlen(xdir));
            batchp+=(x=strlen(batchp)); left-=x;
            if (left <= (x=DIRlen(xdir))) fatal(keyp);
            memmove(batchp,FIELDP(xdir),x); batchp+=x; left-=x;
        }
#if JOINALL
        if (!parmjchk)
            while ((pmfn=posting(mj_tidx,TRMpost+1)) > 0)
                if (VRECrc(record(mj_ridx[ijjj],RDBname,pmfn)) == RCNORMAL) {
                    if (mj_jmax)
                        if (++joined >= mj_jmax) break;
                    if (parmtrace)
                        printf("+++ post=%s=%s: mfn=%"_LD_"\n",RDBname,keyp,pmfn);
                    goto LOOPMFN;
                }
#endif
    }

    *batchp = '\0';
    if ((p=fldupdat(ridx,mj_buffup)) != NULL)
        fatal(p);

    return(0);
}

int mj_free(xmxp)
void *xmxp;
{
    while (mj_n--) {
        if (mj_vtagp[mj_n]) { FREE(mj_vtagp[mj_n]); mj_vtagp[mj_n]=(MJ_TAGS *)NULL; }
        if (mj_vstwp[mj_n]) { FREE(mj_vstwp[mj_n]); mj_vstwp[mj_n]=(MJ_STWS *)NULL; }
        if (mj_pgmp[mj_n]) { fmt_free(mj_pgmp[mj_n]); mj_pgmp[mj_n]=(FMT_CODE *)NULL; }
        if (mj_ridx[mj_n]) {
#if CIAPI
	    FREE(vrecp[mj_ridx[mj_n]]); vrecp[mj_ridx[mj_n]]=NULL; nrecs-=1;
#else /* CIAPI */
	    FREE(vrecp[mj_ridx[mj_n]]); vrecp[mj_ridx[mj_n]]=NULL; nrecs--;
#endif /* CIAPI */
	}
    }
    if (mj_tidx) {
#if CIAPI
	    FREE(vtrmp[mj_tidx]); vtrmp[mj_tidx]=NULL; ntrms-=1;
#else /* CIAPI */
	    FREE(vtrmp[mj_tidx]); vtrmp[mj_tidx]=NULL; ntrms--;
#endif /* CIAPI */
    }
    if (mj_areap) { FREE(mj_areap); mj_areap=(char *)NULL; }
    if (mj_buffup) { FREE(mj_buffup); mj_buffup=(char *)NULL; }
    mj_n=0;
    return(0);
}
