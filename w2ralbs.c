/* ==----------------------------- w2ralbs.c ------------------------------== */

#if WHENFUN
int w2rcox ( int cmd, WTFUN_ARRAY *awtfp, int coll )
{
#endif

    /* read all language bit strigs
    */
#if PARMLANG
    WTFUN_XWT *xwtp=awtfp->vwtfp[coll];  // current

    if (parmlanguage) {
        if (readnlangs >= MAXPARMLANG) fatal("wtrig2/terminverted/lang/MAXPARMLANG");
        for (langidx=0; langidx < readnlangs; langidx++) {
            int xdir;
            /* read */
            RECORD(ifrec,colxdb,readnterms+langidx+1);
            if (RECrc != RCNORMAL) fatal("wtrig2/terminverted/lang/RCNORMAL");
            /* allocate */
            langshits[nlangs]=(char  *)loadfile(NULL,'@',"",NULL,hitbytes,'\0');
            /* store bit string */
            xdir=fieldx(ifrec,TAG35,1); if (xdir < 0) fatal("wtrig2/terminverted/lang/TAG35");
            memmove(langshits[nlangs],FIELDP(xdir),hitbytes);
            /* store name */
            xdir=fieldx(ifrec,TAG34,1); if (xdir < 0) fatal("wtrig2/terminverted/lang/TAG34");
            for (p=FIELDP(xdir), left=DIRlen(xdir); left--; ) {
                if (*p == '^') break;
                *p=isisuctab[*p]; p++;
            }
            *p='\0'; /* ok */
            langsname[nlangs]=strdup(FIELDP(xdir)); //...
            langscount[nlangs++]=0;
        }
        if (nlangs != readnlangs) fatal("wtrig2/terminverted/lang/nlangs");
    }
#endif /* PARMLANG */
    
#if WHENFUN
    return 0;
}
#endif
