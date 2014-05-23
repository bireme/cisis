
    if (argc < 2) {
        //wtsyntax("  ");
        sprintf(reply+strlen(reply),"%s",cicopyr("Utility WTRIG2"));
        sprintf(reply+strlen(reply)," \n");
#if SYNTAX
	    sprintf(reply+strlen(reply),"wtrig2 d[ocuments]=<mf1> c[ollection]=<mf2> \n");
        sprintf(reply+strlen(reply)," \n");
        sprintf(reply+strlen(reply)," input document term vectors from <mf1>.v \n");
        sprintf(reply+strlen(reply)," input collection term vectors from <mf2>.v \n");
        sprintf(reply+strlen(reply)," output similar collection for each input document to <mf1>.y \n");
        sprintf(reply+strlen(reply)," \n");
        sprintf(reply+strlen(reply)," options: maxrel=<max #related>                               [%"_LD_"]   \n",PARMMAXREL);
        sprintf(reply+strlen(reply),"          minsim=<min similarity for related docs>            [%3.1f] \n",PARMMINSIM);
        sprintf(reply+strlen(reply),"          maxsim=<max similarity for related docs>            [%3.1f] \n",PARMMAXSIM);
        sprintf(reply+strlen(reply),"          dmfn=<tag>   collapse doc.v as field <mf1>.v/<tag>  [%s]    \n",(parmcollapse)?itoa(parmcollapse,xa,10):"");
        sprintf(reply+strlen(reply),"          cmfn=<tag>   collapse col.v as field <mf2>.v/<tag>  [%s]    \n",(parmcollmfntag)?itoa(parmcollmfntag,xa,10):"");
        sprintf(reply+strlen(reply),"          dlang=<tag>  restrict col.v as field <mf1>.v/<tag>  [%s]    \n",(parmlanguage)?itoa(parmlanguage,xa,10):"");
        sprintf(reply+strlen(reply)," \n");
        sprintf(reply+strlen(reply),"          doc.v=<mf>   term vector for each document          [document.v]   \n");
        sprintf(reply+strlen(reply),"          col.v=<mf>   term vector for each collection doc    [collection.v] \n");
        sprintf(reply+strlen(reply),"          col.c=<mf>   collection parms                       [collection.c] \n");
        sprintf(reply+strlen(reply),"          col.n=<mf>   terms in collection                    [collection.n] \n");
        sprintf(reply+strlen(reply),"          col.x=<mf>   posting list for each term             [collection.x] \n");
        sprintf(reply+strlen(reply)," \n");
        sprintf(reply+strlen(reply),"          out.y=<mf>   similar collection for each document   [document.y] \n");
        sprintf(reply+strlen(reply)," \n");
        //sprintf(reply+strlen(reply),"          w2all        weight also terms outside collection   [%s]    \n",(parmwsoma)?"w2all":"");
        sprintf(reply+strlen(reply),"          -w2all       ignore doc terms outside collection    [%s] \n",(parmwsoma)?"":"-w2all");
        sprintf(reply+strlen(reply),"          [maxtv=<n>]  trace[={1|2|4|8|16|32|64|3..127}]      [%d] \n",parmtrace);
        sprintf(reply+strlen(reply)," \n");
#endif
	    return -1;
    }


    /* get parameters */
    for (iarg=1; iarg < argc; iarg++) {
        p=argv[iarg];
        if (parmtrace) sprintf(reply+strlen(reply),"+++ %s\n",p);
        if (strncmp(p,"tell?",5) == 0 || strncmp(p,"tell=",5) == 0) {
            if (sscanf(p+5,"%"_LD_,&parmtell) != 1)
                { sprintf(reply,"+++ %s\n",p); return -1; }
            parmtell2=parmtell;
            continue;
        }
        if (strncmp(p,"tell2?",6) == 0 || strncmp(p,"tell2=",6) == 0) {
            if (sscanf(p+6,"%"_LD_,&parmtell2) != 1)
                { sprintf(reply,"+++ %s\n",p); return -1; }
            continue;
        }
        if (strncmp(p,"trace?",6) == 0 || strncmp(p,"trace=",6) == 0) {
            if (sscanf(p+6,"%d",&parmtrace) != 1)
                { sprintf(reply,"+++ %s\n",p); return -1; }
            continue;
        }
        if (strcmp(p,"trace") == 0) {
            parmtrace2=1;
            parmtrace=127;
            continue;
        }
        if (strcmp(p,"trace1") == 0) {
            parmtrace2=0;
            parmtrace=1;
            continue;
        }
    }
