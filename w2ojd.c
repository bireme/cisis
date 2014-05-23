          // reply and/or fldadd Similar in col jd;jd;jd [attribute]

                   if (jk_n /*joinjd*/) {

                      RECSTRU *recp;
#if 0
                      char jd[32000];
                      int jdsize=32000;
#endif
                      int i,noc,repliedkeys=0;
                      int debug=(cmd >= 3)?1:0;

                      //freq - Similar in col
                      memset(JDSIMFREQ,0x00,jdmaxprim*sizeof(LONGX));
                      memset(JDSIMPESO,0x00,jdmaxprim*sizeof(float));

                      //RECORD(irec,coljdb,l->mfn);                      
                      RECORD(irec,collectiondb,l->mfn);
                      
                      if (RECrc != RCNORMAL) ; //fatal("wtrig2/jd/RCNORMAL");
                      else {

                          if (batchp) {
                             sprintf(batchp,"<7 0>%"_LD_"^s%f^m%"_LD_"^h%"_LD_"^1",l->mfn,l->sim,l->xxx,l->hit);
                             batchp+=strlen(batchp);
                          }
#if 0
                          for (i=1, noc=nocc(irec,joinjd/*TAG440*/); i <= noc; i++) {  // for each TAG440 occ
                             int left;
                             char *jdp=jd;
                             recfield(jd,irec,joinjd/*TAG440*/,i,"");
                             left=strlen(jd);
#else
                          for (i=0, noc=jd_n; i < noc; i++) {  // for each jd_occ

                             int left,lns;
                             char *jdp=jd_areap;
                             
                             int j,jki=(-1);
                             for (j=0; j < jk_n; j++) if (i == jk_jdi[j]) { jki=j; break; }
                             if (jki < 0) continue;                             
                             
                             *jd_areap = '\0';                       /* call format */
                             lns=fmt_inter(jd_pgmp[i],irec,jd_asize,jd_areap,jd_asize);
                             if (lns < 0) left=0; //fatal("wtrig2/jd/format execution error");
                             else left=strlen(jd_areap);                             
#endif
                             if (left) {                                      // for each jd
                                  char *p;
                                  for (p=jdp; left; ) {                          // for all chars
                                     int keylen;
                                     char *keyp;

                                     //strip
                                     while (isspace(*p) || *p==';') if (left) { p++; left--; }
                                     if (!left) break;

                                     //new jd value
                                     keyp=p;
                                     for (keylen=0; left;  p++, keylen++, left--) {
                                        if (*p=='\n' || *p=='\r' || *p==';') break;
                                     }

                                     if (keylen) {
                                        LONGX hidx;
                                        int found;
                                        char *h;
                                        hidx=hashindex(jdtable,jdmaxprim,jdtabwidth,keyp,keylen,&found,1);
                                        if (!found) jdtabentries++;
                                        h=jdtable+hidx*jdhwidth;
                                        if (/*parmdebug*/debug) fprintf(stderr,"+++ table.hidx=%"_LD_" [%"_LD_"]  table.h='%s'\n",hidx,jdtabentries,h);
                                        JDCOLFREQ[hidx]++;
                                        JDDOCFREQ[hidx]++;
                                        JDSIMFREQ[hidx]++;
                                        JDCOLPESO[hidx]+=l->sim;
                                        JDDOCPESO[hidx]+=l->sim;
                                        JDSIMPESO[hidx]+=l->sim;

                                        // reply Similar in col jd;jd;jd; [attribute]
                                        if (reply) if (joinjd) {
                                           unsigned char *p;
                                           if (!repliedkeys) {
                                              sprintf(reply," jd=\"");
                                              reply+=strlen(reply);
                                           }
                                           if (repliedkeys) {
                                               sprintf(reply,"; ");
                                               reply+=strlen(reply);
                                           }
                                           if (debug) { sprintf(reply,"%d:",hidx); reply+=strlen(reply); }
                                           for (p=h; *p; p++) {
                                                if (convent[*p]) { strcpy(reply,convent[*p]); reply+=strlen(reply); }
                                                else *reply++ = *p;            
                                           }
                                           repliedkeys++;
                                        }
                                        if (batchp) {
                                           sprintf(batchp,"%d %s%s",hidx,h,";");  // <7>
                                           batchp+=strlen(batchp);
                                        }
                                     }

                                  }  // end for all chars

                             }  // end for each jd

                          }  // end for each TAG440 occ

                          if (batchp) {
                             sprintf(batchp,"</7>");
                             batchp+=strlen(batchp);
                          }

                       } // end RCNORMAL

                       if (reply) {
                           if (repliedkeys) {
                              sprintf(reply,"\"");
                              reply+=strlen(reply);
                           }
                       }

                   }  // end jd reply

