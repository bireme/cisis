/* ==----------------------------- mxrun.h ------------------------------== */

#if OPEN30
#define d33_regs              ((MXFUN_XMX *)xmxp)->d33_regs
#define d33_sregs             ((MXFUN_XMX *)xmxp)->d33_sregs
#define newax                 ((MXFUN_XMX *)xmxp)->newax
#endif

#define parmpftoutp           ((MXFUN_XMX *)xmxp)->parmpftoutp
#define parmpftoutfp          ((MXFUN_XMX *)xmxp)->parmpftoutfp
#define parmprologp           ((MXFUN_XMX *)xmxp)->parmprologp
#define parmepilogp           ((MXFUN_XMX *)xmxp)->parmepilogp
#define irecepilog            ((MXFUN_XMX *)xmxp)->irecepilog
#if CICGI
#define wwrec1                ((MXFUN_XMX *)xmxp)->wwrec1
#define wwrec2                ((MXFUN_XMX *)xmxp)->wwrec2
#define cgixpok               ((MXFUN_XMX *)xmxp)->cgixpok
#endif /* CICGI */

#define pgmp                  ((MXFUN_XMX *)xmxp)->pgmp
#define fmtarea               ((MXFUN_XMX *)xmxp)->fmtarea
#define fmtlw                 ((MXFUN_XMX *)xmxp)->fmtlw
#define fmtlines              ((MXFUN_XMX *)xmxp)->fmtlines
#define fmtasize              ((MXFUN_XMX *)xmxp)->fmtasize
#define fp                    ((MXFUN_XMX *)xmxp)->fp

#define ptabtable             ((MXFUN_XMX *)xmxp)->ptabtable
#define ptabwidth             ((MXFUN_XMX *)xmxp)->ptabwidth
#define ptabcategories        ((MXFUN_XMX *)xmxp)->ptabcategories
#define ptabmaxprim           ((MXFUN_XMX *)xmxp)->ptabmaxprim
#define ptabmaxprim           ((MXFUN_XMX *)xmxp)->ptabmaxprim
#define ptabCOUNT             ((MXFUN_XMX *)xmxp)->ptabCOUNT

#define parmtell              ((MXFUN_XMX *)xmxp)->parmtell
#define parmload              ((MXFUN_XMX *)xmxp)->parmload
#define parmtrace             ((MXFUN_XMX *)xmxp)->parmtrace
#define parmcopy              ((MXFUN_XMX *)xmxp)->parmcopy

#define parmdupp              ((MXFUN_XMX *)xmxp)->parmdupp
#define parmdupx              ((MXFUN_XMX *)xmxp)->parmdupx

#define tagfreq               ((MXFUN_XMX *)xmxp)->tagfreq

#define parmiso1              ((MXFUN_XMX *)xmxp)->parmiso1
#define parmiso2              ((MXFUN_XMX *)xmxp)->parmiso2
#define fpfix                 ((MXFUN_XMX *)xmxp)->fpfix

#if RECXPND
#define parmxpn01             ((MXFUN_XMX *)xmxp)->parmxpn01
#define parmxpn02             ((MXFUN_XMX *)xmxp)->parmxpn02
#endif
#if MXGIZP
#define parmgizp              ((MXFUN_XMX *)xmxp)->parmgizp
#define parmgizhp             ((MXFUN_XMX *)xmxp)->parmgizhp
#endif

#define prompt1p              ((MXFUN_XMX *)xmxp)->prompt1p
#define prompt2p              ((MXFUN_XMX *)xmxp)->prompt2p

#define promptxx              ((MXFUN_XMX *)xmxp)->promptxx

#define line                  ((MXFUN_XMX *)xmxp)->line
#define prmbuff               ((MXFUN_XMX *)xmxp)->prmbuff

#define fpseq                 ((MXFUN_XMX *)xmxp)->fpseq
#define fpprm                 ((MXFUN_XMX *)xmxp)->fpprm

#define vfpprm                ((MXFUN_XMX *)xmxp)->vfpprm
#define nfpprms               ((MXFUN_XMX *)xmxp)->nfpprms

#define tmpseq                ((MXFUN_XMX *)xmxp)->tmpseq
#define mx_bup                ((MXFUN_XMX *)xmxp)->mx_bup
#define mx_buplen             ((MXFUN_XMX *)xmxp)->mx_buplen

#define upcrec                ((MXFUN_XMX *)xmxp)->upcrec
#define upirec                ((MXFUN_XMX *)xmxp)->upirec

#if MXFULLINV
#define fullinv_ifnp          ((MXFUN_XMX *)xmxp)->fullinv_ifnp
#define fullinv_ln1p          ((MXFUN_XMX *)xmxp)->fullinv_ln1p
#define fullinv_ln2p          ((MXFUN_XMX *)xmxp)->fullinv_ln2p
#define fullinv_lk1p          ((MXFUN_XMX *)xmxp)->fullinv_lk1p
#define fullinv_lk2p          ((MXFUN_XMX *)xmxp)->fullinv_lk2p
#define fullinv_pstflag       ((MXFUN_XMX *)xmxp)->fullinv_pstflag
#define fullinv_reset         ((MXFUN_XMX *)xmxp)->fullinv_reset
#define fullinv_balan         ((MXFUN_XMX *)xmxp)->fullinv_balan
#define fullinv_keep          ((MXFUN_XMX *)xmxp)->fullinv_keep
#if LIND
#define fullinv_maxmfn         ((MXFUN_XMX *)xmxp)->fullinv_maxmfn
#endif
#define fullinv_master         ((MXFUN_XMX *)xmxp)->fullinv_master
#endif

