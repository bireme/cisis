            RECSTRU *keeprecp=recp;
            float weight;
            DIRSTRU *dirp2;
            int xdir2;
            FFI keylen2;
            char *fldp2;
            char *fldarea;

            keylen=docv->keylen;
            weight=docv->weight;

            /* convert term key to index: dochidx */
            dochidx=docv->hidx;

            /* keep fldp */
            found=1; fldarea=docv->foundp;
            if (!docv->foundp) { found=0; fldarea=docv->key; }

            if (parmdebug) printf("64xx|%"_LD_"|%d|%"_LD_"|%s|%d\n",docmfn,found,dochidx,fldarea,keylen);

/**/
