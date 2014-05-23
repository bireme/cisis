

/* Begin PROCXSOCKREC
*/
#if PROCXSOCKREC

	     if ( (strncmp(p,"load=",5) == 0 || strncmp(p,"load/",5) == 0) /*
               &&  (strstr(p,"/socket=") || strstr(p,"/socket/")  */       ) {

            LONGX xtag=1;
            int xtagzero=0;
            int parmsocket=0;
            int parmcmd=0;
            int parmhead=0;
            char *filnamp=NULL;

            int parm0=0; //1; // strip 0D
            int parm1=0;      // strip 0A
            int parmprint=0;
            FILE *parmfp=stdout;

            char *xarea=NULL;
            LONGX nbytes;
            char *q;

            char *haddp=NULL;
            char *hlenp=NULL;
            char *hdatap=NULL;
            LONGX hlen=0;

            char *protocol;
            char *serverhost;
            uint16_t port;
            char *docpathp;

            int maxrds=INT_MAX;
            int xport=0;

            char *dbname;
            LONGX mfn;
            char *key;

            char *msgp;
            int rc;

            int dbx;
            int ndbx=3;
            char *vdbname[] = {
                "lilacs",
                "artigo",
                "citacoes",
            };
            char *vdbserver[] = {
                "bases.bireme.br",
                "www.scielo.br",
                "serverofi.bireme.br",
            };
            int vdbport[] = {
                1417,
                1417,
                1417
            };


            /* scan parms
            */
              p+=4;  /* skip load */
              while (*p == '/') {      /* /<tag>/socket */
                p++;
                if (isdigit(*p)) {                                                   /* /<tag> */
                  if (*p=='0') xtagzero=1;  // allow tag /0n
                  for (xtag=0; isdigit(*p); p++ ) xtag=xtag*10+((int)(*p)-(int)'0');
                  if (xtag < 1) fatal("fldupdat/procx/Gload/tag");
                  if (*p=='/' || *p=='='); else fatal("fldupdat/procx/Gload/tag dlm");
/*
                  if (sscanf(p,"%"_LD_,&xtag) != 1) fatal("fldupdat/procx/Gload/socket/tag");
                  while (isdigit(*p)) p++;
*/
                } else
                if (strncmp(p,"no0D=",5) == 0 || strncmp(p,"no0D/",5) == 0 ||        /* /no0D */
                    strncmp(p,"noCR=",5) == 0 || strncmp(p,"noCR/",5) == 0) {        /* /noCR */
                  p+=4;
                  parm0=1;
                } else
                if (strncmp(p,"no0A=",5) == 0 || strncmp(p,"no0A/",5) == 0 ||        /* /no0A */
                    strncmp(p,"noLF=",5) == 0 || strncmp(p,"noLF/",5) == 0) {        /* /noLF */
                  p+=4;
                  parm1=1;
                } else
                if (strncmp(p,"line=",5) == 0 || strncmp(p,"line/",5) == 0 ||        /* /line */
                    strncmp(p,"nonl=",5) == 0 || strncmp(p,"nonl/",5) == 0) {        /* /nonl */
                  p+=4;
                  parm0=parm1=1;
                } else
                if (strncmp(p,"socket=",7) == 0 || strncmp(p,"socket/",7) == 0) {    /* /socket */
                  p+=6;
                  parmsocket=1;
                } else
                if (strncmp(p,"head=",5) == 0 || strncmp(p,"head/",5) == 0) {        /* /head */
                  p+=4;
                  parmhead=1;
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
                fatal("fldupdat/procx/Gload[/<tag>][/nolf][/xml][/socket][/head][={<url>|<file>}]");  
                //+ Gmark[/<tag>]
                //fatal("fldupdat/procx/Gload/socket/option");
              } /* while /x */
              if (*p == '=') p++;      /* <file> | <url> */


            /* alloc working area
            */
            nbytes=(rec_maxmfrl>MAXMFRL)?rec_maxmfrl:MAXMFRL;
            if (nbytes < MFRmfrl) nbytes=MFRmfrl;
            nbytes+=BUFSIZ;     /* buffup */ /* nbytes=MFRmfrl+MFRmfrl+BUFSIZ; */
#if CICPP
            try { xarea = (char *) new char [nbytes]; }
            catch (BAD_ALLOC) { xarea = (char *)ALLONULL; }
#else /* CICPP */
            xarea=(char *)ALLOC((ALLOPARM)nbytes);
#endif /* CICPP */
            if (xarea == (char *)ALLONULL) fatal("fldupdat/procx/Gload/socket/ALLOC");
            *xarea='\0';

            q=xarea; /* batchp */


            /* del/add command
            */
            if (!xtagzero) sprintf(q,"D%"_LD_,xtag);
            else      sprintf(q,"D*");                       q+=strlen(q);   // exec tag /0n

            sprintf(haddp=q,"H%09"_LD_" ",(xtag)?(LONGX)xtag:1L); q+=strlen(q);
            sprintf(hlenp=q,"123456789 ");                   q+=strlen(q);
            hdatap=q;


	     if (parmsocket) {            /* ./mx null "proc='Gload/socket=http://www.bireme.br:80/'" */

            /* scan <url>
            */
            protocol="HTTP";
            q=strstr(p,"://"); /* http:// */
            if (q) {
                char *x;
                int len;
                *q='\0'; len=strlen(p);
                if (len < 8) {
                  protocol=p;                                                    /* http | x1417*/
                  for (x=p; *x; x++) *x=toupper(*x);
                  p=q+3;
                }
                else *q=':';
            }

            serverhost=p;                                                       /* www.bireme.br | serverofi.bireme.br */

            docpathp=strchr(p+1,'/');                                           /* /docpath | /dbname/parms */
            if (docpathp) *docpathp='\0';
            else docpathp="/";

            port=80;                          /* :80 */
            q=strchr(p,':');
            if (q) {
                *q='\0';
                if (sscanf(q+1,"%d",&xport) == 1) port=xport;                   /* :80 | :1417 */
                else xport=0; /*fatal("fldupdat/procx/Gload/socket/port");*/
            }

            /* X1417://serverofi.bireme.br:1417/msg
                                               /dbname/control/maxmfn
                                               /dbname/text/mfn/mfn2/count/step
                                               /dbname/dict/key/key2/contains/count
                                               /dbname/bool/expression
                                               /dbname/rank/text
            */
            if (strcmp(protocol,"X1417") == 0) {
                maxrds=1;                                                        /* single read */
                if (!xport) port=1417;     /* :1417 */
            }

            msgp=hdatap;
            *msgp='\0';
            if (strcmp(protocol,"X1417") == 0) {
                sprintf(msgp,"%s\n",docpathp+1);
            }
            else
            if (strcmp(protocol,"HTTP") == 0) {
                sprintf(msgp,"%s /%s HTTP/1.0\r\n\r\n",(parmhead)?"HEAD":"GET",docpathp+1);
            }

            if (*msgp) rc = mainclient( parmcmd, msgp, serverhost, port,  hdatap, nbytes-strlen(xarea), maxrds);

	     }
         else { /* else parmsocket: <file> */

             filnamp=p;
             p+=strlen(p);

             rc = mainfile  ( parmcmd, filnamp, parm0, parm1, hdatap, nbytes-strlen(xarea) );

	     } /* end parmsocket */



            if (rc < 0) {
                sprintf(hdatap,"Gload%s error %d",parmsocket?"/socket":"",(-rc));
                hlen=strlen(hdatap);
            }
            else hlen=rc;

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

        }  /* end Gload/socket */
        else

#endif /* PROCXSOCKREC */
/* End PROCXSOCKREC
*/


