#include <stdio.h>
#include <stdlib.h> /* exit */

#define MAXR	80

main(argc,argv)
int argc;
char *argv[];
{
    long parmmaxr;
    long maxr=MAXR;
    long count,loop,dots=0;
    unsigned char x;

    if (argc > 1)
	if (sscanf(argv[1],"%ld",&parmmaxr) == 1)
	    maxr=parmmaxr;

    for (count=maxr/10+1, x='0'; count--; x++) {
	putchar(x); dots++;
	if (count)
	    for (loop=(x=='0')?8:9; loop--; )
	        { putchar('.'); dots++; }
    }
    for (loop=maxr-dots; loop--; ) putchar('.');
    if (maxr % 80)
	putchar('\n');

    for (count=maxr   , x='1'; count--; x=(x=='9')?'0':x+1)
	putchar(x);
    putchar('\n');
    
    exit(0);
}
