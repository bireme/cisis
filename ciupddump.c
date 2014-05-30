

/* Begin PROCXDUMPREC
*/
#if PROCXDUMPREC
         if (strncmp(batchp2,"dump=",5) == 0 ||  /* mx null "proc='Gdump/1=dor1.txt'" */
             strncmp(batchp2,"dump/",5) == 0 ||
             strcmp( batchp2,"dump"   ) == 0) {

            LONGX xtag=1; //default tag
            int parmsocket=0;
            int parmcmd=0;
            int rc;
            char *filnamp=NULL;

            int parm1=0;

            char *p,*localbatchp=NULL;

            char *xarea=NULL;
            LONGX nbytes,nbytes2;

            int fd=0;

            char *fldp=NULL;
            FFI len=0; /* output zero chars */

            /* alloc working area
            */
            nbytes=MFRmfrl+BUFSIZ;          /* working */
            nbytes2=(strlen(batchp2)+1);    /* batchp2 */
#if CICPP
            try { xarea = (char *) new char [nbytes]; }
            catch (BAD_ALLOC) { xarea = (char *)ALLONULL; }
            try { localbatchp = (char *) new char [nbytes2]; }
            catch (BAD_ALLOC) { localbatchp = (char *)ALLONULL; }
#else /* CICPP */
            xarea=(char *)ALLOC((ALLOPARM)nbytes);
            localbatchp=(char *)ALLOC((ALLOPARM)nbytes2);
#endif /* CICPP */
            if (xarea == (char *)ALLONULL) fatal("fldupdat/procx/Gdump/ALLOC");
            *xarea='\0';                    /* output */

            strcpy(p=localbatchp,batchp2); /* duplicate batchp2 */

            /* scan parms
            */
            p+=4;  /* skip dump */

            if (*p) {
              while (isspace(*p)) p++;
              while (*p == '/') {      /* /<tag>/socket */
                p++;
                if (isdigit(*p)) {                                                   /* /<tag> */
                  //if (*p=='0') xtagzero=1;  // allow tag /0n
                  for (xtag=0; isdigit(*p); p++ ) xtag=xtag*10+((int)(*p)-(int)'0');
                  if (xtag < 1) fatal("fldupdat/procx/Gload/tag");
                  if (*p=='/' || *p=='='); else fatal("fldupdat/procx/Gload/tag dlm");
                } else
                if (strncmp(p,"line=",5) == 0 || strncmp(p,"line/",5) == 0) {        /* /line */
                  p+=4;
                  parm1=1;
                } else
#if PROCXSERVREC
                if (strncmp(p,"socket=",7) == 0 || strncmp(p,"socket/",7) == 0) {    /* /socket */
                  p+=6;
                  parmsocket=1;
                } else
                if (strncmp(p,"xml=",4) == 0 || strncmp(p,"xml/",4) == 0) {          /* /xml */
                  p+=3;
                  parmcmd=1;
                } else
                if (strncmp(p,"tell=",5) == 0 || strncmp(p,"tell/",5) == 0) {        /* /tell */
                  p+=4;
                  parmcmd=2;
                } else
                if (strncmp(p,"trace=",6) == 0 || strncmp(p,"trace/",6) == 0) {      /* /trace */
                  p+=5;
                  parmcmd=3;
                } else
#endif /* PROCXSERVREC */
                fatal("fldupdat/procx/Gdump/option");
              } /* while /x */
              if (*p == '=') { p++; filnamp=p; p+=strlen(p); }  /* <file> | <url> */
            }
            else { // proc='Gdump'
            }


	        if (parmsocket) {            /* ./mx null "proc='Gdump/socket/:1417'" */
#if PROCXSERVREC_spawn
                uint16_t port=1417;
                int maxconnqueued=1;
                rc = mainserver( parmcmd, port, xarea , nbytes, maxconnqueued);
#endif /* PROCXSERVREC */
	        }
            else { /* else parmsocket: <file> */

             /* get contents */
            xdir=fieldx(irec,(UWORD)xtag,1);
            if (xdir >= 0) { /* data */
                fldp=FIELDP(xdir);
                len=DIRlen(xdir);

                if (len) {
                  if (parm1) {
                    LONGX left=len;
                    char *x=fldp;
                    char *y=xarea;
                    for (; left; left--, x++)
                        if (*x=='\r' || *x=='\n') len--;
                        else *y++= *x;
                  }
                  else memmove(xarea,fldp,len);
                }
                xarea[len]='\0';
            }

            /* rc = maindump  ( parmcmd, filnamp, parm1, buffer, buffersize ); */
            if (filnamp) {
                dbxopenc(NULL,filnamp,&fd,NULL,"fldupdat/procx/Gdump/create",1,0); /* create=1, append=0 */
                if (fd >= 0) {
                    if ((FFI)CIWRITE(fd,xarea,len) != len) fatal("fldupdat/procx/Gdump/write");
                    CLOSE(fd);
                }
            }
            else {
#if USEFPUTS /* fputs */
                if (fputs(xarea,stdout) == EOF) fatal("fldupdat/procx/Gdump/write");
#else /* putchar */
                char *y;
                for (y=xarea; *y; y++) if (*y != '\r') putchar((int)*y);
#endif
            }

	     } /* end parmsocket */


#if CICPP
        delete xarea;
        delete localbatchp;
#else /* CICPP */
        if (xarea) FREE(xarea);
        if (localbatchp) FREE(localbatchp);
#endif /* CICPP */

        return NULL; //isgdump=1;

        }  /* end Gdump */
        else

#endif /* PROCXDUMPREC */
/* End PROCXDUMPREC
*/


