/*
Client program that makes a connection for a byte stream socket in the Internet namespace.
Once it has connected to the server, it sends a text string to the server and gets the response.
Returns the response size or a negative error value.
*/

     #include <stdio.h>
     #include <errno.h>
     #include <stdlib.h>
     #include <unistd.h>
     #include <sys/types.h>
     #include <sys/socket.h>
     #include <netinet/in.h>
     #include <netdb.h>


     #define PORT            80
     #define MESSAGE         "HEAD / HTTP/1.0\n\n"
     #define SERVERHOST      "www.bireme.br"

     #define BUFFERSIZE      51200



#if 0
       "HEAD /'message' HTTP/1.0\n\n"
       "GET / HTTP/1.0\nUser-Agent: Mozilla/3.0 (compatible; Opera/3.0; Windows 95/NT4)\nAccept: */*\nHost: birk105.studby.uio.no:81\n\n"
#endif

     int init_sockaddr (struct sockaddr_in *name, const char *hostname, uint16_t port);

     int mainclient_read_from_server (int cmd, int filedes, char *buffer, int buffersize);

     int mainclient_write_to_server (int cmd, int filedes, char *message, int messagesize);

     int mainclient(int cmd, char *message, char *serverhost, uint16_t port, char *buffer, int buffersize); /* char *referrer, char *useragent, char *from */




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
     mainclient_read_from_server (int cmd, int filedes, char *buffer, int buffersize)
     {
       int nbytes;
       int more;

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
           if (cmd == 3) fprintf (stderr, "Client: got message(%d): '%s'\n", nbytes, buffer);
           if (cmd == 2) fprintf (stderr, "Client: got message(%d) \n", nbytes);

           while ((more = read (filedes, buffer+nbytes, buffersize-1-nbytes)) > 0)
             {
               buffer[nbytes+more]='\0';
               if (cmd == 3) fprintf (stderr, "Client: got message(%d): '%s'\n", more, buffer+nbytes);
               if (cmd == 2) fprintf (stderr, "Client: got message(%d) \n", more);
               nbytes+=more;
             }
           return nbytes;
         }
     }


     int
     mainclient(int cmd,
       char *message,        /* HEAD|GET /docpath HTTP/1.0\n\n */
       char *serverhost,     /* www.bireme.br */
       uint16_t port,        /* 80 */
       char *buffer,         /* results */
       int buffersize        /* max size */
     )
     {
       int sock;
       struct sockaddr_in servername;

       int messagelen=strlen(message);
       int nbytes;
       int rc;

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
           return -2;
         }

       /* Connect to the server. */
       rc = connect (sock, (struct sockaddr *) &servername, sizeof (servername));
       if (rc < 0)
         {
           /*perror ("connect (client)");*/
           return -3;
         }

       /* Send data to the server. */
       nbytes = mainclient_write_to_server (cmd, sock, message, messagelen);
       if (nbytes < 0)
         {
           /*perror ("write (client)");*/
           return -4;
         }

       /* Get data from the server */
       nbytes = mainclient_read_from_server (cmd, sock, buffer, buffersize);
       if (nbytes < 0)
         {
           /*perror ("read (client)");*/
           return -5;
         }

       /* Release connection/socket */
       close (sock);

       return nbytes;
     }



