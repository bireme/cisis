

/* Begin PROCXDECOREC
*/
#if PROCXDECOREC
	     if (strncmp(p,"decod=",6) == 0) {
            DBXSTRU *dbxp;
            char namedbx[CIMPL+1];
#if !CICPP
            LONGX dwrec;
#endif
            char *q1,*q2;
            p+=6;
            for (q2=namedbx, *q2++=';', q1=p; *q1; q1++) if (*q1 == ',') break; else *q2++ = *q1;
            *q2='\0';
            dbxp=dbxstorp(namedbx);
            if (!DBXvderp) {     /* first gizmo definition */
#if CICPP
                recdecod(cisisxp,recp,p);
#else
                for (dwrec=maxnrec; dwrec--; ) {
	                 if (!vrecp[dwrec]) /* ja' decrementado */ break;
                }
                if (dwrec < 0) fatal("fldupdat/procx/G/dwrec");
                recdecod(irec,p,dwrec);
                if (dwrec >= 0) if (vrecp[dwrec]) {
	                 FREE(vrecp[dwrec]); vrecp[dwrec]=NULL; nrecs--;
                }
#endif
            }
            /* dbxp=dbxstorp(namedbx);  dbxflush'ed in gizmread */
#if CICPP
	         recdecex(cisisxp,recp);
#else
	         recdecex(irec);
#endif /* CICPP */

            return NULL; //isgdump=1;

        }
        else
#endif /* PROCXDECOREC */
/* End PROCXDECOREC
*/
