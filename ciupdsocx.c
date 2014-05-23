
/* ciupdsocx.c - included from ciupd.c
*/

/*
Client program that makes a connection for a byte stream socket in the Internet namespace.
Once it has connected to the server, it sends a text string to the server and gets the response.
Returns the response size or a negative error value.
*/
/*
16.9.6 Byte Stream Socket Example
Here is an example client program that makes a connection for a byte stream socket in the Internet namespace. It doesn't do anything particularly interesting once it has connected to the server; it just sends a text string to the server and exits.
This program uses init_sockaddr to set up the socket address; see Inet Example.
*/

#ifndef CISIS_H
#define GEN_MAIN 1
#endif

#if GEN_MAIN
     #include <stdio.h>
     #include <errno.h>
     #include <stdlib.h>
     #include <unistd.h>
     #include <sys/types.h>
     #include <stdint.h>
     #include <netinet/in.h>
     #include <netdb.h>
     #include <time.h>
#endif /*  GEN_MAIN */

#if GEN_MAIN
     #define PORT            80
     #define MESSAGE         "HEAD / HTTP/1.0\r\n\r\n"
     #define SERVERHOST      "www.bireme.br"
     #define PROTOCOL        "HTTP"
     #define PGCC 1
#endif /*  GEN_MAIN */

     #define BUFFERSIZE      51200

#if 0
       "HEAD /'message' HTTP/1.0\r\n\r\n"
       "GET / HTTP/1.0\nUser-Agent: Mozilla/3.0 (compatible; Opera/3.0; Windows 95/NT4)\nAccept: */*\nHost: birk105.studby.uio.no:81\n\n"
#endif

#if PGCC
     int init_sockaddr (struct sockaddr_in *name, const char *hostname, uint16_t port);
#endif
     int mainclient_connect_to_server (int *sock, struct sockaddr_in* addr);
     int mainclient_read_from_server (int cmd, int filedes, char *buffer, int buffersize, int maxrds);
     int mainclient_write_to_server (int cmd, int filedes, char *message, int messagesize);
   //int mainclient (int cmd, char *protocol, char *serverhost, uint16_t port, char *message, char *buffer, int buffersize, int maxrds); /* char *referrer, char *useragent, char *from */

     int sock_debug = 0;

/*
*/
#if PGCC
     int
     init_sockaddr (struct sockaddr_in *name,
                    const char *hostname,
                    uint16_t port)
     {
       struct hostent *hostinfo;

       if (sock_debug) {
           fprintf(stderr, "***DEBUG*** init_sockaddr: host=%s port=%d\n", hostname, (int)port);
       }

       name->sin_family = AF_INET;
       name->sin_port = htons (port);
       hostinfo = gethostbyname (hostname);
       if (hostinfo == NULL)
         {
           /*perror("unknown host");*/
           return -1;
         }
       name->sin_addr = *(struct in_addr *) hostinfo->h_addr;

       return 0;
     }
#endif

