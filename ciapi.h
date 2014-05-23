#ifndef __CIAPI_H
#define __CIAPI_H

/*
	Uso de las funciones de ciapi desde programas en c.
	tratando de modificar los programas existes lo menos posibles.
 */

/*
	En los programas en C usar RECSTRU*, TRMSTR0U* y DBXSTRU* como void*
 */

#if WIN32
#define THREADDEC  __declspec(thread)
#endif /* WIN32 */

#include "cisis.h"
#include "cirun.h"

//nrecs, vrecp, ntrms, vtrmp

#define nrecs			*(cisisnrecs( ciapip ))
#define vrecp			cisisvrecp( ciapip )

#define ntrms			*(cisisntrms( ciapip ))
#define vtrmp			cisisvtrmp( ciapip )

#define ndbxs			*(cisisndbxs( ciapip ))

#define maxnrec		cisismaxnrec( ciapip )
#define maxntrm		cisismaxntrm( ciapip )

// Uso de las variables TRMxxx
#define TRMrc			*(cisisTermTRMrc( trmp ))
#define TRMkey			cisisTermTRMkey( trmp )
#define TRMpmfn		*(cisisTermTRMpmfn( trmp ))
#define TRMptag		*(cisisTermTRMptag( trmp ))
#define TRMpocc		*(cisisTermTRMpocc( trmp ))
#define TRMpcnt		*(cisisTermTRMpcnt( trmp ))
#define TRMpost		*(cisisTermTRMpost( trmp ))
#define TRMlcase		*(cisisTermTRMlcase( trmp ))
#define TRMxytotp		*(cisisTermTRMxytotp( trmp ))
#define TDBname		*(cisisTermTDBname( trmp ))

#define VTRMrc(r)			*(cisisTermVTRMrc( ciapip, r ))
#define VTRMkey(r)		cisisTermVTRMkey( ciapip, r )
#define VTRMxytotp(r)	*(cisisTermVTRMxytotp( ciapip, r ))

#define VTRMpmfn(r)		*(cisisTermVTRMpmfn( ciapip, r ))
#define VTRMptag(r)		*(cisisTermVTRMptag( ciapip, r ))
#define VTRMpocc(r)		*(cisisTermVTRMpocc( ciapip, r ))
#define VTRMpcnt(r)		*(cisisTermVTRMpcnt( ciapip, r ))

#define VTDBname(r)		*(cisisTermVTDBname( ciapip, r ))

// Uso de las variables XXXyyy asociadas a RECSTRU
#define MF0nxtmfn		*(cisisRecordMF0nxtmfn( recp ))
#define RECrc			*(cisisRecordRECrc( recp ))
#define RECtype		*(cisisRecordRECtype( recp ))
#define RECnbytes		*(cisisRecordRECnbytes( recp ))
#define RECdbxp		*(cisisRecordRECdbxp( recp ))
#define RDBmsmfn		*(cisisRecordRDBmsmfn( recp ))
#define RDBvderp		*(cisisRecordRDBvderp( recp ))
#define RDBname		cisisRecordRDBname( recp )
#define RDBvgzrp		*(cisisRecordRDBvgzrp( recp ))
#define MFRmfn			*(cisisRecordMFRmfn( recp ))
#define MFRmfrl		*(cisisRecordMFRmfrl( recp ))
#define MFRmfbwb		*(cisisRecordMFRmfbwb( recp ))
#define MFRmfbwp		*(cisisRecordMFRmfbwp( recp ))
#define MFRnvf			*(cisisRecordMFRnvf( recp ))
#define MFRbase		*(cisisRecordMFRbase( recp ))
#define MFRstatus		*(cisisRecordMFRstatus( recp ))

// Uso de las variables Vxxx
#define VRECdbxp( r )	*(cisisRecordVRECdbxp( ciapip, r ))
#define VRECrc( r )		*(cisisRecordVRECrc( ciapip, r ))
#define VRECtype( r )	*(cisisRecordVRECtype( ciapip, r ))
#define VREClock( r )	*(cisisRecordVREClock( ciapip, r ))
#define VRECwlock( r )	*(cisisRecordVRECwlock( ciapip, r ))
#define VRECgdbl( r )		*(cisisRecordVRECgdbl( ciapip, r ))
#define VRECgdbw( r )		*(cisisRecordVRECgdbw( ciapip, r ))


#define VMFRmfn( r )		*(cisisRecordVMFRmfn( ciapip, r ))
#define VMFRstatus( r ) *(cisisRecordVMFRstatus( ciapip, r ))
#define VMFRnvf( r) 		*(cisisRecordVMFRnvf( ciapip, r ))
#define VMFRmfrl( r )	*(cisisRecordVMFRmfrl( ciapip, r ))
#define VMFRbase( r )	*(cisisRecordVMFRbase( ciapip, r ))


#define VDIRtag( r, x )	cisisRecordVDIRtag( ciapip, r, x )
#define VDIRlen( r, x )	cisisRecordVDIRlen( ciapip, r, x )
#define VFIELDP( irec, xdir ) cisisRecordVFIELDP( ciapip, irec, xdir )

#define VMF0nxtmfn( r )	*(cisisRecordVMF0nxtmfn( ciapip, r ))

