/* ----------------------------------------------------------------- CITPV.C */

/* /////////////////////////////////////////////////////////////////////////

   17.Jun.1997 - Adapted from PREVTRM.CPP and PREVTRM.HPP sources (HEITOR).

   ///////////////////////////////////////////////////////////////////////// */

/* -------------------------------------------------------------- C includes */
#include <string.h>
/* ---------------------------------------------------------------- includes */
#include "../cisis.h"
#define CIENV	0
#if CIENV
#include "globdef.h"
#include "cienv.h"
#else
#include "easyfc.h"
#include "easyci.h"
#endif
#include "citpv.h"

/* ................................................. Previous Term functions */
void IniPreviousTerm(prvtrm)
STRUCT_PREVTERM *prvtrm;
{
 memset(prvtrm,0x00,sizeof(prvtrm));
 PT_CharLimits (prvtrm);
}

void PT_CharLimits (prvtrm)
STRUCT_PREVTERM *prvtrm;
{
 int Count = 0;
 while ((Count < 32) && (isiswctab[Count] == 0)) /* Vai ate o caracter de espaco */
  Count++;
 prvtrm->PT_FirstChar = Count;

 Count = 255;
 while ((Count > prvtrm->PT_FirstChar) && (isiswctab[Count] == 0))
  Count--;
 prvtrm->PT_LastChar = Count;
}

void PT_Search(prvtrm)
STRUCT_PREVTERM *prvtrm;
{
 /* Realiza busca horizontal do termo previo.
    *** Vai-se cortando a palavra no meio ate' que a chave encontrada
    *** por xterm seja diferente da chave inicial, a partir dai'
    *** parte-se para a busca vertical.
 */

 unsigned char Ch;
 unsigned char BCh;
 unsigned char ECh;
 unsigned char MCh;
 size_t CLen;

 prvtrm->PT_Begin = prvtrm->PT_WorkKey + (int)prvtrm->PT_DifPosition;
 prvtrm->PT_End   = prvtrm->PT_WorkKey + strlen(prvtrm->PT_WorkKey) - 1;

 while (prvtrm->PT_Begin < prvtrm->PT_End)
 {
  prvtrm->PT_Middle = prvtrm->PT_Begin + (prvtrm->PT_End - prvtrm->PT_Begin)/2;

  if (prvtrm->PT_Begin == prvtrm->PT_End - 1)
   prvtrm->PT_Middle = prvtrm->PT_End;

  Ch = *prvtrm->PT_Middle;
  *prvtrm->PT_Middle = '\0';

  term (prvtrm->PT_iTrm, (UCHR*)prvtrm->PT_Trmp->trmdbxp->dbxname,
     (UCHR*)prvtrm->PT_WorkKey);

  if (strcmp ((char*)prvtrm->PT_Trmp->trmkey, (char*)prvtrm->PT_Key))
  {
   *prvtrm->PT_Middle = Ch;
    prvtrm->PT_Begin  = prvtrm->PT_Middle;
  }
  else
   prvtrm->PT_End = prvtrm->PT_Middle - 1;
 }

 /* Testa se ja' chegou ao termo previo. */
 strcpy ((char*)prvtrm->PT_AuxWorkKey, (char*)prvtrm->PT_Trmp->trmkey);
 nxterm (prvtrm->PT_iTrm);
 /* RPIVA: Acrescentei o teste do fim do invertido */
 if (prvtrm->PT_Trmp->trmrc != RCEOF &&
  strcmp ((char*)prvtrm->PT_Trmp->trmkey, (char*)prvtrm->PT_Key) == 0)
 {
  strcpy ((char*)prvtrm->PT_WorkKey, (char*)prvtrm->PT_AuxWorkKey);
  return;
 }

 CLen = strlen ((char*)prvtrm->PT_WorkKey);

 while (CLen <= LE2)
 {
  BCh = prvtrm->PT_FirstChar;
  ECh = prvtrm->PT_LastChar;

  while (BCh < ECh)
  {
   MCh = BCh + (ECh - BCh) / 2;

   if (BCh == ECh - 1)
   MCh = ECh;

   prvtrm->PT_WorkKey[CLen - 1] = MCh;

   term (prvtrm->PT_iTrm, (UCHR*)prvtrm->PT_Trmp->trmdbxp->dbxname,
      (UCHR*)prvtrm->PT_WorkKey);

   if (strcmp ((char*)prvtrm->PT_Trmp->trmkey, (char*)prvtrm->PT_Key) < 0)
    BCh = MCh;
   else
    ECh = MCh - 1;
  }

  prvtrm->PT_WorkKey[CLen - 1] = BCh;
  prvtrm->PT_WorkKey[++CLen] = '\0';

  term (prvtrm->PT_iTrm, (UCHR*)prvtrm->PT_Trmp->trmdbxp->dbxname,
     (UCHR*)prvtrm->PT_WorkKey);
  strcpy ((char*)prvtrm->PT_AuxWorkKey, (char*)prvtrm->PT_Trmp->trmkey);
  nxterm (prvtrm->PT_iTrm);

  if (strcmp ((char*)prvtrm->PT_Trmp->trmkey, (char*)prvtrm->PT_Key) == 0)
  {
   strcpy ((char*)prvtrm->PT_WorkKey, (char*)prvtrm->PT_AuxWorkKey);
   return;
  }
 }
}

