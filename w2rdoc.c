
        DOCVEC *dv=docvector;
        LONGX cnt=0;

        if (parmtext) {         /* build term vector from text */

            loadtrig (parmtrace, awtfp, coll, parmtext, strlen(parmtext), parmmaxrf );

        }
        else {                  /* read term vector */

            RECORD(inirec,docvdb,docmfn);
            if (RECrc == RCEOF) break;
            if (RECrc != RCNORMAL) continue;
    /*
            if (fldupdat(inirec,"Gsplit=4=wtrig")) fatal("wtrig2/docvdb/Gsplit");
    */
            for (dirp=MFRdir, xdir=0, loop=MFRnvf; loop--; dirp++, xdir++) {
                if (dirp->tag != TAG4) continue;
                left=dirp->len;
                fldp=FIELDP(xdir);
                if (alwaysrdocwritebin) {
                     float b2aweight;
                     unsigned short int usi_w;
                     while (left) {
                        if (cnt >= readmaxtv) break;
                        if (left < sizeof(usi_w)+1) fatal("wtrig2/loadvector/b2a/left/bin");
                        memmove((void *)&usi_w,fldp,sizeof(usi_w));
                        fldp+=sizeof(usi_w); left-=sizeof(usi_w);
                        b2aweight=(float)usi_w/(float)USHRT_MAX;
                        p=fldp; keylen=strlen(p);
                        dv->hidx=bsrchindex(table,tabentries,readwidth,p,keylen,&found);
                        dv->foundp=NULL; if (found) dv->foundp=table+dv->hidx*hwidth;
                        memmove(dv->key,p,keylen); dv->key[keylen]='\0'; dv->keylen=keylen;
                        while (left) { q=fldp; fldp++; left--; if (!*q) break; }
                        dv->weight=b2aweight;
                        dv++; cnt++;
                    }
                    break;
                }
            }
            docvectcnt=cnt;

        }

