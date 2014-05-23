/* ------------------------------ serw.c ---------------------------------- */


/* Copyright (c) 1990 - 1994 Adalberto Otranto Tardelli. All rights reserved.
 * Written by A.O.Tardelli 7/90
 * Redistribution and use in source and binary forms are freely permitted
 * provided that the above copyright notice and attribution and date of work
 * and this paragraph are duplicated in all such forms.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 * This program performs a MicroISIS master file dump, formatted display,
 * search and record limits, pattern changes, data base join, field update,
 * master file update and ISO-2709 processing. Input can also be a plain file.
 * A description of this program is available in CISIS Interface Application -
 * MX Program. Sao Paulo, BIREME - Latin American and Caribbean Center on
 * Health Sciences Information/PAHO-WHO, April 1991. 12p. See also:
 * - http://www.bireme.br (IsisFamily/CisisUtilities)
 * - ftp://ftp.bireme.br/cisis
 * Currently works with CISIS Interface 4.01 and BorlandC 5.02 or
 * MSVC 6.0 for win32/console application, and for Linux/gcc, HPUX/CC and
 * Sun/CC, among other platforms.  AOT, 21/02/2001.
*/

/*
..Server program that makes a connection for a byte stream socket in the Internet namespace.
*/
/*
*/

#define P000          1                               //1

#ifndef CISIS_H
#define GEN_SERW      1
#define GEN_MAIN      1
#define GEN_MAINLOAD  00                              //00
#endif

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "cisis.h"      /* CISIS Interface header file */
#include "cirun.h"      /* CISIS Interface runtime */

#if GEN_MAIN
//     #include <errno.h>
//     #include <stdlib.h>
#if P000 && PGCC
     #include <unistd.h>
#endif /* P000 */
     #include <sys/types.h>
#endif /*  GEN_MAIN */

#if GEN_MAIN
     #define PORT          1417
     #define BUFFERSIZE   51200
#endif /*  GEN_MAIN */

     #define REPLYSIZE    51200


#if P000
     int mainserver_make_socket (int cmd, uint16_t port);
     int mainserver_read_from_client (int cmd, uint16_t port, int filedes, char *buffer, int buffersize);
     int mainserver_write_to_client (int cmd, uint16_t port, int filedes, char *message, int messagesize);
     int mainserver (int cmd, WTFUN_ARRAY *awtfp, uint16_t port, char *buffer, int buffersize, int maxconnqueued);

/*
*/
     int
     mainserver_make_socket (int cmd, uint16_t port)
     {
       int sock;
#if PGCC
       struct sockaddr_in name;

       /* Create the socket. */
       sock = socket (PF_INET, SOCK_STREAM, 0);
       if (sock < 0)
         {
           /*perror ("socket"); exit (EXIT_FAILURE);*/
           return -1;
         }

       /* Give the socket a name. */
       name.sin_family = AF_INET;
       name.sin_port = htons (port);
       name.sin_addr.s_addr = htonl (INADDR_ANY);
       if (bind (sock, (struct sockaddr *) &name, sizeof (name)) < 0)
         {
           /*perror ("bind"); exit (EXIT_FAILURE);*/
           return -2;
         }
       return sock;
#else
       sock=1; return sock;
#endif
     }


     int
     mainserver_write_to_client (int cmd, uint16_t port, int filedes, char *message, int messagesize)
     {
       int nbytes;
                              /* yyyymmdd hh:mm:ss WDAY YDAY */
       char timestamp[80];    /* 123456789012345678901234 7  8 901 */
       time_t secs_now;
       struct tm *tp;
       time(&secs_now); tp=localtime(&secs_now);
       sprintf(timestamp,"%04d%02d%02d %02d%02d%02d %1d %3d", 1900+tp->tm_year,tp->tm_mon+1,tp->tm_mday, tp->tm_hour,tp->tm_min,tp->tm_sec, tp->tm_wday,tp->tm_yday);

#if GEN_LOGS
       if (cmd == 4) sprintf (logbuff, "Server [%d] [%d] [%s]: reply message (%d bytes): '%s' \n", port, filedes, timestamp, messagesize, message);
#endif
       if (cmd == 3) fprintf (stderr, "Server [%d] [%d] [%s]: reply message (%d bytes): '%s' \n", port, filedes, timestamp, messagesize, message);
       if (cmd == 2) fprintf (stderr, "Server [%d] [%d] [%s]: reply message (%d bytes) \n", port, filedes, timestamp, messagesize);

#if PGCC
       nbytes = send (filedes, message, messagesize, 0);// send (int socket, void *buffer, size_t size, int flags)
       if (nbytes < messagesize)
         {
           /* Write error */
           /*perror ("write");*/
           return -1;
         }
       else
         return nbytes;
#else
       nbytes=1; return(nbytes);
#endif
     }


     int
     mainserver_read_from_client (int cmd, uint16_t port, int filedes, char *buffer, int buffersize)
     {
       int nbytes;

#if PGCC
       buffer[0]='\0';
       nbytes = recv (filedes, buffer, buffersize-1, 0); // recv (int socket, void *buffer, size_t size, int flags)
       if (nbytes < 0)
         {
           /* Read error. */
           /*perror ("read"); exit (EXIT_FAILURE);*/
           return -2;
         }
       else
         {
#if TRACER
           char *p=buffer;
           int loop=nbytes;
           fprintf (stderr, "read(%d): ",nbytes);
           for (; loop--; p++) fprintf (stderr, "%c=%02x ",*p,(int)*p);
           fprintf (stderr, "\n");
#endif
           /* EOF (nbytes=0) or Data read. */
           buffer[nbytes]='\0';
           return nbytes;
         }
#else
       buffer[0]='x'; buffer[1]='\0';
       nbytes=1; return(nbytes);
#endif
     }


