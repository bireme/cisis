/* ==----------------------------- w2ci.c ------------------------------== */

/*
       <request>  
       //<docresponse>
       
            use parmstring
            or  parmstring+=" text=txt1vp+txtvp[\n]"  (if movetxtp)
            
            <trigrams doc="1">         
                coll=trigcoll (cmd, awtfp, parmstring, reply);                                           
            </trigrams>  
            
            <jdlist doc="1" jds="n"> 
                <jd doc="1" rank="n" f="n" w="n">
                </jd>  
            </jdlist>              
       //</docresponse>  
       
            <jdlist doc="all" jds="n">  
                <jd doc="all" rank="n" f="n" w="n">
                 </jd>  
            </jdlist> 
       </request>     
*/

           /* setup text= documents and run them 
           */

           if (txtvp) {  // if text1= and/or text=
                         
              //lines extracted from txtvp (titulos/texto)
              char *p;

              //size of titulos
              int left=txtvlen;
              int xocc=0;
              
              //collection used
              int coll = -1;
              WTFUN_XWT *xwtp=NULL; //=awtfp->vwtfp[coll];  // set defered

              //freq - reinit jdtable
              hashzero(jdtable,jdmaxprim,jdtabwidth); jdtabentries=0;

              //freq - collection (Au set)
              memset(JDCOLFREQ,0x00,jdmaxprim*sizeof(LONGX));
              memset(JDCOLPESO,0x00,jdmaxprim*sizeof(float));


              //header
              sprintf(reply,"<request>\n",0);                    reply+=strlen(reply);
               
              //docheader
              //sprintf(reply,"<docresponse>\n");                 reply+=strlen(reply); 
              
              /* force several text= parms formed by Au[1]+Ti[x]
              */
              for (p=txtvp; left; ) {     // for each line of text=
    
                     //yes text1=..
                     if (movetxtp) {
                         int len;
                         char *x=movetxtp;                        // as loadcoll                       
                         while (isspace(*p)) if (left) { p++; left--; }  //strip
                         if (!left) break;
                         /* new msg: text= */
                         strcpy(parmstring,hdrparmstring);
                         *x++=' ';
                         for (len=0; left;  p++, len++, left--) { // copy 2nd text
                           //if (striptext) 
                           if (*p=='\n' || *p=='\r') break;       // until newline
                           *x++ = *p;
                         }
                         if (!len) continue;                      // skip null lines        
                         *x='\0';
                     }
                     else {
                         left=0;  // single run
                     }
                     //if (1) fprintf(stderr,"[ [parmstring=%s] ]\n",parmstring);
    
                     /* init AuTi
                     */
                     //freq - AuTi subset
                     memset(JDDOCFREQ,0x00,jdmaxprim*sizeof(LONGX));
                     memset(JDDOCPESO,0x00,jdmaxprim*sizeof(float));
    
                     /* get similars
                     */
                     ++xocc; 
    
                     sprintf(reply,"<trigrams doc=\"%d\">\n",xocc);        reply+=strlen(reply); 
                     
                     coll=trigcoll (cmd, awtfp, parmstring, reply);        reply+=strlen(reply); 
                                      
                     sprintf(reply,"</trigrams>\n");                       reply+=strlen(reply); 
    
                     if (coll < 0) return coll;
                     rc=coll; /* return code */
    
                     /* set current xwtfp
                     */
                     xwtp=awtfp->vwtfp[coll];  // set current
                     if (cmd >= 3) fprintf(stderr, "+++ coll used=%d\n", coll);

                     /* show docjdlist
                     */
                     if (jk_n && jk_tabcats[0]/*joinjd*/) {

                        char *batchp=awtdatabuff;
                        int jocc;
                        LONGX hidx;
                        char *h;
#define TOPJD 1
#if TOPJD
                        LISTJD *l;
                        int howmany=jk_tabcats[0]; //6;
                        float minweight=0.1;
                        LONGX freq;
                        float peso;
                        int docjds;
                        docjds=w2topjd (cmd, awtfp, coll, JDDOCFREQ, JDDOCPESO, howmany, minweight);
#else
                        LONGX *docfreqp;
                        LONGX loop;
                        int docjds=0;
                        for (docfreqp=JDDOCFREQ, loop=jdmaxprim; loop--; docfreqp++) {
                            if (*docfreqp) docjds++;
                        }
#endif //TOPJD
                        if (replyhx) {
                            sprintf(reply,"<jdlist doc=\"%d\" jds=\"%d\">\n",xocc,docjds);
                            reply+=strlen(reply);
                        }

                        awtdatabuff[0]='\0';
#if TOPJD
                        for (l=listjdhdr, jocc=0; l; l=l->next) {
                            if (!l) break;
                            hidx=l->hidx; h=jdtable+(l->hidx*jdhwidth);
                            freq=l->freq; //JDDOCPESO[hidx];
                            peso=l->weight;
#else
                        for (docfreqp=JDDOCFREQ, h=jdtable, hidx=0, jocc=0; jocc < docjds; hidx++, h+=jdhwidth, docfreqp++) {
                            if (!*docfreqp) continue;
                            freq= *docfreqp;
                            peso= JDDOCPESO[hidx];
#endif //TOPJD
                            jocc++;

                            //reply AuTi subset
                            if (replyhx) {
                                //sprintf(reply,"<jd doc=\"%d\" rank=\"%d\" f=\"%"_LD_"\" w=\"%f\" h=\"%"_LD_"\">%s</jd>\n",xocc,jocc,freq,peso/* /freq */,hidx,h);
                                //reply+=strlen(reply);
                                char *p;
                                sprintf(reply,"<jd doc=\"%d\" rank=\"%d\" f=\"%"_LD_"\" w=\"%f\">",xocc,jocc,freq,peso);
                                reply+=strlen(reply);
                                for (p=h; *p; p++) {
                                    if (convent[*p]) { strcpy(reply,convent[*p]); reply+=strlen(reply); }
                                    else *reply++ = *p;            
                                }
                                sprintf(reply,"</jd>\n");
                                reply+=strlen(reply);
                            
                            }

                            if (addeight) {
                                sprintf(batchp,"<8 0>%d^r%d^r%d^f%"_LD_"^w%f^h%"_LD_"^d%s</8>",docjds,xocc,jocc,freq,peso/* /freq */,hidx,h);
                                batchp+=strlen(batchp);
                            }
                        }
                        if (awtdatabuff[0]) {
                            if (fldupdat(outirec,awtdatabuff)) fatal("wtrig2/similardb/collapse/fldupdat");
                            if (*docydb) recupdat(outcrec,outirec);
                        }

                        if (replyhx) {
                            sprintf(reply,"</jdlist>\n");
                            reply+=strlen(reply);
                        }

                     } // end if jdjoin

                     //no text1=..
                     if (!txt1vp) break;

              }  // end for each line of text=

              //doctrailer
              //sprintf(reply,"</docresponse>\n");                      reply+=strlen(reply);


              /* show coljdlist
              */
              if (jk_n && jk_tabwidth[0]/*joinjd*/) if (coll) {

                    char *batchp=awtdatabuff;
                    int jocc;
                    LONGX hidx;
                    char *h;
#if TOPJD
                    LISTJD *l;
                    int howmany=jk_tabcats[0]; //6;
                    float minweight=0.1;
                    LONGX freq;
                    float peso;
                    int coljds;
                    coljds=w2topjd (cmd, awtfp, coll, JDCOLFREQ, JDCOLPESO, howmany, minweight);
#else
                    LONGX *colfreqp;
                    LONGX loop;
                    int coljds=0;
                    for (colfreqp=JDCOLFREQ, loop=jdmaxprim; loop--; colfreqp++) {
                        if (*colfreqp) coljds++;
                    }
#endif //TOPJD
                    if (replyhx) {
                        sprintf(reply,"<jdlist doc=\"all\" jds=\"%d\">\n",coljds);
                        reply+=strlen(reply);
                    }

                    awtdatabuff[0]='\0';
#if TOPJD
                        for (l=listjdhdr, jocc=0; l; l=l->next) {
                            if (!l) break;
                            hidx=l->hidx; h=jdtable+(l->hidx*jdhwidth);
                            freq=l->freq; //JDCOLPESO[hidx];
                            peso=l->weight;
#else
                        for (colfreqp=JDCOLFREQ, h=jdtable, hidx=0, jocc=0; jocc < coljds; hidx++, h+=jdhwidth, colfreqp++) {
                            if (!*colfreqp) continue;
                            freq= *colfreqp;
                            peso= JDCOLPESO[hidx];
#endif //TOPJD
                            jocc++;

                            //reply AuTi subset
                            if (replyhx) {
                                //sprintf(reply,"<jd col=\"%d\" rank=\"%d\" f=\"%"_LD_"\" w=\"%f\" h=\"%"_LD_"\">%s</jd>\n",xocc,jocc,freq,peso/* /freq */,hidx,h);
                                //reply+=strlen(reply);
                                char *p;
                                sprintf(reply,"<jd doc=\"all\" rank=\"%d\" f=\"%"_LD_"\" w=\"%f\">",jocc,freq,peso);
                                reply+=strlen(reply);
                                for (p=h; *p; p++) {
                                    if (convent[*p]) { strcpy(reply,convent[*p]); reply+=strlen(reply); }
                                    else *reply++ = *p;
                                }
                                sprintf(reply,"</jd>\n");
                                reply+=strlen(reply);

                            }

                        if (addeight) {
                            sprintf(batchp,"<9 0>%d^r%d^f%"_LD_"^w%f^h%"_LD_"^d%s</9>",coljds,jocc,freq,peso/* /freq */,hidx,h);
                            batchp+=strlen(batchp);
                        }
                    }
                    if (awtdatabuff[0]) {
                        if (fldupdat(outirec,awtdatabuff)) fatal("wtrig2/similardb/collapse/fldupdat");
                        if (*docydb) recupdat(outcrec,outirec);
                    }

                    if (replyhx) {
                        sprintf(reply,"</jdlist>\n");
                        reply+=strlen(reply);
                    }
                    
              } // end if jdjoin

              //trailer
              sprintf(reply,"</request>\n");
              reply+=strlen(reply); 

           }  // end if text1= and/or text=

