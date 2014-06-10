
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

#CICONF1 = -DCIFFI=$(CIFFI) -DLIND=$(LIND) -DISISXL=$(ISISXL) -DISISXL512=$(ISISXL512) -DLINDLUX=$(LINDLUX) -DPROCXSLT=$(PROCXSLT)
CICONF1 = -DCIFFI=$(CIFFI) -DLIND=$(LIND) -DLIND4=$(LIND4) -DISISXL=$(ISISXL) -DISISXL512=$(ISISXL512) -DLINDLUX=$(LINDLUX) -DPROCXSLT=$(PROCXSLT) -D_FILE_OFFSET_BITS=$(_FILE_OFFSET_BITS) -D_LARGEFILE64_SOURCE=$(_LARGEFILE64_SOURCE) -DSIXTY_FOUR=$(SIXTY_FOUR) -DSUPERISIS=$(SUPERISIS) -DMAXMFRL=$(MAXMFRL)
CICONF2 = -DCIWTF=1 -DINCPROCX=1 -DINCPRECX=1 -DEXCFMCGI=0 -DEXCFMXML=0 -D_GLIBC_VERSIONED_SOURCE

CC = cc

# Compilacao com processamento de xslt
# CCFLAGS = -funsigned-char-Wall -I/usr/include/libxml2 -I/usr/local/include/libxslt
# CCLIBS = -lm -lxslt -lxml2 -L/usr/local/lib

CCFLAGS = -funsigned-char -Wall

# Lib para Trigram Server Functions e log e sqrt
CCLIBS = -lm

#Compila
CCOPTS = -c $(CCFLAGS) $(CICONF1) $(CICONF2)

# Link
mx: mx.o mxaot.o ifloadao.o ciifl.o cisisx.o mxrun.o mxrel.o mxtxt.o mxbol.o cib71.o cib72.o cifm1.o cifm2.o cifm3.o cidbx.o cigiz.o cirec.o ciupd.o citrm.o ciupi.o ciiso.o ciutl.o cifst.o ciifu.o cicgi.o
	echo ld mx..
	$(CC) -o mx mx.o mxaot.o ifloadao.o ciifl.o cisisx.o mxrun.o mxrel.o mxtxt.o mxbol.o cib71.o cib72.o cifm1.o cifm2.o cifm3.o cidbx.o cigiz.o cirec.o ciupd.o citrm.o ciupi.o ciiso.o ciutl.o cifst.o ciifu.o cicgi.o $(CCLIBS)

mx.o: mx.c cisis.h
	echo mx..
	$(CC) $(CCOPTS) mx.c

mxaot.o: mxaot.c mx.h cisis.h
	echo mxaot..
	$(CC) $(CCOPTS) mxaot.c

ifloadao.o: ifloadao.c cisis.h
	echo ifloadao..
	$(CC) $(CCOPTS) ifloadao.c

ciifl.o: ciifl.c ciiflh.c ciupi.h cisis.h
	echo ciifl..
	$(CC) $(CCOPTS) ciifl.c

cisisx.o: cisisx.c cisis.h
	echo cisisx..
	$(CC) $(CCOPTS) cisisx.c

mxrun.o: mxrun.c mx.h cisis.h
	echo mxrun..
	$(CC) $(CCOPTS) mxrun.c

mxrel.o: mxrel.c mx.h cisis.h
	echo mxrel..
	$(CC) $(CCOPTS) mxrel.c

mxtxt.o: mxtxt.c mx.h cisis.h
	echo mxtxt..
	$(CC) $(CCOPTS) mxtxt.c

mxbol.o: mxbol.c mx.h cisis.h
	echo mxbol..
	$(CC) $(CCOPTS) mxbol.c

cib71.o: cib71.c cisis.h
	echo cib71..
	$(CC) $(CCOPTS) cib71.c

cib72.o: cib72.c cisis.h
	echo cib72..
	$(CC) $(CCOPTS) cib72.c

cifm1.o: cifm1.c cifmt.h cisis.h
	echo cifm1..
	$(CC) $(CCOPTS) cifm1.c

cifm2.o: cifm2.c cifmt.h cisis.h
	echo cifm2..
	$(CC) $(CCOPTS) cifm2.c

cifm3.o: cifm3.c cifmt.h cisis.h
	echo cifm3..
	$(CC) $(CCOPTS) cifm3.c

cidbx.o: cidbx.c cisis.h
	echo cidbx..
	$(CC) $(CCOPTS) cidbx.c

cigiz.o: cigiz.c cisis.h
	 echo cigiz..
	 $(CC) $(CCOPTS) cigiz.c

cirec.o: cirec.c cisis.h
	 echo cirec..
	 $(CC) $(CCOPTS) cirec.c

ciupd.o: ciupd.c cisis.h
	 echo ciupd..
	 $(CC) $(CCOPTS) ciupd.c

citrm.o: citrm.c cisis.h
	 echo citrm..
	 $(CC) $(CCOPTS) citrm.c

ciupi.o: ciupi.c ciupi.h cisis.h
	 echo ciupi..
	 $(CC) $(CCOPTS) ciupi.c

ciiso.o: ciiso.c cisis.h
	 echo ciiso..
	 $(CC) $(CCOPTS) ciiso.c

ciutl.o: ciutl.c cisis.h
	 echo ciutl..
	 $(CC) $(CCOPTS) ciutl.c

cifst.o: cifst.c cisis.h
	 echo cifst..
	 $(CC) $(CCOPTS) cifst.c

ciifu.o: ciifu.c ciifuh.c ciifu2.c ciupi.h cisis.h
	 echo ciifu..
	 $(CC) $(CCOPTS) ciifu.c

cicgi.o: cicgi.c cisis.h
	 echo cicgi..
	 $(CC) $(CCOPTS) cicgi.c

