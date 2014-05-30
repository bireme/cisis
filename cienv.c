/* ----------------------------------------------------------------- CIENV.C */

/* -------------------------------------------------------------- C includes */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
/* ----------------------------------------------------------- CISIS include */
#include "cisis.h"
#include "globdef.h"
#include "appeasy.h"
#include "cienv.h"
/* --------------------------------------------------------------- externals */
extern RECSTRU *vrecp[];
extern TRMSTRU *vtrmp[];
/* -------------------------------------------------------- global variables */
#if STATIC_ENVB
UCHAR  env_buff[NCTOTENVB+1];
#else /* STATIC_ENVB */
UCHAR  *env_buff = NULL;
#endif /* STATIC_ENVB */

/* -------------------------------------------------------------- env_strtab */
UCHAR *env_strtab(s)
UCHAR *s;
{
   UCHAR *p;

   if (s)
      for (p = s; *p; p++) *p = isisuctab[*p];
   return s;
}
/* ------------------------------------------------------------- env_getfile */
BOOLEAN env_getfile(format_name,b,max,ignore)
UCHAR *format_name,
      *b;
int    max;
UCHAR *ignore;
{
   while (*format_name == ' ') format_name++;
   if (*format_name != FMTFILEID) {

      if (strlen(format_name) > max) {
         printf("env_getfile/max");
         fatal(format_name);
      }

      strcpy(b,format_name);
      return TRUE;
   }
   format_name = dbxcipar(NULL,++format_name,'=');
   return easy_getfile(format_name,b,max,ignore);
}
/* ------------------------------------------------------------- env_fmtfile */
BOOLEAN env_fmtfile(format_name,b,max)
UCHAR *format_name,*b;
int    max;
{
   return env_getfile(format_name,b,max,"\n\r");
}
/* -------------------------------------------------------------- env_recall */
long env_recall(nbytes)
long nbytes;
{
   long idx;

   for (idx = maxnrec; --idx >= 0L; )
     if (!vrecp[idx]) { recallok(idx,nbytes > 0L ? nbytes : MAXMFRL); break; }
   if (idx < 0L) fatal("env_recall");
   return idx;
}
/* ------------------------------------------------------------ env_recdeall */
void env_recdeall(idx)
long idx;
{
   if (!vrecp[idx]) return;
   FREE(vrecp[idx]);
   vrecp[idx] = NULL;
   nrecs--;
}
/* ------------------------------------------------------------ env_creatrec */
long env_creatrec(crec,irec)
long crec,irec;
{
   long nxtmfn;

   nxtmfn = VMF0nxtmfn(crec);
   record(irec,VRDBname(crec),nxtmfn);
   VMFRstatus(irec) = ACTIVE;
   VRECrc(irec) = RCNORMAL;
   return nxtmfn;
}
/* ------------------------------------------------------------- env_workrec */
void env_workrec(idx,mfrl)
long idx;
long mfrl;
{
   recallok(idx,mfrl);
   VMFRstatus(idx) = ACTIVE;
   VRECrc(idx) = RCNORMAL;
   VMFRmfn(idx) = 1L;
   VRECtype(idx) = TYPEMFR;
   VMFRmfrl(idx) = VMFRbase(idx) = LEADER;
}
/* ------------------------------------------------------------- env_copyrec */
void env_copyrec(torec,fromrec)
long torec,fromrec;
{
   VRECrc(torec) = VRECrc(fromrec);
   memmove(VMFX(torec),VMFX(fromrec),VMFRmfrl(fromrec));
}
/* -------------------------------------------------------------- env_recupd */
int env_recupd(irec,db)
long   irec;
UCHAR *db;
{
   long crec;
   int  rc;

   crec = env_recall((long)sizeof(M0STRU));
   record(crec,db,0L);
   if (VRECrc(crec) != RCNORMAL && VRECrc(crec) != RCLOCK) rc = VRECrc(crec);
   else rc = recupdat(crec,irec);
   env_recdeall(crec);
   return rc;
}
/* -------------------------------------------------------------- env_recdel */
int env_recdel(irec)
long irec;
{
   if (!vrecp[irec]) fatal("env_recdel");
   if (VRECrc(irec) != RCNORMAL && VRECrc(irec) != RCLOCK) return VRECrc(irec);
   VMFRstatus(irec) = DELETED;
   return env_recupd(irec,VRDBname(irec));
}
/* -------------------------------------------------------------- env_getfld */
UCHAR *env_getfld(p,idx,tag,occ,nc)
UCHAR *p;
long idx;
int  tag,occ,nc;
{
   int i;

   if (occ > nocc(idx,tag)) return (UCHAR *)NULL;
   if ((i = fieldx(idx,tag,occ)) < 0) return (UCHAR *)NULL;
   if (VDIRlen(idx,i) > nc) fatal("env_getfld");
   strncpy(p,VFIELDP(idx,i),VDIRlen(idx,i));
   *(p+VDIRlen(idx,i)) = '\0';
   return p;
}
/* -------------------------------------------------------------- env_subfld */
UCHAR *env_subfld(to,fld,subf)
UCHAR *to,*fld,*subf;
{
   UCHAR *p,
         *find = NULL,
         *t,
         *subf_up = NULL,
         *subf_lw = NULL;

   strupr(subf_up = strall(subf));
   strlwr(subf_lw = strall(subf));
   for (p = fld; ; p++) {
      find = strstr(p,subf_lw);
      if (!find)
         if ((find = strstr(p,subf_up)) == NULL) break;
      p = find;
      if (p == fld) break;
      if (*(p-1) != SUBFIELD_LITERAL) break;
   }
   strdeall(subf_up);
   strdeall(subf_lw);
   if (!find) return (UCHAR *)NULL;
   for (p += strlen(subf),t = to; *p; t++,p++) {
      if (*p == SUBFIELD_ID)
         if (*(p-1) != SUBFIELD_LITERAL) break;
         else t--;
      *t = *p;
   }
   *t = '\0';
   return to;
}
/* ----------------------------------------------------------- env_getsubfld */
UCHAR *env_getsubfld(p,idx,tag,occ,sf,nc)
UCHAR *p;
long idx;
int tag,occ;
UCHAR *sf;
int nc;
{
   if (!env_getfld(p,idx,tag,occ,nc)) return (UCHAR *)NULL;
   return env_subfld(p,p,sf);
}
/* ------------------------------------------------------------ env_allocfld */
UCHAR *env_allocfld(idx,tag,occ)
long idx;
int  tag,occ;
{
   UCHAR *p;
   int i;

   if (occ > nocc(idx,tag)) return (UCHAR *)NULL;
   if ((i = fieldx(idx,tag,occ)) < 0) return (UCHAR *)NULL;
   p = (UCHAR *)ALLOC((ALLOPARM)VDIRlen(idx,i)+1);
   if (!p) fatal("env_allocfld");
   strncpy(p,VFIELDP(idx,i),VDIRlen(idx,i));
   *(p+VDIRlen(idx,i)) = '\0';
   return p;
}
/* ------------------------------------------------------------- env_getlong */
BOOLEAN env_getlong(value,idx,tag,occ,subfld)
long   *value,idx;
int    tag,occ;
UCHAR *subfld;
{
   if (!env_getfld(env_buff,idx,tag,occ,NCENVBUFF)) return FALSE;
   if (subfld)
      if (!env_subfld(env_buff,env_buff,subfld)) return FALSE;
   *value = atol(env_buff);
   return TRUE;
}
/* ----------------------------------------------------------- env_getnumber */
BOOLEAN env_getnumber(value,idx,tag,occ,subfld)
int   *value;
long   idx;
int    tag,occ;
UCHAR *subfld;
{
   long long_value;

   if (!env_getlong(&long_value,idx,tag,occ,subfld)) return FALSE;
   *value = (int)long_value;
   return TRUE;
}
/* -------------------------------------------------------------- env_delfld */
UCHAR *env_delfld(idx,tag,occ)
long idx;
int tag,occ;
{
   if (tag > 0 && occ > 0) sprintf(env_buff,"D%d/%d",tag,occ);
   else
      if (tag > 0) sprintf(env_buff,"D%d",tag);
      else sprintf(env_buff,"D*");
   return fldupdat(idx,env_buff);
}
/* -------------------------------------------------------------- env_addfld */
UCHAR *env_addfld(idx,tag,p,n)
long idx;
int tag;
UCHAR *p;
long n;
{
   long l;

   if (p) {
      l = strlen(p);
      if (l > NCENVBUFF) {
         fprintf(stderr,"l > NCENVBUFF = %"_LD_" > %"_LD_"\n",l,NCENVBUFF);
         fatal("env_addfld/NCENVBUFF");
      }
      if (VMFRmfrl(idx)+l > VRECnbytes(idx)) {
         fprintf(stderr,"VMFRmfrl(idx)+l > VRECnbytes(idx) = %"_LD_" > %"_LD_"\n",VMFRmfrl(idx)+l,VRECnbytes(idx));
         fatal("env_addfld/mfrl");
      }
      if (l <= 0L) sprintf(env_buff,"A%d||",tag);
      else sprintf(env_buff,"H %d %"_LD_" %s",tag,l,p);
   } else sprintf(env_buff,"A%d|%"_LD_"|",tag,n);
   return fldupdat(idx,env_buff);
}
/* -------------------------------------------------------------- env_modfld */
UCHAR *env_modfld(idx,tag,p,n)
long idx;
int tag;
UCHAR *p;
long n;
{
   UCHAR *rtn;

   rtn = env_delfld(idx,tag,0);
   return rtn ? rtn : env_addfld(idx,tag,p,n);
}
/* -------------------------------------------------------------- env_trmall */
long env_trmall(nbytes)
long nbytes;
{
   long idx;

   for (idx = maxntrm; --idx >= 0L; )
     if (!vtrmp[idx]) { trmalloc(idx,nbytes); break; }
   if (idx < 0L) fatal("env_trmall");
   return idx;
}
/* ------------------------------------------------------------ env_trmdeall */
void env_trmdeall(idx)
long idx;
{
   if (!vtrmp[idx]) return;
   FREE(vtrmp[idx]);
   vtrmp[idx]=NULL;
   ntrms--;
}