#define VMF0mfcxx1( r ) *(cisisRecordVMF0mfcxx1( ciapip, r ))
#define VMF0mfcxx2( r ) *(cisisRecordVMF0mfcxx2( ciapip, r ))
#define VMF0mfcxx3( r ) *(cisisRecordVMF0mfcxx3( ciapip, r ))

#define VMFX( r )			cisisVMFX( ciapip, r )

#define VRDBname( r )	cisisRecordVRDBname( ciapip, r )
#define VRDBnetws( r )	*(cisisRecordVRDBnetws( ciapip, r ))
#define VRDBdelxx( r )	*(cisisRecordVRDBdelxx( ciapip, r ))
#define VRDBewlxx( r )	*(cisisRecordVRDBewlxx( ciapip, r ))
#define VRDBmsopw(r)    *(cisisRecordVRDBmsopw( ciapip, r ))
#define VRDBmsopn(r)		*(cisisRecordVRDBmsopn( ciapip, r ))


//uso de registro ...
#define DIRtag(x)       cisisRecordDIRtag( recp, x )
#define DIRlen(x)       cisisRecordDIRlen( recp, x )
#define FIELDP(xdir)		cisisRecordFIELDP( recp, xdir )

//equivalente a las funciones de CISIS para creacion de RECSTRU
#define record(x,dbn,mfn)			cisisRecord( ciapip, x, dbn, mfn )
#define RECORD(irec,dbn,mfn)		recp=vrecp[record(irec,dbn,mfn)]
#define recallok(irec, nbytes)	cisisRecordAlloc( ciapip, irec, nbytes )
#define RECFREE(x)					cisisRecordDelete( x )
#define recmfp()						cisisRecordrecmfp( recp )

#define recwmast(crecp,recp,comb,comp,newblk,wlen)  cisisrecwmast(crecp,recp,comb,comp,newblk,wlen)
#define recxref(recp,mfn,comb,comp) cisisrecxref(recp,mfn,comb,comp)


//equivalente a las funciones de CISIS para creacion de TRMSTRU
#define term(itrm,dbn,key)			cisisTerm( ciapip,itrm,dbn,key )
#define TERM(itrm,dbn,key)			trmp = vtrmp[cisisTerm( ciapip,itrm,dbn,key )]
#define NXTERM(itrm)					trmp = cisisTermNext( ciapip, itrm )
#define nxterm(itrm)					cisisTermNext( ciapip, itrm )
#define posting(itrm,n)				cisisTermPosting( ciapip, itrm, n)

#define trmalloc( itrm, nbytes)			cisisTermAlloc( ciapip, itrm, nbytes )
#define TRMALLOC( itrm, nbytes)			trmp = cisisTermAlloc( ciapip, itrm, nbytes )

//trace

#define dbxtrace								*(cisisdbxtrace( ciapip ))
#define rectrace								*(cisisrectrace( ciapip ))
#define dectrace								*(cisisdectrace( ciapip ))
#define trmtrace								*(cisistrmtrace( ciapip ))
#define multrace								*(cisismultrace( ciapip ))
#define b70trace								*(cisisb70trace( ciapip ))
#define b40trace								*(cisisb40trace( ciapip ))
#define b50trace								*(cisisb50trace( ciapip ))
#define fmttrace								*(cisisfmttrace( ciapip ))
#define fsttrace								*(cisisfsttrace( ciapip ))
#define cgitrace								*(cisiscgitrace( ciapip ))

// FMT

#define fmt_gener( pgmpp, fmtp )							cisisfmt_gener( ciapip, pgmpp, fmtp )
#define fmt_inter( pgmp, irec, lw, areap, asize )	cisisfmt_inter( ciapip, pgmp,irec, lw, areap, asize)
#define fmt_free( pgmp )									cisisfmt_free( ciapip, pgmp )

#define fmterrxy( pgmp )									cisisfmterrxy( ciapip, pgmp )

#define recfmt(idx,lw,fmt,area,asize)	cisisrecfmt(ciapip,vrecp[idx],lw,fmt,area,asize)
#define recread(recp, mfn)					cisisrecread(ciapip, recp, mfn)
#define recreset(dbnamp, mfn1, mfn2, xrec, parmtell)	cisisrecreset(ciapip, dbnamp, mfn1, mfn2, vrecp[xrec], parmtell)
#define recunlck(irec, option)			cisisrecunlck(ciapip, vrecp[irec], option)



#define recreadl								*(cisisrecreadl( ciapip ))
#define recreadb 								*(cisisrecreadb( ciapip ))
#define recreadp								*(cisisrecreadp( ciapip ))

#define fmt_fsiz								*(cisisfmt_fsiz( ciapip ))
#define fmt_errof								*(cisisfmt_errof( ciapip ))
#define fmt_error								*(cisisfmt_error( ciapip ))

//FST

#define fst_gener(pgmpp, fstspecp)						cisisfst_gener( ciapip, (void**)pgmpp, fstspecp )
#define fst_inter(pgmp, irec, stwp,area1pp, max1,area2pp, max2, qty1p, qty2p)		cisisfst_inter( ciapip,pgmp, vrecp[irec], stwp,area1pp, max1,area2pp, max2,qty1p, qty2p )
#define fst_open(gidbnp, filnamp, treecase)			cisisfst_open( ciapip,gidbnp, filnamp, treecase)
#define fst_clos(treecase)									cisisfst_clos( ciapip, treecase )
#define fst_free(pgmp)										cisisfst_free( ciapip, pgmp )

