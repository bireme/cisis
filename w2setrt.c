        if (!parmcollapse || !currentmfncollapsed) {
            LISTA *l;
            memset(lista,0x00,parmmaxrel*sizeof(LISTA));
            for (l=lista, listavail=lista+1, loop=0; (LONGX)loop<(parmmaxrel-1); loop++, l++, listavail++)
                l->avail=listavail;
            listhdr=listtail=listavail=lista;
        }
        /* init lastmfncollapse */
        if (parmcollapse) if (!currentmfncollapsed) {
            RECSTRU *recp;
            /* get output mfn, from tag parmcollapse of <mf1>.v */
            currentmfncollapsed=atol(recfield(awtdatabuff,irec,parmcollapse,1,""));
            if (currentmfncollapsed < 1) fatal("wtrig2/docvdb/mfn");
            /* init output collapsed results record */
            RECORD(outirec,dbdocydb,currentmfncollapsed); MFRstatus=ACTIVE; //RECsetup
        }
