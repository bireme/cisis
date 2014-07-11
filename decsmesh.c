/* --------------------------- decsmesh.c ----------------------------- */

/* ========================================================================
        Field "DESCRIPTOR" DeCS versus MeSH

        Author: decs9a.c/AOT/RSP, April 1, 1991 (MIT B-day)
	
	Alter: AOT, 30/06/96
	1. PC
	2. Uses std uctab
	3. CISIS Copyright

======================================================================== */

#include <stdio.h>
#include <fcntl.h>
#include <string.h>

#define TRACE1 01
#define TRACE2 01
#define TRACE3 0
#define TRACE4 0

#include "cisis.h"       /* CISIS Interface */
#include "cirun.h"       /* runtime area and defines by AOT */

#define MAXLIN 100
#define MAXCOL 60


unsigned char buffup[MAXMFRL+MAXMFRL];
unsigned char key[BUFSIZ];
unsigned char id[BUFSIZ];

unsigned char cats[2][MAXLIN][MAXCOL+1];
unsigned char *exclpfxp[MAXLIN];
int exclpfxl[MAXLIN];
int nexclpfxs;

#if ANSI
int getcat(int idx, LONGX irec, int tag);
int storcats(int idx, int tag);
#else
int getcat();
int storcats();
#endif

/* main */
#if ANSI
void main (int argc, char *argv[])
#else
main(argc,argv)
int argc;
char *argv[];
#endif
{
    RECSTRU *recp;      /* RECORD */
    TRMSTRU *trmp;      /* TERM/NXTERM */
    LONGX irec1,irec2,upcrec,upirec;
    LONGX itrm2;
    LONGX mfn1,mfn2;

    char *dbn1,*dbn2,*dbnout;

    int tag1,tag2,tagcat1,tagcat2;
    int occ1,occ2;
    LONGX count,errors,empty,xtell;
    int xdir,loop,n,len,xd,lx;
    unsigned char *batchp,*p;

    if (argc < 6) {
        printf("%s",cicopyr("Utility DECSMESH"));
        printf("\n");
        printf("decsmesh <db1> <t1,c1> <db2> <t2,c2> <dbout> [<x1>[<x2>]..]\n");
        printf("\n");
        printf(" <db1>                  -> decs/mesh master file (*)\n");
        printf(" <t1,c1>                -> tag_term,tag_cat \n");
        printf(" <db2>                  -> mesh/decs data base \n");
        printf(" <t2,c2>                -> tag_term,tag_cat \n");
        printf(" <db1out>               -> master file for errors \n");
        printf(" <x1>                   -> exclude cats with prefix <x1> \n");
        printf(" <x2>                   -> exclude cats with prefix <x2> \n");
        printf("  .                         \n");
        printf("  .                         \n");
        printf(" \n");
        printf("       (*) if decs \n");
        printf("              if present 107 \n");
        printf("                  process term=term+\" (NON MESH)\" \n");
        printf("              else \n");
        printf("                  skip term if 106 = p or s \n");
        printf("           else   \n");
        printf("              always process term \n");
        printf(" \n");
	exit(1);
    }

    /* get parms */
    dbn1=argv[1];
    if (sscanf(argv[2],"%d,%d",&tag1,&tagcat1) != 2) fatal(argv[2]);

    dbn2=argv[3];
    if (sscanf(argv[4],"%d,%d",&tag2,&tagcat2) != 2) fatal(argv[4]);

    dbnout=argv[5];

    for (nexclpfxs=0, loop=6; loop < argc; loop++) {
        exclpfxp[nexclpfxs]=argv[loop]; exclpfxl[nexclpfxs]=strlen(argv[loop]);
        nexclpfxs++;
    }

    /* ve entrada 1 */
    RECORD(irec1=nrecs,dbn1,0L);

    xtell=MF0nxtmfn/10;
    if (xtell < 1)
        xtell=1;

    /* ve entrada 2 */
    RECORD(irec2=nrecs,dbn2,0L);
    TERM(itrm2=ntrms,dbn2,"!");


    /* create saida */
    recisis0(dbnout);
    record((upcrec=nrecs),dbnout,0L);
    record((upirec=nrecs),dbnout,1L);

    count=errors=empty=0;

    /*
        processing loop
    */
    for (RECORD(irec1,dbn1,mfn1=1L); ; RECORD(irec1,dbn1,++mfn1)) {

        /* tell */
        if ((++count % xtell) == 0)
	    printf("+++ %"_LD_" (%"_LD_",%"_LD_")\n",count,errors,empty);

        if (RECrc == RCEOF) break;
        if (RECrc != RCNORMAL) continue;

        /* get key */
        if ((xdir=fieldx(irec1,tag1,1)) < 0) {
            /* printf("*** mfn=%"_LD_" tag=%d -> Absent\n",mfn1,tag1); */
            /* fatal("Tag1 absent"); */
            empty++;
            continue;
        }
        len=DIRlen(xdir);

        /* find it */
        if (strcmp(dbn1,"decs") == 0) {

            if(fieldx(irec1,107,1) >= 0) {
                 memmove(key,FIELDP(xdir),len);
                 key[len]='\0';
                 strcat(key," (NON MESH)");
                 for (p=key;*p; p++) *p = isisuctab[*p];
            }
            else {
                  id[0]='\0';
                  if ((xd=fieldx(irec1,106,1)) >= 0) {
                     lx=DIRlen(xd);
                     memmove(id,FIELDP(xd),lx);
                     id[lx]='\0';
                  }
                  if((strcmp(id,"p")!=0) && (strcmp(id,"s")!=0)) {
                     memmove(key,FIELDP(xdir),len);
                     key[len]='\0';
                     for (p=key, loop=len; loop--; p++) *p = isisuctab[*p];
                  } else continue;

            }
        }
        else {
             memmove(key,FIELDP(xdir),len);
             key[len]='\0';
             for (p=key, loop=len; loop--; p++) *p = isisuctab[*p];
           }

        TERM(itrm2,dbn2,key);
        mfn2=posting(itrm2,1L);


        /* setup output */
        record(upirec,dbnout,mfn1); /* get a PDEL record */
        VMFRstatus(upirec)=ACTIVE; /* make it ACTIVE */
        sprintf(buffup,"H 1 %d ",len); batchp=buffup+strlen(buffup);
        memmove(batchp,key,len); batchp+=len;
        *batchp='\0';
        if ((p=fldupdat(upirec,buffup)) != NULL) fatal(p);

        if (TRMrc == RCNORMAL && TRMxytotp != 1) {
            sprintf(buffup,"a2/%s MFN %"_LD_" -> %"_LD_" postings/",
                                                dbn2,mfn2,TRMxytotp);
            if (fldupdat(upirec,buffup)) fatal("fldupdat");
            recupdat(upcrec,upirec); errors++;
            continue;
        }

        if (TRMrc != RCNORMAL) {
            sprintf(buffup,"a2/Not found/");
            if (fldupdat(upirec,buffup)) fatal("fldupdat");
            recupdat(upcrec,upirec); errors++;
            continue;
        }

        /* get corresponding */
        RECORD(irec2,dbn2,mfn2);

        if (RECrc != RCNORMAL) {
            sprintf(buffup,"a2/%s MFN %"_LD_" -> Is deleted /",dbn2,mfn2);
            if (fldupdat(upirec,buffup)) fatal("fldupdat");
            recupdat(upcrec,upirec); errors++;
            continue;
        }

        /* get sorted tagcat's not on exclude list */
        occ1=getcat(0,irec1,tagcat1);
        occ2=getcat(1,irec2,tagcat2);

        if (occ1 != occ2) {

            sprintf(buffup,"a2/%s MFN %"_LD_" TAG %d -> %d occ (%d expected)/",
						dbn2,mfn2,tagcat2,occ2,occ1);
	    storcats(0,3);
            if (fldupdat(upirec,buffup)) fatal("fldupdat");
            recupdat(upcrec,upirec); errors++;
            continue;
        }
#if TRACE1
printf("-- %d\n",mfn1);
#endif

        for (n=0; n < occ1; n++) {

#if TRACE2
printf("term: %s %s=%s %s=%s\n",key,dbn1,cats[0][n],dbn2,cats[1][n]);
#endif

            if (strcmp(cats[0][n],cats[1][n]) == 0) continue;

	    sprintf(buffup,"a2/%s MFN %"_LD_" -> Categories/",dbn2,mfn2);
	    storcats(1,4);
            if (fldupdat(upirec,buffup)) fatal("fldupdat");
            recupdat(upcrec,upirec); errors++;
            break;
        }
    }

    printf("records=%"_LD_" / errors=%"_LD_" / empty=%"_LD_"\n",
        count,errors,empty);
    exit(0);
}



