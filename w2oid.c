
                 if (showid) {
                 
                      RECSTRU *recp;
                      int xdir,nvf;
                      int maxv3len=SHOWV3LEN;                      
                      int xtag=showid;                      
                                               
                      RECORD(irec,collectiondb,l->mfn); 
                      
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
                      
                  }  // end showid reply