LONGX PT_FindPreviousKey(prvtrm,limit,itrm,previous)
STRUCT_PREVTERM *prvtrm;
UCHAR           *limit;
LONGX             itrm;
UCHAR           *previous;
{
 if (!limit || !vtrmp[itrm] || (*(VTRMkey(itrm)) == '\0'))
 {
  strcpy ((char*)previous, "");
  return 0;
 }

 prvtrm->PT_Trmp = vtrmp[itrm];
 prvtrm->PT_iTrm = itrm;

 strncpy ((char*)prvtrm->PT_Limit, (char*)limit, LE2);
 prvtrm->PT_Limit [LE2] = '\0';
 /* GetIsisUctab (prvtrm->PT_Limit, prvtrm->PT_Limit); */
#if CIENV
 env_strtab(prvtrm->PT_Limit);
#else
 eci_uctab(prvtrm->PT_Limit);
#endif

 strcpy ((char*)prvtrm->PT_Key, (char*)prvtrm->PT_Trmp->trmkey);
 strcpy ((char*)prvtrm->PT_WorkKey, (char*)prvtrm->PT_Key);

 /* Verifica se limite inferior e' menor que a chave atual. */
 if (strcmp ((char*)prvtrm->PT_Limit, (char*)prvtrm->PT_Key) >= 0)
 {
  strcpy ((char*)previous, "");
  return 0;
 }

 /* Verifica se existe um termo previo. */
 term (itrm, (UCHR *)VTDBname(itrm), prvtrm->PT_Limit);
 if (!strcmp ((char*)prvtrm->PT_Trmp->trmkey, (char*)prvtrm->PT_Key))
 {
  strcpy ((char*)previous, "");
  return 0;
 }

 /* Encontra a posicao de inicio das diferencas entre a chave e o limite. */
 prvtrm->PT_DifPosition = 0;
 while ((prvtrm->PT_Limit[(int)prvtrm->PT_DifPosition]!= '\0') &&
   (prvtrm->PT_Limit[(int)prvtrm->PT_DifPosition] == prvtrm->PT_Key[(int)prvtrm->PT_DifPosition]))
  prvtrm->PT_DifPosition++;


 /* Efetivamente encontra o termo previo. */
 PT_Search (prvtrm);

 strcpy ((char*)previous, (char*)prvtrm->PT_WorkKey);

 return 1;
}
/* ............................................................ TermPrevious */
LONGX TermPrevious(prvtrm,itrm,prefix,key)
STRUCT_PREVTERM *prvtrm;
LONGX   itrm;
UCHAR *prefix;
UCHAR *key;
{
   if (!PT_FindPreviousKey(prvtrm,prefix,itrm,key)) return -1L;
   term(itrm,VTDBname(itrm),key);
   return VTRMxytotp(itrm);
}
