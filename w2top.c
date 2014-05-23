/* ==----------------------------- w2top.c -----------------------------== */

int w2topjd (int cmd, WTFUN_ARRAY *awtfp, int coll, LONGX *xFREQ, float *xPESO, int howmany, float minweight)
{
        WTFUN_XWT *xwtp;//=awtfp->vwtfp[coll];

        //int parmtrace=cmd;
        int parmtrace2=(cmd >= 3)? cmd : 0;
        LONGX docmfn=cmd;

        LISTJD *hdr,*tail,*avail,*l;
        LISTJD *prev;
        float nextw;

        float soma2;
        //float invsqrts2;
        float weight,termweight;

        LONGX hidx;
        LONGX *xfreqp;
        int jocc;
        LONGX loop;


        //WTFUN_XWT current
        xwtp=awtfp->vwtfp[coll];


        /* setup list of relevant terms
        */
        memset(listjd,0x00,howmany*sizeof(LISTJD));
        for (l=listjd, avail=listjd+1, loop=0; (LONGX)loop<(howmany-1); loop++, l++, avail++) l->avail=avail;
        hdr=tail=avail=listjd;
        listjdhdr=NULL;

        soma2=0;
        for (xfreqp=xFREQ, hidx=0, jocc=0, loop=jdmaxprim; loop--; xfreqp++, hidx++) {

            /* select */
            if (!*xfreqp) continue;
            weight=xPESO[hidx];
            /* optimize processing */
            if (weight < minweight) continue;
            jocc++; soma2+=weight*weight;
            if (parmtrace2) {
                char *jdhp=jdtable+(hidx*jdhwidth);
                fprintf(stderr,"jdtop %d? %"_LD_" %f %"_LD_":%s\n",jocc,*xfreqp,weight,hidx,jdhp);
            }

            /* store in ordered list */
                termweight = weight; /* * invsqrts2 */
                prev=NULL;
                l=NULL; if (hdr->weight) l=hdr;
                while (l) {
                    if (termweight < l->weight) { prev=l; l=l->next; }
                    else break;
                }
                nextw=((l)?l->weight:0.0);
                if (avail) {
                        LISTJD *nextavail=avail->avail;
                        avail->weight=termweight; avail->freq= *xfreqp; avail->hidx=hidx;
                        avail->prev=prev;
                        avail->next=l;
                        if (prev) prev->next=avail;
                        if (nextw) l->prev=avail;
                        if (!avail->prev) hdr=avail;
                        if (!avail->next) tail=avail;
                        avail=nextavail;
                        if (parmtrace2) fprintf(stderr,"top64a|");
                }
                else {
                    if (termweight > tail->weight) {
                        LISTJD *last=tail;
                        LISTJD *repl=last;
                        LISTJD *lnext=NULL;
                        if (l) lnext=l->next;
                        if (last->prev) {
                            tail=last->prev;
                            tail->next=NULL;
                        }
                        repl->weight=termweight; repl->freq= *xfreqp; repl->hidx=hidx;
                        repl->prev=prev;
                        if (prev) prev->next=repl;
                        if (l) if (lnext) {
                            repl->next=l;
                            l->prev=repl;
                        }
                        if (!repl->prev) hdr=repl;
                        if (!repl->next) tail=repl;
                        if (parmtrace2) fprintf(stderr,"top64b|");
                    }
                }
                if (parmtrace2) {
                  char *jdhp=jdtable+(hidx*jdhwidth);
                  fprintf(stderr,"%"_LD_"|%"_LD_":%s|%f\n",docmfn,hidx,jdhp,termweight);
                  for (l=hdr; l; l=l->next) {
                    char *jdhp;
                    if (!l) break;
                    jdhp=jdtable+(l->hidx*jdhwidth);
                    fprintf(stderr,"%"_LD_"|%"_LD_":%s|%f p=%"_LD_"|n=%"_LD_"|a=%"_LD_" hdr=%"_LD_"|lst=%"_LD_"|prev=%"_LD_"\n",docmfn,l->hidx,jdhp,l->weight,
                      (l->prev)?((LISTJD *)(l->prev))->hidx:-1,
                      (l->next)?((LISTJD *)(l->next))->hidx:-1,
                      (l->avail)?((LISTJD *)(l->avail))->hidx:-1,
                      (hdr->hidx)?hdr->hidx:-1,
                      (tail->hidx)?tail->hidx:-1,
                      (prev)?prev->hidx:-1);
                  }
                  fprintf(stderr,"\n");
                }

            /* store in ordered list */
        }
        
        listjdhdr=hdr;

        /* skip empty vectors */
        if (!soma2) {
            return 0;
        }

#if NORMALIZE
        /* calculate normalization factor = sqrt of sum of squares
        */
	    if (soma2) invsqrts2=1/sqrt(soma2);

        /* calculate final weight = local x global x normalization...
        */
        if (normalize)
            if (normalize) {
                LISTJD *l;
            } /* end */
        }
#endif

        /* doc done
        */
        //ffprintf(stderr,stderr,"++ %"_LD_"+%"_LD_"\n",zcount,zcount2);

        return jocc;

} /* end of topjd */

