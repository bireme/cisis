
#define uint16_t int

int mainclient(int cmd, char *message, char *serverhost, uint16_t port, char *buffer, int buffersize, int maxrds);

int mainclient(cmd, message, serverhost, port, buffer, buffersize, maxrds)

       int cmd;
       char *message;        /* HEAD|GET /docpath HTTP/1.0\r\n\r\n */
       char *serverhost;     /* www.bireme.br */
       uint16_t port;        /* 80 */
       char *buffer;         /* results */
       int buffersize;       /* max size */

     {
       /* Fake */
       sprintf(buffer,"%s","HTTP/1.1 200 OK\n...\nContent-Length: 14082\nConnection: close\nContent-Type: text/html\n\n");
       return strlen(buffer);
     }


