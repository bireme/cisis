
     #include <stdio.h>
     #include <errno.h>
     #include <stdlib.h>
     #include <unistd.h>
     #include <sys/types.h>
     #include <sys/socket.h>
     #include <netinet/in.h>
     #include <netdb.h>

//     #define PORT            5555
     #define MESSAGE         "Yow!!! Are we having fun yet?!?"
//   #define SERVERHOST      "mescaline.gnu.org"

     #define PORT            80
     #define SERVERHOST      "www.bireme.br"


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

//     void
//     write_to_server (int filedes)
//     {
//       int nbytes;
//
//       nbytes = write (filedes, MESSAGE, strlen (MESSAGE) + 1);
//       if (nbytes < 0)
//         {
//           perror ("write");
//           exit (EXIT_FAILURE);
//         }
//     }

     void
     read_from_server (int filedes)
     {
       int nbytes=1;
       char buffer[BUFSIZ+1];
       int size=BUFSIZ;
       int flags=0;

       while (nbytes) {

         nbytes = recv (filedes, buffer, size, flags);

         if (nbytes < 0)
           {
             perror ("read");
             exit (EXIT_FAILURE);
           }
         else
           {
             buffer[nbytes]='\0';
             fprintf(stdout,"%s",buffer);
           }
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
//       write_to_server (sock);
       read_from_server (sock);

       close (sock);
       exit (EXIT_SUCCESS);
     }
