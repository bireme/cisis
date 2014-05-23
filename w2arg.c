
    if (parmstring)
      for (/*argv[argc++]="wtrig2",*/ p=parmstring; *p; ) {
        while (isspace(*p)) p++;
        if (argc >= MAXPRMS1) return -1;
        argv[argc++]=p;
        if (strncmp(p,"text=",5)  == 0) break;
        while (*p) if (isspace(*p)) { *p++='\0'; break; } else p++;
      }
    argv[argc]=NULL;
