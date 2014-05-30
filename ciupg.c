

/* Begin PROCG
*/
#if PROCG

  if (batchp) {                                              /* PROCG batchp */

    char *batchp2;
#if PROCXREADREC
    int dot = 0;
    char rfnamp2[CIMPL];
#endif

    for (p=batchp; *p; ) if (isspace(*p)) p++; else break;

	c=toupper(*p); errp=p++;
	switch (c) {                                             /* PROCG switch */

	case 'G':
        batchp2=p;

#if PROCXDUMPREC
#include "ciupddump.c"
#endif /* PROCXDUMPREC */

#if PROCXMARKREC
#include "ciupdmark.c"
#endif /* PROCXMARKREC */

#if PROCXCLPSREC
#include "ciupdclps.c"
#endif /* PROCXCLPSREC */

#if PROCXSPLIREC
#include "ciupdsplt.c"
#endif /* PROCXSPLIREC */

//#if PROCXLOADREC
//#include "ciupdload.c"
//#endif /* PROCXLOADREC */

#if PROCXSOCKREC
//#include "ciupdsock.c"
#include "ciupdmarx.c"
#endif /* PROCXSOCKREC */

#if PROCXDECOREC
#include "ciupddeco.c"
#endif /* PROCXDECOREC */

#if PROCXSLT
#include "ciupdxslt.c"
#endif /* PROCXSLT */

#if PROCXGIZMREC
// Ggizmo
        if (*p) {
            DBXSTRU *dbxp;
            char namedbx[CIMPL+1];
#if !CICPP
            LONGX gwrec;
#endif
            char *q1,*q2;
            for (q2=namedbx, *q2++=';', q1=p; *q1; q1++) if (*q1 == ',') break; else *q2++ = *q1;
            *q2='\0';
            dbxp=dbxstorp(namedbx);
            if (!DBXvgzrp) {     /* first gizmo definition */
#if CICPP
                gizmread(cisisxp,p,&DBXvgzrp);
#else
                for (gwrec=maxnrec; gwrec--; ) {
	                 if (!vrecp[gwrec]) /* ja' decrementado */ break;
                }
                if (gwrec < 0) fatal("fldupdat/procx/G/gwrec");
                gizmread(p,&DBXvgzrp,gwrec);
                if (gwrec >= 0) if (vrecp[gwrec]) {
	                 FREE(vrecp[gwrec]); vrecp[gwrec]=NULL; nrecs--;
                }
#endif
            }
            dbxp=dbxstorp(namedbx); /* dbxflush'ed in gizmread */
#if CICPP
	         recgizmo(cisisxp,recp,DBXvgzrp);
#else
	         recgizmo(irec,DBXvgzrp);
#endif /* CICPP */
        }
        return NULL; /*p="";*/ /* exit */
		//break;
#endif /* PROCXGIZMREC */



/* procx.c */
#if PROCXMFUPDATE
	case 'X':
       keepndbx=ndbxs;
       while (isspace(*p)) p++;
#define COPY   1
#define APPEND 2
#define MERGE  3
       parmcopy=COPY;
       if (strncmp(p,"create=",7) == 0)                   /* create= */
            { p+=7; recisis0(p); }
       else {
            if (strncmp(p,"copy=",5) == 0)                 /* copy= */
                { p+=5; }
            else
            if (strncmp(p,"append=",7) == 0)               /* append= */
                {p+=7; parmcopy=APPEND;}
            else
            if (strncmp(p,"merge=",6) == 0)                /* merge= */
                {p+=6; parmcopy=MERGE;}
            dbxopt_fatal=0; xfd=dbxopen(p,p,mx1extp);
            if (xfd <= 0) recisis0(p); else CLOSE(xfd);
        }

#if CICPP
        upcrecp=new RECSTRU(cisisxp);
        if (!upcrecp) fatal("fldupdat/procx/upcrec");
        upcrecp->xrecalloc(sizeof(M0STRU));
        upirecp=new RECSTRU(cisisxp);
        if (!upirecp) fatal("fldupdat/procx/upirec");
        upirecp->xrecalloc(RECnbytes);
#if MULTI
	    if (strcmp(p,RECdbxp->dbxname) == 0) /* dbxstorp has already set dbxnetws */
	        dbxstorp(p)->dbxnetws=dbxcinet(p);
        if (dbxstorp(p)->dbxnetws != MONONETS) {
            upirecp->reclock=RLOCK;
            upcrecp->reclock=DELOCK;
	    }
#endif /* MULTI */

        upcrecp->xrecord(p,0L);        /* append */	/* DELOCK */
#if MULTI
        if (upcrecp->recrc == RCLOCK) fatal("fldupdat/procx/delock");
        if (upcrecp->recdbxp->dbxnetws != MONONETS) {
            if (parmcopy == APPEND) upirecp->recwlock|=NEWREC;
            else upirecp->recwlock|=NEWREC|FORCE;
        }
#endif /* MULTI */
        upirecp->xrecord(p,MFRmfn); /* RLOCK */
#if MULTI
        if (upirecp->recrc == RCLOCK) fatal("fldupdat/procx/rlock");
#endif /* MULTI */

        if (parmcopy == MERGE) if (upirecp->recrc == RCNORMAL) n=0;
        if (parmcopy == APPEND) if (MFRnvf <= 0) n=0;

        if (n) {
            memmove(upirecp->recmfp->mx,MFX,MFRmfrl);
            if (parmcopy == APPEND) {
                upirecp->recmfp->m1.m1mfn=upcrecp->recmfp->m0.m0nxtmfn;
#if MULTI
                if (upcrecp->recdbxp->dbxnetws != MONONETS) upirecp->recmfp->m1.m1mfn=0L;
#endif /* MULTI */
            }
            upirecp->xrecupdat();
        }
#if MULTI
        if (upcrecp->recdbxp->dbxnetws != MONONETS) {
            if (parmcopy == MERGE) if (!n) upirecp->xrecunlck(RLOCK);
            upcrecp->xrecunlck(DELOCK);
        }
#endif /* MULTI */
        delete upirecp;
        delete upcrecp;
#else /* CICPP */
        for (upcrec=maxnrec; upcrec--; ) if (!vrecp[upcrec]) /* ja' decrementado */ break;
        if (upcrec < 0) fatal("fldupdat/procx/upcrec");
        recallok(upcrec,(LONGX)sizeof(M0STRU));

        for (upirec=maxnrec; upirec--; ) if (!vrecp[upirec]) /* ja' decrementado */ break;
        if (upirec < 0) fatal("fldupdat/procx/upirec");
        recallok(upirec,RECnbytes);

#if MULTI
	    if (strcmp(p,RECdbxp->dbxname) == 0) /* dbxstorp has already set dbxnetws */
	        dbxstorp(p)->dbxnetws=dbxcinet(p);
        if (dbxstorp(p)->dbxnetws != MONONETS) {
            VREClock(upirec)=RLOCK;
            VREClock(upcrec)=DELOCK;
	    }
#endif /* MULTI */
        record(upcrec,p,0L);        /* append */	/* DELOCK */
#if MULTI
        if (VRECrc(upcrec) == RCLOCK) fatal("fldupdat/procx/delock");
        if (VRDBnetws(upcrec) != MONONETS) {
            if (parmcopy == APPEND) VRECwlock(upirec)|=NEWREC;
            else VRECwlock(upirec)|=NEWREC|FORCE;
        }
#endif /* MULTI */
        record(upirec,p,MFRmfn); /* RLOCK */
#if MULTI
        if (VRECrc(upirec) == RCLOCK) fatal("fldupdat/procx/rlock");
#endif /* MULTI */

        if (parmcopy == MERGE) if (VRECrc(upirec) == RCNORMAL) n=0;
        if (parmcopy == APPEND) if (MFRnvf <= 0) n=0;

        if (n) {
            memmove(VMFX(upirec),MFX,MFRmfrl);
            if (parmcopy == APPEND) {
                VMFRmfn(upirec)=VMF0nxtmfn(upcrec);
#if MULTI
                if (VRDBnetws(upcrec) != MONONETS) VMFRmfn(upirec)=0L;
#endif /* MULTI */
            }
            recupdat(upcrec,upirec);
        }
#if MULTI
        if (VRECdbxp(upcrec)->dbxnetws != MONONETS) {
            if (parmcopy == MERGE) if (!n) recunlck(upirec,RLOCK);
            recunlck(upcrec,DELOCK);
        }
#endif /* MULTI */

        FREE(vrecp[upirec]); vrecp[upirec]=NULL; nrecs--;
        FREE(vrecp[upcrec]); vrecp[upcrec]=NULL; nrecs--;
#endif /* CICPP */
#if KEEPDBX
        for (idbx=ndbxs-1; idbx>=keepndbx; idbx--)
            dbxflush(vdbxp[idbx]->dbxname);
#endif /* KEEPDBX */
        return NULL; /*p="";*/ /* exit */
		//break;
#undef COPY
#undef APPEND
#undef MERGE
#endif /* PROCXMFUPDATE */


/*
   'R'v32001^*,,','v32001^m,,',gizmo=dbn,tags[;]',,',decod=dbn[;]'
*/
/* procx.c */
#if PROCXREADREC
	case 'R':
        keepndbx=ndbxs;
        if (*p == '=') {  /* use master file directory as stored in DBXSTRU */
            p++; dot = 1;
        }
        while (isspace(*p)) p++;
        rfnamp=p; rfmfn=MFRmfn;
        rfmfnp=strchr(p,',');
        if (rfmfnp) {
           *rfmfnp++ = '\0';
           if (sscanf(rfmfnp,"%"_LD_,&rfmfn) != 1) fatal(errp);
        }

        if (dot) {
           rp = RDBname + strlen(RDBname) - 1;
           while (rp != RDBname) {
              if (*rp=='/' || *rp=='\\') {
                 break;
              }
              rp--;
           }
           if (*rp=='/' || *rp=='\\') {
              strncpy(rfnamp2, RDBname, (rp - RDBname + 1));
              rfnamp2[rp - RDBname + 1] = 0;
           } else {
              *rfnamp2 = 0;
           }
           p = rfnamp + strlen(p) - 1;
           while (p != rfnamp) {
              if (*p=='/' || *p=='\\') {
                 p++;
                 break;
              }
              p--;
           }
           strcat(rfnamp2, p);
        } else {
           strcpy(rfnamp2, rfnamp);
        }
#if CICPP
        uprrecp=new RECSTRU(cisisxp);
        if (!uprrecp) fatal("fldupdat/procx/uprrec");
        uprrecp->xrecalloc(RECnbytes);
        uprrexp=new RECSTRU(cisisxp); rp=p=(char *)uprrexp;
        if (!uprrexp) fatal("fldupdat/procx/uprrex");
        uprrexp->xrecalloc(RECnbytes);
        uprrecp->xrecord(rfnamp2,rfmfn); keeprecp=recp; recp=uprrecp;
#else /* CICPP */
        for (uprrex=maxnrec; uprrex--; ) if (!vrecp[uprrex]) /* ja' decrementado */ break;
        if (uprrex < 0) fatal("fldupdat/procx/r/uprrex");
        recallok(uprrex,RECnbytes); rp=p=(char *)vrecp[uprrex];
        for (uprrec=maxnrec; uprrec--; ) if (!vrecp[uprrec]) /* ja' decrementado */ break;
        if (uprrec < 0) fatal("fldupdat/procx/r/uprrec");
        recallok(uprrec,RECnbytes);
        record(uprrec,rfnamp2,rfmfn); keeprecp=recp; recp=vrecp[uprrec];
#endif /* CICPP */
        if (RECrc == RCNORMAL) for (xdir=0; xdir < MFRnvf; xdir++) {
            sprintf(rp,"H%"_LD_" %"_LD_" ",(LONGX)DIRtag(xdir),(LONGX)DIRlen(xdir)); rp+=strlen(rp);
            memmove(rp,FIELDP(xdir),DIRlen(xdir)); rp+=DIRlen(xdir);
        }
        *rp='\0'; recp=keeprecp;
#if CICPP
        if (recp->xfldupdat(p)) fatal(errp);
        delete uprrexp;
        delete uprrecp;
#else /* CICPP */
        if (fldupdat(irec,p)) fatal(errp);
        FREE(vrecp[uprrec]); vrecp[uprrec]=NULL; nrecs--;
        FREE(vrecp[uprrex]); vrecp[uprrex]=NULL; nrecs--;
#endif /* CICPP */

#if KEEPDBX && 0
        for (idbx=ndbxs-1; idbx>=keepndbx; idbx--)
            dbxflush(vdbxp[idbx]->dbxname);
#endif /* KEEPDBX */
        return NULL; /*p="";*/ /* exit */
		//break;
#endif /* PROCXREADREC */

/* procx.c */
#if PROCXWRITEFILE
	case 'F':
        keepndbx=ndbxs;
        while (isspace(*p)) p++;
        wfnamp=p;
        wfmtspecp=strchr(wfnamp,',');
        if (wfmtspecp) {
           *wfmtspecp++ = '\0';
        }
        if (!strchr(wfnamp,'='))
            wfnamp=dbxopenc(RDBname,wfnamp,&wffd,NULL,errp,0,1); /* force append= */
        else
            wfnamp=dbxopenc(RDBname,wfnamp,&wffd,NULL,errp,0,0); /* allow create=append= */
        if (wfmtspecp) {
            wfxp=wfmtspecp;
            if (CIWRITE(wffd,wfxp,wflen=strlen(wfxp)) != wflen) fatal(errp);
        }
        CLOSE(wffd);
#if KEEPDBX
        for (idbx=ndbxs-1; idbx>=keepndbx; idbx--)
            dbxflush(vdbxp[idbx]->dbxname);
#endif /* KEEPDBX */
        return NULL; /*p="";*/ /* exit */
		//break;
#endif /* PROCXWRITEFILE */

/* procx.c */
#if PROCXIFPRESET
	case 'Z':
        keepndbx=ndbxs;
        while (isspace(*p)) p++;
        ifprnamp=p; if (!*ifprnamp) fatal(errp);
        ifprmfnp=strchr(ifprnamp,',');
        if (!ifprmfnp) ifprmfn=MFRmfn; else {    /* AOT 05/10/02 */
          *ifprmfnp++ = '\0';
          if (sscanf(ifprmfnp,"%"_LD_,&ifprmfn) != 1) fatal(errp);
        }
#if MULTI
        if (ifprnamp) { /* ok */
            DBXSTRU *dbxp;
            RECSTRU *zrecp;
            LONGX zrec;
            int keepewlrc;
            dbxp=dbxstorp(ifprnamp);
            if (DBXnetws != MONONETS) {
#if CICPP
                zrecp = new RECSTRU(cisisxp);
                zrecp->xrecalloc(RECnbytes); zrecp->reclock=EWLOCK;
                keepewlrc=dbxewlrc; dbxewlrc=1; zrecp->xrecord(ifprnamp,0L);
                if (zrecp->recrc != RCNORMAL) {
                    printf("*** fldupdat/procx/z/EWLOCK \n"); fatal(errp);
                }
#else /* CICPP */
                for (zrec=maxnrec; zrec--; ) if (!vrecp[zrec]) /* ja' decrementado */ break;
                if (zrec < 0) fatal("fldupdat/procx/z/zrec");
                recallok(zrec,RECnbytes); zrecp=vrecp[zrec]; zrecp->reclock=EWLOCK;
                keepewlrc=dbxewlrc; dbxewlrc=1; record(zrec,ifprnamp,0L);
                if (zrecp->recrc != RCNORMAL) {
                    printf("*** fldupdat/procx/z/EWLOCK \n"); fatal(errp);
                }
#endif /* CICPP */
            }
#endif /* MULTI */
#if CICPP
		      recreset(ifprnamp,ifprmfn,ifprmfn,NULL,0L);
#else /* CICPP */
		      recreset(ifprnamp,ifprmfn,ifprmfn,0L,0L);
#endif /* CICPP */
#if MULTI
            if (DBXnetws != MONONETS) {
#if CICPP
                dbxewlrc=1; if (zrecp->xrecunlck(EWLOCK) != RCNORMAL) fatal("fldupdat/procx/z/unlock");
                delete zrecp;
#else /* CICPP */
                dbxewlrc=1; if (recunlck(zrec,EWLOCK) != RCNORMAL) fatal("fldupdat/procx/z/unlock");
                FREE(vrecp[zrec]); vrecp[zrec]=NULL; nrecs--;
#endif /* CICPP */
                dbxewlrc=keepewlrc;
            }
#endif /* MULTI */
        }
#if KEEPDBX
        for (idbx=ndbxs-1; idbx>=keepndbx; idbx--)
            dbxflush(vdbxp[idbx]->dbxname);
#endif /* KEEPDBX */
        return NULL; /*p="";*/ /* exit */
		//break;
#endif /* PROCXIFPRESET */

#if INCPROCX
/* procx.c */
#if PROCXIFUPDATE
	case 'Y':
/*
        keepndbx=ndbxs;
*/
        while (isspace(*p)) p++;
        ifnamp=p;
        mfnamp=strchr(p,','); if (!mfnamp) fatal(errp); *mfnamp++ = '\0';
        mfnp=strchr(mfnamp,','); if (!mfnp) fatal(errp); *mfnp++ = '\0';
        fstspecp=strchr(mfnp,','); if (!fstspecp) fatal(errp);  *fstspecp++ = '\0';
        if (!*mfnamp) mfnamp="null";
        if (!*mfnp) ifmfn=MFRmfn; else if (sscanf(mfnp,"%"_LD_,&ifmfn) != 1) fatal(errp);

        if (strncmp(ifnamp,"create=",7) == 0)                   /* create= */
            { ifnamp+=7; trmisis0(ifnamp); }
        else {
            dbxopt_fatal=0; yfd=dbxopen(ifnamp,ifnamp,iy0extp);
            if (yfd <= 0) {
            dbxopt_fatal=0; yfd=dbxopen(ifnamp,ifnamp,cx1extp); }
            if (yfd <= 0) trmisis0(ifnamp); else CLOSE(yfd);
        }
        if (fstspecp) { /* ok */
            char *specp;
            specp=loadfile(ifnamp,'@',fstspecp,NULL,0,'\n');
            if (!specp) fatal(errp);
            fst_gener(&fstpgmp,specp);
            if (fst_error) {
                printf("\n*** FST error#%"_LD_" at line %"_LD_"\n",fst_error,fst_errl);
                if (fst_error == FSTERRF)
                    printf("\n*** FMT error#%"_LD_" at offset %"_LD_"\n",fmt_error,fmt_errof);
                fatal(errp);
            }
#if CICPP
            delete [] specp;
#else
            FREE(specp);
#endif

            if (strcmp(mfnamp,"null") == 0) {
                LONGX qtylk1,qtylk2;
                char *lnk1p=NULL;
                char *lnk2p=NULL;
                DBXSTRU *dbxp;
                LONGX mfn=MFRmfn; MFRmfn=ifmfn;
                fst_inter(fstpgmp,irec,stwp,
                        &lnk1p,maxlk1,&lnk2p,maxlk2,&qtylk1,&qtylk2);
                if (dbxtrace) {
                    LONGX pmfn,qty;
                    UWORD ptag,pocc,pcnt;
                    LINK1 *lk1p;
                    LINK2 *lk2p;
                    printf("+++ Y/ifn=%s.\n",ifnamp);
                    printf("+++ Y/dbn=%s.\n",mfnamp);
                    printf("+++ Y/mfn=%"_LD_"\n",ifmfn);
                    printf("+++ Y/fst=%s.\n",fstspecp);
                    printf("+++ Y/lnk=%"_LD_"+%"_LD_"\n",qtylk1,qtylk2);
                    for (lk1p=(LINK1 *)lnk1p, qty=qtylk1; qty--; lk1p++) {
                       pstdecod(&lk1p->post,&pmfn,&ptag,&pocc,&pcnt);
                       prtlink("+++ Y/lk1=",pmfn,ptag,pocc,pcnt,lk1p->key,LE1);}
                    for (lk2p=(LINK2 *)lnk2p, qty=qtylk2; qty--; lk2p++) {
                       pstdecod(&lk2p->post,&pmfn,&ptag,&pocc,&pcnt);
                       prtlink("+++ Y/lk2=",pmfn,ptag,pocc,pcnt,lk2p->key,LE2);}
                }

                dbxp=dbxstorp(ifnamp);               /* inverted file ! */
                upif_init(ifnamp);
                svdifupd(lnk1p,qtylk1,lnk2p,qtylk2,             /* add new */
                         NULL,0,NULL,0,                         /*         */
                         pstflag,0L);
                DBXirecs++; DBXipadd[0]+=qtylk1; DBXipadd[1]+=qtylk2;
                upif_end(ifnamp,pstflag);
                invflush(ifnamp);
                if (lnk1p) FREE(lnk1p);
                if (lnk2p) FREE(lnk2p);
                fst_free(fstpgmp);
                MFRmfn=mfn;
            }
            else {
                int reset=ifupd_reset; ifupd_reset=0;
                ifupdat(mfnamp,ifmfn,ifmfn,           /* AOT 05/10/02 */
                        ifnamp,fstpgmp,stwp,maxlk1,maxlk2,pstflag,endup);
                ifupd_reset=reset;
            }
        }
#if KEEPDBX
/*
        for (idbx=ndbxs-1; idbx>=keepndbx; idbx--)
            dbxflush(vdbxp[idbx]->dbxname);
*/
#endif /* KEEPDBX */
        return NULL; /*p="";*/ /* exit */
	   //	break;
#endif /* PROCXIFUPDATE */

/* procx.c */
#if PROCXFSUPDATE
	case 'T':
        keepndbx=ndbxs;
        while (isspace(*p)) p++;
        if (*p=='/') if (toupper(*(p+1))=='H') if (*(p+2)=='=') { p+=3; tyyopth=1; }
        if (p) {
            LONGX rrex,rrec;
            char *stwp=NULL;
            FST_CODE *fstpgmp=NULL;
            LONGX maxlk1=1000;
            LONGX maxlk2=500;
            LONGX qtylk1,qtylk2;
            char *lnk1p=NULL;
            char *lnk2p=NULL;
            char *specp;
            specp=loadfile(RDBname,'@',p,NULL,0,'\n');
            if (!specp) fatal(errp);
            fst_gener(&fstpgmp,specp);
            if (fst_error) {
                printf("\n*** FST error#%"_LD_" at line %"_LD_"\n",fst_error,fst_errl);
                if (fst_error == FSTERRF)
                    printf("\n*** FMT error#%"_LD_" at offset %"_LD_"\n",fmt_error,fmt_errof);
                fatal(errp);
            }
#if CICPP
            delete [] specp;
#else
            FREE(specp);
#endif
            for (rrex=maxnrec; rrex--; ) if (!vrecp[rrex]) /* ja' decrementado */ break;
            if (rrex < 0) fatal("fldupdat/procx/t/rrex");
            recallok(rrex,RECnbytes); fst_batchup=(char *)vrecp[rrex];
            if (tyyopth) {
                for (rrec=maxnrec; rrec--; ) if (!vrecp[rrec]) /* ja' decrementado */ break;
                if (rrec < 0) fatal("fldupdat/procx/t/rrec");
                recallok(rrec,fst_hdrx=RECnbytes); fst_hdrp=(char *)vrecp[rrec];
            }
            *fst_batchup='\0';
            fst_inter(fstpgmp,irec,stwp,
                        &lnk1p,maxlk1,&lnk2p,maxlk2,&qtylk1,&qtylk2);
            if (!*fst_batchup) if (tyyopth) if (*fst_hdrp) {
                char *batchp;
                sprintf(fst_batchup,"H 32767 %d ",fst_hdru);
		        batchp=fst_batchup+strlen(fst_batchup);
		        memmove(batchp,fst_hdrp,fst_hdru); *(batchp+fst_hdru)='\0';
            }
            if (fldupdat(irec,fst_batchup)) fatal(errp);
            fst_batchup=NULL; fst_hdrp=NULL; fst_hdrx=0;
            if (tyyopth) {
                FREE(vrecp[rrec]); vrecp[rrec]=NULL; nrecs--;
            }
            FREE(vrecp[rrex]); vrecp[rrex]=NULL; nrecs--;

            if (lnk1p) FREE(lnk1p);
            if (lnk2p) FREE(lnk2p);
            fst_free(fstpgmp);
        }
#if KEEPDBX
        for (idbx=ndbxs-1; idbx>=keepndbx; idbx--)
            dbxflush(vdbxp[idbx]->dbxname);
#endif /* KEEPDBX */
        return NULL; /*p="";*/ /* exit */
	   //	break;
#endif /* PROCXFSUPDATE */
#endif /* INCPROCX */

#if INCPROCX
/* procxx.c */
#if MXFUN || IFLOADFUN /* PROCXXXFUN */
	case 'C':
        keepndbx=ndbxs;
        while (isspace(*p)) p++;
#if MXFUN
        if (strncmp(p,"mx",2) == 0 && isspace(*(p+2))) { cisis_mx(p+2+1); }
#endif /* MXFUN */
#if IFLOADFUN
        if (strncmp(p,"ifload",6) == 0 && isspace(*(p+6))) { cisis_ifload(p+6+1); }
#endif /* IFLOADFUN */
#if KEEPDBX
        for (idbx=ndbxs-1; idbx>=keepndbx; idbx--)
            dbxflush(vdbxp[idbx]->dbxname);
#endif /* KEEPDBX */
        return NULL; /*p="";*/ /* exit */
	   //	break;
#endif /* MXFUN || IFLOADFUN */ /* PROCXXXFUN */
/* procxx.c */
#endif /* INCPROCX */


	}  /* end PROCG switch */

  }  /* end PROCG batchp */

#endif /* PROCG */
/* End PROCG
*/
