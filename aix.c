#define AIX             1
#if AIX
        /* 1.2 avoid AIX behaviour [fatal: dbxopen/fd=0] */
   int fdAux;
   fdAux = open(argv[0],0);
#endif
 
 
