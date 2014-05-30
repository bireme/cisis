LONGX b5_hit(irec,nord,buffup)                                         /*
-----------
                    abenda se vrecp nao inicializado;
                    abenda se registro irec nao alocado;
                    abenda se registro irec invalido;
                    retorna total de hits se nord = 0;
                    se nord > total de hits
                        zera hit e mfn correntes e retorna -1;
                    mantem controles para chamadas subsequentes;
                    retorna o mfn correspondente ao hit nord
                    (retorna -2 .. -n se algum erro)
                                                                      */
LONGX irec;          /* indice de vrecp, para info/store */
LONGX nord;          /* numero de ordem do hit a analisar */
char *buffup;       /* area addr for fldupdat */
{
    RECSTRU *recp;
    LONGX hits,hcur,hsiz,hmfn;
    int xdir_hits,xdir_hcur,xdir_hsiz,xdir_hmfn,xdir_hmem;
    unsigned char *p;
    LONGX offset,mfn;
    LONGX loop;
    int loop2,n,delta8,delta1;
#if CHKFOUND
    int found;
#endif
#if HITTRACE
 printf("b6_hit - irec,nord: %"_LD_"%"_LD_" \n",irec,nord);
#endif
#if SHOWCORE
 if (b60trace) showcore("b6_hit");
#endif
    strcpy(b62errxy,"b6_hit");          /* b6_hiterr() */
    if (!nrecs)
#if CDROM
        return(-2L);
#else
        fatal("b6_hit/nrecs");
#endif
    recp=vrecp[irec];
    if (!recp)
#if CDROM
        return(-3L);
#else
        fatal("b6_hit/irec");
#endif
    if (RECtype != TYPEMFQ || RECrc != RCNORMAL)
#if CDROM
        return(-4L);
#else
        fatal("b6_hit/check");
#endif
    if ((xdir_hits=fieldx(irec,MFQTHITS,1)) < 0)
#if CDROM
        return(-5L);
#else
        fatal("b6_hit/xdir_hits");
#endif
    for (hits=0, p=FIELDP(xdir_hits), loop=DIRlen(xdir_hits); loop--; p++)
        hits = hits * 10 + (*p - '0');
    if (nord < 0 || nord > (hits+1))
#if CDROM
        return(-6L);
#else
        fatal("b6_hit/nord");
#endif
    if ((xdir_hcur=fieldx(irec,MFQTHCUR,1)) < 0)
#if CDROM
        return(-7L);
#else
        fatal("b6_hit/xdir_hcur");
#endif
    if ((xdir_hmfn=fieldx(irec,MFQTHMFN,1)) < 0)
#if CDROM
        return(-8L);
#else
        fatal("b6_hit/xdir_hmfn");
#endif
    if ((xdir_hsiz=fieldx(irec,MFQTHSIZ,1)) < 0)
#if CDROM
        return(-9L);
#else
        fatal("b6_hit/xdir_hsiz");
#endif
    if (nord == 0 || nord > hits) {
        hcur=hmfn=0;
        if (nord == 0)
            mfn=hits;           /* returning value */
        else
            mfn=(-1L);
    }
    else {
        for (hcur=0, p=FIELDP(xdir_hcur), loop=DIRlen(xdir_hcur); loop--; p++)
            hcur = hcur * 10 + (*p - '0');
        for (hmfn=0, p=FIELDP(xdir_hmfn), loop=DIRlen(xdir_hmfn); loop--; p++)
            hmfn = hmfn * 10 + (*p - '0');
        for (hsiz=0, p=FIELDP(xdir_hsiz), loop=DIRlen(xdir_hsiz); loop--; p++)
            hsiz = hsiz * 10 + (*p - '0');
#if HITTRACx
 printf("b6_hit - rec - hits=%"_LD_" hcur=%"_LD_" hsiz=%"_LD_" hmfn=%"_LD_"\n",
  hits,hcur,hsiz,hmfn);
#endif
        if (hcur == nord)
            mfn=hmfn;
        else {
            if ((xdir_hmem=fieldx(irec,MFQTHMEM,1)) < 0)
#if CDROM
                return(-10L);
#else
                fatal("b6_hit/xdir_hmen");
#endif
            if (nord > hcur) {
#if UNIX
                delta8 =  8; delta1 =  1;
#else           
                delta8 = +8; delta1 = +1;
#endif
            }
            else {
                delta8 = -8; delta1 = -1;
            }
#if CDUSP
            mfn = (hcur == 0) ? 0 : hmfn+delta1;
#else
            mfn = (hmfn == 0) ? 0 : hmfn+delta1;
#endif
            offset=(mfn/8);
            for (p=FIELDP(xdir_hmem)+offset; hcur != nord; ) {
                loop=(nord > hcur) ? hsiz-(mfn/8) : (mfn/8);
#if CHKFOUND
                found=0;
#endif
#if HITTRACx
 printf("b6_hit - hcur=%"_LD_" nord=%"_LD_" mfn=%"_LD_" *p=%02x loop=%"_LD_" - hmfn=%"_LD_"\n",
  hcur,nord,mfn,*p,loop,hmfn);
#endif
                do {
                    if (!*p) {
                        p+=delta1; mfn+=delta8;
#if HITTRACx
 printf("b6_hit - hcur=%"_LD_" nord=%"_LD_" mfn=%"_LD_" *p=%02x loop=%"_LD_" - continue\n",
  hcur,nord,mfn,*p,loop);
#endif
                        continue;
                    }
                    if (nord > hcur)
                        loop2 = 8 - (mfn&BY8RMASK);
                    else
                        loop2 = (mfn&BY8RMASK) + 1;
                    for (; loop2--; mfn+=delta1) {
                        n=mfn&BY8RMASK;
#if HITTRACx
 printf("b6_hit - hcur=%"_LD_" nord=%"_LD_" mfn=%"_LD_" *p=%02x loop=%"_LD_" loop2=%d n=%d\n",
  hcur,nord,mfn,*p,loop,loop2,n);
#endif
                        if (*p & bitmask[n]) {
#if CHKFOUND
                            found=1;
#endif
                            hmfn=mfn; hcur+=delta1;
                            if (nord == hcur)
                                break;
                        }
                    }
#if CHKFOUND
                    if (found) {
#endif
                        if (nord > hcur) {
                            if (n == BY8RMASK)
                                p++;
                        }
                        else
                            if (n == 0)
                                p--;
                        break;
#if CHKFOUND
                    }
                    else
                        fatal("b6_hit/bug1");
#endif
                } while (--loop > 0);
#if CHKFOUND
                if (!found)
                     fatal("b6_hit/bug2");
#endif
            } /* end for hcur != nord */
        } /* end else hcur == nord */
    } /* end else nord == 0 || nord > hits */
    /* store */
    sprintf(buffup,B5EDIT1,hcur);
    memmove(FIELDP(xdir_hcur),buffup,B5UPLEN);
    sprintf(buffup,B5EDIT1,hmfn);
    memmove(FIELDP(xdir_hmfn),buffup,B5UPLEN);
#if HITTRACE
 printf("b6_hit - mfn: %"_LD_"\n",mfn);
#endif
#if SHOWCORE
 if (b60trace) showcore("b6_hit - exit");
#endif
    return(mfn);
}


LONGX b6_hsizh(irxp, nbytes, hsizp)
char *irxp;
LONGX nbytes;
LONGX *hsizp;
{
    LONGX hits,hsiz,loop,mfn;
    unsigned char *p1p;
    int loop2;
    hits=0; hsiz=0;
    for (mfn=0, p1p=irxp, loop=nbytes; loop--; p1p++) {
        if (!*p1p) {
            mfn+=8;
            continue;
        }
        for (loop2=8; loop2--; mfn++)
            if (*p1p & bitmask[mfn&BY8RMASK]) {
                hits++;
                hsiz=mfn;
#if MFNTRACE
 printf("%"_LD_" ",mfn);
#endif
            }
    }
#if MFNTRACE
 printf("\n");
#endif
    *hsizp = hsiz;
    return(hits);
}
#else /* BIGREC */
#endif /* BIGREC */
