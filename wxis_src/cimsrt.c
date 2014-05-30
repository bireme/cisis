/* ---------------------------------------------------------------- CIMSRT.C */

/* ---------------------------------------------------------- C HEADER FILES */
#include <string.h>
/* #include <mem.h> */
/* ------------------------------------------------------------ HEADER FILES */
#include "../cisis.h"
#include "easyfc.h"
#include "easyci.h"
#include "cimsrt.h"
/* --------------------------------------------------------------- externals */
extern RECSTRU *vrecp[];	/* cirun.h */
extern TRMSTRU *vtrmp[];	/* cirun.h */
extern char fmterrxy[];		/* cifm3.c */

/* ============================================================= cms_writptr */
void cms_writptr(LONGX mfn,
                      XRPTR ptr,
                 LONGX idx)
{
    DBXSTRU *dbxp;
    LONGX xbyte;
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

    dbxp = VRECdbxp(idx);

    thispos = (mfn+XRMAXTV1) / XRMAXTIV;

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

    recxref(vrecp[idx],mfn,&wcomb,&wcomp); /* already buffered */
    if (labs(DBXxribp->xrxrpos) != thispos) fatal("msrt/writptr/thispos");
    thisidx = (mfn-1)%XRMAXTIV;
    DBXxribp->xrmfptr[thisidx] = ptr;
    changed = 1;

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
    p = (char *)&DBXxribp->xrmfptr[thisidx];
#if CNV_PCBINUM
    ConvertXRF_PTR(p);
#endif
    if (CIWRITE(DBXxropn,p,XRPTRSIZ) != XRPTRSIZ)
        fatal("msrt/writptr/write");
#endif /* 1 */
}
/* ============================================================= cms_findptr */
XRPTR cms_findptr(LONGX mfn,
                        LONGX idx)
{
    XRPTR ptr;
    DBXSTRU *dbxp;
    LONGX wcomb;
    int wcomp;

    if (recxref(vrecp[idx],mfn,&wcomb,&wcomp) == RCEOF) return 0L;
    dbxp = VRECdbxp(idx);
    ptr = DBXxribp->xrmfptr[(mfn-1)%XRMAXTIV];
    return ptr;
}
/* ============================================================= cms_exchptr */
void cms_exchptr(LONGX i,
                      LONGX j,
                 LONGX idx)
{
    XRPTR ptr;

    ptr = cms_findptr(i,idx);
    cms_writptr(i,cms_findptr(j,idx),idx);
    cms_writptr(j,ptr,idx);
}
/* ============================================================== cms_getkey */
void cms_getkey(char *buff,
                     LONGX buff_size,
                LONGX idx,
                     LONGX mfn,
                LONGX keysize,
                BOOLEAN fmtgen,
                FMT_CODE *pgmp,
                LONGX keyField)
{
    LONGX fmtrc;
   int i;
    unsigned left,len;
   char *p;

    record(idx,VRDBname(idx),mfn);
    if (VRECrc(idx) == RCEOF) {
        printf("*** mfn=%"_LD_"\n",mfn);
        fatal("msrt/getkey/RCEOF");
    }

    if (keyField) {

      i = fieldx(idx,(int)keyField,1);
      memmove(buff,VFIELDP(idx,i),VDIRlen(idx,i));
      *(buff+VDIRlen(idx,i)) = '\0';

   } else {

        fmtrc = fmt_inter(pgmp,idx,buff_size,buff,buff_size);
        if (fmtrc >= 0L && fmtgen) {
        p = strdup(buff);
          *buff = '\0';
        fmtrc = recfmt(idx,buff_size,p,buff,buff_size);
        efc_free(p);
       }
        if (fmtrc < 0L) {
            printf("*** Format RC = %"_LD_,fmtrc);
            fatal(fmterrxy);
        }

   }

    len = strlen(buff);
   if (len > keysize) len = keysize;
    left = keysize-len;

    if (left > 0) memset(buff+keysize-left,' ',left);
    sprintf(buff+keysize,"%06"_LD_,VMFRmfn(idx)); /* parmkeylen */
}
/* =========================================================== cms_sort_list */
void cms_sort_list(LONGX regsin,
                         char *tbuf,
                         char *ibuf,
                     LONGX buf_size,
                     LONGX idx,
                         LONGX keysize,
                     BOOLEAN fmtgen,
                     FMT_CODE *pgmp,
                   LONGX keyField)
{
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    - Comm ACM, v12(3) march, 1969 pp185-187 - Algorithm 347
   - Copied from msrt.c, original name: ordena
   - adapted to receive local parameter instead of global ones

   1.0 - 27.Nov.1998
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
    LONGX parmi;
    LONGX ii,ij,k,l,m;
    LONGX il[64],iu[64];
    LONGX i,j;
    XRPTR ptr;

   m = 0L;
   il[m] = parmi = 1L;
   iu[m] = regsin;

   while (m >= 0) {

        i = il[m];
      j = iu[m];
      m--;

        while ((j - i > 10) || (i == parmi && i < j)) {
            ij = (i + j) / 2; /* le tbuf */
            cms_getkey(tbuf,buf_size,idx,ij,keysize,fmtgen,pgmp,keyField);
            k = i;
            l = j;
            cms_getkey(ibuf,buf_size,idx,i,keysize,fmtgen,pgmp,keyField);
            if (memcmp(ibuf,tbuf,keysize) > 0) {
                cms_exchptr(ij,i,idx);
                memmove(tbuf,ibuf,keysize); /* t=a[ij] */
            }
            cms_getkey(ibuf,buf_size,idx,j,keysize,fmtgen,pgmp,keyField);
            if (memcmp(ibuf,tbuf,keysize) < 0) {
                cms_exchptr(ij,j,idx);
                memmove(tbuf,ibuf,keysize); /* t=a[ij] */
                cms_getkey(ibuf,buf_size,idx,i,keysize,fmtgen,pgmp,keyField);
                if (memcmp(ibuf,tbuf,keysize) > 0) {
                    cms_exchptr(ij,i,idx);
                    memmove(tbuf,ibuf,keysize); /* t=a[ij] */
                }
            }
            while (TRUE) {
                do {
                    l--;
                    cms_getkey(ibuf,buf_size,idx,l,keysize,fmtgen,pgmp,keyField);
                } while (memcmp(ibuf,tbuf,keysize) > 0);
                do {
                    k++;
                    cms_getkey(ibuf,buf_size,idx,k,keysize,fmtgen,pgmp,keyField);
                } while (memcmp(ibuf,tbuf,keysize) < 0);
                if (k <= l) {
                    cms_exchptr(l,k,idx);
                } else
                break;
            } /* while */
            if ((j - i) <= 10) break;
            m++;
            if (l - i > j - k) {
                il[m] = i;
            iu[m] = l;
            i = k;
            } else {
                il[m] = k;
            iu[m] = j;
            j = l;
            }
        }
        for(ii = i,i++; i <= j; i++) {
            ptr = cms_findptr(i,idx);
            cms_getkey(tbuf,buf_size,idx,i,keysize,fmtgen,pgmp,keyField);
            k = i - 1;
            cms_getkey(ibuf,buf_size,idx,k,keysize,fmtgen,pgmp,keyField);
            if (memcmp(ibuf,tbuf,keysize) > 0) {
                do {
                    cms_writptr(k+1,cms_findptr(k,idx),idx);
                k--;
                    if (k < ii) break;
                    cms_getkey(ibuf,buf_size,idx,k,keysize,fmtgen,pgmp,keyField);
                } while (memcmp(ibuf,tbuf,keysize) > 0);
                cms_writptr(k+1,ptr,idx);
         }
        }
    }
}
/* ================================================================ cms_sort */
BOOLEAN cms_sort(char *db,
                  LONGX rec_size,
                      LONGX parmkeylen,
                  FMT_CODE *fmtcode,
                  BOOLEAN fmtgen,
                  BOOLEAN parmmfn,
                 LONGX keyField)
{
    EFC_ERROR err;
   LONGX nxtmfn;
   LONGX idx;
   LONGX fmtkeylen;
   char *ibuf;
   char *tbuf;
   char *p;
    int n,rc;
    DBXSTRU *dbxp;	/* mandatory for defines DBX */
    LONGX mfn;
    off_t seek;
    int fd;
    LONGX wcomb;
    int wcomp;
#if CNV_PCBINUM
    char convmfn[sizeof(mfn)];
#endif
    char *xryyp;

    nxtmfn = eci_last_mfn(&err,db)+1L;
   if (nxtmfn < 1L) return FALSE;

    idx = eci_rec_new(&err,rec_size);
   if (idx < 0L) return FALSE;
   eci_rec_active(idx);
   record(idx,db,1L);

    fmtkeylen = parmkeylen+6; /* %6ld */

   ibuf = efc_new(rec_size);
   tbuf = efc_new(rec_size);
   if (!ibuf || !tbuf) return FALSE;

    mstflush(db);
    mstsetup(db,(LONGX)((nxtmfn/XRMAXTIV + 1) * XRBSIZ)/*loadxrf*/,0L);
    dbxp = VRECdbxp(idx);
    if (DBXxryyp == NULL) fatal("msrt/loadxrf/overflow");
    xryyp = DBXxryyp; /* keep */
    DBXxryyp = NULL;  /* avoid FREE */
    mstflush(db);
    dbxopt_ordwr = O_RDWR;
    mstsetup(db,0L,0L);
    DBXxryyp = xryyp; /* restore */

    /* processing */
    cms_sort_list(nxtmfn-1L,tbuf,ibuf,rec_size,idx,fmtkeylen,fmtgen,fmtcode,keyField);
#if 0 /* 1 */
    cms_writptr(0,0); /* breakout */
#else
    if (!DBXxropw) fatal("msrt/xropw/break");
    if (LSEEK64(DBXxropn,0L,SEEK_SET) != 0) fatal("msrt/seek/break");
    if (!DBXxryyp) fatal("msrt/xryyp/break");
    for (xryyp = DBXxryyp; ; xryyp+=XRBSIZ) {
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
        for (mfn = 1L; ; mfn++) { /* not MFRmfn */
            rc = recxref(vrecp[idx],mfn,&wcomb,&wcomp);
         if (rc == RCEOF) break;
            if (rc == RCPDEL) continue;
            seek = (off_t)((wcomb-1)<<MSBSHIFT);
            seek += wcomp;
            fd = DBXmsopn;
            if (LSEEK64(fd,seek,SEEK_SET) != seek) fatal("msrt/mfn/seek");
            n = sizeof(mfn);
            p = (char *)&mfn;
#if CNV_PCBINUM
            strcpy(convmfn,p);
         ConvertBuffer(convmfn),n);
            p = convmfn;
#endif
            if (CIWRITE(fd,p,n) < n) fatal("msrt/mfn/write");
        }
    }

    efc_free(ibuf);
    efc_free(tbuf);
    eci_rec_free(idx);

    return TRUE;
}

