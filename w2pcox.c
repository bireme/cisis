
    /* setup
    */    
    // in w2set.c, colcdb read

    /* get parameters - cox */
    for (iarg=1; iarg < argc; iarg++) {
        int xint;
        float xfloat;
        p=argv[iarg];
        
        if (strncmp(p,"cmfn?",5) == 0 || strncmp(p,"cmfn=",5) == 0) {
            if (sscanf(p+5,"%d",&xint)   != 1) continue;       // { sprintf(reply,"+++ %s\n",p); return -1; }
            if (xint < 1)                      continue;       // { sprintf(reply,"+++ %s\n",p); return -1; }
            parmcollmfntag=xint;
            continue;
        }

        if (strncmp(p,"maxrf?",6) == 0 || strncmp(p,"maxrf=",6) == 0) {
            if (sscanf(p+6,"%f",&xfloat) != 1) continue;       // { sprintf(reply,"+++ %s\n",p); return -1; }
            if (xfloat <= 0.0 || xfloat > 1.0) continue;       // { sprintf(reply,"+++ %s\n",p); return -1; }
            parmmaxrf=xfloat;
            continue;
        }
        if (strncmp(p,"maxtv?",6) == 0 || strncmp(p,"maxtv=",6) == 0) {
            if (sscanf(p+6,"%d",&xint)   != 1) continue;       // { sprintf(reply,"+++ %s\n",p); return -1; }
            if (xint < 1) continue;                            // { sprintf(reply,"+++ %s\n",p); return -1; }
            parmmaxtv=xint;
            continue;
        }
                
        if (strncmp(p,"maxrel?",7) == 0 || strncmp(p,"maxrel=",7) == 0) {
            if (sscanf(p+7,"%d",&xint)   != 1) continue;       // { sprintf(reply,"+++ %s\n",p); return -1; }
            if (xint < 1)                      continue;       // { sprintf(reply,"+++ %s\n",p); return -1; }
            parmmaxrel=xint;
            continue;
        }
        if (strncmp(p,"maxsim?",7) == 0 || strncmp(p,"maxsim=",7) == 0) {
            if (sscanf(p+7,"%f",&xfloat) != 1) continue;       // { sprintf(reply,"+++ %s\n",p); return -1; }
            if (xfloat <= 0.0 || xfloat > 1.0) continue;       // { sprintf(reply,"+++ %s\n",p); return -1; }
            parmmaxsim=xfloat;
            continue;
        }
        if (strncmp(p,"minsim?",7) == 0 || strncmp(p,"minsim=",7) == 0) {
            if (sscanf(p+7,"%f",&xfloat) != 1) continue;       // { sprintf(reply,"+++ %s\n",p); return -1; }
            if (xfloat <= 0.0 || xfloat > 1.0) continue;       // { sprintf(reply,"+++ %s\n",p); return -1; }
            parmminsim=xfloat;
            continue;
        }        

    } // end for
