

          // reply selected PFT

                   if (jkv_n /*showv3*/) {

                          RECSTRU *recp;
                          int i,noc;
                          //int debug=(cmd >= 3)?1:0;

                          RECORD(irec,collectiondb,l->mfn);

                          if (collcollmfntag/*parmcollmfntag*/) {  // load v3001,v3003 from colvdb

                             char *xp=jdv_areap;
                             char *xfldp;
                             int xdir,xtag,xlen;

                             RECORD(irectv,colvdb,l->hit);

                              //sprintf(xp,"d3003a3003%c",0x01);
                              //xp+=strlen(xp); recfield(xp,irectv,TAG3,1,"");
                              //xp+=strlen(xp); sprintf(xp,"%c",0x01);

                             for (xdir=0; xdir < MFRnvf; xdir++) {
                                 xtag=DIRtag(xdir);
                                 if (xtag == TAG4) continue;
                                 xlen=DIRlen(xdir); xfldp=FIELDP(xdir);
                                 sprintf(xp,"H%d %d ",xtag+3000,xlen); xp+=strlen(xp);
                                 memmove(xp,xfldp,xlen); xp+=xlen;
                             }
                             //add v6-like info
                             sprintf(xp,"<%d 0>%s</%d>A%d|%"_LD_"^s%f^m%"_LD_"^h%"_LD_"|",5+3000,parmtext,5+3000,6+3000,l->mfn,l->sim,l->xxx,l->hit); xp+=strlen(xp);
                             *xp='\0';
                             if (fldupdat(irec,jdv_areap)) fatal("wtrig2/w2ov3/fldupdat");

                          } // end if collmfntag

                          for (i=0, noc=jkv_n; i < noc; i++) {  // for each jkv_occ
                             int jdvi=jkv_jdvi[i];
                             if (jdvi >= 0) { // it is
                                 int lns;
                                 char *p;
                                 *jdv_areap = '\0';                       /* call format */
                                 lns=fmt_inter(jdv_pgmp[jdvi],irec,jdv_asize,jdv_areap,jdv_asize);
                                 if (lns < 0) sprintf(jdv_areap,"<wtrig in=\"w2ov3\" error=\"%d\">format execution error</wtrig>\n",lns);
                                 for (p=jdv_areap; *p; p++) {  // for all chars
                                    if (convent[*p]) { strcpy(reply,convent[*p]); reply+=strlen(reply); }
                                    else *reply++ = *p;
                                 }  // end for all chars
                             }  // end if jdvi
                           }  // end for each jkv

                   } // end if jkv_n

                   
                   else { // else if jkv_n

                   
                   if (showv3) {

                      RECSTRU *recp;
                      int xdir,nvf;
                      int maxv3len=SHOWV3LEN;
                      int xtag=0;

                      if (collcollmfntag/*parmcollmfntag*/) if (showv3 < 0) {
                         RECORD(irectv,colvdb,l->hit);
                         xtag=(-showv3);
                         for (xdir=0, nvf=MFRnvf; xdir < nvf; xdir++) {  // scan dir
                             if (DIRtag(xdir) == xtag) {  // data
                                unsigned char *p=FIELDP(xdir);
                                int n=DIRlen(xdir);
                                if (n > maxv3len) n=maxv3len;
                                for (; n--; p++) {
                                    if (convent[*p]) { strcpy(reply,convent[*p]); reply+=strlen(reply); }
                                    else *reply++ = *p;
                                }
                                *reply='\0';
                                break;
                             } // end data
                         } // end dir
                      } // end if collmfntag

                      if (!xtag) {
                         RECORD(irec,collectiondb,l->mfn);
                         xtag=showv3;
                         for (xdir=0, nvf=MFRnvf; xdir < nvf; xdir++) {  // scan dir
                             if (DIRtag(xdir) == xtag) {  // data
                                unsigned char *p=FIELDP(xdir);
                                int n=DIRlen(xdir);
                                if (n > maxv3len) n=maxv3len;
                                for (; n--; p++) {
                                    if (convent[*p]) { strcpy(reply,convent[*p]); reply+=strlen(reply); }
                                    else *reply++ = *p;
                                }
                                *reply='\0';
                                break;
                             } // end data
                         } // end dir
                      } // end if !xtag

                    }  // end if showv3 reply

                    else { // else showv3 reply

                      sprintf(reply,"%"_LD_,l->mfn);
                      reply+=strlen(reply);

                    }  // end showv3 reply


                  }  // end jkv_n /*showv3*/ reply

// end if w2ov3
