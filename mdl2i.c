/* mdl2i.c    -   NLM Medlars II Unit-Records to CDS/ISIS

   uso: mdl2i <filin> <lrecl>/<filvb0> [create=]<dbnout> [CAN/QAL]
              [skip=[b=]n] [count=n] [tell=n] [test=1] [REPLACE] [+DP]

        <filin>:    arquivo de entrada

        <lrecl>:    tamanho do registro fisico de <filin>
        <filvb0>:   arquivo .vb0

        <dbnout>:   database de saida

        CAN:        rec_id = CAN
        QAL:        rec_id = QAL

        [skip=n]:   Para saltar n registros antes do processamento
                    ou, se skip=b=n, para saltar n bytes

        [count=n]:  Para processar no maximo n registros

        [tell=n]:   Para avisar a cada n registros logicos processados

        [REPLACE]:  Para ler fitas Maint SOF

        [+DP]:      Para gravar <filin>|anoP|dpurecs|urecs|count em mdl2i.log
	
	[force]:    Para nao dar fatal("mdl2i/invalid MDL data");

    Autor:  ccmdl0/AOT
            BRM, 07/06/89
    Alter:  MYB/AOT, 16/05/91
            1. Parametro REPLACE
	    AOT, 23/02/94
	    1. Parametro +DP
    Alter:  MMB/AOT, 15/10/99 (P-day)
            1. Parametro force


----------------------------------------------------------- */


#include <stdio.h>
#include <string.h>

#include "cisis.h"      /* CISIS Interface */
#include "cirun.h"      /* runtime area and defines by AOT */

#define TRACE     0
#define TRACEA    0
#define TRACEL    01
#define TRACVB0   0
#define TRACER    0

#if PC
#define MBUFSIZ 8192
#else
#define MBUFSIZ 32768
#endif

int fx;
int nxleft,ixleft;
unsigned char buffin[MBUFSIZ];            /* readvbf/readvb0 */
int lastleft = 0;

LONGX lastbyt=0,nextbyt=0;
LONGX lastvbx=0;


typedef struct vbcntl {
        short buflen;
        short bufzer;
} VBCNTL;

typedef struct vrcntl {
	short reclen;
	short reczer;
} VRCNTL;

#define VBCSIZ sizeof(VBCNTL)
#define VRCSIZ sizeof(VRCNTL)

VBCNTL vbp;
VRCNTL vrp;


unsigned char buffgo[MBUFSIZ];            /* readvbf/readvb0 */
unsigned char *ibuf = buffgo;

char *parmvb0 = NULL;                             /* readvb0 */
FILE *fpvb0;


unsigned char buffup[MBUFSIZ];            /* fldupdat */

LONGX count=0;
LONGX urecs=0;
LONGX marcfirst=1;
LONGX marclast=0;

typedef struct urstru {
    unsigned char rec_id[3];
    unsigned char recfm;
    unsigned char status[2];
    short reclen;
    LONGX seq_no;
    unsigned char data[1];
} URECORD;

typedef struct datdir {
    short noc;
    short data_ptr;
    short len_oc[1];
} DATDIR;

typedef struct eltdir {
    short eltype;
    short occur_ptr;
} ELTDIR;

typedef struct trailer {
    short anoc;
    short anel;
} TRAILER;


#define TRAILSIZE       sizeof(TRAILER)


#define URIDC           0xc3                            /* EBCDIC Up 'C' */
#define URIDI           0xc9                            /* EBCDIC Up 'I' */
#define URIDT           0xe3                            /* EBCDIC Up 'T' */

#define URIDA           0xc1                            /* EBCDIC Up 'A' */
#define URIDN           0xd5                            /* EBCDIC Up 'N' */

#define URIDQ           0xd8                            /* EBCDIC Up 'Q' */
#define URIDL           0xd3                            /* EBCDIC Up 'L' */

unsigned char uridc = URIDC;
unsigned char uridi = URIDI;
unsigned char uridt = URIDT;
int badurids = 0;

#define URTYPE          0xe4                            /* EBCDIC Up 'U' */

URECORD *urp;
DATDIR *datdirp;
ELTDIR *eltdirp;
TRAILER *trailerp;

