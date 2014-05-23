/* ---------------------------------------------------------------- HLFILL.C */

/* /////////////////////////////////////////////////////////////////////////

   [ Version 1.0 ]
   01.Dez.1998 - High light text filler.

   ///////////////////////////////////////////////////////////////////////// */

/* ---------------------------------------------------------- C HEADER FILES */
#include <string.h>
/* ------------------------------------------------------------ HEADER FILES */
#include "../cisis.h"
#include "easyfc.h"
#include "easyci.h"
#include "hlfill.h"

/* ================================================================= hl_find */
LONGX hl_find(char *comp,      		/* compare text */
             char *key,       		/* find key */
             BOOLEAN truncate)		/* truncate flag */
{
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    1. Set key size
   2. Loop all characters
   3. Find key
   4. Check "word" separator
   5. Return character position where the key was found
   6. Return key not found indicator

   1.0 - 01.Dez.1998
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
   unsigned char *p;	/* auxiliary loop pointer */
   LONGX pos;         /* position in the string */
   LONGX findsize;    /* size of the find key */

    /* 1 */
   findsize = strlen(key);

    /* 2 */
   for (p = comp,pos = 0L; *p; p++,pos++) {

        /* 3 */
      if (*p != *key) continue;
      if (strncmp(p,key,findsize) != 0) continue;

        /* 4 */
      if (pos)
         if (isiswctab[*(p-1)]) continue;
      if (!truncate)
         if (*(p+findsize))
            if (isiswctab[*(p+findsize)]) continue;

        /* 5 */
      return pos;

   } /* for */

    /* 6 */
   return -1L;
}
/* ================================================================== hl_put */
char *hl_put(char *origb,   /* original text */
             char *key,     /* find key */
             char *pfx,     /* prefix text */
             char *sfx,     /* suffix text */
             char *copyb)   /* destination text */
{
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    1. Original text upper case copy
   2. Texts size
   3. Check truncate character
   4. Scan all occurrences of the key
   5. Find the key
   6. Check if the key was found
   7. Inserts prefix and suffix in the destination text
   8. Inserts the remainding text
   9. Garbage collector
   10. Return destination text pointer

   1.0 - 01.Dez.1998
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
   unsigned char *upperb;    		/* upper case of original text */
   unsigned char *findkey;   		/* find key */
   unsigned char *p;         		/* upper case of original text loop pointer */
   unsigned char *c;         		/* destination text loop pointer */
   unsigned char *o;					/* original text loop pointer */
   LONGX lenkey;       	/* size of the find key */
   LONGX lenpfx;       	/* size of the prefix key */
   LONGX lensfx;       	/* size of the suffix key */
   LONGX offset;       	/* offset of characters from the beginning of the scan */
   BOOLEAN truncate;		/* key truncate flag */

    /* 1 */
   upperb = eci_uctab(strdup(origb));
   findkey = strdup(key);

    /* 2 */
   lenkey = strlen(findkey);
   lenpfx = strlen(pfx);
   lensfx = strlen(sfx);

    /* 3 */
   if (*(findkey+lenkey-1) != '$') truncate = FALSE;
   else {
      truncate = TRUE;
      lenkey--;
      *(findkey+lenkey) = '\0';
   }

    /* 4 */
   for (p = upperb,c = copyb,o = origb; *p; p += offset+lenkey) {

        /* 5 */
      offset = hl_find(p,findkey,truncate);

        /* 6 */
      if (offset < 0) break;

        /* 7 */
      strncpy(c,o,offset);  c += offset; o += offset;
      strcpy (c,pfx);       c += lenpfx;
      strncpy(c,o,lenkey);  c += lenkey; o += lenkey;
      strcpy (c,sfx);       c += lensfx;

   } /* for */

    /* 8 */
   strcpy(c,o);

    /* 9 */
   efc_free(upperb);
   efc_free(findkey);

    /* 10 */
   return copyb;
}

