            if (simil >= parmminsim) if (simil <= parmmaxsim) {
              if (parmcollapse) { /* collapse */
                if (simil > collapsesim1) {
                    collapsesim1=simil;
                    collapsemfn1=collhitmfn;
                    collapsehit1=hitmfn;
                    if (parmtrace & 0x40) printf("64|%"_LD_"|%"_LD_"|%"_LD_"|%f\n",docmfn,collhitmfn,hitmfn,simil);
                }
              }
              else { /* not collapse */
                /* mantain list of related */
                LISTA *l=NULL;
                LISTA *prev=NULL;
                LONGX nextm=0; /* flag */

                if (listhdr->mfn) l=listhdr;
                while (l) {
                    if (simil < l->sim) { prev=l; l=l->next; }
                    else break;
                }
                nextm=((l)?l->mfn:0);
                if (listavail) {
                        LISTA *newl=listavail;
                        listavail=listavail->avail;
                        newl->sim=simil; newl->mfn=collhitmfn; /**/newl->xxx=docmfn;/**/ newl->hit=hitmfn;
                        newl->prev=prev;
                        newl->next=l;
                        if (prev) prev->next=newl;
                        if (nextm) l->prev=newl;
                        if (!newl->prev) listhdr=newl;
                        if (!newl->next) listtail=newl;
                        if (parmtrace2) printf("64a|");
                }
                else {
                    if (simil > listtail->sim) {
                        LISTA *last=listtail;
                        LISTA *repl=last;
                        LISTA *lnext=NULL;
                        if (l) lnext=l->next;
                        if (last->prev) {
                            listtail=last->prev;
                            listtail->next=NULL;
                        }
                        repl->sim=simil; repl->mfn=collhitmfn; /**/repl->xxx=docmfn;/**/ repl->hit=hitmfn;
                        repl->prev=prev;
                        if (prev) prev->next=repl;
                        if (l) if (lnext) {
                            repl->next=l;
                            l->prev=repl;
                        }
                        if (!repl->prev) listhdr=repl;
                        if (!repl->next) listtail=repl;
                        if (parmtrace2) printf("64b|");
                    }
                }
                if (parmtrace2) {
                  for (l=listhdr; l; l=l->next) {
                    if (!l) break;
                    //printf("%"_LD_"|%"_LD_"|%f  p=%"_LD_"|n=%"_LD_"|a=%"_LD_"  hdr=%"_LD_" lst=%"_LD_" prev=%"_LD_"\n",docmfn,l->mfn,l->sim,
                    printf("%"_LD_"|%"_LD_"|%"_LD_"|%f  p=%"_LD_"|n=%"_LD_"|a=%"_LD_"  hdr=%"_LD_" lst=%"_LD_"\n",docmfn,l->mfn,l->hit,l->sim,
                      (l->prev)?((LISTA *)(l->prev))->mfn:-1,
                      (l->next)?((LISTA *)(l->next))->mfn:-1,
                      (l->avail)?((LISTA *)(l->avail))->mfn:-1,
                      (listhdr->mfn)?listhdr->mfn:-1,
                      (listtail->mfn)?listtail->mfn:-1);
                      //,(prev)?prev->mfn:-1);
                  }
                }
                if (parmtrace & 0x40) printf("64|%"_LD_"|%"_LD_"|%"_LD_"|%f\n",docmfn,collhitmfn,hitmfn,simil);
              } /* end collapse */
            } /* end if sim>=<= */