int getcat(idx,irec,tag)
int idx;
LONGX irec;
int tag;
{
    int ncats,xdir,loop,n,i,found,loop2;
    RECSTRU *recp;
    unsigned char temp[MAXCOL+1];

    recp=vrecp[irec]; /* set current */
    ncats=0;


    for (xdir=0, loop=MFRnvf; loop--; xdir++) {

        if (DIRtag(xdir) != tag) continue;
        for (found=0, n=nexclpfxs, i=0; n--; i++)
        {

            if (strncmp(FIELDP(xdir),exclpfxp[i],exclpfxl[i]) == 0) {
                found=1;
                break;
            }
        }
        if (found) {
#if TRACE3
printf("excluindo %s %d\n",exclpfxp[n],xdir);
#endif
           continue;
        }
        if (DIRlen(xdir) > MAXCOL) fatal("MAXCOL");
        memmove(cats[idx][ncats],FIELDP(xdir),DIRlen(xdir));
        cats[idx][ncats][DIRlen(xdir)] = '\0';
        ncats++;
#if TRACE4
printf("idx=%d ncats=%d '%s'",idx,ncats,cats[idx][ncats-1]);
if (getchar() != '\n') exit(1);
#endif
    }


    for (n=ncats-1, loop=0; loop < n; loop++)

        for (loop2=loop+1; loop2 <= n; loop2++)

            if (strcmp(cats[idx][loop],cats[idx][loop2]) > 0) {
                strcpy(temp,cats[idx][loop]);
                strcpy(cats[idx][loop],cats[idx][loop2]);
                strcpy(cats[idx][loop2],temp);
            }

    cats[idx][ncats][0] = '\0'; /* end */

    return(ncats);
}


int storcats(idx,tag)
int idx;
int tag;
{
    int icat;
    char *p;

    for (icat=0; cats[idx][icat][0]; icat++) {

	p=buffup+strlen(buffup);

	sprintf(p,"H %d %d %s",
		tag,strlen(cats[idx][icat]),cats[idx][icat]);

    }

    return(icat);
}

