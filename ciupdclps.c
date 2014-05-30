

/* Begin PROCXCLPSREC
*/
#if PROCXCLPSREC
	     if (strncmp(p,"collapse=",9) == 0) {
            char *xarea=NULL;
            LONGX nbytes;
            char *q;
            LONGX xtag;
            int a2b=0;
            short int a2bshort;
            LONGX a2blong;
            int noccs;

            nbytes=MFRmfrl+BUFSIZ;
            nbytes+=(MFRmfrl>BUFSIZ)?MFRmfrl:BUFSIZ; /* <== */

            p+=9;
            if (sscanf(p,"%"_LD_,&xtag) != 1) fatal("fldupdat/procx/Gcollapse/tag");
            while (isdigit(*p)) p++;
            if (*p == '=') {
                p++;
                if (strncmp(p,"short",5) == 0) {
                  a2b=sizeof(a2bshort);
                  p+=5;
                }
                if (strncmp(p,"long",4) == 0) {
                  a2b=sizeof(a2blong);
                  p+=4;
                }
                //if (strncmp(p,"float",5) == 0) {
                //  a2b=sizeof(a2bfloat);
                //  p+=5;
                //}
            }
            if (*p) fatal("fldupdat/procx/Gcollapse");
#if CICPP
            try { xarea = (char *) new char [nbytes]; }
            catch (BAD_ALLOC) { xarea = (char *)ALLONULL; }
#else /* CICPP */
            xarea=(char *)ALLOC((ALLOPARM)nbytes);
#endif /* CICPP */
            if (xarea == (char *)ALLONULL) fatal("fldupdat/procx/Gcollapse/ALLOC");
            q=xarea;
            if (xtag)
              sprintf(q,"D%"_LD_,xtag);
            else
              sprintf(q,"D*");
            q+=strlen(q);
            for (noccs=0, xdir=0; xdir < MFRnvf; xdir++) {
                if (DIRtag(xdir) == (UWORD)xtag || !xtag) {
                    noccs++;
                } /* end tag */
            } /* end xdir */
            sprintf(q,"H %"_LD_" %"_LD_" ",xtag,(LONGX)noccs*a2b);
            q+=strlen(q);
            for (xdir=0; xdir < MFRnvf; xdir++) {
                if (DIRtag(xdir) == (UWORD)xtag || !xtag) {
                    FFI left=DIRlen(xdir);
                    char *fldp=FIELDP(xdir);
                    unsigned LONGX value=0;
                    while (left--) {
                        if (!isdigit(*fldp)) break;
                        value=value*10+((int)(*fldp)-(int)'0');
                        fldp++;
                    }
                    if (a2b == sizeof(a2bshort)) {
                        a2bshort=(short int)value;
                        memmove(q,(void *)&a2bshort,a2b);
                    } else {
                        a2blong=(LONGX)value;
                        memmove(q,(void *)&a2blong,a2b);
                    }
                    q+=a2b;
                } /* end tag */
            } /* end xdir */
            *q='\0';
#if CICPP
        if (recp->xfldupdat(xarea)) fatal(errp);
        delete xarea;
#else /* CICPP */
        if (fldupdat(irec,xarea)) fatal(errp);
        if (xarea) FREE(xarea);
#endif /* CICPP */

          return NULL; //isgdump=1;

        }
        else
#endif /* PROCXCLPSREC */
/* End PROCXCLPSREC
*/
