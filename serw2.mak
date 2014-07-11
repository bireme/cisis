
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
CICONF2 = -DCIWTF=1 -DINCPROCX=0 -DINCPRECX=0 -DEXCFMCGI=1 -DEXCFMXML=1 -D_GLIBC_VERSIONED_SOURCE

CC = cc
CCFLAGS = -funsigned-char -Wall
CCLIBS = -lm
CCOPTS = -c $(CCFLAGS) $(CICONF1) $(CICONF2)

serw2: serw2.o cidbx.o cigiz.o cirec.o ciupd.o citrm.o ciutl.o cifm1.o cifm2.o cifm3.o
	echo ld serw2..
	$(CC) -o serw2 serw2.o cidbx.o cigiz.o cirec.o ciupd.o citrm.o ciutl.o cifm1.o cifm2.o cifm3.o $(CCLIBS)

serw2.o: serw2.c cisis.h w2.c w21.c w2arg.c w2ci.c w2ctv.c w2djk.c w2djv.c w2iscms.c w2loc.c w2oid.c w2ojd.c w2out1.c w2outx.c w2ov3.c w2p0.c w2pcol.c w2pcox.c w2pdoc.c w2ralbs.c w2rcol.c w2rdoc.c w2rel1.c w2rhit.c w2rtphix.c w2set.c w2setrt.c w2simil.c w2slng.c w2tkey.c w2tksum.c w2top.c wtfun.c wtrun.c wt.h wtfun.h wtrun.h wtrunz.h wtz.h
	echo serw2..
	$(CC) $(CCOPTS) serw2.c

cifm1.o: cifm1.c cifmt.h cisis.h
	echo cifm1..
	$(CC) -DINCPRECX=0 -DEXCFMCGI=1 -DEXCFMXML=1 $(CCOPTS) cifm1.c

cifm2.o: cifm2.c cifmt.h cisis.h
	echo cifm2..
	$(CC) -DINCPRECX=0 -DEXCFMCGI=1 -DEXCFMXML=1 $(CCOPTS) cifm2.c

cifm3.o: cifm3.c cifmt.h cisis.h
	echo cifm3..
	$(CC) -DINCPRECX=0 -DEXCFMCGI=1 -DEXCFMXML=1 $(CCOPTS) cifm3.c

cidbx.o: cidbx.c cisis.h
	echo cidbx..
	$(CC) $(CCOPTS) cidbx.c

cigiz.o: cigiz.c cisis.h
	 echo cigiz..
	 $(CC) $(CCOPTS) cigiz.c

cirec.o: cirec.c cisis.h
	 echo cirec..
	 $(CC) $(CCOPTS) cirec.c

ciupd.o: ciupd.c cisis.h ciupdclps.c ciupddeco.c ciupddump.c ciupdload.c ciupdmark.c ciupdserx.c ciupdserxpc.c ciupdsock.c ciupdsocx.c ciupdsocxpc.c ciupdsocy.c ciupdsplt.c
	 echo ciupd..
	 $(CC) $(CCOPTS) ciupd.c

citrm.o: citrm.c cisis.h
	 echo citrm..
	 $(CC) $(CCOPTS) citrm.c

ciutl.o: ciutl.c cisis.h
	 echo ciutl..
	 $(CC) $(CCOPTS) ciutl.c

