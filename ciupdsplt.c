

/* Begin PROCXSPLIREC
*/
#if PROCXSPLIREC
	     if (strncmp(p,"split=",6) == 0 || strncmp(p,"split/clean=",12) == 0) {
            char *xarea=NULL;
            LONGX nbytes;
            int trigrams=0;
            char lasttrigchr1='\0';
            char *q;
            LONGX xtag;
            char c1='\r';
            char c2='\n';
            int clean=0;
            char tabsplit[256];
            int b2a=0;
            short int b2ashort;
            LONGX b2along;
            int letteroption=0;
#define MAXNWORDS 4000
            int sixwords=0;
            char *vwordsp[MAXNWORDS];
            short int vwordsn[MAXNWORDS];
            short int vwordsx[MAXNWORDS+1];
            char tabperiod[256];
            LONGX sixwordsitrm=(-1);  /**/
            char *sixwordsif=NULL;   /**/
#if INCPRECX
            TRMSTRU *trmp=NULL;      /**/
#endif /* INCPRECX */

            nbytes=MFRmfrl+BUFSIZ;
            nbytes+=(MFRmfrl>BUFSIZ)?MFRmfrl:BUFSIZ; /* <== */

            if (strncmp(p,"split/clean=",12) == 0) { clean=1; p+=12; }
            else p+=6;
            if (sscanf(p,"%"_LD_,&xtag) != 1) fatal("fldupdat/procx/Gsplit/tag");
            while (isdigit(*p)) p++;
            memset(tabperiod,0x00,sizeof(tabperiod));
            memset(tabsplit,0x00,sizeof(tabsplit));
            tabsplit[c1]=1;
            tabsplit[c2]=1;
            if (*p == '=') {
                p++;
                if (strncmp(p,"wtrig",5) == 0) {
                  b2a=1;
                  memset(tabsplit,0x00,sizeof(tabsplit));
                  p+=5;
                }
                else
                if (strncmp(p,"short",5) == 0) {
                  b2a=sizeof(b2ashort);
                  memset(tabsplit,0x00,sizeof(tabsplit));
                  p+=5;
                }
                else
                if (strncmp(p,"long",4) == 0) {
                  b2a=sizeof(b2along);
                  memset(tabsplit,0x00,sizeof(tabsplit));
                  p+=4;
                }
                else
                if (strncmp(p,"words",5) == 0) {
                  int i;
                  for (i=0; i<sizeof(tabsplit); i++) tabsplit[i]=1-isiswctab[i];
                  p+=5;
                }
                else
                if (strncmp(p,"6words",6) == 0
#if INCPRECX
                 || strncmp(p,"6words/if=",10) == 0
#endif /* INCPRECX */
                ) {
                  int i;
                  for (i=0; i<sizeof(tabsplit); i++) tabsplit[i]=1-isiswctab[i];
                  p+=6;
                  sixwords=1;
                  tabperiod['.']=tabperiod[',']=tabperiod[';']=tabperiod[':']=1;
#if INCPRECX
                  if (strncmp(p,"/if=",4) == 0) {
                    p+=4;
                    sixwordsif=p; p+=strlen(p);
                    sixwordsitrm=(-1);
#if CICPP
                    try { trmp=new TRMSTRU(cisisxp); }
                    catch (BAD_ALLOC) { fatal("fldupdat/procx/Gsplit/next"); }
#else /* CICPP */
                    for (sixwordsitrm=maxntrm; sixwordsitrm--; ) {
                      if (!vtrmp[sixwordsitrm]) /* ja' decrementado */
                        break;
                    }
                    if (!sixwordsitrm) fatal("fldupdat/procx/Gsplit/next");
#endif /* CICPP */
                  }
#endif /* INCPRECX */
                }
                else
                if (strncmp(p,"letters",7) == 0 || strncmp(p,"letter1",7) == 0 || strncmp(p,"letterA",7) == 0) {
                  int i;
                  letteroption=(strncmp(p,"letter1",7) == 0) ? 1 : 0;
                  letteroption=(strncmp(p,"letterA",7) == 0) ? 2 : letteroption;
                  memset(tabsplit,0x01,sizeof(tabsplit));
                  for (i=(int)'A'; i<=(int)'Z'; i++) tabsplit[i]=0;
                  if (letteroption == 0) for (i=(int)'a'; i<=(int)'z'; i++) tabsplit[i]=0;
                  if (letteroption == 1) for (i=(int)'a'; i<=(int)'z'; i++) tabsplit[i]=0;
                  if (letteroption == 2) {
                    //mx iso=\utl\charac\gansna.iso "pft=if v50.1>='A' and v50.1<='Z' then c21'tabsplit['f(val(v1),3,0)']=0; // 'v50/ fi" lw=0 now
                    tabsplit[192]=0; // A grave
                    tabsplit[193]=0; // A agudo
                    tabsplit[194]=0; // A circu
                    tabsplit[195]=0; // A til
                    tabsplit[196]=0; // A trema
                    tabsplit[197]=0; // A bola
                    tabsplit[199]=0; // Cedilha
                    tabsplit[200]=0; // E grave
                    tabsplit[201]=0; // E agudo
                    tabsplit[202]=0; // E circu
                    tabsplit[203]=0; // E trema
                    tabsplit[204]=0; // I grave
                    tabsplit[205]=0; // I agudo
                    tabsplit[206]=0; // I circu
                    tabsplit[207]=0; // I trema
                    tabsplit[209]=0; // N til
                    tabsplit[210]=0; // O grave
                    tabsplit[211]=0; // O agudo
                    tabsplit[212]=0; // O circu
                    tabsplit[213]=0; // O til
                    tabsplit[214]=0; // O trema
                    tabsplit[217]=0; // U grave
                    tabsplit[218]=0; // U agudo
                    tabsplit[219]=0; // U circu
                    tabsplit[220]=0; // U trema
                    tabsplit[221]=0; // Y agudo
                  }
                  p+=7;
                }
                else
                if (strncmp(p,"numbers",7) == 0) {
                  int i;
                  memset(tabsplit,0x01,sizeof(tabsplit));
                  for (i=(int)'0'; i<=(int)'9'; i++) tabsplit[i]=0;
                  p+=7;
                }
                else
                if (strncmp(p,"trigrams",8) == 0) {
                  int i;
                  memset(tabsplit,0x01,sizeof(tabsplit));
                  for (i=(int)'A'; i<=(int)'Z'; i++) tabsplit[i]=0;
                  for (i=(int)'a'; i<=(int)'z'; i++) tabsplit[i]=0;
                  for (i=(int)'0'; i<=(int)'9'; i++) tabsplit[i]=0;
                  p+=8; trigrams=1;
                }
                else {
                  c1=c2= *p;
                  if (*p) p++;
                  memset(tabsplit,0x00,sizeof(tabsplit));
                  tabsplit[c1]=1;
                }
            }
            if (*p) fatal("fldupdat/procx/Gsplit");
            if (trigrams || sixwords) {
              //nbytes=(MFRmfrl+1)*(1+3+1+1+1+3)+BUFSIZ; /* k+1 * 10 */
              //nbytes=(MFRmfrl+1)*(1+5+1+1+1+4)+BUFSIZ; /* k+1 * 13 */
              FFI haddlen = 1/*H*/ + 5/*tag*/ + 1/* */ + 1/*4*/ + 1/* */; /* =9 */
              FFI ntrigs = 0;
              FFI K;
              FFI totlenwords=0;
/*...*/
              for (xdir=0; xdir < MFRnvf; xdir++) {
                if (DIRtag(xdir) == (UWORD)xtag || !xtag) {
                    char *addp=NULL;
                    FFI addlen=0;
                    FFI left=DIRlen(xdir);
                    FFI len=0;
                    char *fldp,*fldq;
                    unsigned char uc;
                    fldp=fldq=FIELDP(xdir);
                    while (left) {
                      /* upper case */
                      uc = isisuctab[(unsigned char)*fldq]; 
                      if (tabsplit[uc]) {
                        fldq++; left--;
                        if (len) {
                          addp=fldp; addlen=len;
                        }
                        fldp=fldq;
                        len=0;
                      }
                      else {
                        fldq++; len++; left--;
                        if (left == 0) {
                          addp=fldp; addlen=len;
                        }
                      }
                      if (addp) {
                        if (addlen) {
                            K = addlen;
                            ntrigs += 4 + (K+1);
                              /* dna!  distinguished first trig in word */
                              /* dna!  peso 2 */
                              /* d#    distinguished first letter in word */
                              /* d s   initial space initial letter */
                              /* dna   every each 1- to 3-letter trig */
                            totlenwords+=21*(K+1); /* 6+5+4+3+2+1 */
                        }
                        addp=NULL; addlen=0;
                      }
                    } /* end while left */
                } /* end tag */
              } /* end xdir */
              if (trigrams) nbytes = ntrigs*(haddlen+4) + BUFSIZ;
              else nbytes = haddlen + totlenwords + BUFSIZ;
              //printf(" + + nbytes=%"_LD_"\n",nbytes);
/*...*/
            } /* end if trigrams */
#if CICPP
            try { xarea = (char *) new char [nbytes]; }
            catch (BAD_ALLOC) { xarea = (char *)ALLONULL; }
#else /* CICPP */
            xarea=(char *)ALLOC((ALLOPARM)nbytes);
#endif /* CICPP */
            if (xarea == (char *)ALLONULL) fatal("fldupdat/procx/Gsplit/ALLOC");
            q=xarea;
            if (xtag)
              sprintf(q,"D%"_LD_,xtag);
            else
              sprintf(q,"D*");
            q+=strlen(q);
            for (xdir=0; xdir < MFRnvf; xdir++) {
                if (DIRtag(xdir) == (UWORD)xtag || !xtag) {
                    char *addp=NULL;
                    FFI addlen=0;
                    FFI left=DIRlen(xdir);
                    FFI len=0;
                    char *fldp=FIELDP(xdir);
                    char *fldq=fldp;
                    int nwords=0;
                    int followperiod=0;
                    int newperiod=1;
                    if (b2a == 1) {
                     /*char b2awterm[4+1];*/
                     float b2aweight;
                     unsigned short int usi_w;
                     int b2a=sizeof(usi_w);
                     while (left) {
                        if (left < b2a+1) fatal("fldupdat/procx/Gsplit/b2a/left/w");
                        memmove((void *)&usi_w,fldq,b2a);
                        fldq+=b2a; left-=b2a;
                        b2aweight=(float)usi_w/(float)USHRT_MAX;
                        sprintf(q,"<%"_LD_" 0>",(LONGX)DIRtag(xdir)); q+=strlen(q);
                        while (left) {
                          *q = *fldq++; left--;
                          if (*q) q++; else break;
                        }
                        sprintf(q,"^w%f</%"_LD_">",b2aweight,(LONGX)DIRtag(xdir));
                        q+=strlen(q);
                     }
                    }
                    else
                    if (b2a) {
                     if (left % b2a) fatal("fldupdat/procx/Gsplit/b2a/left");
                     while (left) {
                      if (b2a == sizeof(b2ashort)) {
                        memmove((void *)&b2ashort,fldq,b2a);
                        sprintf(q,"A%"_LD_"/%d/",(LONGX)DIRtag(xdir),b2ashort);
                      } else {
                        memmove((void *)&b2along,fldq,b2a);
                        sprintf(q,"A%"_LD_"/%"_LD_"/",(LONGX)DIRtag(xdir),b2along);
                      }
                      q+=strlen(q); fldq+=b2a; left-=b2a;
                     }
                    }
                    else { /* not b2a */
                     while (left) {
                      unsigned char uc;
                      /* upper case */
                      //uc = isisuctab[(unsigned char)*fldq];  //printf("uc=%c\n",uc);
                      uc = (letteroption) ? (unsigned char)*fldq : isisuctab[(unsigned char)*fldq];  
                      if (tabsplit[uc]) {
                        if (tabperiod[(unsigned char)*fldq]) followperiod=1;
                        fldq++; left--;
                        if (len) {
                          addp=fldp; addlen=len;
                        }
                        fldp=fldq;
                        len=0;
                      }
                      else {
                        fldq++; len++; left--;
                        if (left == 0) {
                          addp=fldp; addlen=len;
                        }
                      }
                      if (addp) {
                        if (clean) {
                          unsigned char *p;  /* AOT, 24/06/2005 */
                          FFI left;
                          while (addlen) {
                              if ((unsigned char)addp[0] <= ' ') { addp++; addlen--; }
                              else break;
                          }
                          while (addlen) {
                              if ((unsigned char)addp[addlen-1] <= ' ') addlen--;
                              else break;
                          }
                          for (left=addlen, p=(unsigned char *)addp; left--; p++) if (*p < ' ') *p=' ';
                        }
                        if (addlen) {
                          if (trigrams) { /* trigrams */
                            char trig[4+1];
                            int tlen=3;
                            char *up;
                            int loop;
                            /* upper case */
                            for (up=addp, loop=addlen; loop--; up++) *up = isisuctab[(unsigned char)*up];
                            /* clean: other ' ' to 'Z' */
                            for (up=addp, loop=addlen; loop--; up++)
                                if (*up < ' ' || *up > 'Z') *up = '@';
                            if (addlen < 3) tlen=addlen;

                            /* dna! */
                            memmove(trig,addp,tlen); trig[tlen++]='!';
                            sprintf(q,"H%"_LD_" %"_LD_" ",(LONGX)DIRtag(xdir),(LONGX)tlen); q+=strlen(q);
                            memmove(q,trig,tlen); q+=tlen;
                            /* dna! - peso 2 */
                            sprintf(q,"H%"_LD_" %"_LD_" ",(LONGX)DIRtag(xdir),(LONGX)tlen); q+=strlen(q);
                            memmove(q,trig,tlen); q+=tlen;
                            tlen--;

                            /* d# */
                            sprintf(q,"H%"_LD_" %"_LD_" ",(LONGX)DIRtag(xdir),(LONGX)2); q+=strlen(q);
                            *q++ = trig[0];
                            *q++ = '#';

                            /* d s */
                            if (lasttrigchr1) {
                              sprintf(q,"H%"_LD_" %"_LD_" ",(LONGX)DIRtag(xdir),(LONGX)3); q+=strlen(q);
                              *q++ = lasttrigchr1;
                              *q++ = ' ';
                              *q++ = trig[0];
                            }
                            lasttrigchr1=trig[0];

                            /* dna */
                            sprintf(q,"H%"_LD_" %"_LD_" ",(LONGX)DIRtag(xdir),(LONGX)tlen); q+=strlen(q);
                            memmove(q,trig,tlen); q+=tlen;
                            while (addlen > 3) {
                              addp++;
                              sprintf(q,"H%"_LD_" %"_LD_" ",(LONGX)DIRtag(xdir),(LONGX)3); q+=strlen(q);
                              memmove(q,addp,3); q+=3;
                              addlen--;
                            }

                          }
                          else if (sixwords) { /* sixwords: store */
                            vwordsp[nwords]=addp;
                            vwordsn[nwords]=addlen;
                            vwordsx[nwords]=newperiod; nwords++;
                            newperiod=followperiod; followperiod=0;
                          }
                          else { /* words */
                            if (letteroption) if (addlen) addlen=1;
                            if (addlen) {
                              sprintf(q,"H%"_LD_" %"_LD_" ",(LONGX)DIRtag(xdir),(LONGX)addlen); q+=strlen(q);
                              memmove(q,addp,addlen); q+=addlen;
                            }
                          }
                        }
                        addp=NULL; addlen=0;
                      }
                     } /* end while left */
                    }  /* end b2a */
                    if (sixwords) {  /* sixwords: output */
/*
000002|     5077|OF
000004|     2860|AND
000006|     2530|DE
000011|     1493|THE
000019|      971|IN
000031|      626|DO
000035|      587|DA
000061|      344|LA
000103|      236|ET
000110|      216|Y
000112|      215|ON
*/
                     int loop,loop2,loop3,n,i;
                     FFI len=0;
                     char *lenp;
                     FFI oldlen; /**/
                     char *oldq; /**/


                     sprintf(q,"H%"_LD_" ",(LONGX)xtag); q+=strlen(q);
                     sprintf(lenp=q,"123456789 "); q+=strlen(q);
                     vwordsx[nwords]=1;
                     for (loop=0; loop < nwords; loop++) {
                      oldq=q; oldlen=len;
                      memmove(q,vwordsp[loop],n=vwordsn[loop]); q+=n; len+=n;
                      if (sixwordsif) {
#if INCPRECX
                        *q='\0';
#if CICPP
                        trmp->xterm((unsigned char *)sixwordsif,(unsigned char *)oldq);
#else /* CICPP */
                        TERM(sixwordsitrm,(unsigned char *)sixwordsif,(unsigned char *)oldq);
#endif /* CICPP */
                        if (TRMrc==RCNORMAL) {
                          *q++='\n'; len++;
                        } else {
                          q=oldq; len=oldlen;
                        }
#endif /* INCPRECX */
                      } else {
                        *q++='\n'; len++;
                      }
                      for (loop2=1; loop2 <= 5; loop2++) {
                        if ((n=loop+loop2) == nwords) break;
                        if (vwordsx[n]) break;
                        oldq=q; oldlen=len;
                        memmove(q,vwordsp[loop],n=vwordsn[loop]); q+=n; len+=n;
                        for (loop3=0; loop3 < loop2; loop3++ ) {
                          *q++=' '; len++;
                          i=loop+1+loop3;
                          memmove(q,vwordsp[i],n=vwordsn[i]); q+=n; len+=n;
                        }
                        if (sixwordsif) {
#if INCPRECX
                          *q='\0';
#if CICPP
                          trmp->xterm((unsigned char *)sixwordsif,(unsigned char *)oldq);
#else /* CICPP */
                          TERM(sixwordsitrm,(unsigned char *)sixwordsif,(unsigned char *)oldq);
#endif /* CICPP */
                          if (TRMrc==RCNORMAL) {
                            *q++='\n'; len++;
                          }
                          else {
                            q=oldq; len=oldlen;
                          }
#endif /* INCPRECX */
                        } else {
                          *q++='\n'; len++;
                        }
                      }
                     }
                     sprintf(lenp,"%9"_LD_,(LONGX)len); lenp[9]=' ';
                    } /* end sixwords */
                } /* end tag */
            } /* end xdir */

            *q='\0';

             //printf(" + + nbytes=%"_LD_" x %d \n",nbytes,strlen(xarea));
             
#if CICPP
     delete trmp;
#else /* CICPP */
     if (sixwordsitrm>=0L) if (vtrmp[sixwordsitrm]!=NULL) { /**/
          FREE(vtrmp[sixwordsitrm]); vtrmp[sixwordsitrm]=NULL; ntrms--;
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

        }
        else
#endif /* PROCXSPLIREC */
/* End PROCXSPLIREC
*/

