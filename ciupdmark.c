

/* Begin PROCXMARKREC
*/
#if PROCXMARKREC

	     if (strncmp(p,"mark=",5) == 0 ||  /* mx bulas/xbulas uctab=ansi "proc='Gmark[/1]/decselem=dpffabcd'" */
             strncmp(p,"mark/",5) == 0) {


            char dcs11[BUFSIZ];
            char dcs21[BUFSIZ];

            char *dcs11a="decs-cys.bvsalud.org";
            char *dcs11b="/cgi-bin/wxis1660.exe/decsserver/?IsisScript=../cgi-bin/decsserver/decsserver.xis&previous_page=homepage&task=hierarchic&interface_language=";
            char *dcs12="&mfn_tree=";
            char *dcs21a="decs.ws.bvsalud.org";
            char *dcs21b="/main.php?service=getTree";

            char *dcslang="p";
            //char *isolang="pt";
            char  opclng='p';

            int opcout=1;
            int opcgiz=0;
            int opclog=0;

            int opclink=0;
            LONGX tag02=0;
            LONGX tag03=0;
            int opcdcs=0;
            LONGX tag20=20;

            int opcmark=0;
            char markp[BUFSIZ]={'\0'};

            //int parmprint=0;
            //FILE *parmfp=NULL;

            char *xarea=NULL;
            LONGX nbytes,nbytes0,nbytes1;
            char *flag;
            char *ws;
            char *swif=NULL;
            char *swmf=NULL;
            char *swmfp=NULL;

            TRMSTRU *swtrmp=NULL; /**/
            LONGX switrm=(-1L);    /**/
            RECSTRU *swrecp=NULL; /**/
            LONGX swirec=(-1L);    /**/

            char *q;
            LONGX xtag=0; 
            int i;

            char *haddp=NULL;
            char *hlenp=NULL;
            //char *hdatap=NULL;
            LONGX hlen=0;

            char tabsplit[256];
            char tabperiod[256];

            memset(tabsplit,0x00,sizeof(tabsplit));
            for (i=0; i<=(int)' '; i++) tabsplit[i]=1;

            memset(tabperiod,0x00,sizeof(tabperiod));
            tabperiod['.']=tabperiod[',']=tabperiod[';']=tabperiod[':']=1;

            p+=4;  /* skip mark/ */

            q=p; while (isspace(*q)) q++;
            if (*q=='/') {
                q++; while (isspace(*q)) q++;
                if (isdigit(*q)) {
                  p=q; for (xtag=0; isdigit(*p); p++ ) xtag=xtag*10+((int)(*p)-(int)'0');
                  if (xtag <= 0) fatal("fldupdat/procx/Gmarx/tag");
                  while (isspace(*p)) p++;
                }
            }
            if (!xtag) xtag=1; //default tag

            while (*p == '/') {

                char *x;
                if (strncmp(p,"/lang:",6) == 0) {
                  p+=6;
                  if (*p>='A' && *p<='z') opclng=toupper(*p); p++;
                  opclng=(opclng=='I')?'i': (opclng=='E')?'e': (opclng=='P')?'p': '\0';
                  if (!opclng) fatal("fldupdat/procx/Gmark/lang/iep");
                  //isolang=(opclng=='i')?"en": (opclng=='e')?"es": "pt";
                  dcslang=(opclng=='i')?"i" : (opclng=='e')? "e":  "p";
                } else
                if (strncmp(p,"/tree-id:",9) == 0) {
                  p+=9; if (sscanf(p,"%"_LD_,&tag20) != 1) { while (isdigit(*p)) p++; }
                  else  fatal("fldupdat/procx/Gmark/tree-id");
                } else

                if (strncmp(p,"/keys/",6) == 0 || strncmp(p,"/keys=",6) == 0) { p+=5; opcgiz=1; opcout=0; }
                if (strncmp(p,"/g/"   ,3) == 0 || strncmp(p,"/g="   ,3) == 0) { p+=2; opcgiz=1; opcout=0; }
                //else
                //if (strncmp(p,"/p/",3) == 0 || strncmp(p,"/p=",3) == 0) {
                //  p+=2;
                //  parmfp=stdout;
                //  parmprint=1;
                //} else
                //if (strncmp(p,"/p:",3) == 0) {
                //  char *x;
                //  p+=3;
                //  for (x=p, parmfp=NULL; *p; p++) if (*p=='=' || *p=='/') { char c= *p; *p='\0'; parmfp=fopen(dbxcipar(NULL/*gidbnp*/,x,'='),"w"); *p=c; break; }
                //  if (!parmfp) fatal("fldupdat/procx/Gmark/p:");
                //  parmprint=1;
                //}
                else {

	              if (strncmp(p,"/log1",5)       == 0) { opclog=1; p+=5; }
	              if (strncmp(p,"/log2",5)       == 0) { opclog=2; p+=5; }
	              if (strncmp(p,"/log3",5)       == 0) { opclog=3; p+=5; }
	              if (strncmp(p,"/log4",5)       == 0) { opclog=4; p+=5; }
	              if (strncmp(p,"/log5",5)       == 0) { opclog=5; p+=5; }

	              if (strncmp(p,"/decselem=",10) == 0) { opcdcs=3; p+=10-1; } else
	              if (strncmp(p,"/decselem/",10) == 0) { opcdcs=3; p+=9;    } else
	              if (strncmp(p,"/decsws@",8)    == 0) { opcdcs=2; p+=8; if (!strchr(p,'=')) fatal("fldupdat/procx/Gmark/dcs21a"); else { char *x=strchr(p,'='); dcs21a=p; *x='\0'; p=x+1; }  } else
	              if (strncmp(p,"/decsws=",8)    == 0) { opcdcs=2; p+=8-1;   } else
	              if (strncmp(p,"/decsws/",8)    == 0) { opcdcs=2; p+=7;     } else
	              if (strncmp(p,"/decs@",6)      == 0) { opcdcs=1; p+=6; if (!strchr(p,'=')) fatal("fldupdat/procx/Gmark/dcs11a"); else { char *x=strchr(p,'='); dcs11a=p; *x='\0'; p=x+1; }  } else
	              if (strncmp(p,"/decs=",6)      == 0) { opcdcs=1; p+=6-1;   } else
	              if (strncmp(p,"/decs/",6)      == 0) { opcdcs=1; p+=5;     } else
                  if (*p=='/' && strchr(p+1,',')) {
                    char *q;
                    p++; swmf=p;
                    q=strchr(p,','); for (; *p; p++) if (*p=='=' || *p=='/') { swmfp=p; break; }
                    if (q) {
                      int n;
                      *q++='\0';
                      for (n=0; isdigit(*q); q++ ) n=n*10+((int)(*q)-(int)'0'); tag02=n;
                      if (*q==',') { q++; for (n=0; isdigit(*q); q++ ) n=n*10+((int)(*q)-(int)'0'); tag03=n; }
                      if (*q) if (*p=='=' || *p=='/') ; else fatal("fldupdat/procx/Gmark/mf/tag");
                    }
                    opclink=1;
                  }
                  else
                  if (*p=='/') { p++; for (x=p; *p; p++) if (*p=='=' || *p=='/') { char c= *p; *p='\0'; strcpy(markp,x); *p=c; opcmark=1; break; } if (!*markp) fatal("fldupdat/procx/Gmark/mf/tag"); }

                }

            } /* while /option */

            if (*p=='=') { p++; swif=p; while (*p) p++; }  /* mark= */
            if (!swif) fatal("fldupdat/procx/Gmark/db");
            if (!swmf) swmf=swif;
            if (swmfp) *swmfp='\0';

            if (*p) fatal("fldupdat/procx/Gmark[/<tag>]{/<elem>|/keys|/decs}=<if>[:<mf>,<otag>[,<ctag>]]");


            sprintf(dcs11,"http://%s%s",dcs11a,dcs11b);
            sprintf(dcs21,"http://%s%s",dcs21a,dcs21b);


            nbytes=(rec_maxmfrl>MAXMFRL)?rec_maxmfrl:MAXMFRL;
            if (nbytes < MFRmfrl) nbytes=MFRmfrl;
            nbytes=nbytes0=nbytes+BUFSIZ;     /* data,buffup */ /* nbytes=nbytes0=MFRmfrl+MFRmfrl+BUFSIZ; */
            nbytes+=(nbytes1=MFRmfrl+BUFSIZ); /* flag */
            nbytes+=(        MFRmfrl+BUFSIZ); /* 6words */
#if CICPP
            try { xarea = (char *) new char [nbytes]; }
            catch (BAD_ALLOC) { xarea = (char *)ALLONULL; }
#else /* CICPP */
            xarea=(char *)ALLOC((ALLOPARM)nbytes);
#endif /* CICPP */
            if (xarea == (char *)ALLONULL) fatal("fldupdat/procx/Gmark/ALLOC");
            *xarea='\0';

            q=xarea; /* batchp */
            flag=xarea+nbytes0;
            ws=flag+nbytes1;

            /* process data field
            */
            xdir=fieldx(irec,(UWORD)xtag,1);

            if (xdir >= 0) { /* data */
                    LONGX pmfn=0, nwords;
                    FFI off, len, inlen;
                    char *wsp,*infldp,*x;
                    int strip=1;

                    char *xpw[6];
                    FFI   ilw[6];
                    FFI   vwoff[6];
                    char *vwfldp[6];
                    FFI   vwleft[6];
                    FFI   vgoff;

                    char *fldp;
                    FFI left;
                    FFI mbysize;

                    /* */
                    fldp=FIELDP(xdir);
                    left=DIRlen(xdir);
#if QUOTE2X
                    for (x=fldp; *x; x++) if (*x == '"') *x='!'; /* aspas para debug */
#endif
                    memmove(xarea,fldp,left); xarea[left]='\0';
                    /* */
                    mbysize=left;
                    memset(flag,(int)' ',mbysize); flag[mbysize]='\0';


                    /* flag data chars
                    */
                    if (strip) {
                            char *x=xarea;
                            char *a,*b;
                            FFI xa=0,xb=0;
                            off=0;
                            while ((a=strchr(x,'<')) != NULL)
                              if (strchr(a,'>')) {
                                for (xa=0, a=x; *a != '<'; a++) xa++;
                                flag[(off+=xa)]=upmF3;
                                for (xb=1, b=a; *b != '>'; b++) { xb++; flag[off++]=upmF3; }
                                flag[off++]=upmF3;
                                x=b+1;
                              } else {
                                off++;
                                x++;
                              }
                    }
                    /* everything but tabsplit is word */
                    for (x=xarea, left=DIRlen(xdir), off=0; left--; x++, off++) {
                     if (flag[off] == ' ') {
                      if (tabsplit[(unsigned char)*x]) flag[off]=upmF1; else flag[off]=upmF2;
                     }
                    }
                    /* remove right most period */
                    for (left=DIRlen(xdir), off=left-1; off>=1; off--) {
                     if (flag[off-1] == upmF2) {
                       if (tabperiod[(unsigned char)xarea[off]])
                         flag[off]=upmF4;
                     }
                    }
                    if (left==1) if (tabperiod[(unsigned char)*xarea]) flag[0]=upmF4;

                    /* del/add command
                    */
                    if (xtag) sprintf(q,"D%"_LD_,xtag);
                    else      sprintf(q,"D*");                       q+=strlen(q);

                    sprintf(haddp=q,"H%09"_LD_" ",(xtag)?(LONGX)xtag:1L); q+=strlen(q);
                    sprintf(hlenp=q,"123456789 ");                   q+=strlen(q);

                    //hdatap=q;
                    if (opclog == 1) {
                      sprintf(q,"\nmask|% 9"_LD_"|",(LONGX)DIRlen(xdir));                             hlen+=strlen(q);   q+=strlen(q);
                      memmove(q,flag,mbysize); hlen+=mbysize; q+=mbysize;
                    }


                    /* for chars left
                    */
                    for (fldp=FIELDP(xdir), left=DIRlen(xdir), off=0; left; ) {
                      int xbreak=0;
                      for (pmfn=0, nwords=0, ws[0]='\0', wsp=ws, len=0, infldp=NULL, inlen=0, i=1; i <= 6 && left; i++) {
                        xbreak=0;
                        while (flag[off] != upmF2 && left) { if (len) if (flag[off] == upmF3) { xbreak=1; break; } /*no marks between words*/
                                                             if (opcout) if (!infldp) { *q++ = *fldp; hlen++; }
                                                             off++; fldp++; left--; if (infldp) inlen++; }         *q='\0'; /*debug*/
                        if (xbreak) break;
                        if (flag[off] == upmF2 && i == 1)  { vgoff=off; infldp=fldp; }
                        if (flag[off] == upmF2 && i >= 2)  { *wsp++ = ' '; len++; }
                        while (flag[off] == upmF2 && left) { off++; *wsp++ = *fldp++; left--; len++; inlen++; }
                        if (len)                           { vwoff[nwords]=off; vwfldp[nwords]=fldp; vwleft[nwords]=left; }
                        if (len)                           { xpw[nwords]=wsp; ilw[nwords]=inlen; *wsp='\0'; nwords++; }
                      }

                      if (len) { /* lookup */
                        if (opclog==3) {
                         sprintf(q,"\ntext|% 9"_LD_"|% 9"_LD_"|%s|",nwords,(LONGX)len,ws);                       hlen+=strlen(q);   q+=strlen(q);
                        }
                        for (wsp=ws; *wsp; wsp++) *wsp=isisuctab[(unsigned char)*wsp];
                        while (nwords--) { /* words */
                          TRMSTRU *trmp;
                          if (opclog==4) {
                           sprintf(q,"\nterm|% 9"_LD_"|% 9"_LD_"|%s|",nwords+1,(LONGX)strlen(ws),ws);            hlen+=strlen(q);   q+=strlen(q);
                          }
                          if (!swtrmp) {
#if CICPP
                            try { swtrmp=new TRMSTRU(cisisxp); }
                            catch (BAD_ALLOC) { fatal("fldupdat/procx/Gmark/next/trm"); }
#else /* CICPP */
                            for (switrm=maxntrm; switrm--; ) { if (!vtrmp[switrm]) /* ja' decrementado */ break; }
                            if (switrm<0L) fatal("fldupdat/procx/Gmark/next/trm");
#endif /* CICPP */
                          }

#if CICPP
                          swtrmp->xterm((unsigned char *)swif,(unsigned char *)ws);
#else /* CICPP */
                          TERM(switrm,(unsigned char *)swif,(unsigned char *)ws);
                          swtrmp=vtrmp[switrm]; /* check ci*.c */
#endif /* CICPP */
                          if (TRMrc==RCNORMAL) {

                              char *fws=ws+len+1;
                              char *v20=fws;
                              char *v02=fws;
                              char *v03=v02+1;
                              *v02='\0'; *v03='\0';
#if CICPP
                              swtrmp->xposting(1L);
#else /* CICPP */
                              pmfn=posting(switrm,1L);
#endif /* CICPP */

                              if (opclog>=5) {
                               sprintf(q,"\n% 9"_LD_"|% 9"_LD_"|% 9"_LD_"|% 9"_LD_"|% 9"_LD_"|%s|%s|",MFRmfn,(LONGX)vgoff+1,(LONGX)strlen(ws),nwords+1,pmfn,swif,ws);        hlen+=strlen(q);   q+=strlen(q);
                               if (opclog==6) {
                                 sprintf(q,"% 9"_LD_"|",(LONGX)ilw[nwords]);                                                                                hlen+=strlen(q);   q+=strlen(q);
                                 memmove(q,infldp,ilw[nwords]);                                                                                         hlen+=ilw[nwords]; q+=ilw[nwords];
                               }
                              }

                              if (opcout || opcdcs || opcmark || opclink ) {

                               RECSTRU *recp;
                               if (!swrecp) {
#if CICPP
                                 try { swrecp=new RECSTRU(cisisxp); }
                                 catch (BAD_ALLOC) { fatal("fldupdat/procx/Gmark/next/rec"); }
#else /* CICPP */
                                 for (swirec=maxnrec; swirec--; ) { if (!vrecp[swirec]) /* ja' decrementado */ break; }
                                 if (swirec<0L) fatal("fldupdat/procx/Gmark/next/rec");
#endif /* CICPP */
                               }

#if CICPP
                               swrecp->xrecord(swmf,pmfn);
#else /* CICPP */
                               RECORD(swirec,swmf,pmfn);
                               swrecp=vrecp[swirec]; /* check ci*.c */
#endif /* CICPP */
                               if (RECrc == RCNORMAL) {
                                   int xdir;
                                   if (opcdcs) {
                                     xdir=fieldx(swirec,(int)tag20,1); /* occ 1 */
                                     if (xdir >= 0) { memmove(v20,FIELDP(xdir),DIRlen(xdir)); v20[DIRlen(xdir)]='\0'; }
                                   }
                                   else if (opclink) {
                                     xdir=fieldx(swirec,(int)tag02,1);
                                     if (xdir >= 0) { memmove(v02,FIELDP(xdir),DIRlen(xdir)); v02[DIRlen(xdir)]='\0'; }
                                     v03=v02+strlen(v02)+1; *v03='\0';
                                     if (tag03) {
                                       xdir=fieldx(swirec,(int)tag03,1);
                                       if (xdir >= 0) { memmove(v03,FIELDP(xdir),DIRlen(xdir)); v03[DIRlen(xdir)]='\0'; }
                                     }
                                     if (!*v03) if (*v02=='<') {
                                       char *x;
                                       if ((x=strchr(v02,' '))!=NULL) { *x='\0'; sprintf(v03,"</%s>",v02+1); *x=' '; } else
                                       if ((x=strchr(v02,'>'))!=NULL) { *x='\0'; sprintf(v03,"</%s>",v02+1); *x='>'; }
                                     }
                                   }
                               }

                               if (opclink) if (!*v02) pmfn=0L; /* select */

                              } /* end if opcout || opcdcs || opcmark || opclink || opcgiz20 */

                              if (pmfn) {

                               if (opcgiz) {
                                sprintf(q,"\n% 9"_LD_"|% 9"_LD_"|% 9"_LD_"|% 9"_LD_"|% 9"_LD_"|%s|%s|",MFRmfn,(LONGX)vgoff+1,(LONGX)strlen(ws),nwords+1,pmfn,swif,ws);        hlen+=strlen(q);   q+=strlen(q);
                               }
                               else

                               if (opcdcs==1) {
                                sprintf(q,"<a href=\"%s%s%s%06"_LD_"\">",dcs11,dcslang,dcs12,pmfn);           hlen+=strlen(q);   q+=strlen(q);
                                memmove(q,infldp,ilw[nwords]);                                             hlen+=ilw[nwords]; q+=ilw[nwords];
                                sprintf(q,"</a>");                                                        hlen+=strlen(q);   q+=strlen(q);
                               }
                               else
                               if (opcdcs==2) {
                                sprintf(q,"<a href=\"%s&tree_id=%s\">",dcs21,v20);                        hlen+=strlen(q);   q+=strlen(q);
                                memmove(q,infldp,ilw[nwords]);                                             hlen+=ilw[nwords]; q+=ilw[nwords];
                                sprintf(q,"</a>");                                                        hlen+=strlen(q);   q+=strlen(q);
                               }
                               else
                               if (opcdcs==3) {
                                sprintf(q,"<decs mfn=\"%06"_LD_"\" tree_id=\"%s\">",pmfn,v20);                hlen+=strlen(q);   q+=strlen(q);
                                memmove(q,infldp,ilw[nwords]);                                             hlen+=ilw[nwords]; q+=ilw[nwords];
                                sprintf(q,"</decs>");                                                     hlen+=strlen(q);   q+=strlen(q);
                               }
                               else
                               if (opcmark) {
                                sprintf(q,"<%s key=\"%s\" mfn=\"%"_LD_"\">",markp,ws,pmfn);                   hlen+=strlen(q);   q+=strlen(q);
                                memmove(q,infldp,ilw[nwords]);                                             hlen+=ilw[nwords]; q+=ilw[nwords];
                                sprintf(q,"</%s>",markp);                                                 hlen+=strlen(q);   q+=strlen(q);
                               }
                               else
                               if (opclink) {
                                sprintf(q,"%s",v02);                                                      hlen+=strlen(q);   q+=strlen(q);
                                memmove(q,infldp,ilw[nwords]);                                             hlen+=ilw[nwords]; q+=ilw[nwords];
                                sprintf(q,"%s",v03);                                                      hlen+=strlen(q);   q+=strlen(q);
                               }
                               else { /* opcout default is <swif></swif> */
                                sprintf(q,"<%s key=\"%s\" mfn=\"%"_LD_"\">",swif,ws,pmfn);                    hlen+=strlen(q);   q+=strlen(q);
                                memmove(q,infldp,ilw[nwords]);                                             hlen+=ilw[nwords]; q+=ilw[nwords];
                                sprintf(q,"</%s>",swif);                                                  hlen+=strlen(q);   q+=strlen(q);
                               }

                              } /* end pmfn */

                              break; /* pmfn is ok */

                          } /* end TRMrc == RCNORMAL */

                          if (nwords) *xpw[nwords-1]='\0';
                        } /* end words */
                      }  /* end len lookup  */

                      if (pmfn > 0) {
                        off=vwoff[nwords]; fldp=vwfldp[nwords]; left=vwleft[nwords];
                      }
                      else if (infldp) {
                        if (opcout) { memmove(q,infldp,ilw[0]);                                            hlen+=ilw[0]; q+=ilw[0]; }
                        off=vwoff[0]; fldp=vwfldp[0]; left=vwleft[0];
                      }

                    } /* end chars left */

            } /* end data */

/*...*/
            *q='\0';
            if (hlen) {
                /* actual add lenght */
                sprintf(hlenp,"%9"_LD_,(LONGX)hlen); hlenp[9]=' ';
                ///* print */
                //if (parmprint) fprintf(parmfp,"%s",hdatap);
            }
            else if (haddp) *haddp='\0';

#if CICPP
     delete swtrmp;
#else /* CICPP */
     if (switrm>=0L) if (vtrmp[switrm]!=NULL) { /**/
          FREE(vtrmp[switrm]); vtrmp[switrm]=NULL; ntrms--;
     }
#endif /* CICPP */
#if CICPP
     delete swrecp;
#else /* CICPP */
     if (swirec>=0L) if (vrecp[swirec]!=NULL) { /**/
          FREE(vrecp[swirec]); vrecp[swirec]=NULL; nrecs--;
     }
#endif /* CICPP */

#if CICPP
        if (recp->xfldupdat(xarea)) fatal(errp);
        delete xarea;
#else /* CICPP */
        if (fldupdat(irec,xarea)) fatal(errp);
        if (xarea) FREE(xarea);
#endif /* CICPP */

        return NULL; //isgdump=1;

        }  /* end Gmark */
        else

#endif /* PROCXMARKREC */
/* End PROCXMARKREC
*/

