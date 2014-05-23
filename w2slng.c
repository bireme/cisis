#if PARMLANG
        if (parmlanguage) {
            int xlang;
            langidx=(-1);
            recfield(buff,irec,parmlanguage,1,"");
            if (parmchklang) if (!*buff) {
                printf("*** docmfn=%"_LD_" tag=%d\n",docmfn,parmlanguage);
                fatal("wtrig2/lang/missing");
            }
            for (p=buff; *p; ) { *p=isisuctab[*p]; p++; }
            for (xlang=0; xlang < nlangs; xlang++) {
                if (strcmp(buff,langsname[xlang]) == 0) { langidx=xlang; break; }
            }
            if (parmchklang) if (langidx < 0) if (*buff) {
                printf("*** docmfn=%"_LD_" tag=%d data=%s\n",docmfn,parmlanguage,buff);
                for (xlang=0; xlang < nlangs; xlang++) printf("*** idx=%d/%d name=%s\n",xlang,nlangs,langsname[xlang]);
                fatal("wtrig2/lang/invalid");
            }
        }
#endif /* PARMLANG */
