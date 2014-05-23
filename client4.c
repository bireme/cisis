
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
#endif /*  GEN_MAIN */

     #include <sys/socket.h>
     #include <netinet/in.h>
     #include <netdb.h>

#if GEN_MAIN
     #define PORT            80
     #define MESSAGE         "HEAD / HTTP/1.0\n\n"
     #define SERVERHOST      "www.bireme.br"
#endif /*  GEN_MAIN */

     #define BUFFERSIZE      51200

#if 0
       "HEAD /'message' HTTP/1.0\n\n"
       "GET / HTTP/1.0\nUser-Agent: Mozilla/3.0 (compatible; Opera/3.0; Windows 95/NT4)\nAccept: */*\nHost: birk105.studby.uio.no:81\n\n"
#endif

     int init_sockaddr (struct sockaddr_in *name, const char *hostname, uint16_t port);
     int mainclient_read_from_server (int cmd, int filedes, char *buffer, int buffersize, int maxrds);
     int mainclient_write_to_server (int cmd, int filedes, char *message, int messagesize);
     int mainclient (int cmd, char *message, char *serverhost, uint16_t port, char *buffer, int buffersize, int maxrds); /* char *referrer, char *useragent, char *from */


/*
*/
     int
     init_sockaddr (struct sockaddr_in *name,
                    const char *hostname,
                    uint16_t port)
     {
       struct hostent *hostinfo;

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



     int
     mainclient_write_to_server (int cmd, int filedes, char *message, int messagesize)
     {
       int nbytes;

       if (cmd == 3) fprintf (stderr, "Client: send message(%d): '%s'\n", messagesize, message);
       if (cmd == 2) fprintf (stderr, "Client: send message(%d) \n", messagesize);

       nbytes = write (filedes, message, messagesize);
       if (nbytes < messagesize)
         {
           /* Write error */
           /*perror ("write");*/
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
           return -1;
         }
       else
         {
           /* Data read. */
           buffer[nbytes]='\0';
           if (cmd == 3) fprintf (stderr, "Client: got message(%d): '%s' \n", nbytes, buffer);
           if (cmd == 2) fprintf (stderr, "Client: got message(%d) \n", nbytes);

           while (--maxrds)
            if ((more = read (filedes, buffer+nbytes, buffersize-1-nbytes)) > 0)
             {
               buffer[nbytes+more]='\0';
               if (cmd == 3) fprintf (stderr, "Client: got +message(%d): '%s' \n", more, buffer+nbytes);
               if (cmd == 2) fprintf (stderr, "Client: got +message(%d) \n", more);
               nbytes+=more;
             }
            else break;

           return nbytes;
         }
     }


     int
     mainclient (int cmd,
       char *message,        /* HEAD|GET /docpath HTTP/1.0\n\n */
       char *serverhost,     /* www.bireme.br */
       uint16_t port,        /* 80 */
       char *buffer,         /* results */
       int buffersize,       /* max size */
       int maxrds            /* max reads */
     )
     {
       int sock;
       struct sockaddr_in servername;

       int nbytes;
       int rc;
       char *p,*q;

       /* Trap null msg */
       if (!message)  /*return 0;*/  message="\\n";
       if (!*message) /*return 0;*/  message="\\n";

       if (cmd >= 1) fprintf(stdout, "<message>%s</message><serverhost>%s</serverhost><port>%d</port>\n",message,serverhost,port);

       /* Convert calling \n to LF */
       for (p=buffer, q=message; *q; p++, q++)
         if (*q=='\\' && *(q+1)=='n') {
           *p = '\n'; q++;
         }
         else *p = *q;
       *p='\0';

       /* Create the socket. */
       sock = socket (PF_INET, SOCK_STREAM, 0);
       if (sock < 0)
         {
           /*perror ("socket (client)");*/
           return -1;
         }

       /* Init (init_sockaddr). */
       rc = init_sockaddr (&servername, serverhost, port);
       if (rc < 0)
         {
           /*fprintf (stderr, "Unknown host %s.\n", hostname);*/
           close (sock);
           return -2;
         }

       /* Connect to the server. */
       rc = connect (sock, (struct sockaddr *) &servername, sizeof (servername));
       if (rc < 0)
         {
           /*perror ("connect (client)");*/
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
       close (sock);

       /* parse 1st line
          HTTP/1.1 200 OK
          HTTP/1.1 3   Found
          HTTP/1.1 403 Forbidden
          HTTP/1.1 404 Not Found
          HTTP/1.1 501 Method Not Implemented
       */

       if (cmd >= 1) fprintf(stdout, "<content>%s</content>\n",buffer);

       for (p=buffer; *p; p++) if (*p==' ') break; *p++='\0';   rc=atoi(p);
       for (        ; *p; p++) if (*p==' ') break; *p++='\0';   q=p;
       for (        ; *p; p++) if (*p=='\r' || *p=='\n') break; *p='\0';

       if (cmd >= 1) fprintf(stdout, "<protocol>%s</protocol><code>%d</code><msg>%s</msg>\n",buffer,rc,q);

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
       char *message=MESSAGE;
       char *serverhost=SERVERHOST;
       uint16_t port=PORT;
       int maxrds=1;
       int rc;

       char buffer[BUFFERSIZE];

       if (argc == 1) {
          fprintf(stderr, "client4 0=plain|1=xml|2=tell|3=trace \"HEAD|GET docpath\\n\\n\" [serverhost [port [maxreads] ] ] \n");
         exit (EXIT_FAILURE);
       }

       cmd=atoi(argv[1]);

       if (argc > 2) message=argv[2];
       if (argc > 3) serverhost=argv[3];
       if (argc > 4) port=(uint16_t)atoi(argv[4]);
       if (argc > 5) maxrds=atoi(argv[5]);

       if (cmd >= 3) fprintf(stderr, "client4: \"%s\" %s %d \n",message,serverhost,port);

       rc=mainclient(cmd, message, serverhost, port, buffer, BUFFERSIZE, maxrds);

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

