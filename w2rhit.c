            /* read term's postings from invertedmf and OR them (mfns) into HIT
            */
            if (*colxdb) {
                RECORD(ifrec,colxdb,dochidx+1);
            } /* end if colxdb */
            //if (RECrc != RCNORMAL) fatal("wtrig2/colxdb/deleted");
            if (*colxdb) if (RECrc == RCNORMAL) {
                LONGX nmfns=0;
                for (dirp2=MFRdir, xdir2=0, loop=MFRnvf; loop--; dirp2++, xdir2++) {
                    if (dirp2->tag != TAG33) continue;
                    for (keylen2=0, p=fldp2=FIELDP(xdir2), left=dirp2->len; left; ) {
                        if (*p == '^') {
                            if (left < 2 ) fatal("wtrig2/colxdb/nhits/left");
                            if (*++p != 'n') fatal("wtrig2/colxdb/nhits/left");
                            left-=2;
                            while (left) {
                                if (!isdigit(*++p)) break;
                                nmfns=nmfns*10+((int)(*p)-(int)'0');
                                left--;
                            }
                            break;
                        }
                        keylen2++; p++; left--;
                    }
                }
                if (keylen2 != keylen) fatal("wtrig2/colxdb/keylen");
                if (memcmp(fldarea,fldp2,keylen)) fatal("wtrig2/colxdb/key");

                if (parmtrace & 0x04) printf(" 4|%"_LD_"|%s|%"_LD_"|%f\n",docmfn,table+dochidx*hwidth,MFRmfn,weight);

                /* loop through term's postings and OR the mfn's bit */
                if (alwaysmakemfnhits) {
                    int n=sizeof(hitmfn);
                    for (dirp2=MFRdir, xdir2=0, loop=MFRnvf; loop--; dirp2++, xdir2++) {
                        if (dirp2->tag != TAG5) continue;
                        left=dirp2->len;
                        if (left < n*nmfns) fatal("wtrig2/colxdb/bin/left/1");
                        for (p=FIELDP(xdir2); left; ) {
                            memmove((void *)&hitmfn,p,n);
                            p+=n; left-=n;
                            if (left < 0) fatal("wtrig2/colxdb/bin/left/2");
                            if (hitmfn < 1) fatal("wtrig2/colxdb/bin/mfn invalid/1");
                            if (hitmfn > COLLECTION_SIZE) {
                                fatal("wtrig2/colxdb/bin/mfn invalid/2");
                            }
                            hitmask =hitbitmask[hitmfn&BY8RMASK];
                            hitoffset = hitmfn/8;
#if PARMLANG
                            /* filter language hits */
                            if (langidx >= 0) {
                                unsigned char c = *(langshits[langidx]+hitoffset);
                                if (c & hitmask)
                                    HITS[hitoffset] |= hitmask;
                            }
                            else
#endif /* PARMLANG */
                            HITS[hitoffset] |= hitmask;
                            if (parmtrace & 0x08) printf(" 8|%"_LD_"|%s|%"_LD_"|%"_LD_"|%d/%d/%d|%d/%"_LD_"\n",docmfn,table+dochidx*hwidth,hitmfn,MFRmfn,loop,xdir,left,n,nmfns);
                        }
                        break;
                    } /* end term postings */
                }
            } /* end if RECrc == RCNORMAL */

            recp=keeprecp; /* restore current recp */

            /* end read term's postings from invertedmf and OR them (mfns) into HIT */