#define MAX_CONNECT_TIMES 10
     int
     mainclient_connect_to_server (int *sock, struct sockaddr_in* addr)
     {
        int times;
        int rc = -1;

        /* Connect to the server. */
        for (times = 1; times <= MAX_CONNECT_TIMES; times++) {
           rc = connect (*sock, (struct sockaddr *) addr, sizeof (*addr));
           if (rc >= 0) {
              break;
           }
           close(*sock);
           *sock = socket (PF_INET, SOCK_STREAM, 0);
           if (*sock < 0) {
              fprintf (stderr, "***ERROR*** mainclient_connect_to_server: family=%d type=%d protocol=%d ret=%d errno=%d\n",
                          PF_INET, SOCK_STREAM, 0, *sock, errno);
              break;
           }
        }

        if (rc < 0) {
           fprintf (stderr, "***ERROR*** mainclient_connect_to_server: socket=%d  addr=%p addrlen=%d ret=%d errno=%d try_times=%d\n",
                    *sock, (struct sockaddr *) addr, sizeof (*addr), rc, errno, times);
        /*} else if (times > 1) {*/
        } else if (times > 0) {
            if (sock_debug) {
                fprintf (stderr, "***DEBUG*** mainclient_connect_to_server: times=%d rc=%d\n", times, rc);
            }
        }

        return rc;
     }

     int
     mainclient_write_to_server (int cmd, int filedes, char *message, int messagesize)
     {
       int nbytes;

       if (sock_debug) {
           fprintf (stderr, "***DEBUG*** mainclient_write_to_server: send message(%d): [%s]\n", messagesize, message);
       }

       if (cmd == 3) fprintf (stderr, "Client: send message(%d): '%s'\n", messagesize, message);
       if (cmd == 2) fprintf (stderr, "Client: send message(%d) \n", messagesize);

       nbytes = write (filedes, message, messagesize);
       if (nbytes < messagesize)
         {
           /* Write error */
           /*perror ("write");*/
           fprintf (stderr, "***ERROR*** mainclient_write_to_server/write: socket=%d buf=[%s] buflen=%d ret=%d errno=%d\n",
                    filedes, message, messagesize, nbytes, errno);
           return -1;
         }
       else
         return nbytes;
     }

     int
     mainclient_read_from_server (int cmd, int filedes, char *buffer, int buffersize, int maxrds)
     {
       int nbytes;
       int more;

       buffer[0]='\0';
       nbytes = read (filedes, buffer, buffersize-1);
       if (nbytes < 0)
         {
           /* Read error */
           /*perror ("read");*/
           fprintf (stderr, "***ERROR*** mainclient_read_from_server/read1: socket=%d  buff=[%s] bufflen=%d ret=%d errno=%d\n",
                          filedes, buffer, buffersize-1, nbytes, errno);
           return -1;
         }
       else
         {
           /* Data read. */
           buffer[nbytes]='\0';
           if (cmd == 3) fprintf (stderr, "Client: got message(%d): '%s' \n", nbytes, buffer);
           if (cmd == 2) fprintf (stderr, "Client: got message(%d) \n", nbytes);

           if (sock_debug) {
               fprintf (stderr, "***DEBUG*** mainclient_read_from_server/1: got +message(%d) \n", more);
           }

           while (--maxrds)
            if ((more = read (filedes, buffer+nbytes, buffersize-1-nbytes)) > 0)
             {
               if (sock_debug) {
                   fprintf (stderr, "***DEBUG*** mainclient_read_from_server/2: got +message(%d) \n", more);
               }
               buffer[nbytes+more]='\0';
               if (cmd == 3) fprintf (stderr, "Client: got +message(%d): '%s' \n", more, buffer+nbytes);
               if (cmd == 2) fprintf (stderr, "Client: got +message(%d) \n", more);
               nbytes+=more;
             }
             else {
                 break;
             }

           if (more < 0) {
             fprintf (stderr, "***ERROR*** mainclient_read_from_server/read2: socket=%d  buff=[%s] bufflen=%d ret=%d errno=%d\n",
                          filedes, buffer+nbytes, buffersize-1-nbytes, more, errno);
             return -2;
           }

           return nbytes;
         }
     }


     int
     mainclient (int cmd,
       char *protocol,       /* HTTP */                            /* X1417 */
       char *serverhost,     /* www.bireme.br */                   /* serverofi.bireme.br */
       uint16_t port,        /* 80 */                              /* 1417 */
       char *message,        /* HEAD|GET /docpath HTTP/1.0\r\n\r\n */  /* wtrig2 c=dff maxrel=3 minsim=0.87 text=diabetes mellitus */
       char *buffer,         /* results */
       int buffersize,       /* max size */
       int maxrds            /* max reads */                       /* 1 */
     )
     {
       int sock;
#if PGCC
       struct sockaddr_in servername;
#endif

       int nbytes;
       int rc;
       char *p,*q;

       /* Trap null msg */
       if (!message)  /*return 0;*/  message="\\n"; //\n
       if (!*message) /*return 0;*/  message="\\n";

       if (cmd >= 1) fprintf(stdout, "<mainclient>\n");
       if (cmd >= 1) fprintf(stdout, "<message>%s</message><serverhost>%s</serverhost><port>%d</port>\n",message,serverhost,port);

       /* Convert calling \n to LF */
       for (p=buffer, q=message; *q; p++, q++)
         if (*q=='\\' && *(q+1)=='n') {
           //*p = '\n'; q++;     HB 20081010
           *p++ = '\r';*p = '\n'; q++;
         }
         else *p = *q;
       *p='\0';

#if !PGCC
#if CIWTF
       /* Fake output. */
       if (*buffer) {
         char *msgp=strdup(buffer);
         if (strcmp(protocol,"X1417") == 0) {
           //loadcoll
           rc = loadcoll(cmd, ciawtfp, /*message*/ msgp, buffer);
           if (rc < 0) {
             ///*perror ("read/loadcoll (client)");*/
             nbytes=(-5); *buffer='\0';
           }
           else nbytes=rc;
         }
         else
         if (strcmp(protocol,"HTTP") == 0) {
           sprintf(buffer,"%s","HTTP/1.1 200 OK\n...\nContent-Length: 14082\nConnection: close\nContent-Type: text/html\n\n");
           nbytes=strlen(buffer);
         }
#if CICPP
        delete[] msgp;
#else
        FREE(msgp);
#endif
       }
#endif //CIWTF
#else //!PGCC

       /* Create the socket. */
       sock = socket (PF_INET, SOCK_STREAM, 0);
       if (sock < 0)
         {
           /*perror ("socket (client)");*/
           fprintf (stderr, "***ERROR*** mainclient/socket: family=%d  type=%d protocol=%d ret=%d errno=%d\n",
                          PF_INET, SOCK_STREAM, 0, sock, errno);
           return -1;
         }

       /* Init (init_sockaddr). */
       rc = init_sockaddr (&servername, serverhost, port);
       if (rc < 0)
         {
           /*fprintf (stderr, "Unknown host %s.\n", hostname);*/
           fprintf (stderr, "***ERROR*** mainclient/init_sockaddr: name=%p  hostname=%s port=%u ret=%d errno=%d\n",
                          &servername, serverhost, port, rc, errno);
           close (sock);
           return -2;
         }

       /* Connect to the server. */
       /*
       rc = connect (sock, (struct sockaddr *) &servername, sizeof (servername));
       if (rc < 0)
         {
           fprintf (stderr, "***ERROR*** mainclient/sock: socket=%d  addr=%p addrlen=%d ret=%d errno=%d\n",
                          sock, (struct sockaddr *) &servername, sizeof (servername), rc, errno);
           close (sock);
           return -3;
         }
       */
       rc = mainclient_connect_to_server (&sock, &servername);
       if (rc < 0)
         {
           close (sock);
           return -3;
         }

       /* Send data to the server. */
       nbytes = mainclient_write_to_server (cmd, sock, /*message*/buffer, strlen(/*message*/buffer));
       if (nbytes < 0)
         {
           /*perror ("write (client)");*/
           close (sock);
           return -4;
         }

       /* Get data from the server. */
       nbytes = mainclient_read_from_server (cmd, sock, buffer, buffersize, maxrds);
       if (nbytes < 0)
         {
           /*perror ("read (client)");*/
           close (sock);
           return -5;
         }

       /* Release the socket. */
       rc = close (sock);
       if (rc < 0) {
           fprintf (stderr, "***ERROR*** mainclient/close: ret=%d errno=%d\n", rc, errno);
           return -6;
       }
       if (sock_debug) {
           fprintf (stderr, "***DEBUG*** mainclient/close\n");
       }

#endif //!PGCC

       /* parse 1st line
          HTTP/1.1 200 OK
          HTTP/1.1 3   Found
          HTTP/1.1 403 Forbidden
          HTTP/1.1 404 Not Found
          HTTP/1.1 501 Method Not Implemented
       */

       if (cmd >= 1) {
           fprintf(stdout, "<content>%s</content>\n",buffer);
           if (strncmp(buffer,"HTTP/",5) == 0) {
               for (p=buffer; *p; p++) if (*p==' ') break; *p++='\0';   rc=atoi(p);
               for (        ; *p; p++) if (*p==' ') break; *p++='\0';   q=p;
               for (        ; *p; p++) if (*p=='\r' || *p=='\n') break; *p='\0';
               fprintf(stdout, "<protocol>%s</protocol><code>%d</code><msg>%s</msg>\n",buffer,rc,q);
           }
           fprintf(stdout, "</mainclient>\n");
       }

       return nbytes;
     }