#if MXIFUPD
#define mstcrec               ((MXFUN_XMX *)xmxp)->mstcrec
#define mstirec               ((MXFUN_XMX *)xmxp)->mstirec
#define ifnp                  ((MXFUN_XMX *)xmxp)->ifnp
#define svdifupx              ((MXFUN_XMX *)xmxp)->svdifupx
#define svdifupy              ((MXFUN_XMX *)xmxp)->svdifupy
#endif

#if MXIFUPD
#define pstflag               ((MXFUN_XMX *)xmxp)->pstflag
#define endup                 ((MXFUN_XMX *)xmxp)->endup
#endif

#if MXIFUPD || MXFST
#define stwp                  ((MXFUN_XMX *)xmxp)->stwp
#define fstpgmp               ((MXFUN_XMX *)xmxp)->fstpgmp
#define lnk1p                 ((MXFUN_XMX *)xmxp)->lnk1p
#define lnk2p                 ((MXFUN_XMX *)xmxp)->lnk2p
#define maxlk1                ((MXFUN_XMX *)xmxp)->maxlk1
#define maxlk2                ((MXFUN_XMX *)xmxp)->maxlk2
#endif

#define vprmp                 ((MXFUN_XMX *)xmxp)->vprmp
#define nprms                 ((MXFUN_XMX *)xmxp)->nprms
#define maxnprm               ((MXFUN_XMX *)xmxp)->maxnprm


/* ------------------------------ mx.h ------------------------------------- */

#if RECXPND
#endif

#if RECGIZM
#endif




/* ------------------------------ mxbol.c --------------------------------- */

#define boolqryp              ((MXFUN_XMX *)xmxp)->boolqryp
#define boolridx              ((MXFUN_XMX *)xmxp)->boolridx
#define boolhits              ((MXFUN_XMX *)xmxp)->boolhits
#define boolh                 ((MXFUN_XMX *)xmxp)->boolh

#define booldbnp              ((MXFUN_XMX *)xmxp)->booldbnp
#define booltell              ((MXFUN_XMX *)xmxp)->booltell
#define booltlir              ((MXFUN_XMX *)xmxp)->booltlir
#if CIB71
#define cib7p                 ((MXFUN_XMX *)xmxp)->cib7p
#define booltmpnam            ((MXFUN_XMX *)xmxp)->booltmpnam
#else /* CIB71 */
#if LIND || CIB64
#define boolnby1              ((MXFUN_XMX *)xmxp)->boolnby1
#define boolnbyb              ((MXFUN_XMX *)xmxp)->boolnbyb
#define boolnby0              ((MXFUN_XMX *)xmxp)->boolnby0
#define booltmpnam            ((MXFUN_XMX *)xmxp)->booltmpnam
#define boolerrno             ((MXFUN_XMX *)xmxp)->boolerrno
#else
#define boolnby2              ((MXFUN_XMX *)xmxp)->boolnby2
#endif
#endif /* CIB71 */

/* ------------------------------ mxrel.c --------------------------------- */

/* ========================== mp_scan/mp_edit ============================= */

#define mp_pgmp               ((MXFUN_XMX *)xmxp)->mp_pgmp
#define mp_n                  ((MXFUN_XMX *)xmxp)->mp_n
#define mp_areap              ((MXFUN_XMX *)xmxp)->mp_areap

/* ====================== mj_scan/mj_edit/mj_join ========================= */

#define mj_vtagp              ((MXFUN_XMX *)xmxp)->mj_vtagp
#define mj_vstwp              ((MXFUN_XMX *)xmxp)->mj_vstwp
#define mj_pgmp               ((MXFUN_XMX *)xmxp)->mj_pgmp
#define mj_ridx               ((MXFUN_XMX *)xmxp)->mj_ridx
#define mj_plustag            ((MXFUN_XMX *)xmxp)->mj_plustag
#define mj_tidx               ((MXFUN_XMX *)xmxp)->mj_tidx
#define mj_areap              ((MXFUN_XMX *)xmxp)->mj_areap
#define mj_buffup             ((MXFUN_XMX *)xmxp)->mj_buffup
#define mj_pjchk              ((MXFUN_XMX *)xmxp)->mj_pjchk
#define mj_n                  ((MXFUN_XMX *)xmxp)->mj_n
#define mj_jmax               ((MXFUN_XMX *)xmxp)->mj_jmax
#define mj_jtag               ((MXFUN_XMX *)xmxp)->mj_jtag

/* ------------------------------ mxtxt.c --------------------------------- */

#define mxtext_found          ((MXFUN_XMX *)xmxp)->mxtext_found
#define mxtext_fdir           ((MXFUN_XMX *)xmxp)->mxtext_fdir
#define mxtext_foff           ((MXFUN_XMX *)xmxp)->mxtext_foff
#define mxtext_pshow          ((MXFUN_XMX *)xmxp)->mxtext_pshow

/* ------------------------------ ciapi.c --------------------------------- */

#define ciapip                ((MXFUN_XMX *)xmxp)->mx_ciapip

/* ==----------------------------- mxrun.h ------------------------------== */

