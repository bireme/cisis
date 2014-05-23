
     #include <time.h>

     int mainserver (int cmd, uint16_t port, char *buffer, int buffersize, int maxconnqueued);


/*
     fake mainserver
*/
     int
     mainserver (int cmd, uint16_t port, char *buffer, int buffersize, int maxconnqueued)
     {
       int sock;
       char timestamp[80];    /* 123456789012345678901234 7  8 901 */
       LONGX secs_now;
       struct tm *tp;

       /* Create the socket and set it up to accept connections. */
       sock = 3;

       time(&secs_now); tp=localtime(&secs_now);
       sprintf(timestamp,"%04d%02d%02d %02d%02d%02d %1d %3d", 1900+tp->tm_year,tp->tm_mon+1,tp->tm_mday, tp->tm_hour,tp->tm_min,tp->tm_sec, tp->tm_wday,tp->tm_yday);

       if (cmd == 3) fprintf (stderr, "Server [%d] [%d] [%s]: got socket \n", port, sock, timestamp);

     } /* end of mainserver */