/* Main

./client4 0 "HEAD / HTTP/1.0\n\n"
./client4 1 "HEAD / HTTP/1.0\n\n"
./client4 2 "HEAD / HTTP/1.0\n\n"

./client4 1 "HEAD http://lis.bvs.br/xml2html/xmlListT.php?xml%5B%5D=http://lis.bvs.br/lis-Regional/P/define.xml&xsl=http://lis.bvs.br/lis-Regional/home.xsl HTTP/1.0\n\n" lis.bvs.br 80
./client4 0 "GET http://lis.bvs.br/xml2html/xmlListT.php?xml%5B%5D=http://lis.bvs.br/lis-Regional/P/define.xml&xsl=http://lis.bvs.br/lis-Regional/home.xsl HTTP/1.0\n\n" lis.bvs.br 80

*/


#if GEN_MAIN
     int
     main(int argc, char *argv[])
     {
       /* calling parameters */
       int cmd;
       char *protocol=PROTOCOL;
       char *message=MESSAGE;
       char *serverhost=SERVERHOST;
       uint16_t port=PORT;
       int maxrds=1;
       int rc;

       char buffer[BUFFERSIZE];

       if (argc == 1) {
          fprintf(stderr, "client4 0=plain|1=xml|2=tell|3=trace \"HEAD|GET docpath\\n\\n\" | \"wtrig2 c=col text=x\" [serverhost [port [maxreads [HTTP|X1417]] ] ] \n");
         exit (EXIT_FAILURE);
       }

       cmd=atoi(argv[1]);

       if (argc > 2) message=argv[2];
       if (argc > 3) serverhost=argv[3];
       if (argc > 4) port=(uint16_t)atoi(argv[4]);
       if (argc > 5) maxrds=atoi(argv[5]);
       if (argc > 6) protocol=argv[6];

       if (cmd >= 3) fprintf(stderr, "client4: \"%s\" %s %d \n",message,serverhost,port);

       rc=mainclient(cmd, protocol, serverhost, (uint16_t)port, message, buffer, BUFFERSIZE, maxrds);

       if (rc < 0) {
         if (cmd >= 3) fprintf(stderr, "client4: exit_failure %d \n", rc);
         exit (EXIT_FAILURE);
       }

       /* Output results */
       fprintf(stdout, "%s",buffer);

       if (cmd >= 3) fprintf(stderr, "client4: exit_success \n");
       exit (EXIT_SUCCESS);
     }
#endif /*  GEN_MAIN */

#undef GEN_MAIN