/* Main

./client3 0 "HEAD / HTTP/1.0\n\n"
./client3 1 "HEAD / HTTP/1.0\n\n"
./client3 2 "HEAD / HTTP/1.0\n\n"

./client3 1 "HEAD http://lis.bvs.br/xml2html/xmlListT.php?xml%5B%5D=http://lis.bvs.br/lis-Regional/P/define.xml&xsl=http://lis.bvs.br/lis-Regional/home.xsl HTTP/1.0\n\n" lis.bvs.br 80
./client3 0 "GET http://lis.bvs.br/xml2html/xmlListT.php?xml%5B%5D=http://lis.bvs.br/lis-Regional/P/define.xml&xsl=http://lis.bvs.br/lis-Regional/home.xsl HTTP/1.0\n\n" lis.bvs.br 80

[aot@serverofi socket]$ ./client3 go "HEAD / HTTP/1.0\n\n"
Client: send message(17)
Client: got message(249)
HTTP/1.1 200 OK
Date: Sun, 17 Jul 2005 22:45:57 GMT
Server: Apache/1.3.9 (Unix)
Last-Modified: Wed, 15 Jun 2005 19:22:27 GMT
ETag: "5bd2d-3702-42b07ff3"
Accept-Ranges: bytes
Content-Length: 14082
Connection: close
Content-Type: text/html


[aot@serverofi socket]$ ./client3 go "GET / HTTP/1.0\n\n" >x.htm
Client: send message(16)
Client: got message(1460)
Client: got message(1460)
Client: got message(1460)
Client: got message(1460)
Client: got message(1460)
Client: got message(1460)
Client: got message(1460)
Client: got message(1460)
Client: got message(1460)
Client: got message(1191)
[aot@serverofi socket]$ ll x.htm
-rw-rw-r--    1 aot      aot         14332 Jul 17 19:48 x.htm
[aot@serverofi socket]$ head -11 x.htm
HTTP/1.1 200 OK
Date: Sun, 17 Jul 2005 22:48:14 GMT
Server: Apache/1.3.9 (Unix)
Last-Modified: Wed, 15 Jun 2005 19:22:27 GMT
ETag: "5bd2d-3702-42b07ff3"
Accept-Ranges: bytes
Content-Length: 14082
Connection: close
Content-Type: text/html

<HTML>
[aot@serverofi socket]$

[aot@serverofi socket]$
[aot@serverofi socket]$ ./client3
client3 go|tell|trace "HEAD|GET docpath\n\n" [serverhost [port] ]: Success


*/
     int
     main(int argc, char *argv[])
     {
       /* calling parameters */
       int cmd;
       char *message=MESSAGE;
       char *serverhost=SERVERHOST;
       uint16_t port=PORT;
       int rc;

       char buffer[BUFFERSIZE];
       char *p,*q;

       if (argc == 1) {
          fprintf(stderr, "client3 0=plain|1=xml|2=tell|3=trace \"HEAD|GET docpath\\n\\n\" [serverhost [port] ] \n");
         exit (EXIT_FAILURE);
       }

       cmd=atoi(argv[1]);

       if (argc > 2) message=argv[2];
       if (argc > 3) serverhost=argv[3];
       if (argc > 4) port=(uint16_t)atoi(argv[4]);

       if (cmd >= 3) fprintf(stderr, "client3: \"%s\" %s %d \n",message,serverhost,port);

       /* convert calling \n to LF */
       for (p=buffer, q=message; *q; p++, q++)
         if (*q=='\\' && *(q+1)=='n') {
           *p = '\n'; q++;
         }
         else *p = *q;
       *p='\0';

       if (cmd >= 3) fprintf(stderr, "client3: \"%s\" \n",buffer);
       if (cmd >= 1) fprintf(stdout, "<message>%s</message><serverhost>%s</serverhost><port>%d</port>\n",buffer,serverhost,port);

       rc=mainclient(cmd, buffer, serverhost, port, buffer, BUFFERSIZE);

       if (rc < 0) {
         if (cmd >= 3) fprintf(stdout, "client3: exit_failure %d \n", rc);
         exit (EXIT_FAILURE);
       }

       /* output results */
       if (cmd >= 1) fprintf(stdout, "<content>%s</content>\n",buffer);
       else          fprintf(stdout, "%s"                     ,buffer);

       /* parse 1st line
          HTTP/1.1 200 OK
          HTTP/1.1 3   Found
          HTTP/1.1 403 Forbidden
          HTTP/1.1 404 Not Found
          HTTP/1.1 501 Method Not Implemented
       */
       for (p=buffer; *p; p++) if (*p==' ') break;
       *p++='\0'; rc=atoi(p);
       for (        ; *p; p++) if (*p==' ') break;
       *p++='\0'; q=p;
       for (        ; *p; p++) if (*p=='\r' || *p=='\n') break;
       *p='\0';

       if (cmd >= 1) fprintf(stdout, "<protocol>%s</protocol><code>%d</code><msg>%s</msg>\n",buffer,rc,q);

       if (cmd >= 3) fprintf(stderr, "client3: exit_success \n");
       exit (EXIT_SUCCESS);
     }

