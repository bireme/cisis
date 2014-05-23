
    /* setup
    */    
    collectiondb=NULL;

    /* get parameters - col */
    for (iarg=1; iarg < argc; iarg++) {
        p=argv[iarg];
        if (strncmp(p,"collection?",11) == 0 || strncmp(p,"collection=",11) == 0) {
            collectiondb=p+11;
            if (!*collectiondb) { sprintf(reply,"+++ %s\n",p); return -1; }
            continue;
        }
        if (strncmp(p,"c?",2) == 0 || strncmp(p,"c=",2) == 0) {
            collectiondb=p+2;
            if (!*collectiondb) { sprintf(reply,"+++ %s\n",p); return -1; }
            continue;
        }               
        if (strncmp(p,"col.c?",6) == 0 || strncmp(p,"col.c=",6) == 0) {
            q=p+6; if (!*q) { sprintf(reply,"+++ %s\n",p); return -1; }
            strcpy(colcdb,q);
            continue;
        }
        if (strncmp(p,"col.n?",6) == 0 || strncmp(p,"col.n=",6) == 0) {
            q=p+6; if (!*q) { sprintf(reply,"+++ %s\n",p); return -1; }
            strcpy(colndb,q);
            continue;
        }
        if (strncmp(p,"col.v?",6) == 0 || strncmp(p,"col.v=",6) == 0) {
            q=p+6; if (!*q) { sprintf(reply,"+++ %s\n",p); return -1; }
            strcpy(colvdb,q);
            continue;
        }
        if (strncmp(p,"col.x?",6) == 0 || strncmp(p,"col.x=",6) == 0) {
            q=p+6; if (!*q) { sprintf(reply,"+++ %s\n",p); return -1; }
            strcpy(colxdb,q);
            continue;
        }
        if (strncmp(p,"cmfn?",5) == 0 || strncmp(p,"cmfn=",5) == 0) {
            if (sscanf(p+5,"%d",&parmcollmfntag) != 1) { sprintf(reply,"+++ %s\n",p); return -1; }
            if (parmcollmfntag < 0) { sprintf(reply,"+++ %s\n",p); return -1; }
            continue;
        }
        if (strncmp(p,"dlang?",6) == 0 || strncmp(p,"dlang=",6) == 0) {
            if (sscanf(p+6,"%d",&parmlanguage) != 1) { sprintf(reply,"+++ %s\n",p); return -1; }
            if (parmlanguage < 1) { sprintf(reply,"+++ %s\n",p); return -1; }
            continue;
        }
    }