/*
     mainserver
*/
     int
     mainserver (int cmd, WTFUN_ARRAY *awtfp, uint16_t port, char *buffer, int buffersize, int maxconnqueued)
     {
       int sock;
#if PGCC
       fd_set active_fd_set, read_fd_set;
       struct sockaddr_in clientname;
#endif
       int i;
       size_t size;
       int shutdown=0; /* run */
                              /* yyyymmdd hh:mm:ss WDAY YDAY */
       char timestamp[80];    /* 123456789012345678901234 7  8 901 */
       time_t secs_in;
       time_t secs_now;
       struct tm *tp;

#if GEN_SERW
       char wreply[REPLYSIZE];
       int coll;
#endif

#if GEN_LOGS
        RECSTRU *recp;
        LONGX logcrec;
        LONGX logirec;
        char logsdb[CIMPL];
        
        /* log processing 
        */
        strcpy(logsdb,"serw.log");
                
        if (!ndbxs) dbxinit();  /* init vdbxp/vrecp/vtrmp if not yet init - AOT, 28/10/2005 */
                
        /* alloc///logsdb ctl */
        for (logcrec=maxnrec; logcrec--; ) { if (!vrecp[logcrec]) /* ja' decrementado */ break; }
        if (logcrec<0L) fatal("wtrig2/logcrec");
        recallok(logcrec,sizeof(M0STRU)); 
        
        /* alloc///logsdb */
        for (logirec=maxnrec; logirec--; ) { if (!vrecp[logirec]) /* ja' decrementado */ break; }
        if (logirec<0L) fatal("wtrig2/logirec");
        recallok(logirec,MAXMFRL);  // RECsetup in w2setrt.c / w2logx.c + w2log1.c
    
        RECORD(logcrec,logsdb,0);
        RECORD(logirec,logsdb,MF0nxtmfn); 
        RECrc=RCNORMAL; MFRstatus=ACTIVE;
#endif
    
       /* Create the socket and set it up to accept connections. */
       sock = mainserver_make_socket (cmd, port);

       time(&secs_now); tp=localtime(&secs_now); secs_in=secs_now;
       sprintf(timestamp,"%04d%02d%02d %02d%02d%02d %1d %3d", 1900+tp->tm_year,tp->tm_mon+1,tp->tm_mday, tp->tm_hour,tp->tm_min,tp->tm_sec, tp->tm_wday,tp->tm_yday);

#if GEN_LOGS
       if (cmd) { sprintf (logbuff, "<101 0>Server [%d] [%d] [%s]: got socket</1>", port, sock, timestamp); if (fldupdat(logirec,logbuff)) fatal("wtrig2/fldupdat/1"); }
#endif
       if (cmd == 3) fprintf (stderr, "Server [%d] [%d] [%s]: got socket \n", port, sock, timestamp);

#if PGCC
       if (sock < 0)   /* <== */
         {
           /*perror ("sock"); exit (EXIT_FAILURE);*/
           return -1;
         }
       if (listen (sock, maxconnqueued) < 0)   /* <== at most 1 connection queued */
         {
           /*perror ("listen"); exit (EXIT_FAILURE);*/
           return -1;
         }

       /* Initialize the set of active sockets. */
       FD_ZERO (&active_fd_set);
       FD_SET (sock, &active_fd_set);
#else
       i=sock;
#endif

       awtfp->globalport=(int)port;       /* validade collection in w2.c */        
       awtfp->globalsock=sock;            /* sock/fd */

       while (!shutdown)
         {
#if PGCC
           /* Block until input arrives on one or more active sockets. */
           read_fd_set = active_fd_set;
           if (select (FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0)
             {
               /*perror ("select"); exit (EXIT_FAILURE);*/
               return -2;
             }
           /* Service all the sockets with input pending. */
           for (i = 0; i < FD_SETSIZE; ++i) {
             if (FD_ISSET (i, &read_fd_set))
               {
                 if (i == sock)
                   {
                     /* Connection request on original socket. */
                     int new;
                     size = sizeof (clientname);
                     new = accept (sock,
                                   (struct sockaddr *) &clientname,
                                   &size);
                     if (new < 0)
                       {
                         /*perror ("accept"); exit (EXIT_FAILURE);*/
                         return -3;
                       }
#if GEN_LOGS
                     if (cmd) sprintf (logbuff, "<102 0>Server [%d] [%d] [%s]: connect from host %s, port %hd</1>", port, sock, timestamp, inet_ntoa (clientname.sin_addr), ntohs (clientname.sin_port)); if (fldupdat(logirec,logbuff)) fatal("wtrig2/fldupdat/2"); }
#endif
                     if (cmd == 3) fprintf (stderr, "Server [%d] [%d] [%s]: connect from host %s, port %hd \n", port, sock, timestamp, inet_ntoa (clientname.sin_addr), ntohs (clientname.sin_port));
                     FD_SET (new, &active_fd_set);
                   }
                 else
                   {
#endif
                     /* Data arriving on an already-connected socket. */
                     int nread = mainserver_read_from_client (cmd, port, i, buffer, buffersize);
                     int code;
                     char replybuf[REPLYSIZE];
                     char *reply=replybuf;
                     replybuf[0]='\0';

                     time(&secs_now); tp=localtime(&secs_now);
                     sprintf(timestamp,"%04d%02d%02d %02d%02d%02d %1d %3d", 1900+tp->tm_year,tp->tm_mon+1,tp->tm_mday, tp->tm_hour,tp->tm_min,tp->tm_sec, tp->tm_wday,tp->tm_yday);

#if GEN_LOGS
                     if (cmd) sprintf (logbuff, "<103 0>Server [%d] [%d] [%s]: got message (%d bytes)^v%s</1>", port, sock, timestamp, nread, buffer); if (fldupdat(logirec,logbuff)) fatal("wtrig2/fldupdat/3"); }
#endif
                     if (cmd == 3) fprintf (stderr, "Server [%d] [%d] [%s]: got message (%d bytes): '%s' \n", port, sock, timestamp, nread, buffer);

                     if (strlen(buffer) == 0)              {         reply="serw 100 eof msg\n"; /* never */          code=100; }
#if GEN_SERW
                     if (strncmp(buffer,"wtrig2 ",7) == 0) {
                                                             coll = loadcoll(cmd, awtfp, buffer, wreply);
                                                             if (coll < 0) code=400; else code=200;
                                                             //sprintf(replybuf,"serw %d coll#%d [%s]\n",code,coll,wreply);
                                                             sprintf(replybuf,"%s\n",wreply);
                     }
                     else
#endif /* GEN_SERW */
                     if (strcmp(buffer,"\n") == 0)         {         reply="serw 101 null msg\n";                     code=101; }
                     else
                     if (strncmp(buffer,"shutdown",8) == 0)   {         reply="serw 200 thanks\n";                       code=0;   shutdown=1; }
                     else
                     if (strncmp(buffer,"hello",5) == 0)      {         reply="serw 200 hi\n";                           code=200; }
                     else
                     if (strncmp(buffer,"echo ",5) == 0)   { sprintf(replybuf,"serw 200 %s\n",buffer+5);  code=200; }
#define GEN_HTTP 1
#if GEN_HTTP
                     else
                     if (strncmp(buffer,"GET /wtrig2+",12) == 0)      { 
                         int http=0;
                         int lumlen;
                         char *p;
                         char *lfp=strchr(buffer,'\r');
                         if (!lfp) lfp=strchr(buffer,'\n');
                         if (lfp) {
                             char k = *lfp;
                             *lfp='\0';
                             lumlen=strlen(buffer);  // 123456789012
                             if (lumlen>=12) {       // X / HTTP/1.x
                                 p=lfp-9;
                                 if (strcmp(p," HTTP/1.0") == 0) http=10; else
                                 if (strcmp(p," HTTP/1.1") == 0) http=11;
                             }
                             //*lfp = k;
                         }
#if 0
if (http)
sprintf(replybuf,"000 OK\nDate: Sun, 09 Feb 2003 08:08:53 GMT\nServer: Acme HTTP Server 0.01 alpha\nContent-Length: 92\nContent-Type: text/xml\nLast-Modified: Thu, 08 Nov 2001 22:12:35 GMT\n\n<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?><greeting>hello world and Pré-Diabetes</greeting>");
else
#endif
                         if (http) {
                             *p='\0';
                             for (p=buffer+5; *p; p++) {
                                 if (*p == '+') *p=' '; else
                                 if (*p == '&') *p=' ';
                             }
                                                             coll = loadcoll(cmd, awtfp, buffer+5, wreply);
                                                             if (coll < 0) code=400; else code=200;
                                                             //sprintf(replybuf,"serw %d coll#%d [%s]\n",code,coll,wreply);
//                                                             sprintf(replybuf,
//"HTTP/1.0 %d OK\nDate: Fri, 16 Aug 1996 11:48:52 GMT\nServer: Apache/1.1.1 UKWeb/1.0\nLast-modified: Fri, 09 Aug 1996 14:21:40 GMT\nContent-length: %6d\nContent-type: text/xml; charset=\"ISO-8859-1\"\n\n<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n<yourrequest>%s</yourrequest>\n" ,code,strlen(wreply)+134+22,wreply);
                                                             sprintf(replybuf,
"HTTP/1.0 %d OK\nDate: Fri, 16 Aug 1996 11:48:52 GMT\nServer: Apache/1.1.1 UKWeb/1.0\nLast-modified: Fri, 09 Aug 1996 14:21:40 GMT\nContent-length: %6d\nContent-type: text/xml\n\n<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n<yourrequest>%s</yourrequest>\n" ,code,strlen(wreply),wreply);
                         } 
                         else
                                                           {         reply="serw 404 invalid msg\n";                  code=400; }
                     }
#else/* GEN_HTTP */
                     else
                     if (strcmp(buffer+nread-1,"\n") != 0) {         reply="serw 505 bad msg\n";                      code=500; }
                     else
                                                           {         reply="serw 404 invalid msg\n";                  code=400; }
#endif /* GEN_HTTP */

                     /* Response */
#if PGCC
                     if (strlen(reply) > 0) {
                       int nbytes=strlen(reply);
                       nbytes = mainserver_write_to_client (cmd, port, i, reply, nbytes);
                       if (nbytes < 0)
                         {
                           /*perror ("write (client)");*/
                           close (i);
                           FD_CLR (i, &active_fd_set);
                           /* return -4; */
                         }
#if GEN_LOGS
                     time(&secs_now); tp=localtime(&secs_now);
                     sprintf(timestamp,"%04d%02d%02d %02d%02d%02d %1d %3d", 1900+tp->tm_year,tp->tm_mon+1,tp->tm_mday, tp->tm_hour,tp->tm_min,tp->tm_sec, tp->tm_wday,tp->tm_yday);
                                                             if (cmd) sprintf (logbuff, "<104 0>Server reply [%d] [%d] [%s] code=%d coll#%d (%d bytes)^v%s</1>", port, sock, timestamp, code, coll, strlen(reply), reply); if (fldupdat(logirec,logbuff)) fatal("wtrig2/fldupdat/4"); }
                                                             
#endif
                     }

                     close (i);
                     FD_CLR (i, &active_fd_set);

                   } /* end data read */

               } /* end if FD_ISSET */
#endif

               if (shutdown) break;

 #if PGCC
           } /* end for FD_SETSIZE */
 #endif
         } /* end while */

         /* Release the socket. */
 #if PGCC
         close (sock);
 #endif

         time(&secs_now); tp=localtime(&secs_now);
         sprintf(timestamp,"%04d%02d%02d %02d%02d%02d %1d %3d", 1900+tp->tm_year,tp->tm_mon+1,tp->tm_mday, tp->tm_hour,tp->tm_min,tp->tm_sec, tp->tm_wday,tp->tm_yday);

