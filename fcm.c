/* fcm.c - display a FC DOS command output pausing at end of blocks

   AOT - 03/03/2001, from fcx.c
*/

#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <fcntl.h>

#if 0
char *fgets(char *s, int n, FILE *stream);
wchar_t *fgetws(wchar_t *s, int n, FILE *stream); // Unicode version

Description
Gets a string from a stream.
fgets reads characters from stream into the string s.
The function stops reading when it reads either n - 1 characters or a newline character whichever comes first.
fgets retains the newline character at the end of s.
A null byte is appended to s to mark the end of the string.

Return Value
On success fgets returns the string pointed to by s; it returns NULL on end-of-file or error.
#endif

char *fcx_gets(char line[], int linlen, FILE *fp);

char *fcx_gets(char line[], int linlen, FILE *fp)
{
    static int got=0;
    char *p;
    int left;

    if (fp) return(fgets(line,linlen,fp));

    line[0]='\0'; if (got == EOF) return(NULL);

    for (left=linlen-1, p=line; ; ) {
        if (left <= 0) { *p='\0'; return(line); }
        got=getchar();
        if (got == EOF) { *p='\0'; return(line); }
        *p++ = (char)got;
        left--;
        if (got == '\n') { *p='\0'; return(line); }
    }
}

void main(int argc, char *argv[])
{
        char line[BUFSIZ],*p;
        FILE *fp=NULL;
        int xch,xmorx,xmore=24;

#define xprint(x)                       \
printf("%s",x); xmorx++;                \
if (xmorx % xmore == 0) {               \
    xch=getch();                        \
    if (xch == 'q')  break;             \
    if (xch == 'x')  break;             \
    if (xch == 0x03) break;             \
    if (xch == 0x04) break;             \
    xmorx=0;                            \
}                                       \
/* end xprint */

        if (argc > 1) fp=fopen(argv[1],"r");

        for (;;) {
                clrscr(); xmorx=0;
                p=fcx_gets(line,sizeof(line),fp);
                if (strncmp(line,"Comparing files ",16) == 0) {
                        xprint(line);
                        p=fcx_gets(line,sizeof(line),fp);
                }
                xprint(line);
                while ((p=fcx_gets(line,sizeof(line),fp)) != NULL) {
                        if (strncmp(line, "*****", 5) == 0) break;
                        xprint(line);
                }
                xprint(" \n");
                xprint(line);
                while ((p=fcx_gets(line,sizeof(line),fp)) != NULL) {
                        if (strncmp(line, "*****", 5) == 0) break;
                        xprint(line);
                }
                p=fcx_gets(line,sizeof(line),fp);
                /* xprint(line); */

                if (p == NULL) break;

                xch=getch();
                if (xch == 'q')  break;
                if (xch == 'x')  break;
                if (xch == 0x03) break;
                if (xch == 0x04) break;
        }
        if (fp != NULL) {
                getch();
                fclose(fp);
        }
}
