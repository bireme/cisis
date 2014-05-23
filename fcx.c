#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <fcntl.h>
void main(int argc, char *argv[])
{
        char line[BUFSIZ],*p;
        FILE *fp;
        int xch,xmorx,xmore=24;
        
        if (argc >= 3) if (sscanf(argv[2],"%d",&xmore) != 1) xmore=24;

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

        for (fp=fopen(argv[1],"r"); fp != NULL; ) {
                clrscr(); xmorx=0;
                p=fgets(line,sizeof(line),fp);
                if (strncmp(line,"Comparing files ",16) == 0) {
                        xprint(line);
                        p=fgets(line,sizeof(line),fp);
                }
                xprint(line);
                while ((p=fgets(line,sizeof(line),fp)) != NULL) {
                        if (strncmp(line, "*****", 5) == 0) break;
                        xprint(line); 
                }
                xprint(" \n");
                xprint(line);
                while ((p=fgets(line,sizeof(line),fp)) != NULL) {
                        if (strncmp(line, "*****", 5) == 0) break;
                        xprint(line);
                }
                p=fgets(line,sizeof(line),fp);
                /* xprint(line); */

                if (p == NULL) break;

                xch=getch();
                if (xch == 'q')  break;
                if (xch == 'x')  break;
                if (xch == 0x03) break;
                if (xch == 0x04) break;
        }
	getch();
}
