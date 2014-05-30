LONGX CIB7::b6_hit(RECSTRU *irecp, LONGX nord, LONGX setno)
#else /* CICPP */
LONGX b6_hit(cib6p,irec,nord,setno)                                   /*
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
b6_CIB7 *cib6p;
LONGX irec;		/* indice de vrecp, para info/store */
LONGX nord;		/* numero de ordem do hit a analisar */
LONGX setno;		/* query number */
#endif /* CICPP */
{
    RECSTRU *recp;
    LONGX hits,hcur,hsiz,hmfn;
    int xdir_hits,xdir_hcur,xdir_hsiz,xdir_hmfn,xdir_hmem;
    unsigned char *p;
    char buffup[B7UPLEN+1];       /* area for fldupdat */
    char filname[CIMPL+1];
    int fd;
    LONGX offset,mfn;
    LONGX by8left,block,lastblock,maxblock;
    LONGX bufleft,bufsiz,lastbsize;
    off_t seek;
    unsigned int toread,nr;
    int loop,n,delta8,delta1,found;

   /* b6_CIB7 *debug=cib6p; */

#if HITTRACE
 printf("b6_hit - irec,nord: %"_LD_"%"_LD_" \n",irec,nord);
#endif
#if SHOWCORE
 if (b60trace) showcore("b6_hit");
#endif
    strcpy(b6errxy,"b6_hit");          /* b6_hiterr() */

#if CICPP
    recp=irecp;
    if (!recp)
#if CDROM
        return(-2L);
#else
        fatal("b6_hit/recp");
#endif
    if (!recp->recmfp)
#if CDROM
        return(-3L);
#else
        fatal("b6_hit/recmfp");
#endif
#else /* CICPP */
    recp=vrecp[irec];
    if (!recp)
#if CDROM
        return(-3L);
#else
        fatal("b6_hit/irec");
#endif
#endif /* CICPP */

    if (RECtype != TYPEMFQ || RECrc != RCNORMAL)
#if CDROM
        return(-4L);
#else
        fatal("b6_hit/check");
#endif

#if CICPP
    if ((xdir_hits=irecp->xfieldx(MFQTHITS,1)) < 0)
#else /* CICPP */
    if ((xdir_hits=fieldx(irec,MFQTHITS,1)) < 0)
#endif /* CICPP */
#if CDROM
        return(-5L);
#else
        fatal("b6_hit/xdir_hits");
#endif

    for (hits=0, p=(unsigned char *)FIELDP(xdir_hits), loop=DIRlen(xdir_hits); loop--; p++)
        hits = hits * 10 + (*p - '0');
    if (nord < 0 || nord > (hits+1))
#if CDROM
        return(-6L);
#else
        fatal("b6_hit/nord");
#endif

#if CICPP
    if ((xdir_hcur=irecp->xfieldx(MFQTHCUR,1)) < 0)
#else /* CICPP */
    if ((xdir_hcur=fieldx(irec,MFQTHCUR,1)) < 0)
#endif /* CICPP */
#if CDROM
        return(-7L);
#else
        fatal("b6_hit/xdir_hcur");
#endif

#if CICPP
    if ((xdir_hmfn=irecp->xfieldx(MFQTHMFN,1)) < 0)
#else /* CICPP */
    if ((xdir_hmfn=fieldx(irec,MFQTHMFN,1)) < 0)
#endif /* CICPP */
#if CDROM
        return(-8L);
#else
        fatal("b6_hit/xdir_hmfn");
#endif

#if CICPP
    if ((xdir_hsiz=irecp->xfieldx(MFQTHSIZ,1)) < 0)
#else /* CICPP */
    if ((xdir_hsiz=fieldx(irec,MFQTHSIZ,1)) < 0)
#endif /* CICPP */
#if CDROM
        return(-9L);
#else
        fatal("b6_hit/xdir_hsiz");
#endif


    if (nord == 0 || nord > hits) {
        if (nord == 0)
            mfn=hits;           /* returning value */
        else
            mfn=(-1L);
    }
    else {
        for (hcur=0, p=(unsigned char *)FIELDP(xdir_hcur), loop=DIRlen(xdir_hcur); loop--; p++)
            hcur = hcur * 10 + (*p - '0');
        for (hmfn=0, p=(unsigned char *)FIELDP(xdir_hmfn), loop=DIRlen(xdir_hmfn); loop--; p++)
            hmfn = hmfn * 10 + (*p - '0');
        for (hsiz=0, p=(unsigned char *)FIELDP(xdir_hsiz), loop=DIRlen(xdir_hsiz); loop--; p++)
            hsiz = hsiz * 10 + (*p - '0');
#if HITTRACx
 printf("b6_hit - rec - hits=%"_LD_" hcur=%"_LD_" hsiz=%"_LD_" hmfn=%"_LD_"\n",
  hits,hcur,hsiz,hmfn);
#endif
        if (hcur == nord)
            mfn=hmfn;
        else {
#if CICPP
            if ((xdir_hmem=irecp->xfieldx(MFQTHMEM,1)) < 0)
#else /* CICPP */
            if ((xdir_hmem=fieldx(irec,MFQTHMEM,1)) < 0)
#endif /* CICPP */
#if CDROM
                return(-10L);
#else
                fatal("b6_hit/xdir_hmem");
#endif
       if (!b6bufferp)
#if CICPP
       	b6_hballoc(hsiz,&(b6bufferp));
	    if (b6bufferp->bufsiz <= 0) fatal("b6_hit/bufsiz");
	    if (hcur == 0 || b6bufferp->setno != setno) b6_hbinit(setno,b6bufferp);
#else /* CICPP */
       	b6_hballoc(cib6p,hsiz,&(b6bufferp));
	    if (b6bufferp->bufsiz <= 0) fatal("b6_hit/bufsiz");
	    if (hcur == 0 || b6bufferp->setno != setno) b6_hbinit(cib6p,setno,b6bufferp);
#endif /* CICPP */
	    fd=0; found=0;
            if (nord > hcur) {
                delta8 =  8; delta1 =  1;
	    }
	    else {
                delta8 = -8; delta1 = -1;
	    }
        mfn = (hcur == 0) ? 0 : hmfn+delta1;
        offset = (mfn/8);

		hsiz = hsiz/8 + 1;

	    bufsiz = (b6bufferp->bufsiz <= hsiz) ? b6bufferp->bufsiz : hsiz;
	    block = offset / bufsiz;
	    maxblock = (hsiz-1) / bufsiz;
	    bufleft = (block+1)*bufsiz - offset;
	    p = b6bufferp->area + bufsiz - bufleft;
            if (nord > hcur) {
		lastblock = maxblock;
		lastbsize = hsiz % bufsiz; /* hsiz - (lastblock+1)*bufsiz; */
		if (lastbsize == 0) lastbsize=bufsiz;
	        by8left = hsiz - offset;
		if (block == lastblock) bufleft=lastbsize;
	    }
	    else {
		lastblock = 0;
		lastbsize = bufsiz;
	        by8left = offset + 1;
		bufleft = bufsiz - bufleft + 1;
	    }
	    if (bufleft <= 0 || bufleft > bufsiz) fatal("b6_hit/bufleft");
	    if (lastbsize <= 0 || lastbsize>bufsiz) fatal("b6_hit/lastbsize");
	    if (b6bufferp->block != block) {
		memmove(filname,FIELDP(xdir_hmem),n=DIRlen(xdir_hmem));
		filname[n]='\0';
		fd=dbxopen(b6_gidbnp,filname,"");
		seek=block*bufsiz;
		if (LSEEK64(fd,seek,SEEK_SET) != seek) fatal("b6_hit/lseek");
		toread = (block == lastblock) ? lastbsize : bufsiz;
		nr=(unsigned)CIREAD(fd,b6bufferp->area,toread);
		if (nr != toread) fatal("b6_hit/read/1");
		b6bufferp->block = block;
#if HITTRACx
 printf("b6_hit - left/1=%"_LD_"/%"_LD_" hcur=%"_LD_" nord=%"_LD_" mfn=%"_LD_" block=%"_LD_"=%d",
  by8left,bufleft,hcur,nord,mfn,block,nr);
 for (n=0; n < nr; n++) printf(" %02x",bufferp->area[n]);
 printf("\n");
#endif
	    }
            for (; by8left--; p+=delta1) {
		if (!bufleft--) {
		    block+=delta1;
		    if (block < 0 || block > maxblock) fatal("b6_hit/block");
		}
#if HITTRACx
 printf("b6_hit - hcur=%"_LD_" nord=%"_LD_" mfn=%"_LD_" hmfn=%"_LD_" left=%"_LD_"/%"_LD_"
block=%"_LD_"\n",
  hcur,nord,mfn,hmfn,by8left,bufleft,block);
#endif
		if (b6bufferp->block != block) {
		    if (fd == 0) {
			memmove(filname,FIELDP(xdir_hmem),n=DIRlen(xdir_hmem));
			filname[n]='\0';
			fd=dbxopen(b6_gidbnp,filname,"");
		    }
		    seek=block*bufsiz;
		    if (LSEEK64(fd,seek,SEEK_SET) != seek) fatal("b6_hit/lseek");
		    toread = (block == lastblock) ? lastbsize : bufsiz;
		    nr=(unsigned)CIREAD(fd,b6bufferp->area,toread);
		    if (nr != toread) fatal("b6_hit/read/2");
		    b6bufferp->block = block;
		    bufleft = bufsiz;
		    if (block == lastblock) bufleft=lastbsize;
	            if (nord > hcur)
		        p = b6bufferp->area;
		    else
			p = b6bufferp->area + (bufsiz - 1);
#if HITTRACx
 printf("b6_hit - left/2=%"_LD_"/%"_LD_" hcur=%"_LD_" nord=%"_LD_" mfn=%"_LD_" block=%"_LD_"=%d",
  by8left,bufleft,hcur,nord,mfn,block,nr);
 for (n=0; n < nr; n++) printf(" %02x",b6bufferp->area[n]);
 printf("\n");
#endif
		    bufleft--;
		}
                if (!*p) {
                    mfn+=delta8;
#if HITTRACx
 printf("b6_hit - hcur=%"_LD_" nord=%"_LD_" mfn=%"_LD_" *p=%02x - continue\n",
  hcur,nord,mfn,*p);
#endif
                }
		else {
                    if (nord > hcur)
		        loop = 8 - (mfn&BY8RMASK);
                    else
		        loop = (mfn&BY8RMASK) + 1;
		    for (; loop--; mfn+=delta1) {
                        n=mfn&BY8RMASK;
#if HITTRACx
 printf("b6_hit - hcur=%"_LD_" nord=%"_LD_" mfn=%"_LD_" *p=%02x loop=%d n=%d\n",
  hcur,nord,mfn,*p,loop,n);
#endif
                        if (*p & bitmask[n]) {
                            hmfn=mfn; hcur+=delta1;
                            if (nord == hcur) {
			        found=1;
			        by8left=0; /* break */
                                break;
			    }
                        }
                    }
		}
            } /* end for by8left */
	    if (!found) fatal("b6_hit/found");

	    if (fd) CLOSE(fd);

	    /* store */
	    sprintf(buffup,B7EDIT1,hcur);
	    memmove(FIELDP(xdir_hcur),buffup,B7UPLEN);
	    sprintf(buffup,B7EDIT1,hmfn);
	    memmove(FIELDP(xdir_hmfn),buffup,B7UPLEN);

         } /* end else hcur == nord */

   } /* end else nord == 0 || nord > hits */

#if HITTRACE
 printf("b6_hit - mfn: %"_LD_"\n",mfn);
#endif
#if SHOWCORE
 if (b60trace) showcore("b6_hit - exit");
#endif
    return(mfn);
}