short anelleft;
short occurleft;
short ioc;
unsigned char *fldp;

int replace;                                            /* REPLACE switch */

#define REPLOFF         60                              /* REPLACE offset */
#define REPLTAGUI       18                              /* first UI field */
#define REPLTAGST       56                              /* status field */

FILE *fout=NULL;		/* arquivo a mais */
FILE *fplog=NULL;		/* +DP */
LONGX dpurecs=0L;
int dpareal=0;
int dpxdir=EOF;
char dparea[BUFSIZ];

#define NLMTAGDP        354     /* A640 [aaaammdd: A650=650] */

/* prototypes */
#if ANSI
int readmdl(char *ibuf);
int readxaot(char *xbuf, int xbufsize);
int readvb0(void);
#else
int readmdl();
int readxaot();
int readvb0();
#endif

void main(argc,argv)

int argc;
char *argv[];

{
    RECSTRU *recp; /* mandatory for defines REC RDB MF0 MFR DIR, FIELDP */

    LONGX crec,irec,nxtmfn;
    char *dbnp,*batchp;
    int rc;

    int n1;
    int ibufsize;
    LONGX parmskip,parmskpb,parmcount,parmtell;
    int parmtest;
    int parmforce=0;

    unsigned char *p,*q;

    int i,n,loop;


    if (argc < 4) {
        printf("%s",cicopyr("Utility MDL/MDL2I"));
        printf("\n");
        printf("mdl2i <filin> {<lrecl>/<filvb0>} [create=]<dbnout>");
	printf(" [CAN/QAL]\n");
        printf("      [skip=[b=]n] [count=n] [tell=n] [test=1] [REPLACE]");
	printf(" [+DP] [force]\n");
	exit(1);
    }

    if ((fx=open(argv[1],O_RDONLY|O_BINARY)) == -1) fatal(argv[1]);

    if ((n=strlen(argv[2])) >= 3 && strcmp(argv[2]+n-3,"vb0") == 0) {
        parmvb0=argv[2];
        if ((fpvb0=fopen(parmvb0,"r")) == NULL) fatal(parmvb0);
    }
    else {
        if ((sscanf(argv[2],"%d",&ibufsize)) != 1) fatal(argv[2]);
        if (ibufsize > MBUFSIZ) fatal(argv[2]);
    }

    dbnp=argv[3];



    parmskip=0; parmskpb=0; parmcount=999999999L;
    parmtell=0; parmtest=0; replace=0;

    for (i=4; i<argc; i++) {

        if (!strncmp(argv[i],"skip=",5)) {
            p=argv[i]+5; q=p;
            if (*q == 'b' && *(q+1) == '=') {
                if ((sscanf(q+2,"%"_LD_,&parmskpb)) != 1) fatal(argv[i]);
            }
            else if ((sscanf(p,"%"_LD_,&parmskip)) != 1) fatal(argv[i]);
            if (parmskip)
                printf("+++ skip=%"_LD_" recs de %d bytes\n",parmskip,ibufsize);
            if (parmskpb) 
	        printf("+++ skip=%"_LD_" bytes\n",parmskpb);
        }
        else
        if (!strncmp(argv[i],"count=",6)) {
            p=argv[i]+6;
            if ((sscanf(p,"%"_LD_,&parmcount)) != 1) fatal(argv[i]);
            printf("+++ count=%"_LD_" lrecs\n",parmcount);
        }
        else
        if (!strncmp(argv[i],"tell=",5)) {
            p=argv[i]+5;
            if ((sscanf(p,"%"_LD_,&parmtell)) != 1) fatal(argv[i]);
        }
        else
        if (!strncmp(argv[i],"test=",5)) {
            p=argv[i]+5;
            if ((sscanf(p,"%d",&parmtest)) != 1) fatal(argv[i]);
        }
        else
        if (!strcmp(argv[i],"CAN")) {
            uridc = URIDC;
            uridi = URIDA;
            uridt = URIDN;
            printf("+++ mdl2i for CANCERLIT\n");
        }
        else
        if (!strcmp(argv[i],"QAL")) {
            uridc = URIDQ;
            uridi = URIDA;
            uridt = URIDL;
            printf("+++ mdl2i for VOCABULARY\n");
        }
        else
        if (!strcmp(argv[i],"REPLACE")) {
            replace = 1;
            printf("+++ mdl2i for Maint SOF\n");
        }
        else
        if (!strcmp(argv[i],"+DP")) {
	    if ((fplog=fopen("mdl2i.log","w")) == NULL) fatal("mdl2i.log");
            printf("+++ mdl2i.log\n");
	    memset(dparea,'0',dpareal=4); dparea[dpareal]='\0';
        }
	else
        if (!strcmp(argv[i],"force")) {
	    parmforce=1;
            printf("+++ mdl2i forcing bad records!!!\007\007\007\n");
        }
	else
	    fatal(argv[i]);
    }

    /*
        processa parametros
    */
    LSEEK64(fx,parmskpb+parmskip*(LONGX)ibufsize,0);

    /*
        inicializa readmdl e faz primeira leitura
    */
    nxleft=0;
#if 0
    n1=(ibufsize) ? readvbf() : readvb0();
#endif
    n1=readmdl(ibuf);
    
    if (n1 == EOF) fatal(argv[1]);
    count++;


    /*
        inicializa bases de dados
    */
    if (strncmp(dbnp,"create=",7) == 0) {
        rc=recisis0((dbnp+=7));
        /* printf("+++ recisis0=%d\n",rc); */
    }

    RECORD((crec=0L),dbnp,0L);
    nxtmfn=MF0nxtmfn;
    RECORD((irec=crec+1),dbnp,nxtmfn);


    /*
        loop principal
    */
    while (n1 != EOF && count <= parmcount) {

#if ONLYSEERECLEN
printf("+++ %6"_LD_" - buflen=%d reclen=%d \n",
	count,vbp.buflen,vrp.reclen);
count++;
n1 = readmdl(ibuf);
continue;
#endif



        urp = (replace) ? (URECORD *)(ibuf+REPLOFF) : (URECORD *)ibuf;
        batchp=buffup;
#if BEFORE20000330
#else
        *batchp='\0'; 
#endif

#if TRACE
printf("ibuf=%p urp=%p",ibuf,urp);
getchar();
#endif

#if SWAPPED /* PC */
p=(unsigned char *)(&urp->reclen);
urp->reclen=((*p)<<8)+(*(p+1));
#endif

/*      if (urp->rec_id[0] != uridc ||
            urp->rec_id[1] != uridi ||
            urp->rec_id[2] != uridt ||
*/
        if (urp->recfm != URTYPE || TRACE) {
            fprintf(stderr,"+++ %"_LD_"/%"_LD_" recs [#%"_LD_":%d +%"_LD_"+%d] \n",
                urecs,count,
	        lastvbx,vbp.buflen+VBCSIZ,lastbyt,vrp.reclen+VRCSIZ);
            printf("+++ id=%02x%02x%02x fm=%02x st=%02x%02x len=%d\n",
                urp->rec_id[0],urp->rec_id[1],urp->rec_id[2],
                urp->recfm,
                urp->status[0],urp->status[1],
                urp->reclen);
            if (urp->recfm != URTYPE)
                if (++badurids > 50)
                    fatal("badurids");
            goto NEWREC_LAB;
        }

        badurids=0;
        urecs++;
        if (parmtest) {
            sprintf(batchp,
                "A1#%02x%02x%02x#A2#%02x#A3#%02x%02x#A4#%5d#",
                urp->rec_id[0],urp->rec_id[1],urp->rec_id[2],
                urp->recfm,
                urp->status[0],urp->status[1],
                urp->reclen);
            batchp+=strlen(batchp);
        }

        if (replace) {
            sprintf(batchp,
                "A%d#%.8s#A%d#%.2s#",
                REPLTAGUI,ibuf,
                REPLTAGST,urp->status);
            batchp+=strlen(batchp);
        }

        trailerp=(TRAILER *)(&urp->rec_id[0] + urp->reclen - TRAILSIZE);

#if SWAPPED /* PC */
p=(unsigned char *)(&trailerp->anoc);
trailerp->anoc=((*p)<<8)+(*(p+1));
p=(unsigned char *)(&trailerp->anel);
trailerp->anel=((*p)<<8)+(*(p+1));
#endif

        if (trailerp->anoc < trailerp->anel || TRACE) {
            printf("*** count=%"_LD_" anoc=%d anel=%d\n",
                count,
                trailerp->anoc,
                trailerp->anel);
            if (trailerp->anoc < trailerp->anel)
                goto NEWREC_LAB;
        }


        eltdirp=(ELTDIR *)(trailerp); eltdirp-=(anelleft=trailerp->anel);

        for (; anelleft; eltdirp++, anelleft--) {

#if SWAPPED /* PC */
p=(unsigned char *)(&eltdirp->eltype);
eltdirp->eltype=((*p)<<8)+(*(p+1));
p=(unsigned char *)(&eltdirp->occur_ptr);
eltdirp->occur_ptr=((*p)<<8)+(*(p+1));
#endif

            datdirp=(DATDIR *)(&urp->rec_id[0] + eltdirp->occur_ptr);

#if SWAPPED /* PC */
p=(unsigned char *)(&datdirp->noc);
datdirp->noc=((*p)<<8)+(*(p+1));
p=(unsigned char *)(&datdirp->data_ptr);
datdirp->data_ptr=((*p)<<8)+(*(p+1));
#endif
            occurleft=datdirp->noc;


            fldp=(unsigned char *)(&urp->rec_id[0] + datdirp->data_ptr);

            for (ioc=0; occurleft; ioc++, occurleft--) {
#if SWAPPED /* PC */
p=(unsigned char *)(&datdirp->len_oc[ioc]);
datdirp->len_oc[ioc]=((*p)<<8)+(*(p+1));
#endif
                sprintf(batchp,"A%d%c",eltdirp->eltype,0x01);
                batchp+=strlen(batchp);

                for (p=fldp, loop=datdirp->len_oc[ioc]; loop--; p++) {
#if TRACEA
putchar(*p);
#endif
                    if (*p < 0x40) {    /* ebcdic space */
                        printf("*** count=%"_LD_" tag=%d occ=%d chr#%d/%d=%02x\n",
                            count,eltdirp->eltype,ioc+1,
                            datdirp->len_oc[ioc],
                            datdirp->len_oc[ioc]-loop,
                            *p);
			if (loop < 0) {
			    if (parmforce) {
			        printf("*** invalid MDL data!!!\007\007\007\n");
				break;
			    }
			    fatal("mdl2i/invalid MDL data");
			}
                        *batchp++ = 0x40;
                    }
                    else
                        *batchp++ = *p;
                }
                *batchp++ = 0x01;
#if TRACEA
putchar('\n');
#endif
                fldp+=datdirp->len_oc[ioc];
            }
        }


#if BEFORE2000330
        *batchp=NULL;
#else
        *batchp='\0';
#endif

        MFRmfn=nxtmfn++;
        MFRmfrl=LEADER;
        MFRmfbwb=0; MFRmfbwp=0;
        MFRbase=LEADER;
        MFRnvf=0;
        MFRstatus=ACTIVE;

        RECtype=TYPEMFR;
        RECrc=RCNORMAL;

        p=fldupdat(irec,buffup);
        if (p) {
            printf("*** count=%"_LD_"\n",count);
            printf("+++ fldupdat='%s'\n",p);
#if BEFORE2000330
#else
            printf("+++ buffup=");
            for (p=buffup; *p; p++) printf("%c=%02x ",*p,*p);
#endif
            fatal("fldupdat");
        }
#if TRACEL
 printf("Leader - %"_LD_",%d,%"_LD_",%d,%d,%d,%d\n",
  MFRmfn,MFRmfrl,MFRmfbwb,MFRmfbwp,MFRbase,
  MFRnvf,MFRstatus);
#endif

        rc=recupdat(crec,irec);
        if (rc) {
            printf("*** count=%"_LD_"\n",count);
            printf("+++ recupdat=%d\n",rc);
            fatal("recupdat");
        }

	dpurecs++;
        if (fplog) {
            dpxdir=fieldx(irec,NLMTAGDP,1);
            if (dpxdir < 0) {
		sprintf(buffup,"A%d#0000#",NLMTAGDP);
		if (fldupdat(irec,buffup)) fatal(buffup);
                dpxdir=fieldx(irec,NLMTAGDP,1);
	    }
	    else {
	        for (p=FIELDP(dpxdir), loop=DIRlen(dpxdir); loop--; p++)
		    *p = (char )((int)*p - 0xF0 + (int)'0'); /* EBCDIC '0' */
	    }
	    if (urecs == 1) {
	        dpareal=DIRlen(dpxdir); if (dpareal > 4) dpareal=4;
		memmove(dparea,FIELDP(dpxdir),dpareal); dparea[dpareal]='\0';
	    }
        }



NEWREC_LAB:
#if 0
	n1 = (ibufsize) ? readvbf() : readvb0();
#endif
	n1 = readmdl(ibuf);

	if (fplog && (dpxdir >= 0 || n1 == EOF)) {
	    if (n1 == EOF) n=1;
	    else
	        if (DIRlen(dpxdir) < 4) n=1;
		else n=memcmp(FIELDP(dpxdir),dparea,4);
	    if (n) {
#if 0
                fprintf(fplog,"%s|",argv[1]);
		fprintf(fplog,"%s|",argv[3]+7);
#else
                fprintf(fplog,"%s|",dbnp);
#endif
		for (p=dparea, loop=dpareal; loop--; p++)
		    fprintf(fplog,"%c",*p);
                marclast=count - 1;
	        if (n1 == EOF) marclast++;
        	fprintf(fplog,"|%"_LD_"|ur=%"_LD_"|lr=%"_LD_"\n",dpurecs,marcfirst,marclast);
		dpareal=DIRlen(dpxdir); if (dpareal > 4) dpareal=4;
		memmove(dparea,FIELDP(dpxdir),dpareal); dparea[dpareal]='\0';
		dpurecs=0L;
                marcfirst=marclast + 1;


            }
	}
	dpxdir=EOF;

        if (n1 > 0) {
            count++;
            if (parmtell)
                if (!(count % parmtell))
		  fprintf(stderr,"+++ %"_LD_"/%"_LD_" recs [#%"_LD_":%d +%"_LD_"+%d] \n",
			urecs,count,
			lastvbx,vbp.buflen+VBCSIZ,lastbyt,vrp.reclen+VRCSIZ);
	}

    }

    if (count > parmcount)
        count--;


    printf("mdl2i: %"_LD_" unit records     \n",urecs);
    printf("       %"_LD_" logical records\n",count);

    exit(0);
}


