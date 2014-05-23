/* ======================= splitem: split uupc/e-mail ====================== */
/*                                                                           */
/*                                                      A. O. Tardelli       */
/*                                                      BRM, 01/03/93        */
/* ========================================================================= */

#include <stdio.h>
#include <string.h>

#include "cisis.h"      /* CISIS Interface */
#include "cirun.h"      /* runtime area and defines by AOT */


void main(argc,argv)
int argc;
char *argv[];
{
    char buffer[BUFSIZ+1];
    char filout[BUFSIZ+1];

    LONGX nmsgs,xmsg,ext;
    FILE *fpin,*fpout;
    char *leftp,*outp,*filp,*extp;

    char *dbnp=NULL;
    RECSTRU *recp;
    LONGX crec,irec,mfn;
    char buffup[MAXMFRL+BUFSIZ],*batchp;
    int emptyline,moveheader,amat;
    int trace=1,tell=1,notzero=1;
    int day,month,year,hour,min,sec;
    char dax[3+1],mox[3+1],buffx[BUFSIZ];
    char *fstrp="From ";
    int ismozilla=0;

    if (argc < 3 || argc > 4) {
        printf("%s",cicopyr("Utility AOT/SPLITEM"));
        printf("\n");
        printf("splitem <msgs> <filin> [<pfxout>|isis=<dbn>] \n");
        printf("\n");
        printf("msgs        numero maximo de e-mails em cada arquivo de saida\n");
        printf("filin       nome do arquivo a ser dividido (mailbox) \n");
        printf("pfxout      prefixo dos arquivos que serao gerados (pfxout.001)\n");
        printf("\n");
        printf("isis=<dbn>  write a record to <dbn> (Date/From/To/Cc/Subject)\n");
        printf("            and the msg in <dbn>.mfn for each input e-mail\n");
        printf("            (needs <msgs> set to 1, or 0) \n");
        printf("\n");
        printf("Ex: splitem 10 mailsent x\n");
        printf("\n");
        printf("    grava os 10 primeiros e-mails de mailsent em x.001\n");
        printf("          os 10 seguintes em x.002\n");
        printf("          etc\n");
        printf("\n");
        exit(1);
    }

    if (sscanf(argv[1],"%"_LD_,&nmsgs) != 1) {
        printf("<msgs> invalido: %s\n",argv[1]);
        exit(1);
    }
    /* if (nmsgs < 1) nmsgs=1; */

    filp=argv[2];
    if ((fpin=fopen(filp,"r+b")) == NULL) {
        printf("impossivel abrir o arquivo '%s'\n",filp);
        exit(1);
    }

    if (argc > 3) {
        outp=argv[3];
        if (strncmp(outp,"isis=",5) == 0) {
            outp+=5; dbnp=outp;
            if (nmsgs != 0 && nmsgs != 1) {
                printf("invalid <msgs> for option isis: %s\n",argv[1]);
                exit(1);
            }
            if (nmsgs == 0) notzero=0;
            trace=0;
        }
    }
    else
        outp=argv[2];

    if (trace) {
        printf("+++ numero de e-mails de cada arquivo de saida ...... %"_LD_"\n",nmsgs);
        printf("+++ nome do arquivo a ser dividido .................. %s\n",filp);
        printf("+++ prefixo dos arquivos que serao gerados .......... %s\n",outp);
    }

    strcpy(filout,outp); extp=filout+strlen(outp);

    ext=1;
    if (notzero) {
        sprintf(extp,".%03"_LD_,ext);
        if (trace) printf("+++ gerando arquivo %s...\n",filout);
        if ((fpout=fopen(filout,"w+b")) == NULL) {
            printf("impossivel criar o arquivo '%s'\n",filout);
            exit(1);
        }
    }
    if (dbnp) {
        if (trace) printf("+++ criando %s...\n",dbnp);
        recisis0(dbnp);
        RECORD(crec=nrecs,dbnp,0L);
        RECORD(irec=nrecs,dbnp,mfn=1L); buffup[0]='\0';
    }

    leftp=fgets(buffer,BUFSIZ,fpin);
    if (strlen(buffer)>1) if (buffer[strlen(buffer)-2] == '\r') {
        buffer[strlen(buffer)-2] = '\n';
        buffer[strlen(buffer)-1] = '\0';
        ismozilla=1;
        fstrp="From - ";
    }
    xmsg=0; emptyline=1; moveheader=0;

    while (leftp) {
#if UUPC
        if (*buffer == 0x01 && buffer[1] == 0x01) xmsg++;
#endif
        /* ^J^JFrom mmori@brm.bireme.br Tue Mar 30 11:06:40 SAT 1999^J */
        if (emptyline) if (strncmp(buffer,/*"From ",5*/fstrp,strlen(fstrp)) == 0) {
            if (dbnp) {
                if (xmsg) {
                    if (fldupdat(irec,buffup)) fatal("splitem/fldupdat1");
                    recupdat(crec,irec);
                    mfn++;
                }
                RECORD(irec,dbnp,mfn); RECrc=RCNORMAL; MFRstatus=ACTIVE;
                buffup[0]='\0'; batchp=buffup; moveheader=1;
            }
            xmsg++;
        }
        if ((xmsg-1) == nmsgs) {
            if (notzero) {
                fclose(fpout);
                ext++;
                sprintf(extp,".%03"_LD_,ext);
                if (trace) printf("+++ gerando arquivo %s...\n",filout);
                if ((fpout=fopen(filout,"w+b")) == NULL) {
                    printf("impossivel criar o arquivo '%s'\n",filout);
                    exit(1);
                }
            }
            xmsg=1;
        }
        if ((xmsg-1) == nmsgs) {
            if (notzero) {
                if (fputs(buffer,fpout) == EOF) {
                    printf("*** erro na gravacao de %s\n",filout);
                    exit(1);
                }
            }
        }
        if (dbnp) {
          if (moveheader == 1) {
                sprintf(batchp,"h10 %d %s",strlen(filout),filout);
                batchp+=strlen(batchp);
                sprintf(batchp,"h11 %d %s",strlen(buffer)-1,buffer);
                batchp+=strlen(batchp);
                moveheader=2; amat=0;
          }
          if (moveheader) {
              if (buffer[0]=='\n') moveheader=0;
              if (buffer[0]!=' ') amat=0;
          }
          if (moveheader) {
            if (strncmp(buffer,"Date: ",6) == 0) {
                sprintf(batchp,"h1 %d %s",strlen(buffer)-1,buffer);
                batchp+=strlen(batchp);
                /* Date: Tue, 05 Oct 1999 13:43:56 -0300 */
                sscanf(buffer+6,"%3c, %d %3c %d %d:%d:%d",
                               dax,&day,mox,&year,&hour,&min,&sec);
                month=0;
                if (strncmp(mox,"Jan",3) == 0) month=1;
                if (strncmp(mox,"Feb",3) == 0) month=2;
                if (strncmp(mox,"Mar",3) == 0) month=3;
                if (strncmp(mox,"Apr",3) == 0) month=4;
                if (strncmp(mox,"May",3) == 0) month=5;
                if (strncmp(mox,"Jun",3) == 0) month=6;
                if (strncmp(mox,"Jul",3) == 0) month=7;
                if (strncmp(mox,"Aug",3) == 0) month=8;
                if (strncmp(mox,"Sep",3) == 0) month=9;
                if (strncmp(mox,"Oct",3) == 0) month=10;
                if (strncmp(mox,"Nov",3) == 0) month=11;
                if (strncmp(mox,"Dez",3) == 0) month=12;
                sprintf(buffx,"%04d%02d%02d %02d%02d%02d",
                                 year,month,day,hour,min,sec);
                sprintf(batchp,"h12 %d %s",strlen(buffx),buffx);
                batchp+=strlen(batchp);
            }
            if (strncmp(buffer,"From: ",6) == 0) {
                sprintf(batchp,"h2 %d %s",strlen(buffer)-1,buffer);
                batchp+=strlen(batchp);
            }
            if (strncmp(buffer,"To: ",4) == 0 || amat==3) {
                sprintf(batchp,"h3 %d %s",strlen(buffer)-1,buffer);
                batchp+=strlen(batchp); amat=3;
            }
            if (strncmp(buffer,"Cc: ",2) == 0 || amat==4) {
                sprintf(batchp,"h2 %d %s",strlen(buffer)-1,buffer);
                batchp+=strlen(batchp); amat=4;
            }
            if (strncmp(buffer,"Subject: ",9) == 0) {
                sprintf(batchp,"h5 %d %s",strlen(buffer)-1,buffer);
                batchp+=strlen(batchp); amat=5;
            }
          }
        }
        emptyline=0; if (!buffer[1]) if (buffer[0]=='\n') emptyline=1;
        leftp=fgets(buffer,BUFSIZ,fpin);
        if (ismozilla) if (strlen(buffer)>1) if (buffer[strlen(buffer)-2] == '\r') {
            buffer[strlen(buffer)-2] = '\n';
            buffer[strlen(buffer)-1] = '\0';
        }
    }

    if (dbnp) {
        if (buffup) {
            if (fldupdat(irec,buffup)) fatal("splitem/fldupdat2");
            recupdat(crec,irec);
        }
        if (tell) printf("splitem: %"_LD_" records written to %s\n",VMF0nxtmfn(crec)-1,dbnp);
    }
    exit(0);
}

