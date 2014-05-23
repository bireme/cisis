/* --------------------------------------------------------------- LISTENV.C */

/* /////////////////////////////////////////////////////////////////////////

   [ Version 1.0 ]
   26.May.1999 - Listing environment variables.

   ///////////////////////////////////////////////////////////////////////// */

/* ---------------------------------------------------------- C HEADER FILES */
#include <stdio.h>
#include <string.h>
/* #include <dir.h> */
/* #include <dos.h> */
/* -------------------------------------------------------------------- main */
void main(int argc,       /* quantity of arguments */
          char *argv[],   /* arrray of arguments */
          char *envv[])   /* arrray of environment variables */
{
	int i;
	/* extern char **environ; */
	char curdir[BUFSIZ];

	printf("Content-type: text/plain\n\n");

	for (i = 0; i < argc; i++) {
   	printf("argv[%d]: %s\n",i,argv[i]);
   }
	for (i = 0; envv[i]; i++) {
   	printf("env[%d]: %s\n",i,envv[i]);
   }
/*
	for (i = 0; environ[i]; i++) {
   	printf("environ[%d]: %s\n",i,environ[i]);
   }
*/
/*        getcurdir(0,curdir);
	printf("getcurdir: %s\n",curdir);
*/

} /* main */

