

    if (*docydb || parmtext) if (parmcollapse) {
        LISTA *l;
        char *batchp=awtdatabuff;

        if (parmdebug) printf("64X|%s\n",docydb);
            
        awtdatabuff[0]='\0';
        for (yocc=0, l=listhdr; l; l=l->next) {
            if (!l->mfn) break;
            yocc++;

            if (parmdebug) printf("64Z|%"_LD_"^s%f^m%"_LD_"^h%"_LD_"\n",l->mfn,l->sim,l->xxx,l->hit);

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
    }

    if (replyhere) {
        sprintf(reply,"</similarlist>\n");
        reply+=strlen(reply);
    }

