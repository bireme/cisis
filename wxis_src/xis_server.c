/*
..Server program that makes a connection for a byte stream socket in the
Internet namespace.
..Once it has connected to the server, it sends a text string to the server and
gets the response.
Returns 0 or a negative error value.
*/
/*
16.9.7 Byte Stream Connection Server Example
The server end is much more complicated. Since we want to allow multiple clients
to be connected to the server at the same time, it would be incorrect to wait
for input from a single client by simply calling read or recv. Instead, the
right thing to do is to use select (see Waiting for I/O) to wait for input on
all of the open sockets. This also allows the server to deal with additional
connection requests.

This particular server doesn't do anything interesting once it has gotten a
message from a client. It does close the socket for that client when it detects
an end-of-file condition (resulting from the client shutting down its end of the
connection).
This program uses make_socket to set up the socket address; see Inet Example.
*/

#define GEN_MAIN 1

#if GEN_MAIN
     #include <stdio.h>
     #include <errno.h>
     #include <stdlib.h>
     #include <string.h>
     #include <unistd.h>
     #include <sys/types.h>
#endif /*  GEN_MAIN */

     #include <sys/socket.h>
     #include <netinet/in.h>
     #include <netdb.h>
     #include <time.h>
     #include <setjmp.h>
     #include <fcntl.h>
     #include <sys/types.h>
     #include <sys/stat.h>

     #include "xis_server.h"
     #include "cisis.h"
     //#include "chronometer.h"

#if GEN_MAIN
     #define BUFFERSIZE    2097152 //51200
#endif /*  GEN_MAIN */

     #define REPLYSIZE     1048576 //51200
     #define MAXCONNQUEUED 30


/*----------------------------------------------------------------------------*/
     int mainFunc(int argc,       /* wxis main function */
                  char *argv[],
                  char *envv[],
                  OUT_BUFFER *outBuff);

     void wxisMainFunc(int   argc,     /* quantity of arguments */
                       char *argv[],    /* array of arguments */
                       char *envv[],    /* array of environment variables */
                       OUT_BUFFER *outBuff);  /* string output of wxis */

     int xis_mainserver_make_socket (uint16_t port,
                                     int trace);
     int xis_mainserver_read_from_client (uint16_t port,
                                          int filedes,
                                          char *buffer,
                                          int buffersize,
                                          int trace);
     int xis_mainserver_write_to_client (uint16_t port,
                                         int filedes,
                                         char *message,
                                         int messagesize,
                                         int trace);
     int xis_mainserver (char *envv[],
                         uint16_t port,
                         char *buffer,
                         int buffersize,
                         OUT_BUFFER *outBuff,
                         int maxconnqueued,
                         int trace);