#define fst_fd									cisisfst_fd( ciapip )
#define fst_fx									cisisfst_fx( ciapip )
#define fst_hdrx								*(cisisfst_hdrx( ciapip ))
#define fst_hdru								*(cisisfst_hdru( ciapip ))
#define fst_hdrt								*(cisisfst_hdrt( ciapip ))
#define fst_hdrp								*(cisisfst_hdrp( ciapip ))
#define fst_errl								*(cisisfst_errl( ciapip ))
#define fst_errp								*(cisisfst_errp( ciapip ))
#define fst_error								*(cisisfst_error( ciapip ))

#define fst_batchup							*(cisisfst_batchup( ciapip ))
#define fst_batchp							*(cisisfst_batchp( ciapip ))
#define fst_batch0							*(cisisfst_batch0( ciapip ))

#define fst_wlupifnp                   *(cisisfst_wlupifnp( ciapip ))
#define fst_wluptrmp                   *(cisisfst_wluptrmp( ciapip ))
#define fst_wlupitrm                   *(cisisfst_wlupitrm( ciapip ))

#define cifstfix								*(cisiscifstfix( ciapip ))
#define cifstfim								*(cisiscifstfix( ciapip ))

//IFL

#define ciiflfix								*(cisisciiflfix( ciapip ))
#define ciiflfim								*(cisisciiflfix( ciapip ))

#define ifl_balan								*(cisisifl_balan( ciapip ))

//ISO

#define iso_open(gidbnp, filnam, lrecb, lrfix, isfix)		cisisiso_open( ciapip, gidbnp, filnam, lrecb, lrfix, isfix)
#define iso_read(crec, irec, ldrtag)			 				cisisiso_read( ciapip, vrecp[irec], ldrtag)
#define iso_creat(gidbnp, filnam, lrecb, lrfox, isfox)	cisisiso_creat( ciapip, gidbnp, filnam, lrecb, lrfox, isfox)
#define iso_write(recp, isoxfs, isoxgs, ldrtag)				cisisiso_write( ciapip, recp, isoxfs, isoxgs, ldrtag)
#define iso_close()													cisisiso_close( ciapip )
// miscelaneas ...

#define DBXmsmfn									*(cisisDbxDBXmsmfn( dbxp ))
#define DBXirecs									*(cisisDbxDBXirecs( dbxp ))
#define DBXipadd									cisisDbxDBXipadd( dbxp )
#define DBXipdel									cisisDbxDBXipdel( dbxp )
#define DBXvgzrp									*(cisisDbxDBXvgzrp( dbxp ))
#define DBXxryyp									*(cisisDbxDBXxryyp( dbxp ))
#define DBXxropw									*(cisisDbxDBXxropw( dbxp ))
#define DBXxropn									*(cisisDbxDBXxropn( dbxp ))
#define DBXmsopn									*(cisisDbxDBXmsopn( dbxp ))

#define VDBXvgzrp( r )							*(cisisDbxVDBXvgzrp( ciapip, r ))
#define VDBXname( r )							cisisDbxVDBXname( ciapip, r )

#define dbnp_dbxnetws(p)						*(cisisDbnpdbxnetws( ciapip, p ))
#define dbnp_dbxitrac(p)						*(cisisDbnpdbxitrac( ciapip, p ))
#define dbnp_dbxirecs(p)						*(cisisDbnpdbxirecs( ciapip, p ))
#define dbnp_dbxipadd(p)						cisisDbnpdbxipadd( ciapip, p )

#define DBXxribpxrmfptr							cisisDBXxribpxrmfptr( dbxp )
#define DBXxribpxrxrpos							*(cisisDBXxribpxrxrpos( dbxp ))

#define dbxopt_fatal								*(cisisdbxopt_fatal( ciapip ))
#define dbxopt_ordwr								*(cisisdbxopt_ordwr( ciapip ))

#define dbxsleep									*(cisisdbxsleep( ciapip ))
#define dbxewlrc									*(cisisdbxewlrc( ciapip ))
#define dbxfloop									*(cisisdbxfloop( ciapip ))
#define dbxwloop									*(cisisdbxwloop( ciapip ))
#define dbxiloop									*(cisisdbxiloop( ciapip ))

#define dbxcdcip									*(cisisdbxcdcip( ciapip ))


#define cicopyr(namep) cisiscicopyr(namep)

#define dbxopen(gidbnp, dbnamp, extp )		cisisdbxopen( ciapip,gidbnp, dbnamp, extp )
#define dbxopenc(gidbnp, filnamp,opnp,opwp,errmsgp, xcreate, xappend)	cisisdbxopenc(ciapip, gidbnp, filnamp,opnp,opwp,errmsgp, xcreate, xappend)
#define dbxopenw(gidbnp, dbnamp,extp,opnp,opwp,errmsgp) cisisdbxopenw( ciapip,gidbnp, dbnamp,extp,opnp,opwp,errmsgp )

