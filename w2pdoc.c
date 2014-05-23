
    /* setup
    */    
    parmdocumentsdb=NULL;
    *docvdb    ='\0';
    *docydb    ='\0';

    
    jk_n=0;   //init eval/jk/jd parms
    jkv_n=0;  //init eval/jkv/jdv parms

    /* get parameters -  doc */
    for (iarg=1; iarg < argc; iarg++) {
        p=argv[iarg];
        if (strncmp(p,"documents?",10) == 0 || strncmp(p,"documents=",10) == 0) {
            parmdocumentsdb=p+10;
            if (!*parmdocumentsdb) { sprintf(reply,"+++ %s\n",p); return -1; }
            continue;
        }
        if (strncmp(p,"d?",2) == 0 || strncmp(p,"d=",2) == 0) {
            parmdocumentsdb=p+2;
            if (!*parmdocumentsdb) { sprintf(reply,"+++ %s\n",p); return -1; }
            continue;
        }
        if (strncmp(p,"text?",5) == 0 || strncmp(p,"text=",5) == 0) {
            parmtext=p+5;
            if (!*parmtext) { sprintf(reply,"+++ %s\n",p); return -1; }
            continue;
        }
        if (strncmp(p,"show?",5) == 0 || strncmp(p,"show=",5) == 0) {
            parmdisp=p+5;
            //returns jv_n or 0
            if (w2docjv (cmd, awtfp, coll, parmdisp) < 0) { sprintf(reply,"+++ %s\n",p); return -1; }
            continue;
        }
        if (strncmp(p,"eval?",5) == 0 || strncmp(p,"eval=",5) == 0) {
            parmeval=p+5;
            //returns jk_n or 0
            if (w2docjk (cmd, awtfp, coll, parmeval) < 0) { sprintf(reply,"+++ %s\n",p); return -1; }
            continue;
        }
        if (strncmp(p,"jd?",3) == 0 || strncmp(p,"jd=",3) == 0) {
            joinjd=atoi(p+3);
            if (joinjd < 0) { sprintf(reply,"+++ %s\n",p); return -1; }
            continue;
        }
        if (strncmp(p,"v?",2) == 0 || strncmp(p,"v=",2) == 0) {
            showv3=atoi(p+2);
            if (showv3 < 0) { sprintf(reply,"+++ %s\n",p); return -1; }
            continue;
        }
        if (strncmp(p,"id?",3) == 0 || strncmp(p,"id=",3) == 0) {
            showid=atoi(p+3);
            if (showid < 0) { sprintf(reply,"+++ %s\n",p); return -1; }
            continue;
        }
        if (strncmp(p,"doc.v?",6) == 0 || strncmp(p,"doc.v=",6) == 0) {
            q=p+6; if (!*q) { sprintf(reply,"+++ %s\n",p); return -1; }
            strcpy(docvdb,q);
            continue;
        }
        if (strncmp(p,"out.y?",6) == 0 || strncmp(p,"out.y=",6) == 0) {
            q=p+6; if (!*q) { sprintf(reply,"+++ %s\n",p); return -1; }
            strcpy(docydb,q);
            continue;
        }
        if (strncmp(p,"dmfn?",5) == 0 || strncmp(p,"dmfn=",5) == 0) {
            if (sscanf(p+5,"%d",&parmcollapse) != 1) { sprintf(reply,"+++ %s\n",p); return -1; }
            if (parmcollapse < 0) { sprintf(reply,"+++ %s\n",p); return -1; }
            continue;
        }
        if (strcmp(p,"w2all") == 0) {
            parmwsoma=1;
            continue;
        }
        if (strcmp(p,"-w2all") == 0) {
            parmwsoma=0;
            continue;
        }
    }