#if GEN_LOGS
         if (cmd) sprintf (logbuff, "<105 0>Server [%d] [%d] [%s]: close socket (%"_LD_" seconds)</1>", port, sock, timestamp,(LONGX)secs_now-secs_in); if (fldupdat(logirec,logbuff)) fatal("wtrig2/fldupdat/5"); }
#endif
         if (cmd == 3) fprintf (stderr, "Server [%d] [%d] [%s]: close socket (%"_LD_" seconds) \n", port, sock, timestamp,(LONGX)secs_now-secs_in);

         return 0;

     } /* end of mainserver */


#endif /* P000 */




/* Main

./serw 3
./serw 3 2>>xlog &

*/

#if GEN_MAIN
     int
     main(int argc, char *argv[])
     {
       /* calling parameters */
       int cmd=0;
       int rc;
       int i;

       uint16_t port=PORT;
       char *cipfile="serw.cip";

       int buffsize=MAXMFRL*2;
       char *buffer;

       WTFUN_ARRAY *awtfp=NULL;

       if (argc == 1) {
           printf("%s",cicopyr("Utility SERW"));
           printf("\n");
           //printf("serw [tell/debug] [cipar=<file>] [port=1417|<port>] \"msg[|]\" \"msg[|]\".. \n");
           printf("serw [tell/debug] [cipar=<file>] [port=1417|<port>]");
#if GEN_MAINLOAD
           printf(" \"msg[|]\" \"msg[|]\"..");
#endif
           printf(" \n");
           printf("\n");
           exit (EXIT_FAILURE);
       }

       /* working area */
       buffer=(char *)loadfile(NULL,'@',"",NULL, buffsize, '\0');
       if (!buffer) {
            fprintf(stderr, "serw: exit_failure buffer=%d bytes\n", buffsize);
            exit (EXIT_FAILURE);
       }

       /* setup or loadcoll()
       */
       for (i=1; i < argc; i++) {
            char *p=argv[i];
            if (strcmp(p,"tell") == 0) {
                cmd=1;
                continue;
            }
            if (strcmp(p,"debug") == 0) {
                cmd=3;
                continue;
            }
            if (strncmp(p,"cipar=",6) == 0) {
                cipfile=p+6;
                continue;
            }
            if (strncmp(p,"port=",5) == 0) {
                LONGX xval;
                if (sscanf(p+5,"%"_LD_,&xval) != 1) fatal(p);
                port=(uint16_t)xval;
                continue;
            }
#if GEN_MAINLOAD
            if (p) {
                char *p;

                if (!ciawtfp) if (dbxciset(cipfile)) {
                    fprintf(stderr, "loadcoll failure: cipar=%s \n",cipfile);
                    exit (EXIT_FAILURE);
                }

                for (p=argv[i]; *p; p++) if (*p=='|') *p='\n'; // test text=..|..

                rc = loadcoll(cmd, ciawtfp, argv[i], buffer);
                if (rc < 0) { 
                    if (cmd >= 3) fprintf(stderr, "loadcoll failure %d \n", rc);
                    exit (EXIT_FAILURE);
                }
                else printf("%s",buffer);
           }
#else /*  GEN_MAINLOAD */
           fatal(p);
#endif /*  GEN_MAINLOAD */
       }
    
#if PGCC
       if (!ciawtfp) if (dbxciset(cipfile)) {
           fprintf(stderr, "serw/loadcoll failure: cipar \n");
           exit (EXIT_FAILURE);
       }
       awtfp=ciawtfp;

       // server
       rc=(-1);
       if (awtfp) {
           if (cmd >= 3) fprintf(stderr, "serw: %d \n",port);
           rc = mainserver(cmd, awtfp, port, buffer, buffsize, 1);
           //free awtfp
       }
#endif //PGCC
       
       //free buffer

       if (rc < 0) {
           if (cmd >= 3) fprintf(stderr, "serw: exit_failure %d \n", rc);
           exit (EXIT_FAILURE);
       }

       if (cmd >= 3) fprintf(stderr, "serw: exit_success \n");
       exit (EXIT_SUCCESS);

       return 1;
     }
#endif /*  GEN_MAIN */

//#undef P000
//#undef GEN_MAINLOAD
//#undef GEN_MAIN

