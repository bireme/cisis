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

     void
     write_to_server (int filedes)
     {
       int nbytes;

       nbytes = write (filedes, MESSAGE, strlen (MESSAGE) + 1);
       if (nbytes < 0)
         {
           perror ("write");
           exit (EXIT_FAILURE);
         }
     }


     int
     main (void)
     {
       extern void init_sockaddr (struct sockaddr_in *name,
                                  const char *hostname,
                                  uint16_t port);
       int sock;
       struct sockaddr_in servername;

       /* Create the socket. */
       sock = socket (PF_INET, SOCK_STREAM, 0);
       if (sock < 0)
         {
           perror ("socket (client)");
           exit (EXIT_FAILURE);
         }

       /* Connect to the server. */
       init_sockaddr (&servername, SERVERHOST, PORT);
       if (0 > connect (sock,
                        (struct sockaddr *) &servername,
                        sizeof (servername)))
         {
           perror ("connect (client)");
           exit (EXIT_FAILURE);
         }

       /* Send data to the server. */
       write_to_server (sock);
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