int readvb0()                                                  /*
--- ---------                                                   */

{
    int i,n1;

    off_t vboffset;
    int  vblength;


#if PC
    i=fscanf(fpvb0,"%"_LD_"|%d"  ,&vboffset,&vblength);
#else
    i=fscanf(fpvb0,"%"_LD_"|%d\n",&vboffset,&vblength);
#endif

    if (i != 2)
        return(EOF);

#if TRACVB0
printf("vb0: offset=%"_LD_" length=%d\n",vboffset,vblength);
#endif

    if (vblength > MBUFSIZ) {
        fprintf(stderr,"count:     %"_LD_"\n",count);
        fprintf(stderr,"vblength:  %d\n",vblength);
        fprintf(stderr,"MBUFSIZ:   %d\n",MBUFSIZ);
	fatal("readvb0/MBUFSIZ");
    }

    LSEEK64(fx,vboffset,SEEK_SET);
    n1=read(fx,buffin,vblength);

    if (n1 == 0)
        return(EOF);

    if (n1 != vblength) {
        fprintf(stderr,"count:     %"_LD_"\n",count);
        fprintf(stderr,"n1:        %d\n",n1);
        fprintf(stderr,"vblength:  %d\n",vblength);
        fatal("readvb0/vblength");
    }

    return(n1);
}