/*
0........1.........2.........3.........4.........5.........6.........7.........8
12345678901234567890123456789012345678901234567890123456789012345678901234567890
Tue Oct  5 13:45:24 SAT 1999
From spd-l-admin@listas.bireme.br Tue Oct  5 13:45:24 SAT 1999
Received: from listas.bireme.br (listas.bireme.br [200.6.42.35])
	by brm.bireme.br (8.9.3/8.9.3) with ESMTP id NAA01462;
	Tue, 5 Oct 1999 13:45:22 -0300 (SAT)
Received: from listas.bireme.br (localhost [127.0.0.1])
	by listas.bireme.br (8.9.1a/8.9.1) with ESMTP id NAA21807;
	Tue, 5 Oct 1999 13:45:11 -0200
Received: from brm.bireme.br (brm.bireme.br [200.6.42.33])
	by listas.bireme.br (8.9.1a/8.9.1) with ESMTP id NAA21798
	for <spd-l@listas.bireme.br>; Tue, 5 Oct 1999 13:45:10 -0200
Received: from bireme.br ([200.6.42.94])
	by brm.bireme.br (8.9.3/8.9.3) with ESMTP id NAA01459;
	Tue, 5 Oct 1999 13:45:19 -0300 (SAT)
Message-ID: <37FA2ACC.B1059D79@bireme.br>
Date: Tue, 05 Oct 1999 13:43:56 -0300
From: Joao Rodolfo Suarez de Oliveira <jrodolfo@bireme.br>
X-Mailer: Mozilla 4.7 [en] (Win95; I)
X-Accept-Language: en
MIME-Version: 1.0
To: SPD-1 <spd-l@brm.bireme.br>, Julio Cesar T Takayama <julio@bireme.br>,
        Silvia <silvia@brm.bireme.br>
Content-Type: text/plain; charset=us-ascii
Content-Transfer-Encoding: 7bit
Subject: [Spd-l] Apresentacoes em PowerPoint do Curso BVS
Sender: spd-l-admin@listas.bireme.br
Errors-To: spd-l-admin@listas.bireme.br
X-Mailman-Version: 1.0rc2
Precedence: bulk
List-Id: SPD - Users <spd-l.listas.bireme.br>
X-BeenThere: spd-l@listas.bireme.br
Status: RO
X-Status:
X-Keywords:
X-UID: 94

...
*/

