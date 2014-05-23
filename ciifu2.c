#if LINDLUX /* 2y */

#define LUXPRINT0 0
#define LUXPRINT1 0
#define LUXPRINT2 0
#define LUXPRINT3 0

#if LUXPRINT0
    printf("\n");
#endif

if (invp->lvxiflind) {

    /* process .iyp */

    if (!invp->lvxpages) return;

    /* create .iyp file */
    fpccreat(NULL,DBXname,ix1extp,0);

#if DUPMFNSTRING
    /* check duplicated posting - MFNSTRING */
    for (luxtree=0; luxtree < 2; luxtree++) {

#if LUXPRINT0
    printf("+++lvx treecase=%d \n",luxtree);
#endif
        /* left to right */
        for (luxpunt=1; luxpunt <= invp->luxpages[luxtree]; ) {
            luxpagp=invp->luxvpagp[luxtree][luxpunt];
            if (luxtree) {
                luxp2=(L2STRU *)luxpagp;
                for (luxn=0; luxn < luxp2->ock; luxn++) {
                    luxp2->idx[luxn].info1 = luxaddr;
                    luxx=luxp2->idx[luxn].info2 * PMFNSIZ;
                    if (luxx > invp->luxthresh) {
                        luxx=invp->luxthresh;
                        luxp2->idx[luxn].info3info4.info3 = -luxx;
                    }
                    else
                        if (luxp2->idx[luxn].info3info4.info3)
                            fatal("ifupd2/info3");
                    luxaddr+=luxx;
                }
            }
            else {
                luxp1=(L1STRU *)luxpagp;
                for (luxn=0; luxn < luxp1->ock; luxn++) {
                    luxp1->idx[luxn].info1 = luxaddr;
                    luxx=luxp1->idx[luxn].info2 * PMFNSIZ;
                    if (luxx > invp->luxthresh) {
                        luxx=invp->luxthresh;
                        luxp1->idx[luxn].info3info4.info3 = -luxx;
                    }
                    else
                        if (luxp1->idx[luxn].info3info4.info3)
                            fatal("ifupd2/info3");
                    luxaddr+=luxx;
                }
            }

            /* get next leaf */
            luxpunt=luxpagp->ps;

            if (!luxpunt) break;

        } /* end for luxpunt */

    } /* end for luxtree */
#endif /* DUPMFNSTRING */

    /* write posting blocks */
    for (luxx=0; luxx < invp->lvxpages; luxx++) {
        luxp=(char *)invp->lvxvpagp[luxx];
        fpcwrite(luxp,LVXIYPBS);
    } /* end for luxx */

    /* end write posting blocks */
    fpcclose();

} else {  /* else if invp->lvxlind */

    /* process node's */

    for (luxtree=0; luxtree < 2; luxtree++) {

#if LUXPRINT0
    printf("+++ltx treecase=%d \n",luxtree);
#endif

        if (!invp->ltxpages[luxtree]) return;

        /* create .n0x file */
        fpccreat(NULL,DBXname,nx12extp[luxtree],0);

        /* write node's */
        for (luxpunt=1; luxpunt <= invp->ltxpages[luxtree]; luxpunt++) {
            luxx=(luxtree)?N2BSIZ:N1BSIZ;
            luxp=(char *)invp->ltxvpagp[luxtree][luxpunt];
            fpcwrite(luxp,luxx);
        } /* end for luxpunt */

        /* end write leaf's */
        fpcclose();

    } /* end for luxtree */


    /* process leaf's */

    luxaddr=0L;
    luxpagp=NULL;
    
    for (luxtree=0; luxtree < 2; luxtree++, luxpagp=NULL) {

#if LUXPRINT0
    printf("+++lux treecase=%d \n",luxtree);
#endif

#if LUXPRINT0
        printf("*%s\n",DBXname);
        printf("idtype ordn ordf   n   k  liv    ");
        printf("posrx nmaxpos  fmaxpos  abnormal\n");
        printf("%4d  %4d %4d %4d%4d",invp->cn[luxtree].idtype,
                                        invp->cn[luxtree].ordn,
                                        invp->cn[luxtree].ordf,
                                        invp->cn[luxtree].n,
                                        invp->cn[luxtree].k);
        printf("%4d %8"_LD_,invp->cn[luxtree].liv,invp->cn[luxtree].posrx);
        printf("%8"_LD_" %8"_LD_"    %4d\n",invp->cn[luxtree].nmaxpos,
                                    invp->cn[luxtree].fmaxpos,
                                    invp->cn[luxtree].abnormal);
#endif

        if (!invp->luxpages[luxtree]) return;

        /* create .l0x file */
        fpccreat(NULL,DBXname,lx12extp[luxtree],0);

        /* update psb - left to right */
        luxpos=0; luxx=0;
        for (luxpunt=1; luxpunt <= invp->luxpages[luxtree]; ) {
            luxpagp=invp->luxvpagp[luxtree][luxpunt];
            if (luxpagp->pos != luxpunt) fatal("ifupd2/pos");
            luxpagp->psb=luxpos; luxpos=luxpunt; luxpunt=luxpagp->ps; 
#if LUXPRINT1
printf("+++ps=%"_LD_"/%"_LD_" pos=%"_LD_" punt=%"_LD_" \n",luxpagp->psb,luxpagp->ps,
                                                        luxpos,luxpunt);
#endif
            luxx++;
            if (luxx == invp->luxpages[luxtree]) 
                if (luxpunt) fatal("ifupd2/ps");
            if (!luxpunt) break;
        } /* end for luxpunt */
        if (luxx != invp->luxpages[luxtree]) fatal("ifupd2/pages");

        /* update info1 - left to right */
        for (luxpunt=1; luxpunt <= invp->luxpages[luxtree]; ) {
            luxpagp=invp->luxvpagp[luxtree][luxpunt];
#if LUXPRINT2
printf("+++pos=%"_LD_" punt=%"_LD_" thresh=%"_LD_" \n",luxpagp->pos,luxpunt,
                                                        invp->luxthresh);
#endif
            if (luxtree) {
                luxp2=(L2STRU *)luxpagp;
                for (luxn=0; luxn < luxp2->ock; luxn++) {
#if LUXPRINT3
 printf(" %2d ",luxn+1);
 for (luxp=luxp2->idx[luxn].key, luxx=vlex[luxtree]; luxx--; luxp++)
  printf("%c",*luxp);
 printf(" %"_LD_,luxp2->idx[luxn].info1);
 printf("/%"_LD_,luxp2->idx[luxn].info2);
 printf("/%"_LD_,luxp2->idx[luxn].info3info4.info3);
 printf(" [%"_LD_"] \n",invp->luxthresh);
#endif
                    luxp2->idx[luxn].info1 = luxaddr;
                    luxx=luxp2->idx[luxn].info2 * PMFNSIZ;
                    if (luxx > invp->luxthresh) {
                        luxx=invp->luxthresh;
                        luxp2->idx[luxn].info3info4.info3 = -luxx;
                    }
                    else
                        if (luxp2->idx[luxn].info3info4.info3)
                            fatal("ifupd2/info3");
                    luxaddr+=luxx;
                }
            }
            else {
                luxp1=(L1STRU *)luxpagp;
                for (luxn=0; luxn < luxp1->ock; luxn++) {
#if LUXPRINT3
 printf("%2d ",luxn+1);
 for (luxp=luxp1->idx[luxn].key, luxx=vlex[luxtree]; luxx--; luxp++)
  printf("%c",*luxp);
 printf(" %"_LD_,luxp1->idx[luxn].info1);
 printf("/%"_LD_,luxp1->idx[luxn].info2);
 printf("/%"_LD_,luxp1->idx[luxn].info3info4.info3);
 printf(" [%"_LD_"] \n",invp->luxthresh);
#endif
                    luxp1->idx[luxn].info1 = luxaddr;
                    luxx=luxp1->idx[luxn].info2 * PMFNSIZ;
                    if (luxx > invp->luxthresh) {
                        luxx=invp->luxthresh;
                        luxp1->idx[luxn].info3info4.info3 = -luxx;
                    }
                    else
                        if (luxp1->idx[luxn].info3info4.info3)
                            fatal("ifupd2/info3");
                    luxaddr+=luxx;
                }
            }

            /* get next leaf */
            luxpunt=luxpagp->ps;

            if (!luxpunt) break;

        } /* end for luxpunt */

        /* write leaf's */
        for (luxpunt=1; luxpunt <= invp->luxpages[luxtree]; luxpunt++) {
            luxx=(luxtree)?L2BSIZ:L1BSIZ;
            luxp=(char *)invp->luxvpagp[luxtree][luxpunt];
            fpcwrite(luxp,luxx);
        } /* end for luxpunt */

        /* end write leaf's */
        fpcclose();

    } /* end for luxtree */

} /* end if invp->lvxlind */

#endif /* LINDLUX 2y */