/*----------------------------------------------------------------------------*/
extern int     fatal_errcod;    /* cisis fatal() error code */
extern char    fatal_iomsg[];   /* cisis fatal() flag/msg */
extern jmp_buf fatal_jumper;    /* cisis fatal() longjmp() */
extern int     dbxopt_errno;    /* dbxopen() - dbname/errno msg before fatal */
/*----------------------------------------------------------------------------*/

    void clearBuff(OUT_BUFFER *outBuff) {
        if (outBuff) {
            outBuff->freeSize = outBuff->totalSize;
            outBuff->textSize = 0;
            if (outBuff->buffer) {
                outBuff->buffer[0] = '\0';
            }
        }
    }

    int writeBuff(char *mess,
                  OUT_BUFFER *outBuff,
                  int append) {
        int msize = (mess ? strlen(mess) : 0);
        int osize = 0;
        int maxsize = outBuff->totalSize;

        if (!mess) {
            return 0;
        }
        if (!outBuff) {
            return 0;
        }

        if (!append) {
            clearBuff(outBuff);
        }

        osize = outBuff->textSize + msize;

        if (msize >= outBuff->freeSize) {
            maxsize = (osize > maxsize) ? (2 * osize) : (2 * maxsize);
            maxsize = (maxsize < 32768 ? 32768 : maxsize);
            outBuff->buffer = (char *)realloc(outBuff->buffer, maxsize);
            outBuff->totalSize = maxsize;
            if (!outBuff->buffer) {
                outBuff->totalSize = 0;
                outBuff->freeSize = 0;
                outBuff->textSize = 0;
                return 0;
            }
        }

        if (append) {
            outBuff->freeSize = outBuff->totalSize - osize - 1;
            outBuff->textSize = osize;
            strcat(outBuff->buffer, mess);
        } else {
            outBuff->freeSize = outBuff->totalSize - msize - 1;
            outBuff->textSize = msize;
            strcpy(outBuff->buffer, mess);
        }

        return msize;
    }

    int checkFileName(char *buffer) {
        char *p = buffer;
        int handle;
        int ret = 0;

        if (buffer) {
            while(*p) {
                if (*p == ' ') {
                    p++;
                } else {
                    break;
                }
            }
            if (*p) {
                if (*p == '@') {
                    ret = 1;
                    handle = open(++p, O_RDONLY);
                    if (handle == -1) {
                        buffer[0] = '\0';
                    }
                    if (read(handle, buffer, BUFFERSIZE) == -1) {
                        buffer[0] = '\0';
                    }
                    close(handle);
                }
            }
        }

        return ret;
   }

   int writeToFile(char *fname,
                   OUT_BUFFER *outBuff) {
        int handle;

        if ((!fname) || (!outBuff)) {
            return -1;
        }
        handle = open(fname, O_CREAT|O_TRUNC|O_WRONLY,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
        if (handle == -1) {
            return -1;
        }

        if (outBuff->textSize > 0) {
            if (write(handle, outBuff->buffer, outBuff->textSize) == -1) {
                return -1;
            }
        }
        close(handle);

        return 0;
    }

    int xis_mainserver_make_socket (uint16_t port,
                                    int trace) {
        int sock;
        struct sockaddr_in name;

        /* Create the socket. */
        sock = socket (PF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            if (trace) {
                fprintf(stderr, "xis_mainserver_make_socket/socket < 0 \n");
            }
            return -1;
        }

        /* Give the socket a name. */
        name.sin_family = AF_INET;
        name.sin_port = htons (port);
        name.sin_addr.s_addr = htonl (INADDR_ANY);
        if (bind (sock, (struct sockaddr *) &name, sizeof (name)) < 0) {
            if (trace) {
                fprintf(stderr, "xis_mainserver_make_socket/bind < 0 \n");
            }
            return -2;
        }

        return sock;
    }

    int xis_mainserver_write_to_client(uint16_t port,
                                       int filedes,
                                       char *message,
                                       int messagesize,
                                       int trace) {
        int nbytes;
                               /* yyyymmdd hh:mm:ss WDAY YDAY */
        char timestamp[80];    /* 123456789012345678901234 7  8 901 */
        LONGX secs_now;
        struct tm *tp;

        time(&secs_now);
        tp = localtime(&secs_now);

        sprintf(timestamp,"%04d%02d%02d %02d%02d%02d %1d %3d", 1900+tp->tm_year,
                tp->tm_mon+1,tp->tm_mday, tp->tm_hour,tp->tm_min,tp->tm_sec,
                tp->tm_wday,tp->tm_yday);

        if (trace) {
            fprintf (stderr,
                  "xis_server [%d] [%d] [%s]: reply message (%d bytes): '%s'\n",
                  port, filedes, timestamp, messagesize, message);
        }

        nbytes = write (filedes, message, messagesize);
        if (nbytes < messagesize) {
            if (trace) {
                fprintf (stderr, "xis_mainserver_write_to_client/write \n");
            }
            return -1;
        }

        return nbytes;
    }

    int xis_mainserver_read_from_client(uint16_t port,
                                        int filedes,
                                        char *buffer,
                                        int buffersize,
                                        int trace) {
        int nbytes;

        buffer[0]='\0';

        nbytes = read (filedes, buffer, buffersize-1);
        if (nbytes < 0) {
            if (trace) {
                fprintf (stderr, "xis_mainserver_read_from_client/read \n");
            }
            return -2;
        }

        if (trace) {
            char *p = buffer;
            int loop = nbytes;

            fprintf (stderr, "xis_mainserver_read_from_client/read(%d): ",
                                                                     nbytes);
            for (; loop--; p++) {
                fprintf (stderr, "%c=%02x ", *p, (int)*p);
            }
            fprintf (stderr, "\n");
        }

        /* EOF (nbytes=0) or Data read. */
        buffer[nbytes]='\0';

        return nbytes;
     }

/*
     xis_mainserver
*/
    int xis_mainserver(char *envv[],
                       uint16_t port,
                       char *buffer,
                       int buffersize,
                       OUT_BUFFER *outBuff,
                       int maxconnqueued,
                       int trace) {

        int sock;
        fd_set active_fd_set, read_fd_set;
        int i;
        struct sockaddr_in clientname;
        size_t size;
                               /* yyyymmdd hh:mm:ss WDAY YDAY */
        char timestamp[80];    /* 123456789012345678901234 7  8 901 */
        LONGX secs_in;
        LONGX secs_now;
        struct tm *tp;

        /* Create the socket and set it up to accept connections. */
        sock = xis_mainserver_make_socket (port, trace);

        time(&secs_now);
        tp=localtime(&secs_now);
        secs_in=secs_now;

        sprintf(timestamp,"%04d%02d%02d %02d%02d%02d %1d %3d",
               1900+tp->tm_year,tp->tm_mon+1,tp->tm_mday, tp->tm_hour,
               tp->tm_min,tp->tm_sec, tp->tm_wday,tp->tm_yday);

        if (trace) {
            fprintf (stderr, "xis_server [%d] [%d] [%s] [%d]: got socket \n",
                     port, sock, timestamp, maxconnqueued);
        }

        /* <== at most 1 connection queued */
        if (listen (sock, maxconnqueued) < 0) {
            if (trace) {
                fprintf (stderr, "xis_mainserver/listen < 0 \n");
            }
            return -1;
        }

        /* Initialize the set of active sockets. */
        FD_ZERO (&active_fd_set);
        FD_SET (sock, &active_fd_set);

        while (1) {
            /* Block until input arrives on one or more active sockets. */
            read_fd_set = active_fd_set;
            if (select (FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0) {
                if (trace) {
                    fprintf (stderr, "xis_mainserver/select < 0 \n");
                }
                return -2;
            }

            /* Service all the sockets with input pending. */
            for (i = 0; i < FD_SETSIZE; ++i) {
                if (FD_ISSET (i, &read_fd_set)) {
                    if (i == sock) {

                        /* Connection request on original socket. */
                        int new;
                        size = sizeof (clientname);

                        new = accept (sock,
                                      (struct sockaddr *) &clientname,
                                      &size);
                        if (new < 0) {
                            if (trace) {
                                fprintf (stderr,
                                                "xis_mainserver/accept < 0 \n");
                            }
                            return -3;
                        }

                        if (trace) {
                            fprintf (stderr,
                                     "xis_server [%d] [%d] [%s]: connect from host %s, port %hd \n",
                                     port, sock, timestamp,
                                     inet_ntoa (clientname.sin_addr),
                                     ntohs (clientname.sin_port));
                        }

                        FD_SET (new, &active_fd_set);

                    } else {
                        char ** argv = NULL;
                        int argc = 0;
                        int c = 0;
                        int len = 0;
                        int nbytes = 0;
                        int isFile = 0;

                        /* Data arriving on an already-connected socket. */
                        int nread = 0;

                        nread = xis_mainserver_read_from_client (port, i,
                                                    buffer, buffersize, trace);
                        time(&secs_now);
                        tp=localtime(&secs_now);

                        sprintf(timestamp,"%04d%02d%02d %02d%02d%02d %1d %3d",
                                    1900+tp->tm_year,tp->tm_mon+1,tp->tm_mday,
                                    tp->tm_hour,tp->tm_min,tp->tm_sec,
                                    tp->tm_wday,tp->tm_yday);

                        if (trace) {
                            fprintf (stderr,
                                     "xis_server [%d] [%d] [%s]: got message (%d bytes): '%s' \n",
                                     port, sock, timestamp, nread, buffer);
                        }

                        len = strlen(buffer);
                        if (len == 0) {
                            writeBuff("xis_server 100 eof msg\n", outBuff, 0); /* never */
                        } else if (strcmp(buffer,"\n") == 0) {
                            writeBuff("xis_server 101 null msg\n", outBuff, 0);
                        } else if (strcmp(buffer + nread - 1,"\n") != 0) {
                            writeBuff("xis_server 505 bad msg\n", outBuff, 0);
                        } else {
                            while(--len >= 0) {
                                if ((buffer[len] == '\n') ||
                                    (buffer[len] == '\r')) {
                                    buffer[len] = 0x00;
                                } else {
                                    break;
                                }
                            }
                            isFile = checkFileName(buffer);
                            if (buffer[0] == '\0') {
                                writeBuff("xis_server invalid input msg\n", outBuff, 0);
                            } else {
                                argc = createArgv(buffer, &argv);
                                if (trace) {
                                    for (c = 0; c < argc; c++) {
                                        if (argv[c] == NULL) {
                                            printf("argv[%d]=NULL\n", c);
                                        } else {
                                            printf("argv[%d]=[%s]\n", c, argv[c]);
                                        }
                                    }
                                }

                                clearBuff(outBuff);

                                wxisMainFunc(argc, argv, envv, outBuff);

                                for (c = 0; c < argc; c++) {
                                     free(argv[c]);
                                     argv[c] = NULL;
                                }
                                free(argv);
                            }
                        }
                        /* Response */
                        if (isFile) {
                            writeToFile("/home/scielo/www/htdocs/xis_server.out", outBuff);
                            clearBuff(outBuff);
                            writeBuff("@/home/scielo/www/htdocs/xis_server.out", outBuff, 0);
                        }
                        if (outBuff->textSize > 0) {
                            nbytes = xis_mainserver_write_to_client (port, i, outBuff->buffer,
                                                                 outBuff->textSize, trace);
                            if (nbytes < 0) {
                                if (trace) {
                                    fprintf (stderr,
                                             "xis_mainserver/write error=%d\n",
                                             nbytes);
                                }
                            }
                        }
                        clearBuff(outBuff);
                        close (i);
                        FD_CLR (i, &active_fd_set);

                    } /* end data read */
                } /* end if FD_ISSET */
            } /* end for FD_SETSIZE */
        } /* end while */

        /* Release the socket. */
        close (sock);

        time(&secs_now);
        tp=localtime(&secs_now);

        if (trace) {
            sprintf(timestamp, "%04d%02d%02d %02d%02d%02d %1d %3d",
                    1900+tp->tm_year,tp->tm_mon+1,tp->tm_mday, tp->tm_hour,
                    tp->tm_min,tp->tm_sec, tp->tm_wday,tp->tm_yday);
            fprintf(stderr,
                    "xis_server [%d] [%d] [%s]: close socket (%"_LD_" seconds) \n",
                    port, sock, timestamp,secs_now-secs_in);
        }

        return 0;
    } /* end of xis_mainserver */

    void wxisMainFunc(int   argc,      /* quantity of arguments */
                      char *argv[],    /* array of arguments */
                      char *envv[],    /* array of environment variables */
                      OUT_BUFFER *outBuff) { /* string output of wxis */
        int     z_xerrno;
        int     z_errcod;
        char    z_iomsg[80];
        jmp_buf z_jumper;
        int     z_jumprc;

        /* save/notrap */
        strcpy(z_iomsg,fatal_iomsg);
        memmove(z_jumper,fatal_jumper,sizeof(fatal_jumper));
        z_errcod = fatal_errcod;
        z_xerrno = dbxopt_errno;

        /* trap cisis fatal() */
        strcpy(fatal_iomsg,"trap");

        /* disable dbxopen msg */
        dbxopt_errno=0;
        z_jumprc = setjmp(fatal_jumper);

        if (z_jumprc == 0) {
            mainFunc(argc, argv, envv, outBuff);
        } else { /* error */
            char aux[256];
            sprintf(aux, "\nWXIS|error|%s\n", fatal_iomsg);
            writeBuff(aux, outBuff, 0);
        }

        /* restore/notrap */
         memmove(fatal_jumper,z_jumper,sizeof(fatal_jumper));
         strcpy(fatal_iomsg,z_iomsg);
         fatal_errcod = z_errcod;
         dbxopt_errno = z_xerrno;
     }

    int createArgv(char *in,
                   char ***argv) {
        int argc = 0;
        int max = 40;
        int size = max * sizeof(char *);
        char *aux = NULL;
        int pos = 0;
        char **tokens = NULL;

        if (in != NULL) {
            tokens = malloc(size);
            memset(tokens, 0x00, size);
            tokens[argc++] = strdup("wxis_server");
            aux = malloc(strlen(in));

            while((argc < max) && (*in != 0x00)) {
                pos = 0;

                while(*in == ' ') {
                    in++;
                }
                if (*in == 0x00) {
                    break;
                }
                if (*in == '"') {
                    in++;
                    while(1) {
                        if ((*in == 0x00) || (*in == '"')) {
                            aux[pos] = 0x00;
                            tokens[argc++] = strdup(aux);
                            if (*in != 0x00) {
                                in++;
                            }
                            break;
                        }
                        aux[pos++] = *in;
                        in++;
                    }
                } else {
                    while(1) {
                        if ((*in == 0x00) || (*in == ' ')) {
                            aux[pos] = 0x00;
                            tokens[argc++] = strdup(aux);
                            if (*in != 0x00) {
                                in++;
                            }
                            break;
                        }
                        aux[pos++] = *in;
                        in++;
                    }
                }
            }
            free(aux);
        }
        *argv = tokens;
/*{
int c = 0;
for (c=0; c<argc; c++) {
    printf("argv[%d]=%s\n", c, tokens[c]);
    printf("argv[%d]=%s\n", c, argv[0][c]);
}
}*/
        return argc;
    }

#if GEN_MAIN
    int main(int argc,
             char *argv[],
             char *envv[]) {

        /* calling parameters */
        int port = 0;
        int rc = 0;
        int trace = 0;

        char buffer[BUFFERSIZE];

        /* Create the global output buffer */
        OUT_BUFFER outBuff;

        outBuff.totalSize = 50000;
        outBuff.freeSize = 50000;
        outBuff.textSize = 0;
        outBuff.buffer = malloc(50000);
        outBuff.buffer[0] = '\0';

        //chrono_init("chrono_wxis_server.txt");

        if (argc == 1) {
            fprintf(stderr, "xis_server port=<PORT> [trace] \n");
            exit (EXIT_FAILURE);
        }

        if (strncmp(argv[1], "port=", 5) != 0) {
            fprintf(stderr, "xis_server port=<PORT> [trace] \n");
            exit (EXIT_FAILURE);
        }

        port = atoi(argv[1] + 5);
        if (port <= 0) {
            fprintf(stderr, "xis_server port [%d] <= 0\n", port);
            exit (EXIT_FAILURE);
        }

        if (argc > 2) {
            if (strcmp(argv[2], "trace") == 0) {
                trace = 1;
            } else {
                fprintf(stderr, "xis_server port=<PORT> [trace] \n");
                exit (EXIT_FAILURE);
            }
        }

        if (trace) {
            char *div = "-----------------------------------------------------------------------------\n";

            argc = createArgv("what", &argv);
            fprintf(stderr, div);
            wxisMainFunc(argc, argv, envv, &outBuff);
            if (outBuff.buffer) {
                fprintf(stderr, outBuff.buffer);
            }
            fprintf(stderr, "xis_server: %d \n", port);
        }

        rc = xis_mainserver(envv, (uint16_t)port, buffer,
                            BUFFERSIZE, &outBuff, MAXCONNQUEUED, trace);

        if (outBuff.buffer) {
            free(outBuff.buffer);
        }

        //chrono_end();

        if (rc < 0) {
            if (trace) {
                fprintf(stderr, "xis_server: exit_failure %d \n", rc);
            }
            exit (EXIT_FAILURE);
        }

        if (trace) {
            fprintf(stderr, "xis_server: exit_success \n");
        }

        exit (EXIT_SUCCESS);
    }
#endif /*  GEN_MAIN */

#undef GEN_MAIN