#define dbxcipar(gidbnp, argkey, argchar)	cisisdbxcipar( ciapip,gidbnp, argkey, argchar)
#define subfldp(fldp, dlm, lenp)				cisissubfldp( fldp, dlm, lenp )
#define subfldn(sfldp, len)					cisissubfldn( sfldp, len )
#define recisis0( x )							cisisrecisis0( ciapip, x )
#define trmisis0( x )							cisistrmisis0( ciapip, x )
#define dbxstorp(x)								cisisdbxstorp( ciapip, x )
#define fldupdat(irec, batchp)				cisisfldupdat( vrecp[irec], batchp )
#define recupdat(crec,irec)							cisisrecupdat( vrecp[irec] )
#define nocc(irec, tag)							cisisnocc( vrecp[irec], tag )
#define fieldx(irec, tag, occ)				cisisfieldx( vrecp[irec], tag, occ )
#define fldocc(irec, diridx)					cisisfldocc( ciapip, vrecp[irec], diridx )

#define fatal(x)									cisisfatal( ciapip, x )

//#define b70trace    *( cisisb70trace( ciapip ))

#define MFX	cisisMFX( recp )

#define loaductb(gidbnp,uctbp,ucfilp) cisisloaductb(ciapip, gidbnp, uctbp, ucfilp)
#define loadactb(gidbnp,actbp,acfilp) cisisloadactb(ciapip, gidbnp, actbp, acfilp)

//variables de cirun.h
#define bitmask	cisisbitmask( ciapip )
#define xx1extp	cisisxx1extp( ciapip )

#define loadfile(gidbnp,at,atp,areap,asize,lf2x)	cisisloadfile( ciapip, gidbnp, at, atp, areap, asize, lf2x )

#define showcore(msg)				cisisshowcore(ciapip,msg)
#define prtfields(recp,mfn) 		cisisprtfields(recp, mfn)

#define gizmread(gizdbnp, vgizpp, irec )	cisisgizmread(ciapip, gizdbnp, (void*)vgizpp)

#define loadstw(gidbnp, atp,areap,asize,nstws)		cisisloadstw(ciapip, gidbnp, atp,areap,asize,nstws)

#define dbxcinet(dbnamp) 		   cisisdbxcinet(ciapip, dbnamp)

#define prtcontrol(recp, dbname)	cisisprtcontrol(recp, dbname)
#define prtxref(recp, mfn)			cisisprtxref(ciapip, recp, mfn)
#define prtleader(recp, mfn)		cisisprtleader(recp, mfn)
#define prtdir(recp, mfn)			cisisprtdir(recp, mfn);


#define recfgets(crec, irec, line, linsiz, fpconv)		cisisrecfgets(ciapip, vrecp[crec], vrecp[irec], line, linsiz, fpconv)

#define recgizmo(irec, vgizmap)	cisisrecgizmo(ciapip, vrecp[irec], vgizmap)


#define reccopy(upirec, upcrec, recp, mfn) cisisreccopy(ciapip, vrecp[upirec],vrecp[upcrec],recp,mfn)

#define invflush(dbnamp)	cisisinvflush(ciapip, dbnamp);

#define invsetup(dbnamp, loadn0x,loadl0x,loadpst)	cisisinvsetup(ciapip, dbnamp, loadn0x,loadl0x,loadpst)

#define dbxflush(dbnamp)	cisisdbxflush(ciapip, dbnamp)

#define dbxtmpnm(dirtmp, strip, filnamp)				cisisdbxtmpnm(ciapip, dirtmp, strip, filnamp)
#define dbxcipfp			*(cisisdbxcipfp(ciapip))
#define dbxcipok			*(cisisdbxcipok(ciapip))

#define rec_maxmfrl				*(cisisrec_maxmfrl( ciapip ))

#define highv						cisishighv( ciapip )

#define cipnetws					*(cisiscipnetws( ciapip ))

#define mx1extp					*(cisismx1extp( ciapip ))

#define cicgi0(argc, argv, wwwp, taghxsub, taghpfxp)		cisiscicgi0(ciapip, argc, argv, wwwp, taghxsub, taghpfxp)
//#define cicgi1(argc, argv, entries, nentries, cicgi_p)	cisiscicgi1(ciapip, argc, argv, entries, nentries, cicgi_p)

#define upif_end(dbnp, pstflag)			cisisupif_end(ciapip, dbnp, pstflag)
#define upif_init(dbnp)						cisisupif_init(ciapip, dbnp)

#define mstsetup(dbnamp, loadxrf, loadmst)		cisismstsetup(ciapip, dbnamp, loadxrf, loadmst)
#define mstflush(dbnamp)								cisismstflush(ciapip, dbnamp)


#define ifupdat(mdbnp,mfn1,mfn2,idbnp,fstp,stwp,maxlk1,maxlk2,pstflag,endup)	cisisifupdat(ciapip,mdbnp,mfn1,mfn2,idbnp,fstp,pstflag,endup)

#define ifupd_reset			*(cisisifupd_reset(ciapip))
#define ifupd_wrnmsg			*(cisisifupd_wrnmsg(ciapip))

#define svdifupd(lnk1p,qtylk1,lnk2p,qtylk2,olnk1p,oqtylk1,olnk2p,oqtylk2,pstflag,tell)	cisisvdifupd(ciapip,lnk1p,qtylk1,lnk2p,qtylk2,olnk1p,oqtylk1,olnk2p,oqtylk2,pstflag,tell)

