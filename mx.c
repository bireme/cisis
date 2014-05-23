/* ------------------------------ mx.c ------------------------------------- */

/* Copyright (c) 1990-2005 Adalberto Otranto Tardelli. All rights reserved.
 * Written by A.O.Tardelli 7/90
 * Redistribution and use in source and binary forms are freely permitted
 * provided that the above copyright notice and attribution and date of work
 * and this paragraph are duplicated in all such forms.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 * This program performs a MicroISIS master file dump, formatted display,
 * search and record limits, pattern changes, data base join, field update,
 * master file update and ISO-2709 processing. Input can also be a plain file.
 * Field update commands now perform hierarchical parsing of a plain xml file,
 * markup (e-vamp) of texts and support split of data contents, as trigrams.
 * A description of this program is available in CISIS Interface Application -
 * MX Program. Sao Paulo, BIREME - Latin American and Caribbean Center on
 * Health Sciences Information/PAHO-WHO, April 1991. 12p. See also:
 * - http://www.bireme.br (Products or IsisFamily/Cisis Utilities)
 * Available via anonymous ftp:
 * - ftp://ftp.bireme.br/cisis/pc/windows or
 *   ftp://ftp.bireme.br/cisis/pc/linux
 * Currently works with CISIS Interface 4.52 and BorlandC 5.02 or
 * MSVC 6.0 or later for win32/console application and for Linux/gcc,
 * among other platforms.  AOT, 11/01/2006.
*/


#ifndef MX_SERX
#define MX_SERX   0
#endif


#if MX_SERX

#include "serx.c"      /* AOT/serx.c source code */

#else //MX_SERX

#include <stdio.h>
#include <string.h>

#if CIAPI
#include "ciapi.h"   /* CISIS Interface API header file - includes cisis, implements cirun.h and ci*.c using ci*.?pp
                                                          the z3950server product used the wxis.c source code simply
                                                          by #including ciapi.h
                     */
#else /* CIAPI */
#include "cisis.h"   /* CISIS Interface header file */
#include "cirun.h"   /* CISIS Interface runtime declarations */
#endif /* CIAPI */

void main(argc,argv)
int argc;
char *argv[];
{
    int rc;
#if MXFUN
    char area[BUFSIZ+BUFSIZ+1];
    char *p;
    int iarg;
    for (p=area, iarg=1; iarg<argc; iarg++) {
        strcpy(p,argv[iarg]); p+=strlen(p); if (iarg<argc-1) *p++='\n';
    }
    *p='\0';
    rc=cisis_mx(area);
#else /* MXFUN */
    rc=mxexec(argc,argv);
#endif /* MXFUN */
    exit(rc);
}

#endif //MX_SERX
