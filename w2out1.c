        
        if (replyhere) {
            sprintf(reply,"<similarlist>\n");
            reply+=strlen(reply);
        }
            
        if (*docydb || parmtext) {
            RECSTRU *recp;
            LISTA *l;
            char *batchp=awtdatabuff;
            awtdatabuff[0]='\0';
            
            if (parmdebug) printf("64x|%s=%s|\n",docydb,dbdocydb);
            
            if (parmcollapse) { /* collapse */
                LONGX mfncollapse;
                /* get output mfn, from tag parmcollapse of <mf1>.v */
                mfncollapse=atol(recfield(awtdatabuff,inirec,parmcollapse,1,""));
                if (mfncollapse < currentmfncollapsed) fatal("wtrig2/similardb/termvector/mfn");
                /* collapse results */
                if (mfncollapse == currentmfncollapsed) if (collapsemfn1) {
                    /* mantain list of related */
                    LISTA *l=listhdr;
                    LISTA *prev=NULL;
                    while (l) {
                        if (collapsemfn1 == l->mfn) {
                            if (collapsesim1 > l->sim) {
                                /* remove from list */
                                LISTA *next=l->next;
                                LISTA *prev=l->prev;
                                if (prev) prev->next=l->next;
                                if (next) next->prev=prev;
                                l->sim=0.0; l->mfn=0; l->xxx=0; l->hit=0;
                                if (!prev) /* deletando cabeca de lista */
                                    if (next) listhdr=next; else listhdr=lista;
                                if (!next) /* deletando fim de lista */
                                    if (prev) listtail=prev; else listtail=lista;
                                l->avail=listavail; listavail=l; /* reuse it */
                                l=NULL; /* force add */
                            }
                            if (parmtrace2) printf("64=|\n");
                            break; /* l-> will remain in list */
                        } else l=l->next;
                    }
                    if (!l) {
                      /* add to list */
                      if (listhdr->mfn) l=listhdr;
                      while (l) {
                           if (collapsesim1 < l->sim) { prev=l; l=l->next; }
                           else break;
                      }
                      if (listavail) {
                            LISTA *newl=listavail;
                            LONGX nextm; /* flag */
                            nextm=((l)?l->mfn:0);
                            listavail=listavail->avail;
                            newl->sim=collapsesim1; newl->mfn=collapsemfn1; newl->xxx=docmfn; newl->hit=collapsehit1;
                            newl->prev=prev;
                            newl->next=l;
                            if (prev) prev->next=newl;
                            if (nextm) l->prev=newl;
                            if (!newl->prev) listhdr=newl;
                            if (!newl->next) listtail=newl;
                            if (parmtrace2) printf("64A|\n");
                      }
                      else {
                        if (collapsesim1 > listtail->sim) {
                            LISTA *last=listtail;
                            LISTA *repl=last;
                            LISTA *lnext=NULL;
                            if (l) lnext=l->next;
                            if (last->prev) {
                                listtail=last->prev;
                                listtail->next=NULL;
                            }
                            repl->sim=collapsesim1; repl->mfn=collapsemfn1; repl->xxx=docmfn; repl->hit=collapsehit1;
                            repl->prev=prev;
                            if (prev) prev->next=repl;
                            if (l) if (lnext) {
                                repl->next=l;
                                l->prev=repl;
                            }
                            if (!repl->prev) listhdr=repl;
                            if (!repl->next) listtail=repl;
                            if (parmtrace2) printf("64B|\n");
                        }
                      }
                    } /* end if l */
                    if (parmtrace2) {
                      for (l=listhdr; l; l=l->next) {
                        if (!l) break;
                        //printf("  |%"_LD_"|%"_LD_"|%f|%"_LD_"  p=%"_LD_"|n=%"_LD_"|a=%"_LD_"  hdr=%"_LD_" lst=%"_LD_" prev=%"_LD_" \n",docmfn,l->mfn,l->sim,l->xxx,
                        printf("  |%"_LD_"|%"_LD_"|%f|%"_LD_"|%"_LD_"  p=%"_LD_"|n=%"_LD_"|a=%"_LD_"  hdr=%"_LD_" lst=%"_LD_"\n",docmfn,l->mfn,l->sim,l->xxx,l->hit,
                          (l->prev)?((LISTA *)(l->prev))->mfn:-1,
                          (l->next)?((LISTA *)(l->next))->mfn:-1,
                          (l->avail)?((LISTA *)(l->avail))->mfn:-1,
                          (listhdr->mfn)?listhdr->mfn:-1,
                          (listtail->mfn)?listtail->mfn:-1);
                          //,(prev)?prev->mfn:-1);
                      }
                    }
                    if (parmtrace & 0x40) printf("64:|%"_LD_"|%"_LD_"|%"_LD_"|%f\n",docmfn,collapsehit1,collapsemfn1,collapsesim1);
                }
                /* output collapsed results */
                if (mfncollapse != currentmfncollapsed) {
                    if (parmdebug) printf("64x|%s=%s|collapse \n",docydb,dbdocydb);
                    awtdatabuff[0]='\0';
                    for (yocc=0, l=listhdr; l; l=l->next) {
                        if (!l->mfn) break;
                        yocc++;
                        if (parmdebug) printf("64c|%"_LD_"^s%f^m%"_LD_"^h%"_LD_"\n",l->mfn,l->sim,l->xxx,l->hit);
                        
                        //reply..
                        if (replyhere) {
                            w2reply (cmd, awtfp, coll, yocc, l, joinjd, showv3, showid, reply, batchp );
                            reply+=strlen(reply);
                            batchp+=strlen(batchp);
                        }

                        sprintf(batchp,"<6 0>%"_LD_"^s%f^m%"_LD_"^h%"_LD_"</6>",l->mfn,l->sim,l->xxx,l->hit);
                        batchp+=strlen(batchp);
                    }
                    if (awtdatabuff[0]) {
                        if (fldupdat(outirec,awtdatabuff)) fatal("wtrig2/similardb/collapse/fldupdat");
                        if (*docydb) recupdat(outcrec,outirec);
                    }
                    /* re-init list of relevant terms */
                    memset(lista,0x00,parmmaxrel*sizeof(LISTA));
                    for (l=lista, listavail=lista+1, loop=0; (LONGX)loop<(parmmaxrel-1); loop++, l++, listavail++)
                        l->avail=listavail;
                    listhdr=listtail=listavail=lista;
                    /* set current collapsed output mfn */
                    currentmfncollapsed=mfncollapse;
                    /* re-init output collapsed results record */
                    RECORD(outirec,dbdocydb,mfncollapse); MFRstatus=ACTIVE; //RECsetup - setup1 in w2setrt.c
                }
            }
            else { /* not collapse */
                if (parmdebug) printf("64x|%s=%s|not collapse \n",docydb,dbdocydb);
                RECORD(outirec,dbdocydb,docmfn); MFRstatus=ACTIVE; //RECsetup
                for (yocc=0, l=listhdr; l; l=l->next) {
                    if (!l->mfn) break;
                    yocc++;
                    if (parmdebug) printf("64z|%"_LD_"^s%f^m%"_LD_"^h%"_LD_"\n",l->mfn,l->sim,l->xxx,l->hit);

                    //reply..
                    if (replyhere) {
                        w2reply (cmd, awtfp, coll, yocc, l, joinjd, showv3, showid, reply, batchp );
                        reply+=strlen(reply);
                        batchp+=strlen(batchp);
                    }
                    
                    sprintf(batchp,"<6 0>%"_LD_"^s%f^m%"_LD_"^h%"_LD_"</6>",l->mfn,l->sim,l->xxx,l->hit);
                    batchp+=strlen(batchp);
                }
                if (awtdatabuff[0]) {
                    if (fldupdat(outirec,awtdatabuff)) fatal("wtrig2/similardb/fldupdat");
                    if (*docydb) recupdat(outcrec,outirec);
                }
            }
        }