#define svdifload(dbnp,filekeys_1,filekeys_2,pstflag,tell)		cisissvdifload(ciapip,dbnp,filekeys_1,filekeys_2,pstflag,tell)

#define fatal_errcod			cisisfatal_errcod(ciapip)

#define recinit()				cisisrecinit( ciapip )
#define trminit()				cisistrminit( ciapip )
#define dbxinit()				cisisdbxinit( ciapip )

// Funciones implementadas en CIAPI

#ifdef __WIN32__
#include <windows.h>
#define THREADDEC __declspec(thread)
#else
#define CALLBACK
#define THREADDEC
#endif

#ifdef __cplusplus
extern "C" {
#endif

void* CALLBACK cisisApplication(LONGX xmaxnrec, LONGX xmaxntrm);
void CALLBACK cisisApplicationDelete(void *ciapip);

LONGX* CALLBACK cisisnrecs(void *xciapip);
void** CALLBACK cisisvrecp( void *xciapip );

LONGX* CALLBACK cisisntrms(void *xciapip);
void** CALLBACK cisisvtrmp( void *xciapip );

LONGX* CALLBACK cisisndbxs(void *xciapip);

LONGX CALLBACK cisismaxnrec(void *xciapip);
LONGX CALLBACK cisismaxntrm(void *xciapip);

LONGX CALLBACK cisisRecord(void *ciapip, LONGX irec, char *dbnp, LONGX mfn);
LONGX CALLBACK cisisRecordAlloc(void *xciapip, LONGX irec, LONGX nbytes);
void CALLBACK cisisRecordDelete( void *xrecp );
int* CALLBACK cisisRecordRECrc(void *xrecp);
int* CALLBACK cisisRecordRECtype( void *xrecp );
LONGX* CALLBACK cisisRecordRECnbytes( void *xrecp );
void** CALLBACK cisisRecordRECdbxp( void *xrecp );
LONGX* CALLBACK cisisRecordMF0nxtmfn( void *xrecp );
char* CALLBACK cisisRecordRDBname( void *xrecp );
LONGX* CALLBACK cisisRecordRDBmsmfn( void *xrecp );
void** CALLBACK cisisRecordRDBvgzrp( void *xrecp );
void** CALLBACK cisisRecordRDBvderp( void *xrecp );
LONGX* CALLBACK cisisRecordMFRmfn( void *xrecp );
FFI* CALLBACK cisisRecordMFRmfrl( void *xrecp );
LONGX* CALLBACK cisisRecordMFRmfbwb( void *xrecp );
UWORD* CALLBACK cisisRecordMFRmfbwp( void *xrecp );
FFI* CALLBACK cisisRecordMFRbase( void *xrecp );
UWORD* CALLBACK cisisRecordMFRnvf( void *xrecp );
UWORD* CALLBACK cisisRecordMFRstatus( void *xrecp );

void* CALLBACK cisisRecordrecmfp( void *xrecp );

int CALLBACK cisisrecwmast(void *xcrecp, void *xrecp, LONGX b, int p, int nblk, FFI wlen);
int CALLBACK cisisrecxref(void *xrecp, LONGX mfn, LONGX *comb, int *comp);

UWORD CALLBACK cisisRecordDIRtag( void *xrecp, int x );
FFI CALLBACK cisisRecordDIRlen( void *xrecp, int x );
char* CALLBACK cisisRecordFIELDP( void *xrecp, int x );

void** CALLBACK cisisRecordVRECdbxp( void *xciapip, LONGX irec );
int* CALLBACK cisisRecordVRECrc( void *xciapip, LONGX irec );
int* CALLBACK cisisRecordVRECtype( void *xciapip, LONGX irec );
int *CALLBACK cisisRecordVREClock( void *xciapip, LONGX irec );
int *CALLBACK cisisRecordVRECwlock( void *xciapip, LONGX irec );
int* CALLBACK cisisRecordVRECgdbl( void *xciapip, LONGX irec );
int* CALLBACK cisisRecordVRECgdbw( void *xciapip, LONGX irec );

LONGX* CALLBACK cisisRecordVMFRmfn( void *xciapip, LONGX irec );
UWORD* CALLBACK cisisRecordVMFRstatus( void *xciapip, LONGX irec );
UWORD* CALLBACK cisisRecordVMFRnvf( void *xciapip, LONGX irec );
FFI *CALLBACK cisisRecordVMFRmfrl( void *xciapip, LONGX irec );
LONGX* CALLBACK cisisRecordVMF0nxtmfn( void *xciapip, LONGX irec );
LONGX* CALLBACK cisisRecordVMF0mfcxx1( void *xciapip, LONGX irec );
LONGX* CALLBACK cisisRecordVMF0mfcxx2( void *xciapip, LONGX irec );
LONGX* CALLBACK cisisRecordVMF0mfcxx3( void *xciapip, LONGX irec );
FFI* CALLBACK cisisRecordVMFRbase(  void *xciapip, LONGX irec );

char* CALLBACK cisisVMFX( void *xciapip, LONGX irec );

char* CALLBACK cisisRecordVRDBname( void *xciapip, LONGX irec );
int* CALLBACK cisisRecordVRDBnetws( void *xciapip, LONGX irec );
int* CALLBACK cisisRecordVRDBdelxx( void *xciapip, LONGX irec );
int* CALLBACK cisisRecordVRDBewlxx( void *xciapip, LONGX irec );
int* CALLBACK cisisRecordVRDBmsopw( void *xciapip, LONGX irec );
int* CALLBACK cisisRecordVRDBmsopn( void *xciapip, LONGX irec );

UWORD CALLBACK cisisRecordVDIRtag( void *xciapip, LONGX irec, int x );
FFI CALLBACK cisisRecordVDIRlen( void *xciapip, LONGX irec, int x );
char* CALLBACK cisisRecordVFIELDP( void *xciapip, LONGX irec, int x );


LONGX CALLBACK cisisTerm(void *xciapip, LONGX itrm, UCHR *dbnp, UCHR *keyp);
void* CALLBACK cisisTermAlloc(void *xciapip, LONGX itrm, LONGX nbytes );
void* CALLBACK cisisTermNext(void *xciapip, LONGX itrm);
LONGX* CALLBACK cisisTermTRMrc(void *xtrmp);
LONGX* CALLBACK cisisTermTRMpost(void *xtrmp);
int * CALLBACK cisisTermTRMlcase( void *xtrmp);
LONGX* CALLBACK cisisTermTRMpmfn(void *xtrmp);
UWORD* CALLBACK cisisTermTRMptag(void *xtrmp);
UWORD* CALLBACK cisisTermTRMpocc(void *xtrmp);
UWORD* CALLBACK cisisTermTRMpcnt(void *xtrmp);
UCHR* CALLBACK cisisTermTRMkey(void *xtrmp);
LONGX* CALLBACK cisisTermTRMxytotp(void *xtrmp);
LONGX CALLBACK cisisTermPosting(void *xciapip, LONGX itrm, LONGX nord);

char** CALLBACK cisisTermTDBname(void *xtrmp);

LONGX* CALLBACK cisisTermVTRMrc( void *xciapip, LONGX itrm );
UCHR* CALLBACK cisisTermVTRMkey( void *xciapip, LONGX itrm );
LONGX* CALLBACK cisisTermVTRMxytotp( void *xciapip, LONGX itrm );

LONGX* CALLBACK cisisTermVTRMpmfn( void *xciapip, LONGX itrm );
UWORD* CALLBACK cisisTermVTRMptag( void *xciapip, LONGX itrm );
UWORD* CALLBACK cisisTermVTRMpocc( void *xciapip, LONGX itrm );
UWORD* CALLBACK cisisTermVTRMpcnt( void *xciapip, LONGX itrm );

char** CALLBACK cisisTermVTDBname( void *xciapip, LONGX itrm );

LONGX* CALLBACK cisisDbxDBXmsmfn( void *xdbxp );
LONGX* CALLBACK cisisDbxDBXirecs( void *xdbxp );
LONGX* CALLBACK cisisDbxDBXipadd( void *xdbxp );
LONGX* CALLBACK cisisDbxDBXipdel( void *xdbxp );
void ** CALLBACK cisisDbxDBXvgzrp( void *xdbxp );

char** CALLBACK cisisDbxDBXxryyp( void *xdbxp );
int* CALLBACK cisisDbxDBXxropw( void *xdbxp );
int* CALLBACK cisisDbxDBXxropn( void *xdbxp );
int* CALLBACK cisisDbxDBXmsopn( void *xdbxp );


void** CALLBACK cisisDbxVDBXvgzrp( void *xciapip, LONGX idbx );
char *CALLBACK cisisDbxVDBXname( void *xciapip, LONGX idbx );

int* CALLBACK cisisDbnpdbxnetws( void *xciapip, char *dbnp );
int* CALLBACK cisisDbnpdbxitrac( void *xciapip, char *dbnp );
LONGX* CALLBACK cisisDbnpdbxirecs( void *xciapip, char *dbnp );
LONGX* CALLBACK cisisDbnpdbxipadd( void *xciapip, char *dbnp );

XRPTR* CALLBACK cisisDBXxribpxrmfptr( void *xdbxp );
XRPTR* CALLBACK cisisDBXxribpxrxrpos( void *xdbxp );

int* CALLBACK cisisdbxtrace( void *xciapip );
int* CALLBACK cisisrectrace( void *xciapip );
int* CALLBACK cisisdectrace( void *xciapip );
int* CALLBACK cisistrmtrace( void *xciapip );
int* CALLBACK cisismultrace( void *xciapip );
int* CALLBACK cisisb70trace( void *xciapip );
int* CALLBACK cisisb40trace( void *xciapip );
int* CALLBACK cisisb50trace( void *xciapip );
int* CALLBACK cisisfmttrace( void *xciapip );
int* CALLBACK cisisfsttrace( void *xciapip );
int* CALLBACK cisiscgitrace( void *xciapip );


LONGX CALLBACK cisisfmt_gener( void *xciapip, void **pgmpp, char *fmtp );
LONGX CALLBACK cisisfmt_inter( void *xciapip, void *xpgmp, LONGX irec, LONGX lw, char *areap, LONGX asize);
void CALLBACK cisisfmt_free( void *xciapip, void *xpgmp);

char* CALLBACK cisisfmterrxy( void *xciapip, void *xpgmp);

LONGX CALLBACK cisisrecfmt(void *xciapip, void *recp,LONGX lw,char *fmt,char *area,LONGX asize);
int CALLBACK cisisrecread(void *xciapip, void* xrecp, LONGX mfn);
void CALLBACK cisisrecreset(void *xciapip, char *dbnamp, LONGX mfn1, LONGX mfn2, void* xrecp, LONGX parmtell);
int CALLBACK cisisrecunlck(void *xciapip, void *xrecp, int option);


FFI* CALLBACK cisisrecreadl( void* xciapip );
LONGX* CALLBACK cisisrecreadb( void* xciapip );
int* CALLBACK cisisrecreadp( void* xciapip );

LONGX* CALLBACK cisisfmt_fsiz( void *xciapip );
LONGX* CALLBACK cisisfmt_errof( void *xciapip );
LONGX* CALLBACK cisisfmt_error( void *xciapip );

LONGX CALLBACK cisisfst_gener( void *xciapip, void **pgmpp, char *fstspecp );
LONGX CALLBACK cisisfst_inter( void *xciapip, void *pgmp, void *xrecp, char *stwp,char **area1pp, LONGX max1,char **area2pp, LONGX max2,LONGX *qty1p, LONGX *qty2p );
int CALLBACK cisisfst_open( void *xciapip, char *gidbnp, char *filnamp, int treecase );
int CALLBACK cisisfst_clos( void *xciapip, int treecase );
void CALLBACK cisisfst_free( void *xciapip, void *pgmpp );

int* CALLBACK cisisfst_fd( void *xciapip );
LONGX* CALLBACK cisisfst_fx( void *xciapip );
int* CALLBACK cisisfst_hdrx( void *xciapip );
int* CALLBACK cisisfst_hdru( void *xciapip );
int* CALLBACK cisisfst_hdrt( void *xciapip );
char** CALLBACK cisisfst_hdrp( void *xciapip );
LONGX* CALLBACK cisisfst_errl( void *xciapip );
char** CALLBACK cisisfst_errp( void *xciapip );
LONGX* CALLBACK cisisfst_error( void *xciapip );

char** CALLBACK cisisfst_batchup( void* xciapip );
char** CALLBACK cisisfst_batchp( void* xciapip );
int* CALLBACK cisisfst_batch0( void* xciapip );

char** CALLBACK cisisfst_wlupifnp( void *xciapip );
TRMSTRU** CALLBACK cisisfst_wluptrmp( void *xciapip );
LONGX* CALLBACK cisisfst_wlupitrm( void *xciapip );

int* CALLBACK cisiscifstfix( void* xciapip );
int* CALLBACK cisiscifstfim( void* xciapip );

int* CALLBACK cisisciiflfix( void* xciapip );
int* CALLBACK cisisciiflfim( void* xciapip );
int* CALLBACK cisisifl_balan( void* xciapip );

int CALLBACK cisisiso_open( void* xcisisp, char *gidbnp, unsigned char filnam[], int lrecb, int lrfix, int isfix);
int CALLBACK cisisiso_read( void* xcisisp, RECSTRU* xrecp, unsigned int ldrtag);
int CALLBACK cisisiso_creat( void* xcisisp, char *gidbnp, unsigned char filnam[], int lrecb, int lrfox, int isfox);
int CALLBACK cisisiso_write( void* xcisisp, void *xrecp, unsigned char isoxfs, unsigned char isoxgs, unsigned int ldrtag);
int CALLBACK cisisiso_close( void* xciapip );

int* CALLBACK cisisdbxopt_fatal( void *xciapip );

int* CALLBACK cisisdbxopt_ordwr( void *xciapip );

int* CALLBACK cisisdbxsleep(  void *xciapip );
int* CALLBACK cisisdbxewlrc(  void *xciapip );
int* CALLBACK cisisdbxfloop(  void *xciapip );
int* CALLBACK cisisdbxwloop(  void *xciapip );
int* CALLBACK cisisdbxiloop(  void *xciapip );

char** CALLBACK cisisdbxcdcip(  void *xciapip );

char* CALLBACK cisiscicopyr(char *namep);
int CALLBACK cisisdbxopen( void *xciapip, char *gidbnp, char *dbname, char *extp);
char* CALLBACK cisisdbxopenc(void *xciapip, char *gidbnp, char *filnamp,int *opnp,int *opwp,char *errmsgp, int xcreate, int xappend);
int CALLBACK cisisdbxopenw(void *xciapip, char *gidbnp, char *dbnamp,char *extp,int *opnp,int *opwp,char *errmsgp);
char* CALLBACK cisisdbxcipar( void *xciapip, char *gidbnp,char *argkey,char  argchar);
UBYTE* CALLBACK cisissubfldp( UBYTE *fldp, UBYTE dlm, FFI *lenp );
FFI CALLBACK cisissubfldn(UBYTE *sfldp, FFI len);
int CALLBACK cisisrecisis0(void *xciapip, char *dbname );
int CALLBACK cisistrmisis0(void *xciapip, char *dbname );
void* CALLBACK cisisdbxstorp(void *ciapip, char *dbnamp);
char* CALLBACK cisisfldupdat( void *xrecp, char *batchp);
int CALLBACK cisisrecupdat( void *xrecp);
int CALLBACK cisisnocc(void* xrecp, int tag);
int CALLBACK cisisfieldx(void *xrecp, int tag, int occ);
int CALLBACK cisisfldocc( void *xciapip, void* xrecp, int diridx );

void CALLBACK cisisfatal(void *xciapip, char *msg);

unsigned char* CALLBACK cisisbitmask( void *xciapip );
char* CALLBACK cisisxx1extp( void *xciapip );

int* CALLBACK cisisb70trace( void *xciapip );

char* CALLBACK cisisMFX( void *xrecp );

/* cirun.h */
int* CALLBACK cisisNXPAGES( void *xciapip );
int* CALLBACK cisisisiswctot( void *xciapip );
unsigned char* CALLBACK cisisisiswctab( void *xciapip );
unsigned char* CALLBACK cisisisisuctab( void *xciapip );

char* CALLBACK cisisloaductb(void *xciapip, char *gidbnp,char *uctbp,char *ucfilp);
char* CALLBACK cisisloadactb(void *xciapip, char *gidbnp,char *actbp,char *acfilp);

char* CALLBACK cisisloadfile( void *xciapip, char *gidbnp, char at, char *atp, char *areap, LONGX asize, char lf2x);
unsigned LONGX CALLBACK cisisshowcore(void *xciapip, char *msg);
LONGX CALLBACK cisisprtfields(RECSTRU *recp, LONGX mfn);

int CALLBACK cisisgizmread(void *xciapip, char *gizdbnp, void **vgizpp);

char* CALLBACK cisisloadstw(void *xciapip, char *gidbnp, char *atp,char *areap,LONGX asize,int *nstws);

int CALLBACK cisisdbxcinet(void *xciapip, char *dbnamp);

LONGX CALLBACK cisisprtcontrol(void *xrecp, char dbname[]);
LONGX CALLBACK cisisprtxref(void *xciapip, RECSTRU *recp, LONGX mfn);
LONGX CALLBACK cisisprtleader(RECSTRU *recp, LONGX mfn);
LONGX CALLBACK cisisprtdir(RECSTRU *recp, LONGX mfn);

int CALLBACK cisisrecfgets(void *xciapip, RECSTRU *crec, RECSTRU *irec, char line[], FFI linsiz, FILE *fpconv);
LONGX CALLBACK cisisrecgizmo(void *xciapip, RECSTRU *recp, VGIZPSTRU *vgizmap);


int CALLBACK cisisreccopy(void *xciapip, RECSTRU *upirec,RECSTRU *upcrec,RECSTRU *recp,LONGX mfn);

void CALLBACK cisisinvflush(void *xciapip, char *dbnamp);

void CALLBACK cisisinvsetup(void *xciapip, UCHR *dbnamp, LONGX loadn0x,LONGX loadl0x,LONGX loadpst);

void CALLBACK cisisdbxflush(void *xciapip, char *dbnamp);

char* CALLBACK cisisdbxtmpnm(void *xciapip, char *dirtmp, int strip, char *filnamp);
FILE** CALLBACK cisisdbxcipfp( void* xciapip );
int* CALLBACK cisisdbxcipok( void* xciapip );

LONGX* CALLBACK cisisrec_maxmfrl(void *xciapip);

char* CALLBACK cisishighv(void *xciapip);

int* CALLBACK cisiscipnetws(void *xciapip);

char** CALLBACK cisismx1extp(void *xciapip);

int CALLBACK cisiscicgi0(void *xciapip, int *argc, char *argv[], char **wwwp, UWORD taghxsub, char *taghpfxp);
int CALLBACK cisiscicgi1(void *xciapip, int *argc, char *argv[], CICGI_ENTRY entries[], int nentries, char *cicgi_p);

void CALLBACK cisisupif_init(void *xciapip, char *dbnp );
int CALLBACK cisisupif_end(void *xciapip, char *dbnp, int pstflag);

void CALLBACK cisismstsetup(void *xciapip, char *dbnamp, LONGX loadxrf, LONGX loadmst);
void CALLBACK cisismstflush(void *xciapip, char *dbnamp);

int CALLBACK cisisifupdat(void *xciapip, char *mdbnp, LONGX mfn1, LONGX mfn2, char *idbnp, void *xfst, int pstflag, int endup);

int* CALLBACK cisisifupd_reset(void *xciapip);
int* CALLBACK cisisifupd_wrnmsg(void *xciapip);


void CALLBACK cisisvdifupd(void *xciapip,char *lnk1p,LONGX qtylk1,char *lnk2p,LONGX qtylk2,char *olnk1p,LONGX oqtylk1,char *olnk2p,LONGX oqtylk2,int pstflag,LONGX tell);

LONGX CALLBACK cisissvdifload(void *xciapip,char *dbnp,char *filekeys_1,char *filekeys_2,int pstflag,LONGX tell);

int CALLBACK cisisfatal_errcod(void *xciapip);

void CALLBACK cisisrecinit(void *xciapip);
void CALLBACK cisistrminit(void *xciapip);
void CALLBACK cisisdbxinit(void *xciapip);


#ifdef __cplusplus
}
#endif /* extern "C" */


#endif	/* __CIAPI_H */
