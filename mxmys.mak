
CIFFI = 0
LIND = 0
LIND4 = 0
ISISXL = 0
ISISXL512 = 0
LINDLUX = 0
PROCXSLT = 0
_FILE_OFFSET_BITS = 0
_LARGEFILE64_SOURCE = 0
SIXTY_FOUR = 0
SUPERISIS = 0
MAXMFRL=0

CICONF1 = -DCIFFI=$(CIFFI) -DLIND=$(LIND) -DLIND4=$(LIND4) -DISISXL=$(ISISXL) -DISISXL512=$(ISISXL512) -DLINDLUX=$(LINDLUX) -DPROCXSLT=$(PROCXSLT) -D_FILE_OFFSET_BITS=$(_FILE_OFFSET_BITS) -D_LARGEFILE64_SOURCE=$(_LARGEFILE64_SOURCE) -DSIXTY_FOUR=$(SIXTY_FOUR) -DSUPERISIS=$(SUPERISIS) -DMAXMFRL=$(MAXMFRL)
#CICONF1 = -DCIFFI=$(CIFFI) -DLIND=$(LIND) -DISISXL=$(ISISXL) -DISISXL512=$(ISISXL512) -DLINDLUX=$(LINDLUX)
CICONF2 = -DCIWTF=0 -DINCPROCX=0 -DINCPRECX=0 -DEXCFMCGI=1 -DEXCFMXML=1 -D_GLIBC_VERSIONED_SOURCE

CC = cc
CCFLAGS = -funsigned-char -Wall
CCLIBS = -lm
CCOPTS = -c $(CCFLAGS) $(CICONF1) $(CICONF2)

#CC = /usr/bin/gcc296

mxmys: mx.o mxaot.o ifloadao.o ciifl.o cisisx.o mxrun.o mxrel.o mxtxt.o mxbol.o cib71.o cib72.o cifm1.o cifm2.o cifm3.o cidbx.o cigiz.o cirec.o ciupd.o citrm.o ciupi.o ciiso.o ciutl.o cifst.o ciifu.o mys.o cicgi.o
	echo ld mx..
	$(CC) -o mxmys mx.o mxaot.o ifloadao.o ciifl.o cisisx.o mxrun.o mxrel.o mxtxt.o mxbol.o cib71.o cib72.o cifm1.o cifm2.o cifm3.o cidbx.o cigiz.o cirec.o ciupd.o citrm.o ciupi.o ciiso.o ciutl.o cifst.o ciifu.o mys.o cicgi.o $(CCLIBS)

mx.o: mx.c cisis.h
	echo mx..
	$(CC) -DMYSFUN=1 $(CCOPTS) mx.c

mxaot.o: mxaot.c mx.h cisis.h
	echo mxaot..
	$(CC) -DMYSFUN=1 $(CCOPTS) mxaot.c

ifloadao.o: ifloadao.c cisis.h
	echo ifloadao..
	$(CC) -DMYSFUN=1 $(CCOPTS) ifloadao.c

ciifl.o: ciifl.c ciiflh.c ciupi.h cisis.h
	echo ciifl..
	$(CC) -DMYSFUN=1 $(CCOPTS) ciifl.c

cisisx.o: cisisx.c cisis.h
	echo cisisx..
	$(CC) -DMYSFUN=1 $(CCOPTS) cisisx.c

mxrun.o: mxrun.c mx.h cisis.h
	echo mxrun..
	$(CC) -DMYSFUN=1 $(CCOPTS) mxrun.c

mxrel.o: mxrel.c mx.h cisis.h
	echo mxrel..
	$(CC) -DMYSFUN=1 $(CCOPTS) mxrel.c

mxtxt.o: mxtxt.c mx.h cisis.h
	echo mxtxt..
	$(CC) -DMYSFUN=1 $(CCOPTS) mxtxt.c

mxbol.o: mxbol.c mx.h cisis.h
	echo mxbol..
	$(CC) -DMYSFUN=1 $(CCOPTS) mxbol.c

cib71.o: cib71.c cisis.h
	echo cib71..
	$(CC) -DMYSFUN=1 $(CCOPTS) cib71.c

cib72.o: cib72.c cisis.h
	echo cib72..
	$(CC) -DMYSFUN=1 $(CCOPTS) cib72.c

cifm1.o: cifm1.c cifmt.h cisis.h
	echo cifm1..
	$(CC) -DMYSFUN=1 $(CCOPTS) cifm1.c

cifm2.o: cifm2.c cifmt.h cisis.h
	echo cifm2..
	$(CC) -DMYSFUN=1 $(CCOPTS) cifm2.c

cifm3.o: cifm3.c cifmt.h cisis.h
	echo cifm3..
	$(CC) -DMYSFUN=1 $(CCOPTS) cifm3.c

cidbx.o: cidbx.c cisis.h
	echo cidbx..
	$(CC) -DMYSFUN=1 $(CCOPTS) cidbx.c

cigiz.o: cigiz.c cisis.h
	 echo cigiz..
	 $(CC) -DMYSFUN=1 $(CCOPTS) cigiz.c

cirec.o: cirec.c cisis.h
	 echo cirec..
	 $(CC) -DMYSFUN=1 $(CCOPTS) cirec.c

ciupd.o: ciupd.c cisis.h
	 echo ciupd..
	 $(CC) -DMYSFUN=1 $(CCOPTS) ciupd.c

citrm.o: citrm.c cisis.h
	 echo citrm..
	 $(CC) -DMYSFUN=1 $(CCOPTS) citrm.c

ciupi.o: ciupi.c ciupi.h cisis.h
	 echo ciupi..
	 $(CC) -DMYSFUN=1 $(CCOPTS) ciupi.c

ciiso.o: ciiso.c cisis.h
	 echo ciiso..
	 $(CC) -DMYSFUN=1 $(CCOPTS) ciiso.c

ciutl.o: ciutl.c cisis.h
	 echo ciutl..
	 $(CC) -DMYSFUN=1 $(CCOPTS) ciutl.c

cifst.o: cifst.c cisis.h
	 echo cifst..
	 $(CC) -DMYSFUN=1 $(CCOPTS) cifst.c

ciifu.o: ciifu.c ciifuh.c ciifu2.c ciupi.h cisis.h
	 echo ciifu..
	 $(CC) -DMYSFUN=1 $(CCOPTS) ciifu.c

mys.o: mys.c cisis.h
	 echo mys..
	 $(CC) -DMYSFUN=1 $(CCOPTS) mys.c

cicgi.o: cicgi.c cisis.h
	 echo cicgi..
	 $(CC) $(CCOPTS) cicgi.c
