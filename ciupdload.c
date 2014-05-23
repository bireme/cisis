

/* Begin PROCXLOADREC
*/
#if PROCXLOADREC

	     if (strncmp(p,"load=",5) == 0 ||  /* mx null "proc='Gload=dor1.txt=1'" */
             strncmp(p,"load/",5) == 0) {

            int parm0=1;
            int parm1=0;
            int parmprint=0;
            FILE *parmfp=stdout;

            char *xarea=NULL;
            LONGX nbytes,nbytes0,nread;
            char *filnamp;
            int fd=0;

            char *q;
            LONGX xtag=1;

            char *haddp=NULL;
            char *hlenp=NULL;
            char *hdatap=NULL;
            LONGX hlen=0;

            /* scan parms
            */
            if (strncmp(p,"load/",5) == 0) {
              p+=4;     /* load/ */
              while (*p == '/') {

                p++;
                if (isdigit(*p)) {
                  for (xtag=0; isdigit(*p); p++ ) xtag=xtag*10+((int)(*p)-(int)'0');
                  if (xtag < 0) fatal("fldupdat/procx/Gload/tag");  // allow tag /0
                  if (*p=='/' || *p=='='); else fatal("fldupdat/procx/Gload/tag dlm");
                }
                if (strncmp(p,"nonl/",5) == 0 || strncmp(p,"nonl=",5) == 0) {
                  p+=4;
                  parm1=1;
                }
                if (*p=='=') { p++; break; }
                if (*p=='/') ; else fatal("fldupdat/procx/Gload[/tag][/<option>][=<file>]");   // Gload[/<tag>][/nolf][/xml][/socket][/head][={<url>|<file>}] + Gmark[/<tag>]
                //fatal("fldupdat/procx/Gload/option");

              } /* while /x */
            }
            else p+=5;  /* load= */

            filnamp=p;


            /* alloc working area
            */
	    nbytes=(rec_maxmfrl>MAXMFRL)?rec_maxmfrl:MAXMFRL;
            if (nbytes < MFRmfrl) nbytes=MFRmfrl;
            nbytes=(nbytes0=nbytes)+BUFSIZ;     /* buffup */ /* nbytes=MFRmfrl+MFRmfrl+BUFSIZ; */
#if CICPP
            try { xarea = (char *) new char [nbytes]; }
            catch (BAD_ALLOC) { xarea = (char *)ALLONULL; }
#else /* CICPP */
            xarea=(char *)ALLOC((ALLOPARM)nbytes);
#endif /* CICPP */
            if (xarea == (char *)ALLONULL) fatal("fldupdat/procx/Gload/ALLOC");
            *xarea='\0';

            q=xarea; /* batchp */


            /* del/add command
            */
            if (xtag) sprintf(q,"D%"_LD_,xtag);
            else      sprintf(q,"D*");                       q+=strlen(q);

            sprintf(haddp=q,"H%09"_LD_" ",(xtag)?(LONGX)xtag:1L); q+=strlen(q);
            sprintf(hlenp=q,"123456789 ");                   q+=strlen(q);

            hdatap=q;

            if (*filnamp) {
                fd=dbxopen(NULL/*gidbnp*/,filnamp,"");
                if (fd <= 0) fatal("fldupdat/procx/Gload/open");
                else {
                    nread=CIREAD(fd,hdatap,nbytes0);
                    CLOSE(fd);
                    if (nread > 0L) {
                        q+=nread; hlen+=nread;
                    }
                    if (parm0) if (nread > 0L) {
                        LONGX left=nread;
                        int found=0;
                        char *x,*y;
                        for (x=y=hdatap; left; left--, x++)
                            if (*x=='\r') { found=1; hlen--; q--; }
                            else { if (found) *y= *x; y++; }
                    }
                    if (parm1) if (nread > 0L) {
                        LONGX left=nread;
                        int found=0;
                        char *x,*y;
                        for (x=y=hdatap; left; left--, x++)
                            if (*x=='\n') { found=1; hlen--; q--; }
                            else { if (found) *y= *x; y++; }
                    }
                }
            }
            else {
                LONGX left=nbytes0;
#if USEFGETS /* fgets looses last char if LF is missing in last line */
                int nread=(left>INT_MAX)?INT_MAX:left;
                char *linep;
                while ((linep=fgets(q,nread,stdin)) != NULL) {
                    int nread=strlen(linep);
                    if (parm1) if (nread) if (linep[nread-1]=='\n') nread--;
                    q+=nread; hlen+=nread; left-=nread;
                }
#else /* getchar() */
                int c;
                int nread;
                while (left && (c=getchar()) != EOF) {
                    nread=1;
                    if (parm1) if ((char)c=='\n') nread=0;
                    if (nread) { *q++=(unsigned char)c; hlen++; left--; }
                }
#endif
            }

            *q='\0';
            if (hlen) {
                /* actual add lenght */
                sprintf(hlenp,"%9"_LD_,(LONGX)hlen); hlenp[9]=' ';
                /* print */
                if (parmprint) fprintf(parmfp,"%s",hdatap);
            }
            else if (haddp) *haddp='\0';

#if CICPP
        if (recp->xfldupdat(xarea)) fatal(errp);
        delete xarea;
#else /* CICPP */
        if (fldupdat(irec,xarea)) fatal(errp);
        if (xarea) FREE(xarea);
#endif /* CICPP */


        return NULL; //isgdump=1;

        }  /* end Gload */
        else

#endif /* PROCXLOADREC */
/* End PROCXLOADREC
*/

