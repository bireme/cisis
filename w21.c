/* ==----------------------------- w21.c -----------------------------== */


int loadtrig (int cmd, WTFUN_ARRAY *awtfp, int coll, char *text, int textlen, float parmmaxrf )
{
        WTFUN_XWT *xwtp;//=awtfp->vwtfp[coll];

        RECSTRU *recp;
        LONGX docmfn=1L;
        
        int LIST1A_DOCVEC=1;

    LONGX hidx;
    char *h;
    int found;

    FFI loop;
    DIRSTRU *dirp;
    int xdir;
    char *fldp;
    int len;

    int parmtrace=cmd;
    int parmtrace2=cmd;
    int parmdebug=0;
    int parmdebug2=0;

    LONGX tell=0;


        LONGX countok=0;
        LONGX xcount=0;
        LONGX ycount=0;

        LIST1A *hdr,*tail,*avail,*l;

        LONGX zcount,zcount2;
        int *docump;

        float soma2,fator,invsqrts2;
        float *prodp;


        //WTFUN_XWT current
        xwtp=awtfp->vwtfp[coll];

        /* add text
        */
        RECORD(zirec,"null",1L); /*MFRstatus=ACTIVE;*/
        sprintf(trigbuff,"D*H1 %"_LD_" ",(LONGX)textlen);
        len=strlen(trigbuff);
        memmove(trigbuff+len,text,textlen);
        trigbuff[len+textlen]='\0';
        if (fldupdat(zirec,trigbuff)) fatal("loadtrig/proc add field");
        // prtleader(recp,1L);
        if (parmdebug) prtfields(recp,1L);

        /* extract via Gsplit=tag={trigrams|words}
        */
        strcpy(trigbuff,"Gsplit=1=trigrams");
        //printf("+1+ ntrms=%"_LD_"\n",ntrms);
        if (fldupdat(zirec,trigbuff)) fatal("loadtrig/Gsplit=1=trigrams");
        //printf("+2+ ntrms=%"_LD_"\n",ntrms);
        // prtleader(recp,1L);
        if (parmdebug) prtfields(recp,1L);
        

        /* setup list of relevant terms
        */
        memset(list1a,0x00,readmaxtv*sizeof(LIST1A));
        for (l=list1a, avail=list1a+1, loop=0; (LONGX)loop<(readmaxtv-1); loop++, l++, avail++) l->avail=avail;
        hdr=tail=avail=list1a;
        zcount=zcount2=0;

        /* zero and build f(t)
        */
        memset(DOCUMENT,0x00,readnterms*sizeof(int));
        for (dirp=MFRdir, xdir=0, loop=MFRnvf; loop--; dirp++, xdir++) {
            //FFI ucloop;
            //unsigned char *p;
            if (dirp->tag != 1) continue;
            fldp=FIELDP(xdir);
            /* convert words to upper case */
            //if (!trigrams) if (parmuppercase) for (p=(unsigned char *)fldp, ucloop=dirp->len; ucloop--; p++) *p=isisuctab[*p];
            
            /* search term in collection via bsrchindex */
            hidx=bsrchindex(table,tabentries,readwidth,fldp,dirp->len,&found);
            if (parmdebug) {
                printf(" .|%"_LD_"|%d|%d|",docmfn,xdir,dirp->len);
                for (len=0; (FFI)len<dirp->len; len++) printf("%c",fldp[len]);
                printf("|%d|%"_LD_"\n",found,hidx);   
            }
            
            /* skip absent terms in collection */
            if (!found) continue;
#if 1 //DISCART
            /* optimize term processing */
            if (parmmaxrf != 1.0) {
                if ((float)COLLECTION[hidx]/COLLECTION_SIZE > parmmaxrf) continue;
            }
#endif
            /* count term */
            DOCUMENT[hidx]++;
        }

        /* calculate localweight x globalweight
        */
        memset(PRODW,0x00,readnterms*sizeof(float));
        soma2=0;
        for (docump=DOCUMENT, h=table, hidx=0; hidx < tabentries; hidx++, h+=hwidth, docump++) {
            if (*h) if (*docump) {
                  double logarg = (double)(1 + *docump);
                  float localweight;        /* l(t) - trigram local weight */
                  localweight = (float)log(logarg);
                  PRODW[hidx] = localweight*GLOBALW[hidx];
                  fator=PRODW[hidx];
                  soma2+=fator*fator;
                  if (parmtrace & 0x08) printf(" 8|%"_LD_"|%s|%d\n",docmfn,h,*docump);
                  if (parmtrace & 0x10) printf("16|%"_LD_"|%s|%f\n",docmfn,h,localweight);
                  if (parmtrace & 0x20) printf("32|%"_LD_"|%s|%f\n",docmfn,h,PRODW[hidx]);
            }
        }

        /* calculate normalization factor = sqrt of sum of squares
        */
	    if (soma2) invsqrts2=1/sqrt(soma2);
        if (parmtrace & 0x40) printf("64|%"_LD_"|    |%f|%f\n",docmfn,soma2,invsqrts2);

        /* skip empty vectors */
        if (!soma2) {
            ycount++;
//            continue;
            return 0;
        }

        /* calculate final term weight = local x global x normalization
        */
        for (prodp=PRODW, h=table, hidx=0; hidx < readnterms; hidx++, h+=hwidth, prodp++) {
            if (*h) if (*prodp) {
                LIST1A *l,*prev;
                float nextw;
                float prodw= *prodp;
                float termweight;         /* w(t) - trigram normalized weight */
                termweight = prodw * invsqrts2;
                prev=NULL;
                l=NULL; if (hdr->weight) l=hdr;
                while (l) {
                    if (termweight < l->weight) { prev=l; l=l->next; }
                    else break;
                }
                nextw=((l)?l->weight:0.0);
                if (avail) {
                        LIST1A *nextavail=avail->avail;
                        avail->weight=termweight; avail->hidx=hidx;
                        avail->prev=prev;
                        avail->next=l;
                        if (prev) prev->next=avail;
                        if (nextw) l->prev=avail;
                        if (!avail->prev) hdr=avail;
                        if (!avail->next) tail=avail;
                        avail=nextavail;
                        if (parmtrace2) printf("64a|");
                        zcount++;
                }
                else {
                    if (termweight > tail->weight) {
                        LIST1A *last=tail;
                        LIST1A *repl=last;
                        LIST1A *lnext=NULL;
                        if (l) lnext=l->next;
                        if (last->prev) {
                            tail=last->prev;
                            tail->next=NULL;
                        }
                        repl->weight=termweight; repl->hidx=hidx;
                        repl->prev=prev;
                        if (prev) prev->next=repl;
                        if (l) if (lnext) {
                            repl->next=l;
                            l->prev=repl;
                        }
                        if (!repl->prev) hdr=repl;
                        if (!repl->next) tail=repl;
                        if (parmtrace2) printf("64b|");
                        zcount2++;
                    }
                }
                if (parmtrace2) {
                  printf("%"_LD_"|%s|%f\n",docmfn,h,termweight);
                  for (l=hdr; l; l=l->next) {
                    char *p;
                    if (!l) break;
                    p=table+(l->hidx*hwidth);
                    printf("%"_LD_"|%"_LD_"=%s|%f p=%"_LD_"|n=%"_LD_"|a=%"_LD_" hdr=%"_LD_"|lst=%"_LD_"|prev=%"_LD_"\n",docmfn,l->hidx,p,l->weight,
                      (l->prev)?((LIST1A *)(l->prev))->hidx:-1,
                      (l->next)?((LIST1A *)(l->next))->hidx:-1,
                      (l->avail)?((LIST1A *)(l->avail))->hidx:-1,
                      (hdr->hidx)?hdr->hidx:-1,
                      (tail->hidx)?tail->hidx:-1,
                      (prev)?prev->hidx:-1);
                  }
                  printf("\n");
                }
            } /* end if h */
        } /* end for hidx */

if (parmdebug2) printf("+++ 1 +++|\n");
        /*  copy term vector to DOCVEC structure (w2rdoc.c compliance)
        */
        if (LIST1A_DOCVEC) {
            DOCVEC *dv=docvector;
            docvectcnt=0;
            for (l=hdr; l; l=l->next) {
                if (!l->weight) break;
                hidx=l->hidx; h=table+hidx*hwidth;            
                /* list term vector */
                if (parmtrace & 0x40) printf("64|%"_LD_"|%s|%f\n",docmfn,h,l->weight);
                /* copy term vector */
                dv->weight=l->weight;
                dv->hidx=l->hidx;
                dv->foundp=table+dv->hidx*hwidth;
                dv->keylen=strlen(dv->foundp);
                memmove(dv->key,dv->foundp,dv->keylen); dv->key[dv->keylen]='\0';
                docvectcnt++; dv++; 
            }
        }
if (parmdebug2) printf("+++ 2 +++|\n");

        /* doc done
        */
        countok++;
        if (tell) if (countok%tell == 0) fprintf(stderr,"++ %"_LD_"+%"_LD_" %"_LD_" %"_LD_"+%"_LD_"\n",countok,xcount,ycount,zcount,zcount2);

        return 1;

} /* end of loadtrig */

