#if CICPP
     int RECSTRU :: mainfile(int cmd, char *filnamp, int parm0, int parm1, char *buffer, int buffersize)
#else /* CICPP */

     int mainfile(int cmd, char *filnamp, int parm0, int parm1, char *buffer, int buffersize);

     int
     mainfile(int cmd,
       char *filnamp,        /* HEAD|GET /docpath HTTP/1.0\r\n\r\n */
       int parm0,
       int parm1,
       char *buffer,         /* results */
       int buffersize        /* max size */
     )
#endif /* CICPP */
     {
            int hlen=0;
            char *q=buffer;
            LONGX nread;

            if (cmd >= 1) fprintf(stdout, "<message>%s</message><crstrip>%d</crstrip><lfstrip>%d</lfstrip>\n",filnamp,parm0,parm1);

            if (filnamp) {
                int fd;
                dbxopt_fatal=0; fd=dbxopen(NULL/*gidbnp*/,filnamp,"");
                if (fd <= 0) hlen=(-1);  /*fatal("fldupdat/procx/Gload/open");*/
                else {
                    nread=CIREAD(fd,buffer,buffersize-1);
                    CLOSE(fd);
                    if (nread > 0L) {
                        q+=nread; hlen+=nread;
                    }
                    if (parm0) if (nread > 0L) {
                        LONGX left=nread;
                        int found=0;
                        char *x,*y;
                        for (x=y=buffer; left; left--, x++)
                            if (*x=='\r') { found=1; hlen--; q--; }
                            else { if (found) *y= *x; y++; }
                    }
                    if (parm1) if (nread > 0L) {
                        LONGX left=nread;
                        int found=0;
                        char *x,*y;
                        for (x=y=buffer; left; left--, x++)
                            if (*x=='\n') { found=1; hlen--; q--; }
                            else { if (found) *y= *x; y++; }
                    }
                }
            }
            else {
                LONGX left=buffersize-1;
#if USEFGETS /* fgets looses last char if LF is missing in last line */
                int nread=(left>INT_MAX)?INT_MAX:left;
                char *linep;
                while ((linep=fgets(q,nread,stdin)) != NULL) {
                    int nread=strlen(linep);
                    if (parm1) if (nread) if (linep[nread-1]=='\n') nread--;
                    q+=nread; hlen+=nread; left-=nread;
                }
#else /* getchar() */
                int c;
                int nread;
                while (left && (c=getchar()) != EOF) {
                    nread=1;
                    if (parm1) if ((char)c=='\n') nread=0;
                    if (nread) { *q++=(unsigned char)c; hlen++; left--; }
                }
#endif
            }

            *q='\0';

            if (cmd == 3) fprintf (stderr, "File: got data(%d): '%s'\n", (int)hlen, buffer);
            if (cmd == 2) fprintf (stderr, "File: got data(%d) \n", (int)hlen);

            if (cmd >= 1) fprintf(stdout, "<content>%s</content>\n",buffer);

            return hlen;

     }
