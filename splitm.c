#include <stdio.h>
#include <string.h>
#include <ctype.h>  /* tolower */
#include <stdlib.h> /* exit */

#define PC    0
#define MPE   0
#define ANSI  0
#define TRACE 0

#if PC
#include <process.h>
#endif

#if ANSI
void erro(char *s1, char *s2);
#else
void erro();
#endif

char line[16*BUFSIZ+1];

void main(argc,argv)
int argc;
char *argv[];
{
    FILE *fp;
    char filn[64+1];
    char *p,*q;
    long count,skipl;

    if (argc > 1) {
	if (sscanf(argv[1],"%ld",&skipl) != 1)
	    erro("fatal: ",argv[1]);
	while (skipl-- > 0) gets(line);
    }

    p=gets(line);
    count=0;

    while (p) {

	if (strncmp(line,"/******",7))
	    erro("not 7 aster: ",line);

	gets(line); sscanf(line,"%s",filn);
	if (argc > 2) {
	    for (q=filn; *q ; q++)
		if (*q == '.') {
		    *q='\0';
		    break;
		}
	}
	for (q=filn; *q ; q++) 
                if (*q>='A' && *q<='Z') *q = tolower(*q);
        printf("\nWriting file %s.. \n",filn);
#if MPE
	fp=fopen(filn,"w R80");
	fprintf(stderr,"\g\g+++ w R80");
#else
	fp=fopen(filn,"w");
#endif
	if (fp == NULL)
	    erro("Open error: ",filn);
	gets(line); /* 2nd aster line */

        while ((p=gets(line)) != NULL) {
	    if (strncmp(line,"}/******",8) == 0) {    /* final crlf missing */
		fprintf(fp,"}\n");
		count++;
		strcpy(line,"/******");
	    }
	    if (strncmp(line,"/******",7)) {
		fprintf(fp,"%s\n",line);
		count++;
#if TRACE
 printf("%6ld: %s",count,line);
 getchar();
#endif
            }
	    else {
		printf("%ld lines written\n",count);
		fclose(fp);
		count=0;
		break;
	    }
	}
    }

    if (count)
	printf("%ld lines written\n",count);
	
    exit(0);
}

void erro(s1,s2)
char *s1,*s2;
{
    fprintf(stderr,"\n%s%s\n\n",s1,s2);
    exit(1);
}
