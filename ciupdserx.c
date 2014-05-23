/*
..Server program that makes a connection for a byte stream socket in the Internet namespace.
..Once it has connected to the server, it sends a text string to the server and gets the response.
Returns 0 or a negative error value.
*/
/*
16.9.7 Byte Stream Connection Server Example
The server end is much more complicated. Since we want to allow multiple clients to be connected to the server at the same time, it would be incorrect to wait for input from a single client by simply calling read or recv. Instead, the right thing to do is to use select (see Waiting for I/O) to wait for input on all of the open sockets. This also allows the server to deal with additional connection requests.
This particular server doesn't do anything interesting once it has gotten a message from a client. It does close the socket for that client when it detects an end-of-file condition (resulting from the client shutting down its end of the connection).
This program uses make_socket to set up the socket address; see Inet Example.
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
     #include <time.h>

#if GEN_MAIN
     #define PORT          1417
     #define BUFFERSIZE   51200
#endif /*  GEN_MAIN */

     #define REPLYSIZE    51200


     int mainserver_make_socket (int cmd, uint16_t port);
     int mainserver_read_from_client (int cmd, uint16_t port, int filedes, char *buffer, int buffersize);
     int mainserver_write_to_client (int cmd, uint16_t port, int filedes, char *message, int messagesize);
     int mainserver (int cmd, uint16_t port, char *buffer, int buffersize, int maxconnqueued);



/*
*/
     int
     mainserver_make_socket (int cmd, uint16_t port)
     {
       int sock;
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

       if (cmd == 3) fprintf (stderr, "Server [%d] [%d] [%s]: reply message (%d bytes): '%s' \n", port, filedes, timestamp, messagesize, message);
       if (cmd == 2) fprintf (stderr, "Server [%d] [%d] [%s]: reply message (%d bytes) \n", port, filedes, timestamp, messagesize);

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
     mainserver_read_from_client (int cmd, uint16_t port, int filedes, char *buffer, int buffersize)
     {
       int nbytes;

       buffer[0]='\0';
       nbytes = read (filedes, buffer, buffersize-1);
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
     }


/*
     mainserver
*/
     int
     mainserver (int cmd, uint16_t port, char *buffer, int buffersize, int maxconnqueued)
     {
       int sock;
       fd_set active_fd_set, read_fd_set;
       int i;
       struct sockaddr_in clientname;
       size_t size;
       int shutdown=0; /* run */
                              /* yyyymmdd hh:mm:ss WDAY YDAY */
       char timestamp[80];    /* 123456789012345678901234 7  8 901 */
       time_t secs_in;
       time_t secs_now;
       struct tm *tp;

       /* Create the socket and set it up to accept connections. */
       sock = mainserver_make_socket (cmd, port);

       time(&secs_now); tp=localtime(&secs_now); secs_in=secs_now;
       sprintf(timestamp,"%04d%02d%02d %02d%02d%02d %1d %3d", 1900+tp->tm_year,tp->tm_mon+1,tp->tm_mday, tp->tm_hour,tp->tm_min,tp->tm_sec, tp->tm_wday,tp->tm_yday);

       if (cmd == 3) fprintf (stderr, "Server [%d] [%d] [%s]: got socket \n", port, sock, timestamp);

       if (listen (sock, maxconnqueued) < 0)   /* <== at most 1 connection queued */
         {
           /*perror ("listen"); exit (EXIT_FAILURE);*/
           return -1;
         }

       /* Initialize the set of active sockets. */
       FD_ZERO (&active_fd_set);
       FD_SET (sock, &active_fd_set);

       while (!shutdown)
         {
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
                     if (cmd == 3) fprintf (stderr, "Server [%d] [%d] [%s]: connect from host %s, port %hd \n", port, sock, timestamp, inet_ntoa (clientname.sin_addr), ntohs (clientname.sin_port));
                     FD_SET (new, &active_fd_set);
                   }
                 else
                   {
                     /* Data arriving on an already-connected socket. */
                     int nread = mainserver_read_from_client (cmd, port, i, buffer, buffersize);
                     int code;
                     char replybuf[REPLYSIZE];
                     char *reply=replybuf;
                     replybuf[0]='\0';

                     time(&secs_now); tp=localtime(&secs_now);
                     sprintf(timestamp,"%04d%02d%02d %02d%02d%02d %1d %3d", 1900+tp->tm_year,tp->tm_mon+1,tp->tm_mday, tp->tm_hour,tp->tm_min,tp->tm_sec, tp->tm_wday,tp->tm_yday);

                     if (cmd == 3) fprintf (stderr, "Server [%d] [%d] [%s]: got message (%d bytes): '%s' \n", port, sock, timestamp, nread, buffer);

                     if (strlen(buffer) == 0)              {         reply="server4 100 eof msg\n"; /* never */          code=100; }
                     else
                     if (strcmp(buffer,"\n") == 0)         {         reply="server4 101 null msg\n";                     code=101; }
                     else
                     if (strcmp(buffer+nread-1,"\n") != 0) {         reply="server4 505 bad msg\n";                      code=500; }
                     else
                     if (strcmp(buffer,"shutdown\n") == 0) {         reply="server4 200 thanks\n";                       code=0;   shutdown=1; }
                     else
                     if (strcmp(buffer,"hello\n") == 0)    {         reply="server4 200 hi\n";                           code=200; }
                     else
                     if (strncmp(buffer,"echo ",5) == 0)   { sprintf(reply,"server4 200 %s\n",buffer+5); reply=replybuf; code=200; }
                     else
                                                           {         reply="server4 404 invalid msg\n";                  code=400; }

                     /* Response */
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
                     }

                     close (i);
                     FD_CLR (i, &active_fd_set);

                   } /* end data read */

               } /* end if FD_ISSET */

               if (shutdown) break;

           } /* end for FD_SETSIZE */
         } /* end while */

         /* Release the socket. */
         close (sock);

         time(&secs_now); tp=localtime(&secs_now);
         sprintf(timestamp,"%04d%02d%02d %02d%02d%02d %1d %3d", 1900+tp->tm_year,tp->tm_mon+1,tp->tm_mday, tp->tm_hour,tp->tm_min,tp->tm_sec, tp->tm_wday,tp->tm_yday);

         if (cmd == 3) fprintf (stderr, "Server [%d] [%d] [%s]: close socket (%"_LD_" seconds) \n", port, sock, timestamp,(LONGX)secs_now-secs_in);

         return 0;

     } /* end of mainserver */




/* Main

./server4 3
./server4 3 2>>xlog &

*/

#if GEN_MAIN
     int
     main(int argc, char *argv[])
     {
       /* calling parameters */
       int cmd;
       uint16_t port=PORT;
       int rc;

       char buffer[BUFFERSIZE];

       if (argc == 1) {
          fprintf(stderr, "server4 0=plain|1=xml|2=tell|3=trace [port%d] \n",PORT);
         exit (EXIT_FAILURE);
       }

       cmd=atoi(argv[1]);

       if (argc > 2) port=(uint16_t)atoi(argv[2]);

       if (cmd >= 3) fprintf(stderr, "server4: %d \n",port);

       rc = mainserver(cmd, port, buffer, BUFFERSIZE, 1);

       if (rc < 0) {
         if (cmd >= 3) fprintf(stderr, "server4: exit_failure %d \n", rc);
         exit (EXIT_FAILURE);
       }

       if (cmd >= 3) fprintf(stderr, "server4: exit_success \n");
       exit (EXIT_SUCCESS);
     }
#endif /*  GEN_MAIN */

#undef GEN_MAIN


