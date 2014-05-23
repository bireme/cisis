/*

16.9.6 Byte Stream Socket Example
Here is an example client program that makes a connection for a byte stream socket in the Internet namespace. It doesn't do anything particularly interesting once it has connected to the server; it just sends a text string to the server and exits.

This program uses init_sockaddr to set up the socket address; see Inet Example.
*/

     #include <stdio.h>
     #include <errno.h>
     #include <stdlib.h>
     #include <unistd.h>
     #include <sys/types.h>
     #include <sys/socket.h>
     #include <netinet/in.h>
     #include <netdb.h>

     #define PORT            5555
     #define MESSAGE         "Yow!!! Are we having fun yet?!?"
     #define SERVERHOST      "serverofi.bireme.br"

     #define MAXMSG  512

     int
     read_from_server (int filedes)
     {
       char buffer[MAXMSG];
       int nbytes;

       nbytes = read (filedes, buffer, MAXMSG);
       if (nbytes < 0)
         {
           /* Read error. */
           perror ("read");
           exit (EXIT_FAILURE);
         }
       else if (nbytes == 0)
         /* End-of-file. */
         return -1;
       else
         {
           /* Data read. */
           fprintf (stderr, "Client: got message: `%s'\n", buffer);
           return 0;
         }
     }

     void
     write_to_server (int filedes, char *message)
     {
       int nbytes;

       nbytes = write (filedes, message, strlen (message) + 1);
       if (nbytes < 0)
         {
           perror ("write");
           exit (EXIT_FAILURE);
         }
     }


     int
     main(int argc, char *argv[])
     {
       extern void init_sockaddr (struct sockaddr_in *name,
                                  const char *hostname,
                                  uint16_t port);
       int sock;
       struct sockaddr_in servername;

       /* calling parameters */
       char *message=MESSAGE;
       char *serverhost=SERVERHOST;
       uint16_t port=PORT;
       if (argc < 2 || argc > 4) {
           perror ("client2 msg [serverhost [port] ]");
           exit (EXIT_FAILURE);
       }
       if (argc > 1) message=argv[1];
       if (argc > 2) serverhost=argv[2];
       if (argc > 3) port=(uint16_t)atoi(argv[3]);

       /* Create the socket. */
       sock = socket (PF_INET, SOCK_STREAM, 0);
       if (sock < 0)
         {
           perror ("socket (client)");
           exit (EXIT_FAILURE);
         }

       /* Connect to the server. */
       init_sockaddr (&servername, serverhost, port);
       if (0 > connect (sock,
                        (struct sockaddr *) &servername,
                        sizeof (servername)))
         {
           perror ("connect (client)");
           exit (EXIT_FAILURE);
         }

       /* Send data to the server. */
       write_to_server (sock, message);
       
       /* and Read...
       */
       read_from_server (sock);

       close (sock);
       exit (EXIT_SUCCESS);
     }




/*
*/

#if 1

     #include <stdio.h>
     #include <stdlib.h>
     #include <sys/socket.h>
     #include <netinet/in.h>
     #include <netdb.h>

     void
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
           fprintf (stderr, "Unknown host %s.\n", hostname);
           exit (EXIT_FAILURE);
         }
       name->sin_addr = *(struct in_addr *) hostinfo->h_addr;
     }

#endif

