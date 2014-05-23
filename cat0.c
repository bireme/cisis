/* --------------------------------- cat -------------------------------- */

#include <stdio.h>
#include <string.h>

#define PC     1
#define ANSI   1

#if PC
#include <dos.h>
#include <dir.h>
#include <io.h>
#include <process.h>
#endif

#define TRACE	0

#if PC
struct ffblk ffblk;
char drive[MAXDRIVE];
char dir[MAXDIR];
char file[MAXFILE];
char ext[MAXEXT];
int flag;
#else
#define MAXPATH 4096
#endif

char *parmv;
char *parms;
char *parmm;
LONGX parml;
char *parmmc;

int lastchar='\n';
int done;

#if ANSI
void concat(FILE *fp);
#else
void concat();
#endif

void concat(fp)
FILE *fp;
{
    int c;
    LONGX countl;

    countl=parml;
    while ((c=fgetc(fp)) != EOF) {
	if (c == '\n')
	    countl=parml;
	else
            if (!countl) {
	        putchar('\n');
	        countl=parml;
	    }
	putchar(c); lastchar=c;
	countl--;
    }
}

void main(argc,argv)
int argc;
char *argv[];
{
    int iarg;
    char *argvp;
    char path[MAXPATH];
    char filename[MAXPATH];
    FILE *fp;
    char *p;


    parmv=parms=parmm=parmmc=NULL; parml=999999999L;

    if (argc == 1) {
	concat(stdin);
	exit(0);
    }

    for (iarg=1; iarg < argc; iarg++) {
	argvp=argv[iarg];
#if TRACE
 fprintf(stderr,"+++arg '%s'\n",argvp);
#endif
        if (*argvp == '-') {
	    if (*(argvp+1) == 'l') {
		sscanf(argvp+2,"%ld",&parml);
		fprintf(stderr,"+++ f=%ld bytes\n",parml);
		continue;
	    }
	    for (p=argvp+1; *p; p++)
		switch (*p) {
		    case 'v':
			parmv=p;
			break;
		    case 's':
			parms=p;
			break;
		    case 'm':
			parmm=p;
			break;
		    case 'c':
			parmmc=p;
			break;
		    default:
			fprintf(stderr,"*** invalid option: %s\n",p);
		}
	    continue;	/* next argv */
	}

        strcpy(path,argvp);
#if TRACE
 fprintf(stderr,"+++path '%s'\n",path);
#endif

#if PC
        flag=fnsplit(argvp,drive,dir,file,ext);
if (parms || TRACE)
 fprintf(stderr,"+s+ drive=%s, dir=%s, file=%s, ext=%s\n",drive,dir,file,ext);

        done=findfirst(path,&ffblk,0xFF);
        while (!done) {
            if (ffblk.ff_attrib & FA_ARCH) {
	        strcpy(filename,drive);
	        strcat(filename,dir);
	        strcat(filename,ffblk.ff_name);
#else
	        strcpy(filename,path);
#endif
	        if ((fp=fopen(filename,"r")) != NULL ) {
		  if (parmv || TRACE) fprintf(stderr,
#if PC
			"+v+% 10ld bytes in %s\n",ffblk.ff_fsize,filename);
#else 
		        "+v+ %s\n",filename);
#endif
		  if (parmm || parmmc || TRACE) {
		      if (lastchar != '\n') putchar('\n');
		      if (parmmc) printf("\f\n");
  printf(       "/******************************************************\n");
  printf(       "%s\n",filename);
  printf(       "******************************************************/\n");
		  }
	          concat(fp);
		  fclose(fp);
		}
#if PC
	    }
            done=findnext(&ffblk);
	}
#endif
    }
    
    exit(0);
}
