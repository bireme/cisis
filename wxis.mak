
CIFFI = 0
LIND = 0
LIND4 = 0
ISISXL = 0
ISISXL512 = 0
LINDLUX = 0
PROCXSLT = 0
WWWISIS = 1
_FILE_OFFSET_BITS = 0
_LARGEFILE64_SOURCE = 0
SIXTY_FOUR = 0
SUPERISIS = 0
MAXMFRL=0

#CICONF1 = -DCIFFI=$(CIFFI) -DLIND=$(LIND) -DISISXL=$(ISISXL) -DISISXL512=$(ISISXL512) -DLINDLUX=$(LINDLUX) -DWWWISIS=$(WWWISIS)
CICONF1 = -DCIFFI=$(CIFFI) -DLIND=$(LIND) -DLIND4=$(LIND4) -DISISXL=$(ISISXL) -DISISXL512=$(ISISXL512) -DLINDLUX=$(LINDLUX) -DPROCXSLT=$(PROCXSLT) -D_FILE_OFFSET_BITS=$(_FILE_OFFSET_BITS) -D_LARGEFILE64_SOURCE=$(_LARGEFILE64_SOURCE) -DWWWISIS=$(WWWISIS) -DSIXTY_FOUR=$(SIXTY_FOUR) -DSUPERISIS=$(SUPERISIS) -DMAXMFRL=$(MAXMFRL)
CICONF2 = -DCIWTF=1 -DINCPROCX=1 -DINCPRECX=1 -DEXCFMCGI=0 -DEXCFMXML=0 -D_GLIBC_VERSIONED_SOURCE

CC = cc

# Compilacao com processamento de xslt
# CCFLAGS = -funsigned-char-Wall -I/usr/include/libxml2 -I/usr/local/include/libxslt
# CCLIBS = -lm -lxslt -lxml2 -L/usr/local/lib

CCFLAGS = -funsigned-char -Wall
CCLIBS = -lm
CCOPTS = -c $(CCFLAGS) $(CICONF1) $(CICONF2)

XISOBJS = xis_exec.o \
     xis_comp.o 
CIOBJS  = cidbx.o cirec.o cigiz.o citrm.o \
     ciupd.o cifm1.o cifm2.o cifm3.o \
     ciutl.o cib71.o cib72.o cifst.o ciifu.o ciupi.o \
     ciifl.o cimsrt.o cicgi.o cixml.o ciiso.o
OTHOBJS = easyfc.o easyci.o arglist.o cgilist.o citpv.o freqlist.o \
     hide.o hlfill.o chronometer.o mxaot.o mxrun.o mxrel.o mxtxt.o mxbol.o\
     ifloadao.o

EASYINCS = wxis_src/easyfc.h wxis_src/easyci.h

wxis :$(CIOBJS) $(OTHOBJS) $(XISOBJS) xis.o
	 $(CC) -o wxis $(CIOBJS) $(OTHOBJS) $(XISOBJS) xis.o $(CCLIBS)

cidbx.o   : cisis.h
	    $(CC) $(CCOPTS) cidbx.c

cirec.o   : cisis.h
	    $(CC) $(CCOPTS) cirec.c

cigiz.o   : cisis.h
	    $(CC) $(CCOPTS) cigiz.c

citrm.o   : cisis.h
	    $(CC) $(CCOPTS) citrm.c

ciupd.o   : cisis.h
	    $(CC) $(CCOPTS) ciupd.c

cifm1.o   : cisis.h
	    $(CC) $(CCOPTS) cifm1.c

cifm2.o   : cisis.h
	    $(CC) $(CCOPTS) cifm2.c

cifm3.o   : cisis.h
	    $(CC) $(CCOPTS) cifm3.c

cicgi.o   : cisis.h
	    $(CC) $(CCOPTS) cicgi.c

ciutl.o   : cisis.h
	    $(CC) $(CCOPTS) ciutl.c

cib71.o   : cisis.h cib70.h
	    $(CC) $(CCOPTS) cib71.c

cib72.o   : cisis.h cib70.h
	    $(CC) $(CCOPTS) cib72.c

cifst.o   : cisis.h
	    $(CC) $(CCOPTS) cifst.c

ciifu.o   : cisis.h
	    $(CC) $(CCOPTS) ciifu.c

ciupi.o   : cisis.h
	    $(CC) $(CCOPTS) ciupi.c

ciifl.o   : cisis.h
	    $(CC) $(CCOPTS) ciifl.c

ciiso.o: ciiso.c cisis.h
	$(CC) $(CCOPTS) ciiso.c

cimsrt.o   : cisis.h wxis_src/cimsrt.h
	    $(CC) $(CCOPTS) wxis_src/cimsrt.c

easyfc.o : cisis.h wxis_src/easyfc.h
	    $(CC) $(CCOPTS) wxis_src/easyfc.c

easyci.o   : cisis.h $(EASYINCS)
	    $(CC) $(CCOPTS) wxis_src/easyci.c

arglist.o   : cisis.h wxis_src/arglist.h
	    $(CC) $(CCOPTS) wxis_src/arglist.c

cgilist.o   : cisis.h wxis_src/cgilist.h
	    $(CC) $(CCOPTS) wxis_src/cgilist.c

citpv.o   : cisis.h wxis_src/citpv.h
	    $(CC) $(CCOPTS) wxis_src/citpv.c

freqlist.o   : cisis.h $(EASYINCS) wxis_src/freqlist.h
	    $(CC) $(CCOPTS) wxis_src/freqlist.c

hide.o	: wxis_src/hide.h
	    $(CC) $(CCOPTS) wxis_src/hide.c

hlfill.o   : cisis.h $(EASYINCS) wxis_src/hlfill.h
	    $(CC) $(CCOPTS) wxis_src/hlfill.c

cixml.o   : cisis.h wxis_src/cixml.h
	    $(CC) $(CCOPTS) wxis_src/cixml.c

xis_comp.o   : cisis.h $(EASYINCS) wxis_src/xis_comp.h
	    $(CC) $(CCOPTS) wxis_src/xis_comp.c

xis_exec.o   : cisis.h $(EASYINCS) wxis_src/xis_comp.h wxis_src/xis_exec.h
	    $(CC) $(CCOPTS) wxis_src/xis_exec.c

xis.o : cisis.h $(MONINCS) $(EASYINCS)
	    $(CC) $(CCOPTS) wxis_src/xis.c

chronometer.o : wxis_src/chronometer.h
	$(CC) $(CCOPTS) wxis_src/chronometer.c

mxaot.o : mxaot.c mx.h cisis.h
	$(CC) $(CCOPTS) mxaot.c

mxrun.o : mxrun.c mx.h cisis.h
	$(CC) $(CCOPTS) mxrun.c

mxrel.o: mxrel.c mx.h cisis.h
	$(CC) $(CCOPTS) mxrel.c

mxtxt.o: mxtxt.c mx.h cisis.h
	$(CC) $(CCOPTS) mxtxt.c

mxbol.o: mxbol.c mx.h cisis.h
	$(CC) $(CCOPTS) mxbol.c

ifloadao.o: ifloadao.c cisis.h
	$(CC) $(CCOPTS) ifloadao.c