int readmdl(ibuf)
char *ibuf;
{
    int n1;
#if SWAPPED /* PC */
    unsigned char *p;
#endif
#if TRACER
printf("+++ readmdl \n");
#endif

    if (lastvbx) lastbyt=nextbyt;
    
    if (!lastleft) {
	n1=readxaot((char *)&vbp,VBCSIZ);
	if (n1 ==  EOF) return(n1);
	if (n1 < VBCSIZ) {
            fprintf(stderr,"count: %"_LD_"\n",count);
            fprintf(stderr,"n1: %d\n",n1);
            fprintf(stderr,"lastc: %"_LD_"\n",lastvbx);
            fprintf(stderr,"lastb: %"_LD_"\n",lastbyt);
            fatal("readmdl/read/block length");
        }

#if SWAPPED /* PC */
	p=(unsigned char *)(&vbp.buflen);
	vbp.buflen=((*p)<<8)+(*(p+1));
#endif
	vbp.buflen-=VBCSIZ;
	lastleft=vbp.buflen;
#if TRACER
printf("+++ readmdl - buflen/lastleft=%d/%d \n",vbp.buflen,lastleft);
#endif
	lastvbx++; 
	if (vbp.buflen < VBCSIZ || vbp.buflen > MBUFSIZ) {
            fprintf(stderr,"count: %"_LD_"\n",count);
            fprintf(stderr,"buflen: %d\n",vbp.buflen);
            fprintf(stderr,"lastc: %"_LD_"\n",lastvbx);
            fprintf(stderr,"lastb: %"_LD_"\n",lastbyt);
            fatal("readmdl/block length");
        }
    }

    n1=readxaot((char *)&vrp,VRCSIZ);
    if (n1 < VRCSIZ) {
        fprintf(stderr,"count: %"_LD_"\n",count);
        fprintf(stderr,"n1: %d\n",n1);
        fprintf(stderr,"lastc: %"_LD_"\n",lastvbx);
        fprintf(stderr,"lastb: %"_LD_"\n",lastbyt);
        fatal("readmdl/read/record length");
    }

#if SWAPPED /* PC */
    p=(unsigned char *)(&vrp.reclen);
    vrp.reclen=((*p)<<8)+(*(p+1));
#endif
    if (vrp.reclen < VRCSIZ || vrp.reclen > MBUFSIZ) {
        fprintf(stderr,"count: %"_LD_"\n",count);
        fprintf(stderr,"reclen: %d\n",vrp.reclen);
        fprintf(stderr,"lastc: %"_LD_"\n",lastvbx);
        fprintf(stderr,"lastb: %"_LD_"\n",lastbyt);
        fatal("readmdl/record length");
    }
    lastleft-=VRCSIZ;

    vrp.reclen-=VRCSIZ;
    n1=readxaot(ibuf,vrp.reclen);
    if (n1 < vrp.reclen) {
        fprintf(stderr,"count: %"_LD_"\n",count);
	fprintf(stderr,"reclen: %d\n",vrp.reclen);
        fatal("readmdl/read/record");
    }
    lastleft-=vrp.reclen;

#if TRACER
printf("+++ readmdl - buflen/lastleft=%d/%d reclen=%d \n",
	vbp.buflen,lastleft,vrp.reclen);
#endif

    if (lastleft < 0) fatal("readmdl/lastleft");

    n1=vrp.reclen;
    
    nextbyt=vbp.buflen+VBCSIZ-lastleft;
    
    return(n1);
}


int readxaot(xbuf,xbufsize)
char *xbuf;
int xbufsize;
{
    int nx,n;

    if (nxleft >= xbufsize) {
        nx=xbufsize;
	memmove(xbuf,&buffin[ixleft],xbufsize);
        nxleft-=xbufsize;
        ixleft+=xbufsize;
    }
    else {
        nx=nxleft;
	memmove(xbuf,&buffin[ixleft],nx);
	nxleft=read(fx,buffin,MBUFSIZ);
        ixleft=0;
        if (nx+nxleft) {
            n=xbufsize-nx;
            if (nxleft < n) {
		fprintf(stderr,"count: %"_LD_"\n",count);
                fprintf(stderr,"xbufsize: %d\n",xbufsize);
                fprintf(stderr,"nx: %d\n",nx);
		fatal("readxaot/more bytes expected");
            }
	    memmove(&xbuf[nx],buffin,n);
            nx+=n;
            nxleft-=n;
            ixleft+=n;
        }
        else {
            memset(xbuf,0xff,xbufsize);
            nx=EOF;
        }
    }

    return(nx);
}
