/*-------------------------------------------------------------------------
ciifuh.c
SVD 10-03-93
	Rotinas para atualizar o dicionario (btree)

-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 Alteracao: SVD 26-11-94
 Problema: Quando havia necessidade de se fazer um split de uma folha
           ou um no', o algoritmo que constroi uma folha(no') temporaria 
           com ORD+1  elementos estava incorreto. Se o elemento a ser
           inserido fosse menor que o elemento[0] da folha(no), inseria
           lixo.
---------------------------------------------------------------------------*/

#define GERA_KEYS   (0 || DEBIFUPD)
#define ONLY_INSERTION 0
#if TRACEUPIF
#if !CICPP
static  UCHR tkey[LE2+1]; /* AOT 11/09/2000 */
#endif /* CICPP */
#endif

#define TRCBTREE 1

#if !CICPP
static  TREE_PATH lfpath;
static  PUNT puntlf ;
#endif /* CICPP */

/*--------------------------------------------------------------------------
  upif_find_key_node
    .Retorna em 'pos' a posicao em que deve ser inserida a chave 'key' com
     tamanho 'keysize' em um vetor  '*p' com 'ock' elementos     compostos
     cada um com tamanho 'itemsize'.
    .Se a chave existe a funcao retorna 'TRUE' e em 'pos' a posicao que esta
    .Caso contrario a funcao retorna 'FALSE' e em 'pos' a posicao onde
     devera ser inserido.
    .Supoe-se que o vetor esta classificado em ordem crescente da chave.
--------------------------------------------------------------------------*/

#if CICPP
BOOLEAN CIIFU :: upif_find_key_node(N0STRU *np,
                                    UCHR   *key,
			            int     treecase,
			            int    *pos)
#else /*CICPP*/
 BOOLEAN upif_find_key_node(np,key,treecase,pos)
 N0STRU *np;
 UCHR *key;
 int treecase;
 int *pos;
#endif /*CICPP*/
{
   BOOLEAN existe;
   N1STRU *n1p;
   N2STRU *n2p;
   int n,cmp,keysize,ock;

   n1p=(N1STRU *)np;
   n2p=(N2STRU *)np;
   ock=np->ock;
   keysize=vlex[treecase];
   existe=FALSE;
   if (treecase == 0) {
     for (n=ock-1; n>=0; n--) {
       *pos=n;
    cmp= strncmp((CONST char *)key,(CONST char *)n1p->idx[n].key,keysize);
    if (cmp==0) {
	existe=TRUE;
	break;
    }
    else
    { if (cmp > 0) {
	existe=FALSE;                  /* Not found */
	*pos= *pos+1;
	break;
      }
     }
      } /*for*/
   }/*treecase*/
   else {
     for (n=ock-1; n>=0; n--) {
       *pos=n;
    cmp= strncmp((CONST char *)key,(CONST char *)n2p->idx[n].key,keysize);
    if (cmp==0) {
	existe=TRUE;
	break;
    }
    else
    { if (cmp > 0) {
	existe=FALSE;                  /* Not found */
	*pos= *pos+1;
	break;
      }
     }
      } /*for*/
   }

 return(existe);
}
/*--------------------------------------------------------------------------
  upif_find_key_leaf
    .Retorna em 'pos' a posicao em que deve ser inserida a chave 'key' com
     tamanho 'keysize' em um vetor  '*p' com 'ock' elementos     compostos
     cada um com tamanho 'itemsize'.
    .Se a chave existe a funcao retorna 'TRUE' e em 'pos' a posicao que esta
    .Caso contrario a funcao retorna 'FALSE' e em 'pos' a posicao onde
     devera ser inserido.
    .Supoe-se que o vetor esta classificado em ordem crescente da chave.
--------------------------------------------------------------------------*/

#if CICPP
 BOOLEAN CIIFU :: upif_find_key_leaf(L0STRU *lp,
			             UCHR   *key,
                                     int     treecase,
			             int    *pos)
#else /*CICPP*/
 BOOLEAN upif_find_key_leaf(lp,key,treecase,pos)
 L0STRU *lp;
 UCHR *key;
 int treecase;
 int *pos;
#endif /*CICPP*/
{
   BOOLEAN existe;
   L1STRU *l1p;
   L2STRU *l2p;
   int n,cmp,keysize,ock;

   l1p=(L1STRU *)lp;
   l2p=(L2STRU *)lp;
   ock=lp->ock;
   keysize=vlex[treecase];
   existe=FALSE;
   if (treecase == 0) {
     for (n=ock-1; n>=0; n--) {
       *pos=n;
    cmp= strncmp((CONST char *)key,(CONST char *)l1p->idx[n].key,keysize);
    if (cmp==0) {
        existe=TRUE;
        break;
    }
    else
    { if (cmp > 0) {
	existe=FALSE;                  /* Not found */
        *pos= *pos+1;
        break;
      }
     }
      } /*for*/
   }/*treecase*/
   else {
     for (n=ock-1; n>=0; n--) {
       *pos=n;
    cmp= strncmp((CONST char *)key,(CONST char *)l2p->idx[n].key,keysize);
    if (cmp==0) {
        existe=TRUE;
        break;
    }
    else
    { if (cmp > 0) {
        existe=FALSE;                  /* Not found */
        *pos= *pos+1;
        break;
      }
     }
      } /*for*/
   }

 return(existe);
}
/* -----------------------------------------------------------------------
 .Criar uma nova raiz
 .Se first=true cria a raiz com apenas um elemento igual a branco
   (Isto corresponde a inicializar o arquivo invertido).
 .Se first=false cria uma nova raiz que contem oprimeiro elemento branco
  e o segundo elemento a chave promovida de elementos de um nivel mais baixo
 -------------------------------------------------------------------------*/

#if CICPP
PUNT  CIIFU :: upif_create_root(DBXSTRU *dbxp,
		                PUNT     esq,
		                UCHR    *key,
		                PUNT     dir,
		                int      treecase,
		                BOOLEAN  first)
#else /*CICPP*/
PUNT  upif_create_root(dbxp,esq,key,dir,treecase,first)
DBXSTRU *dbxp;
PUNT esq;
UCHR *key;
PUNT dir;
int treecase;
BOOLEAN first;
#endif /*CICPP*/
{
 N1STRU *n1p,nw_node1;
 N2STRU *n2p,nw_node2;
 N0STRU *n0p;

 N1IDXE item_no1;
 N2IDXE item_no2;

 INVMAP *invp;

 UWORD level;
 BOOLEAN isroot;
 UCHR tmp[LE2+1];
 PUNT nodenumber;
 int
     keysize,i,ind;



 invp=DBXifmap;
 keysize=vlex[treecase];
 /*Branqueia para pior caso */
 upif_branqueia((UCHR *)tmp,LE2);
 nodenumber=invp->cn[treecase].nmaxpos+1;
 if (treecase==0) {
   n0p=(N0STRU *)&nw_node1;
   n1p=(N1STRU *)&nw_node1;
   memmove(item_no1.key,tmp,keysize);
   item_no1.punt=esq;
 }
 else {
   n0p=(N0STRU *)&nw_node2;
   n2p=(N2STRU *)&nw_node2;
   memmove(item_no2.key,tmp,keysize);
   item_no2.punt=esq;
 }

 level=invp->cn[treecase].liv+1;
 n0p->it =treecase+1;
 n0p->pos = nodenumber;
 n0p->ock =2;
 if (first==TRUE)n0p->ock=1;

 ind=0;
 if (treecase == 0) {
   n1p->idx[ind]=item_no1;
 }
 else {
   n2p->idx[ind]=item_no2;
 }

 if (first==FALSE) {
   ind++;
   if (treecase == 0) {
     memmove(n1p->idx[ind].key,key,keysize);
     n1p->idx[ind].punt=dir;
   }
   else {
     memmove(n2p->idx[ind].key,key,keysize);
     n2p->idx[ind].punt=dir;
   }
 }
 isroot=TRUE;
 upif_branqueia(tmp,keysize);
 if (treecase == 0) {
   memmove(item_no1.key,tmp,keysize);
   item_no1.punt=(INFO )0;
  }
  else {
   memmove(item_no2.key,tmp,keysize);
   item_no2.punt=(INFO )0;
  }
 for (i=ind+1;i<=TWORDN-1;i++){
    if (treecase == 0)
    n1p->idx[i]=item_no1;
    else
    n2p->idx[i]=item_no2;
 }
#if TRACEUPIF
   upif_print_node(n0p);
#endif

 nodewrit(dbxp,(N0STRU *)n0p,level,isroot);
/* retorna a nova raiz */
 return (nodenumber);
}
/*-------------------------------------------------------------------------
 upif_insert_index:
  .Insere a chave "b_key" na pagina de indice apontada por "b_punt".
   (Chave e apontador que vieram de "b_otton".
  .Se a pagina esta cheia, cria uma nova pagina do indice, divide os
   elementos entre as duas paginas. Na divisao a pagina velha fica com ORDN
   itens e nova com ORDN+1. O primeiro item da nova pagina e' escolhido
   para ser inserido em um nivel superior. Esta operacao e' chamada de
   "promover" uma chave.
  .Se houver promocao a funcao retorna TRUE e a chave em "p_key" e seu
   apontador em "p_punt". O registro CNT deve ser atualizado e regravado.
   As paginas apontadas por "punt" e "b_punt" tambem tem que ser atualizadas
   e regravadas.
  .Caso contrario a funcao retorna FALSE; neste caso "p_key" e "p_punt"
   ficam indefinidos. Somente a pagina apontada por "punt" deve ser
   atualizada e regravada.
-------------------------------------------------------------------------*/

#if CICPP
BOOLEAN CIIFU :: upif_insert_index (DBXSTRU *dbxp,
			            PUNT  punt,
			            int   treecase,
			            int   level,
			            int   isroot,
			            UCHR *b_key,
			            PUNT  b_punt,
			            UCHR *p_key,
			            PUNT *p_punt)
#else /*CICPP*/
BOOLEAN upif_insert_index (dbxp,punt,treecase,level,isroot,
			b_key,b_punt,p_key,p_punt)
DBXSTRU *dbxp;
PUNT punt;
int treecase;
int level;
int isroot;
UCHR *b_key;
PUNT b_punt;
UCHR *p_key;
PUNT *p_punt;
#endif /*CICPP*/
{
 INVMAP *invp;
 UCHR *np;
 static N2STRU node; /* pior caso*/
 UWORD ock;
 UCHR tmp[LE2+1];
 BOOLEAN found;
 PUNT nodenumber;
 int pos, keysize,i,cmp;
 int ileft,ifrom,iright,inew;
 N0STRU *n0p,*n0pnw;
 static N1STRU *n1p, n1nw, *n1pnw;
 static N2STRU *n2p, n2nw, *n2pnw;
 static N1STRUW n1w, *n1wp;
 static N2STRUW n2w, *n2wp;
 N1IDXE no1;
 N2IDXE no2;
#if TRACEUPIF
 N0STRU *n0wp;
#endif

 invp=DBXifmap;
 keysize=vlex[treecase];
 /* Inicializa um no do ttipo 1 ou tipo 2 */
 if (treecase == 0){
   memmove(no1.key,b_key,keysize);
   no1.punt=b_punt;
 }
 else {
   memmove(no2.key,b_key,keysize);
   no2.punt=b_punt;
 }

 np=(UCHR *)noderead(invp,treecase,level,punt);
 memmove((UCHR *)&node,np,sizeof(N2STRU)); /* pega maior caso*/
 np=(UCHR *)&node;
 n0p=(N0STRU *)np;
 n1p=(N1STRU *)np;
 n2p=(N2STRU *)np;
 n1wp= &n1w;
 n2wp= &n2w;
 ock = n0p->ock;

 found=upif_find_key_node(n0p,b_key,treecase,&pos);
 if (found)               /* Erro de concepcao!!! */
                 fatal("ciifuh/upif_insert_key/EC/tp=0");


 if (ock < TWORDN) {        /* Cabe na pagina */
    if (treecase == 0) {
          for (i=ock;i>pos;i--){ /* Abre espaco para novo item  */
                 n1p->idx[i] = n1p->idx[i-1];
      }
          n1p->idx[pos] = no1;   /* Insere o novo no */
        }else {
          for (i=ock;i>pos;i--){ /* Abre espaco para novo item  */
                 n2p->idx[i] = n2p->idx[i-1];
      }
      n2p->idx[pos] = no2;
        }
        n0p->ock++;    /* Atualiza ocorrencias       */
#if TRACEUPIF
        printf("\n<insert_index> Apos insercao ");
        upif_print_node(n0p);
#endif
   nodewrit(dbxp,n0p,level,isroot);
   return (FALSE);    /* Nao precisou fazer split   */
 }

 /* Nao cabe na pagina. Precisa fazer Split
        E criada uma pagina (wk)  com TWORDN+1 elementos, inserindo elemento na
        posicao correta. A copia e feita do fim para o comeco.
        Se key < Original entao copia original, muda origem e destino
        Se Key > Original entao copia key e muda somente destino e transforma
                key em '\0';
 */
 /* Proxima posicao livre de idx */
 nodenumber= invp->cn[treecase].nmaxpos+1;
 memmove((UCHR *)tmp,b_key,keysize);
 tmp[keysize]='\0';
 /* Copia o no para area de trabalho para inserir mais um elemento.
        Processa do Fim para Comeco
 */
 ifrom=ock-1;
 if (treecase == 0) {
#if TRACEUPIF
   n0wp = (N0STRU *)n1wp;
#endif
   /* SVD 26-11-94 for (i=ock;i>=0;i--) { */
   i=ock; /* SVD 26-11-94 */
   for (;ifrom>=0;) {
          cmp = strncmp((CONST char *)n1p->idx[ifrom].key,(CONST char *)tmp,keysize);
          if ( cmp>0 ) {
                n1wp->idx[i]=n1p->idx[ifrom];
                ifrom--;
                i--;   /* SVD 26-11-94 */ 
          }else
                if ( cmp<0 ) {
                  n1wp->idx[i]=no1;
                  i--; /* SVD 26-11-94 */
                  tmp[0]='\0'; /* forca para copiar resto do vetor */
                }else
                  if ( cmp==0 ) {
                        /* Lembrar que o primeiro elemento da arvore contem uma
                         chave "branco". Neste caso nao pode ser considerada 
                         como erro.
                        */
                        if (i>0 ) /*Termo duplicado- erro de concepcao */
                          fatal("ciifuh/upif_insert_index/EC/tp=1");
			n1wp->idx[i]=n1p->idx[ifrom];
                        i--; /* SVD 26-11-94 */
                  }
   }/*for*/
  /* SVD 26-11-94 
      Verifica se a insercao ainda nao foi feita,ou seja , deve ser 
     inserida na primeira posicao 
  */ 
   if (tmp[0]!='\0'){
     n1wp->idx[0]=no1;
     tmp[0]='\0';
  }
 }/*treecase */
  else {
#if TRACEUPIF
   n0wp = (N0STRU *)n2wp;
#endif
/* SVD 26-11-94    for (i=ock;i>=0;i--) { */
   i=ock; /* SVD 26-11-94 */
   for (;ifrom>=0;) {
          cmp = strncmp((CONST char *)n2p->idx[ifrom].key,(CONST char *)tmp,keysize);
          if ( cmp>0 ) {
		n2wp->idx[i]=n2p->idx[ifrom];
                ifrom--;
                i--;   /* SVD 26-11-94 */ 
          }else
                if ( cmp<0 ) {
                  n2wp->idx[i]=no2;
                  i--;   /* SVD 26-11-94 */ 
                  tmp[0]='\0'; /* forca para copiar todo vetor */
                }else
                  if ( cmp==0 ) {
                        /* Lembrar que o primeiro elemento da arvore contem uma
                           chave "branco". Neste caso nao pode ser considerada
                           como erro.
                        */
           if (i>0 )/*Termo duplicado- erro de concepcao */
                         fatal("ciifuh/Upif_insert_index/EC/tp=2");
                         n2wp->idx[i]=n2p->idx[ifrom];
                     i--;   /* SVD 26-11-94 */ 
                   }
   }/*for*/
  /* SVD 26-11-94 
      Verifica se a insercao ainda nao foi feita,ou seja , deve ser 
     inserida na primeira posicao
  */ 
  if (tmp[0]!='\0'){
     n2wp->idx[0]=no2;      
     tmp[0]='\0';
  }
 }

  ock++;
#if TRACEUPIF
  n0wp->ock=ock;
  n0wp->it=treecase+1;
  n0wp->pos=nodenumber;      /* numero de elementos em wk */
  printf("\n<insert_node> Apos insercao em wk");
  upif_print_node((N0STRU *)n0wp);
#endif

  /* Metade esquerda */
  /* A primeira  metade do vetor de trabalho volta para o no'  original
         A segunda metade + o novo item inserido vai para o no' novo
         Lembrar que os indices comecam de zero !
  */
  n1pnw= &n1nw;
  n2pnw= &n2nw;
  
  iright = ORDN;  /* Metade + 1 */
  inew = 0;
  for (ileft=0; ileft < ORDN; ileft++) {
        if (treecase == 0 ) {
          n1p->idx[ileft] = n1wp->idx[ileft];
          n1pnw->idx[inew] =n1wp->idx[iright];
        }else {
          n2p->idx[ileft] = n2wp->idx[ileft];
          n2pnw->idx[inew] =n2wp->idx[iright];
        }
        iright++;
        inew++;
  }
  /* Copia o ultimo elemento */
  if (treecase == 0) {
        n0p = (N0STRU *)n1p;
        n0pnw =(N0STRU *)n1pnw;
        n1pnw->idx[inew] = n1wp->idx[iright];
  }else {
	n0p = (N0STRU *)n2p;
        n0pnw =(N0STRU *)n2pnw;
        n2pnw->idx[inew] = n2wp->idx[iright];
  }
  inew++;
  iright++;
  n0p->ock=ORDN;

  /* Completa informacoes da nova pagina */
  n0pnw->it=n0p->it;      /* Mesmo tipo que a original */
  n0pnw->pos=nodenumber;  /* Proxima folha */
  n0pnw->ock=ORDN+1;      /* O item a ser promovido fica
                             armazenado na 1. posicao da nova arvore
                          */
  upif_branqueia((UCHR *)tmp,LE2);
  if (treecase == 0) {
        memmove(no1.key,tmp,keysize);
        no1.punt=(INFO )0;
  }else {
        memmove(no2.key,tmp,keysize);
        no2.punt=(INFO )0;
  }

 /* Branqueia itens disponiveis da pagina velha */
  for (i=ORDN+1-1;i<=TWORDN-1;i++){
        if (treecase == 0 ) {
          n1p->idx[i]=no1;
        }else {
          n2p->idx[i]=no2;
        }
  }
  for (i=ORDN+2-1; i<=TWORDN-1;i++){ /* Indice comeca de zero */
         if (treecase == 0) {
                n1pnw->idx[i]=no1;
         }else {
           n2pnw->idx[i]=no2;
         }
  }
  /* Grava as novas paginas */
  nodewrit(dbxp,n0p,level,isroot);
  nodewrit(dbxp,n0pnw,level,isroot);
#if TRACEUPIF
   printf("\n<insert_node> Apos insercao. Velha= ");
   upif_print_node(n0p);
   printf("\n<insert_node> Apos insercao.Nova= ");
   upif_print_node(n0pnw);
#endif

 /* Retorna o primeiro item da nova pagina  ser promovido */
  if (treecase == 0) {
        memmove(p_key,n1pnw->idx[0].key,keysize);
  }else {
    memmove(p_key,n2pnw->idx[0].key,keysize);
  }

  *p_punt=nodenumber;  /* numero da folha definida  ??*/
  return(TRUE);
}
/*-------------------------------------------------------------------------
 upif_branqueia
  .Transforma um vetor em um string com "size"brancos
-------------------------------------------------------------------------*/
#if CICPP
void CIIFU :: upif_branqueia(UCHR *str,
		             int   size)
#else /*CICPP*/
void upif_branqueia(str,size)
UCHR *str;
int size;
#endif /*CICPP*/
{
#if 0
 int i;
 for (i=0; i<size; i++) str[i]=' ';
#else
 memset(str,(int)' ',(size_t)size);
#endif
 str[size]='\0';
}

/* -----------------------------------------------------------------------*/

#if CICPP
BOOLEAN CIIFU :: upif_insert_leaf(DBXSTRU *dbxp,
			          L0STRU  *lp,
                                  UCHR    *key,
			          int      treecase,
			          UCHR    *p_b_key,
			          PUNT    *p_b_punt,
			          int      pos,
			          INFO     TSTblk,
			          INFO     TSToff)
#else /*CICPP*/
BOOLEAN upif_insert_leaf(dbxp,lp,key,treecase,p_b_key,p_b_punt,
						 pos,TSTblk,TSToff)
DBXSTRU *dbxp;
L0STRU *lp;
UCHR *key;
int treecase;
UCHR *p_b_key;
PUNT *p_b_punt;
int pos;
INFO TSTblk;
INFO TSToff;
#endif /*CICPP*/
{
 INVMAP *invp;
 L0STRU *l0p,*l0pnw;
 static L1STRU *l1p, l1nw, *l1pnw;
 static L2STRU *l2p, l2nw, *l2pnw;
 static L1STRUW l1w, *l1wp;
 static L2STRUW l2w, *l2wp;
 int ifrom,iright,inew,ileft;

#if TRACEUPIF
 UCHR *pnw_item;
  L0STRU *l0wp;
#endif

 PUNT leafnumber;
 int i,ock,keysize,cmp;
 UCHR     tmp[LE2+1];
 L1IDXE leaf_el1;
 L2IDXE leaf_el2;

 invp=DBXifmap;
 keysize=vlex[treecase];

 l1p=(L1STRU *)lp;
 l2p=(L2STRU *)lp;
 l2wp = &l2w;
 l1wp = &l1w;
 if (treecase == 0) {
   memmove(leaf_el1.key,key,keysize);
   leaf_el1.info1=TSTblk;
   leaf_el1.info2=TSToff;
#if LIND /* 55 */
   leaf_el1.info3info4.info3=0L;
#endif /* LIND 55 */
#if TRACEUPIF
   pnw_item=(UCHR *)&leaf_el1;
#endif
 }
 else {
   memmove(leaf_el2.key,key,keysize);
   leaf_el2.info1=TSTblk;
   leaf_el2.info2=TSToff;
#if LIND /* 56 */
   leaf_el2.info3info4.info3=0L;
#endif /* LIND 56 */
#if TRACEUPIF
   pnw_item=(UCHR *)&leaf_el2;
#endif
 }
#if TRACEUPIF
 upif_print_lfitem(treecase,(UCHR *)pnw_item);
#endif


 /* Deve ser aberto espaco para inserir o elemento na posicao apontada por
    'pos' . Varre o vetor do fim para comeco, dando um shift em cada elemento
    ate chegar em pos.
 */
 ock=lp->ock;
 if (ock<TWORDF){              /* Cabe na pagina              */
   if (treecase == 0) {
          for (i=ock;i>pos;i--){  /* Abre espaco para novo item  */
         l1p->idx[i] = l1p->idx[i-1];
      }
      l1p->idx[pos] = leaf_el1;
   }
   else {
          for (i=ock;i>pos;i--){  /* Abre espaco para novo item  */
         l2p->idx[i] = l2p->idx[i-1];
      }
      l2p->idx[pos] = leaf_el2;
   }
   lp->ock++;               /* Atualiza ocorrencias       */
#if TRACEUPIF
   printf("\n<insert_leaf> Apos insercao ");
   upif_print_leaf(lp);
#endif
  leafwrit(dbxp,(L0STRU *)lp);
  return (FALSE);                /* Nao precisou fazer split   */
 }

 /* Nao cabe na pagina. Precisa fazer Split
        E criada um pagina (wk)  com TWORDF+1 elementos, inserindo elemento na
        posicao correta. A copia e feita do fim para o comeco.
        Se key < Original entao copia original, muda origem e destino
        Se Key > Original entao copia key e muda somente destino e transforma
                key em '\0';
  */

 leafnumber=invp->cn[treecase].fmaxpos+1;
 memmove((UCHR *)tmp,key,keysize); /* para nao estragar chave original */
 tmp[keysize]='\0';
 /* A copia devera ser feita para todo vetor original (ock vezes).
   Para o vetor novo devera ser feita ock+1 vezes.
   Compara-se a chave com cada elemento do vetor. Se o elemento
   do vetor
   Cuidado : indice comeca de 0
 */
 ifrom=ock-1;
 if (treecase == 0) {
#if TRACEUPIF
    l0wp = (L0STRU *)l1wp;
#endif
/* SVD 26-11-94     for (i=ock;i>=0;i--) { */
    i=ock; /* SVD 26-11-94 */
    for (;ifrom>=0;) {
       cmp = strncmp((CONST char *)l1p->idx[ifrom].key,(CONST char *)tmp,keysize); /*11*/
       if ( cmp>0 ) {
          l1wp->idx[i]=l1p->idx[ifrom];
	  ifrom--;
          i--;   /* SVD 26-11-94 */
        }
        else
         if ( cmp<0 ) {
          l1wp->idx[i]=leaf_el1;
          i--;   /* SVD 26-11-94 */
          tmp[0]='\0';
         }
         else
          if ( cmp==0 ) { /* Ja existe termo - erro de logica */
           fatal("ciifuh/upif_insert_leaf_it1/EL");
          }
    }/*for*/
  /* SVD 26-11-94
      Verifica se a insercao ainda nao foi feita,ou seja , deve ser
     inserida na primeira posicao
  */
  if (tmp[0]!='\0'){
     l1wp->idx[0]=leaf_el1;
     tmp[0]='\0';
  }
 }
 else {
#if TRACEUPIF
    l0wp = (L0STRU *)l2wp;
#endif
/* SVD 26-11-94    for (i=ock;i>=0;i--) { */
    i=ock; /* SVD 26-11-94 */
    for (;ifrom>=0;) {
       cmp = strncmp((CONST char *)l2p->idx[ifrom].key,(CONST char *)tmp,keysize);
       if ( cmp>0 ) {
          l2wp->idx[i]=l2p->idx[ifrom];
          ifrom--;
          i--;   /* SVD 26-11-94 */
        }else
        if ( cmp<0 ) {
          l2wp->idx[i]=leaf_el2;
          i--;   /* SVD 26-11-94 */
          tmp[0]='\0';
        }else
        if ( cmp==0 ) { /* Ja existe termo - erro de logica */
           fatal("ciifuh/upif_insert_leaf_it2/EL");
        }
    }/*for*/
  /* SVD 26-11-94
      Verifica se a insercao ainda nao foi feita,ou seja , deve ser
     inserida na primeira posicao
  */
  if (tmp[0]!='\0'){
     l2wp->idx[0]=leaf_el2;
     tmp[0]='\0';
  }
 }

 ock++;
#if TRACEUPIF
/* teste retirar depois */
  l0wp->ock=ock;
  l0wp->it=treecase+1;
  l0wp->pos=leafnumber;          /* numero de elementos em wk */
   printf("\n<insert_leaf> Apos insercao em wk");
   upif_print_leaf((L0STRU *)l0wp);
#endif

        /* Metade esquerda */
 /* A primeira  metade do vetor de trabalho volta para a folha original
    A segunda metade + o novo item inserido vai para a folha nova
    Lembrar que os indices comecam de zero !
 */
 l1pnw= &l1nw;
 l2pnw= &l2nw;
 ileft = 0;
 iright = ORDF;  /* Metade +1 */
 inew = 0;
 for (ileft=0; ileft < ORDF; ileft++) {
   if (treecase == 0 ) {
     l1p->idx[ileft] = l1wp->idx[ileft];
     l1pnw->idx[inew] =l1wp->idx[iright];
   }
   else {
     l2p->idx[ileft] = l2wp->idx[ileft];
     l2pnw->idx[inew] =l2wp->idx[iright];
   }
   iright++;
   inew++;
 }
 if (treecase == 0) {
   l0p = (L0STRU *)l1p;
   l0pnw =(L0STRU *)l1pnw;
   l1pnw->idx[inew] = l1wp->idx[iright];
 }
 else {
   l0p = (L0STRU *)l2p;
   l0pnw =(L0STRU *)l2pnw;
   l2pnw->idx[inew] = l2wp->idx[iright];
 }
 inew++;
 iright++;
 l0p->ock=ORDF;

 /* Completa informacoes da nova pagina */
  l0pnw->it=l0p->it;         /* Mesmo tipo que a original */
  l0pnw->pos=leafnumber;     /* Proxima folha */
  l0pnw->ock=ORDF+1;         /* O item a ser promovido fica
                                armazenado na 1. posicao da
                                nova arvore */
  l0pnw->ps=l0p->ps;        /* Acerta apontadores */
  l0p->ps=leafnumber;

 upif_branqueia((UCHR *)tmp,LE2);
 if (treecase ==0) {
    strncpy((char *)leaf_el1.key,(CONST char *)tmp,keysize);
    leaf_el1.info1=(INFO )0;
    leaf_el1.info2=(INFO )0;
#if LIND /* 53 */
    leaf_el1.info2=1L;
    leaf_el1.info3info4.info3=(INFO)0;
#endif /* LIND 53 */
 }
 else {
    strncpy((char *)leaf_el2.key,(CONST char *)tmp,keysize);
    leaf_el2.info1=(INFO )0;
    leaf_el2.info2=(INFO )0;
#if LIND /* 54 */
    leaf_el2.info2=1L;
    leaf_el2.info3info4.info3=(INFO)0;
#endif /* LIND 54 */
 }
 /* Branqueia itens disponiveis da pagina velha */
 for (i=ORDF+1-1;i<=TWORDF-1;i++){ /* indice comeca de zero*/
   if (treecase == 0 ) {
     l1p->idx[i]=leaf_el1;
   }
   else {
     l2p->idx[i]=leaf_el2;
   }
 }
 for (i=ORDF+2-1; i<=TWORDF-1;i++){ /* Indice comeca de zero */
    if (treecase == 0) {
        l1pnw->idx[i]=leaf_el1;
    }
    else {
        l2pnw->idx[i]=leaf_el2;
    }
 }

 /* Grava as novas paginas */
 leafwrit(dbxp,l0pnw);
 leafwrit(dbxp,l0p);

#if TRACEUPIF
   printf("\n<insert_leaf> Apos insercao em velha ");
   upif_print_leaf(l0p);
#endif

#if TRACEUPIF
   printf("\n<insert_leaf> Apos insercao na nova ");
   upif_print_leaf(l0pnw);
#endif

 /* Retorna o primeiro item da nova pagina  ser promovido */
 if (treecase == 0) {
    memmove(p_b_key,l1pnw->idx[0].key,keysize);
 }
 else {
    memmove(p_b_key,l2pnw->idx[0].key,keysize);
 }
 *p_b_punt= -leafnumber;  /* numero da nova folha inserida */
 return(TRUE);
}

/*-------------------------------------------------------------------------
 upif_init_index:
-------------------------------------------------------------------------*/

#if CICPP
PUNT CIIFU :: upif_init_index(DBXSTRU *dbxp,
		              int treecase,
		              UCHR *key,
		              INFO TSTblk,
                              INFO TSToff)
#else /*CICPP*/
PUNT upif_init_index(dbxp,treecase,key,TSTblk,TSToff)
DBXSTRU *dbxp;
int treecase;
UCHR *key;
INFO TSTblk;
INFO TSToff;
#endif /*CICPP*/
{
 INVMAP *invp;
 L0STRU *l0p;
 L1STRU  nw_leaf1;
 L2STRU  nw_leaf2;

 PUNT leafnumber,root,puntdir;
 int keysize,i,ind;
 UCHR tmp[LE2+1];

 invp=DBXifmap;
 keysize=vlex[treecase];


  if (treecase == 0) {
   memmove(nw_leaf1.idx[0].key,key,keysize);
   nw_leaf1.idx[0].info1=TSTblk;
   nw_leaf1.idx[0].info2=TSToff;
#if LIND /* 57 */
   nw_leaf1.idx[0].info3info4.info3=(INFO)0;
#endif /* LIND 57 */
   l0p=(L0STRU *)&nw_leaf1;
 }
 else {
   memmove(nw_leaf2.idx[0].key,key,keysize);
   nw_leaf2.idx[0].info1=TSTblk;
   nw_leaf2.idx[0].info2=TSToff;
#if LIND /* 58 */
   nw_leaf2.idx[0].info3info4.info3=(INFO)0;
#endif /* LIND 58 */
   l0p=(L0STRU *)&nw_leaf2;
 }
 ind=0;
 leafnumber=invp->cn[treecase].fmaxpos+1;
 /* Completa informacoes da nova pagina */
  l0p->it=treecase+1;
  l0p->pos=leafnumber;
  l0p->ock=1;
  l0p->ps=0;

 /* Branqueia itens disponiveis na folha velha */
 upif_branqueia(tmp,keysize);
 for (i=ind+1;i<TWORDF;i++){   /*Atencao: Indices de vetor 0..twordf-1*/
   if (treecase == 0) {
      memmove(nw_leaf1.idx[i].key,tmp,keysize);
      nw_leaf1.idx[i].info1=(INFO )0;
      nw_leaf1.idx[i].info2=(INFO )0;
#if LIND /* 59 */
      nw_leaf1.idx[i].info2=1L;
      nw_leaf1.idx[i].info3info4.info3=(INFO)0;
#endif /* LIND 59 */
   }
   else {
      memmove(nw_leaf2.idx[i].key,tmp,keysize);
      nw_leaf2.idx[i].info1=(INFO )0;
      nw_leaf2.idx[i].info2=(INFO )0;
#if LIND /* 59 */
      nw_leaf2.idx[i].info2=1L;
      nw_leaf2.idx[i].info3info4.info3=(INFO)0;
#endif /* LIND 59 */
   }
 }

 leafwrit(dbxp,l0p);

#if TRACEUPIF
   printf("\n<insert_leaf> Apos insercao na nova ");
   upif_print_leaf((L0STRU *)l0p);
#endif

 puntdir=(PUNT )0;
 root =upif_create_root(dbxp, -leafnumber ,key,
                        puntdir,treecase,ISFIRST);
/* Forca a gravar a raiz, poque no UPDATE so grava no fim*/
/*  cntwrit(dbxp); *Ja grava quando grava no e esta fazendo update no ifp*/
 return (root);
}



/* -----------------------------------------------------------------------*/
#if CICPP
PUNT CIIFU :: upif_search_btree(INVMAP *invp,
                                int     treecase,
                                UCHR   *key)
#else /*CICPP*/
PUNT upif_search_btree(invp,treecase,key)
INVMAP *invp;
int treecase;
UCHR *key;
#endif /*CICPP*/
{
  PUNT punt;
  int mx_liv;
  int pos,level;
  UCHR  *np;
  N2STRU node;
  BOOLEAN found;
  N0STRU *n0p;
  N1STRU *n1p;
  N2STRU *n2p;

  lfpath.top=0;
  punt = invp->cn[treecase].posrx;
  mx_liv=invp->cn[treecase].liv;

  if (mx_liv >=MAX_TREE_LEVEL)  /* Aumentar a definicao do array Path */
         fatal("ciifuh/upif_search_btree/MAX_TREE_LEVEL");

  /* Comeca de zero porque considera a raiz como um no normal           */

  for (level=0;level<=mx_liv;level++){
	 lfpath.stck[level]=punt;
         np = (UCHR *)noderead(invp,treecase,level,punt);
         memmove((UCHR *)&node,np,sizeof(N2STRU));
         np = (UCHR *)&node;
         n0p = (N0STRU *)np;
         n1p = (N1STRU *)np;
         n2p = (N2STRU *)np;
         found=upif_find_key_node(n0p,key,treecase,&pos);
         /* para escolher o proximo no deve-se levar em conta que
                pos aponta para a posicao onde encontrou ou onde esta a chave
                ou onde devera ser inserida
                found=true -> desvia para punt(pos);
                found=FALSE-> desvia para punt(pos-1);
                found=false e pos=0 (acho que por construcao nao deveria
                                     acontecer)
         */
         if (found==FALSE && pos==0) /* Acho que nao deveria acontecer */
           fatal("ciifuh/upif_search_btree/AQNDA");
         if (found==FALSE) pos--;

         if (treecase)
           punt=n2p->idx[pos].punt;
	 else
           punt=n1p->idx[pos].punt;
}
   /* punt=0 e mx_liv= -1 => arvore vazia */
   /* O ultimo apontador tem que ser para uma folha */
   lfpath.top=mx_liv;
   if (punt > 0) fatal("ciifuh/upif_search_btree/punt>=0");
   return(-punt);
}
/*--------------------------------------------------------------------------
  upif_fnd_pif
    .Retorna o pointer para o arquivo invertido onde estao os posts da chave
    . A atualizacao e feita de acordo com a seguinte tabela:
Puntlf   Found    Pstflag  Oper          Acoes
-------|-----------------------------------------------------------------
       |        |                 | I            Return
       |        |       Dic       |--------------------------------------
       |        |                 | D            Return
       |        |--------------------------------------------------------
       |        |                 | I            pega(blk,off) da arvore
       | TRUE   |  IFP            |              atualiza invertido
       |        -----------------------------------------------
       |        |                 | D            pega(blk,off) da arvore
       |        |                 |              atualiza invertido
EXISTE |-----------------------------------------------------------------
       |        |                 | I            blk=1;off=2;
       |        |                 |              Atualiza arvore
       |                |Dic      |--------------------------------------
       |                |         | D            Link not found
       |                |------------------------------------------------
       |                |         | I            cria new pst(&blk,&off)
       | FALSE  |  IFP  |              atualiza arvore
       |                |         ---------------------------------------
       |                |         | D            Link not found
       |        |       |
----------------|--------------------------------------------------------
-------|-----------------------------------------------------------------
        |       |                 | I            blk=1;off=2;
        |               |         |              Atualiza arvore-Cria raiz
        |               |Dic      |---------------------------------------
        |               |         | D            Link not found
  NAO   |               |-------------------------------------------------
 Existe |               |         | I            cria    new pst(&blk,&off)
        |        FALSE  |  IFP    |              cria arvore
	|               |         ----------------------------------------
        |               |         | D            Link not found
        |               |         |
        |-----------------------------------------------------------------
--------------------------------------------------------------------------*/

#if CICPP
void CIIFU :: upif_fnd_pifp(DBXSTRU  *dbxp,
		            UCHR     *key,
		            int       treecase,
		            POSTSTRU *pst,
		            UCHR      oper,
		            int       pstflag)
#else /*CICPP*/
void upif_fnd_pifp(dbxp,key,treecase,pst,oper,pstflag)
DBXSTRU  *dbxp;
UCHR *key;
int treecase;
POSTSTRU *pst;
UCHR oper;
int pstflag;
#endif /*CICPP*/
{
  INVMAP *invp;
  PUNT p_b_punt,root,node,key_punt;
  L0STRU *lp;

  L1STRU *l1p;
  L2STRU *l2p;

  UWORD isroot;
  UCHR  *p_b_key,a_p_b_key[LE2+1];
  UCHR *lbufp;
  BOOLEAN promoted;
  BOOLEAN found;
  int topo,level;
  int pos,
      keysize;
  INFO xblk,xoff;
  static L2STRU xxbufp;

/* ==================== xupd =======================> */
#if LINDLUX /* 15x */
  LONGX *luxinfo1p,*luxinfo2p,*luxinfo3p;
  char *luxkeyp;
  LONGX luxmfn;
  LONGX luxyaddr;
  int luxycase,luxn;
  unsigned char luxmask;
#endif /* LINDLUX 15x */
/* <==================== xupd ======================= */

  invp=DBXifmap;

  keysize=vlex[treecase];
  isroot=0;
  root=invp->cn[treecase].posrx;
  p_b_key=(UCHR *)a_p_b_key;
  lbufp=(UCHR *)&xxbufp;

  memset(lbufp,0x00,sizeof(L2STRU));

#if TRACEUPIF
  upif_chgtostr(key,keysize,tkey);
  printf("\nfnd key=%s treecase=%d",tkey,treecase);
#endif

  /* Pesquisa na arvore qual a folha onde deve ficar o posting  */
  puntlf=upif_search_btree( invp ,treecase,(UCHR *)key);

/* ==================== xupd =======================> */
#if LINDLUX /* 15x */
  if (invp->lvxiflind) {        /* iflind Step#2 */
    if (!invp->lvxpages) {
        invp->lvxpages=invp->lvxmaxmfn/8+1;
        if (invp->lvxpages > LVXPAGES) fatal("svdifupd/lvx/LVXPAGES");
        for (luxx=0; luxx < invp->lvxpages; luxx++ ) {
            invp->lvxvpagp[luxx]=(char *)ALLOC((ALLOPARM)LVXIYPBS);
            if (invp->lvxvpagp[luxx] == (char *)ALLONULL)
                                                fatal("svdifupd/lvx/ALLOC");
#if 0
            printf("+++ luxx=%"_LD_" coreleft=%"_LD_"\n",luxx,CORELEFT());
#endif
            memset(invp->lvxvpagp[luxx],0x00,(size_t)LVXIYPBS);
        }
    }
    if (puntlf != 0) {          /* Achou a folha */
        lp=leafread(NULL,invp,treecase,puntlf,0); /* lp=invp->luxvpagp[][] */
        found=upif_find_key_leaf((L0STRU *)lp, key,treecase, &pos);
	if (found==TRUE) {
            if (pstflag==IFUPDICT) fatal("svdifupd/lvx/pstflag");
            if (treecase==0) {
                l1p=(L1STRU *)lp;
                luxinfo1p = &(l1p->idx[pos].info1);
                luxinfo2p = &(l1p->idx[pos].info2);
                luxinfo3p = &(l1p->idx[pos].info3info4.info3);
                luxkeyp   = l1p->idx[pos].key;
            } else {
                l2p=(L2STRU *)lp;
                luxinfo1p = &(l2p->idx[pos].info1);
                luxinfo2p = &(l2p->idx[pos].info2);
                luxinfo3p = &(l2p->idx[pos].info3info4.info3);
                luxkeyp   = l2p->idx[pos].key;
            }
            /* upif_process_posting(pst,xblk,xoff,oper); */
            luxp=(char *)pst;
            luxmfn=100*(LONGX)luxp[0]+10*(LONGX)luxp[1]+(LONGX)luxp[2];
            if (luxmfn < 1 || luxmfn > invp->lvxmaxmfn) {
                fprintf(stderr,"*** key=%s mfn=%"_LD_" maxmfn=%"_LD_,
                                luxkeyp,luxmfn,invp->lvxmaxmfn);
                fatal("svdifupd/lvx/offset");
	    }
            luxyaddr = *luxinfo1p;
            if (*luxinfo2p <= 0) fatal("svdifupd/lvx/docsp");
            if (*luxinfo3p < 0) {
                luxycase=BITSTRING;
                luxyaddr+=luxmfn/8;             /* bp= &areabits[mfn/8]; */
            }
	    else
            if (*luxinfo3p == 0) {
                luxycase=MFNSTRING;
                luxyaddr-=(*luxinfo2p)*PMFNSIZ; /* (*luxinfo2p)*PMFNSIZ */
            }
            else fatal("svdifupd/lvx/case");
#if 0 /* TRPTRACE */
            printf("+++ %"_LD_" key=%s info1=%"_LD_" info2=%"_LD_" info3=%"_LD_" \n",
                luxmfn,keyp,*luxinfo1p,*luxinfo2p,*luxinfo3p);
#endif
            luxx=luxyaddr/LVXIYPBS;
            if (luxx >= invp->lvxpages) fatal("svdifupd/lvx/pages");
            luxp=invp->lvxvpagp[luxx];
            luxx=luxyaddr%LVXIYPBS;
            luxp+=luxx;
	    switch (luxycase) {
            case BITSTRING:
                /* bp =  &areabits[ luxmfn / 8]; */
                luxmask = (unsigned char)bitmask [ luxmfn & BY8RMASK ];
                if ((unsigned char)(*luxp) & luxmask) {
                    if (invp->lvxfplog)
                        fprintf(invp->lvxfplog,
                                "duplicated posting|%8"_LD_"|%s\n",
                                luxmfn,keyp);
                }
                else {
                    *luxp = (unsigned char)(*luxp) | luxmask;
                    (*luxinfo2p)--;     /* one more doc unflagged */
                }
                break;
            case MFNSTRING:
                if (luxx+PMFNSIZ < LVXIYPBS) memmove(luxp,(char *)pst,PMFNSIZ);
		else {
		    memmove(luxp,(char *)pst,(luxn=PMFNSIZ-(LVXIYPBS-luxx)));
		    luxyaddr+=luxn;
		    luxx=luxyaddr/LVXIYPBS;
		    if (luxx >= invp->lvxpages) fatal("svdifupd/lvx/pages");
		    luxp=invp->lvxvpagp[luxx];
		    luxx=luxyaddr%LVXIYPBS;
		    luxp+=luxx;
		    memmove(luxp,((char *)pst)+luxn,PMFNSIZ-luxn);
		}
                (*luxinfo2p)--;		/* one more doc unflagged */
	    	(*luxinfo1p)+=PMFNSIZ;	/* one more doc stored */
                break;
            } /* end of switch luxycase */
        }
        else fatal("svdifupd/lvx/found");
    }
    else fatal("svdifupd/lvx/puntlf");
  }
#endif /* LINDLUX 15x */
/* <==================== xupd ======================= */

  if (puntlf != 0) {            /* Achou a folha */
        lp=leafread(lbufp,invp,treecase,puntlf,0);
#if TRACEUPIF2
        printf("\n<fnd> Vai pesquisar na folha ");
        upif_print_leaf((L0STRU*)lp);
#endif
        found=upif_find_key_leaf((L0STRU *)lp, key,treecase, &pos);
  }

  if ( (puntlf !=0) && (found==TRUE)) {
#if !LIND /* 15 */
        if (pstflag==IFUPDICT) return;
          if (treecase==0) {
                 l1p=(L1STRU *)lp;
                 xblk=l1p->idx[pos].info1;
                 xoff=l1p->idx[pos].info2;
           }else {
                 l2p=(L2STRU *)lp;
                 xblk=l2p->idx[pos].info1;
                 xoff=l2p->idx[pos].info2;
           }
           upif_process_posting(pst,xblk,xoff,oper);
#else /* !LIND 15 */
        if (pstflag!=IFUPDICT && pstflag!=IFUPISIS) fatal("ciifuh/lind");
          if (treecase==0) {
                 l1p=(L1STRU *)lp;
                 l1p->idx[pos].info2++;
		 leafwrit(dbxp,(L0STRU *)l1p);
           }else {
                 l2p=(L2STRU *)lp;
                 l2p->idx[pos].info2++;
                 leafwrit(dbxp,(L0STRU *)l2p);
           }
#endif /* !LIND 15 */
#if TRACEUPIF
         printf("\nfnd Encontrou chave folha=%"_LD_" pos=%d ",lp->pos,pos);
#endif
  }

  if ((puntlf !=0 ) && (found==FALSE)) {

        if (oper == DELETION){
                 upif_print_msg(pst,(UCHR *)lnk_not_found_msg);
                 return;
        }

#if !LIND /* 13 */
        if (pstflag == IFUPDICT) {
          xblk=1L;
	  xoff=2L;
        }else {
          upif_process_new_key(pst,&xblk,&xoff);
        }

#else /* !LIND 13 */
        xblk=0L;
        xoff=1L;
#endif /* !LIND 13 */

   promoted=upif_insert_leaf(
                        dbxp,(L0STRU *)lp,key,treecase,p_b_key,&p_b_punt,
                        pos,xblk,xoff);

   topo=lfpath.top;
                /* se houve necessidade de criar um novo no,
                   faz insercao em niveis superiores */
   for (level=topo; level>=0 && promoted==TRUE; level--){
           memmove(key,p_b_key,keysize);
           key_punt=p_b_punt;
           node=lfpath.stck[level];
#if TRACEUPIF
	   upif_chgtostr(p_b_key,keysize,tkey); /* AOT 11/09/2000 */
           printf("\nfnd Vai promover para node=%"_LD_" keypunt=%"_LD_" key=%s",
                                node,key_punt,tkey);
#endif
                   isroot=(root==node)?TRUE:FALSE;
           promoted=upif_insert_index(dbxp,node,treecase,level,isroot,
                                                  key,key_punt,
                                                  p_b_key,&p_b_punt);
        } /* for  */

         /* Se houve promocao ate a raiz cria uma nova raiz           */
         if (promoted==TRUE){
           memmove(key,p_b_key,keysize);
           root=upif_create_root(dbxp,node,key,p_b_punt,treecase,NOFIRST);
         }
  }

  if (puntlf == 0) {
        if (oper == DELETION) {
          upif_print_msg(pst,(UCHR *)lnk_not_found_msg);
          return;
        }

#if !LIND /* 12 */
        if (pstflag == IFUPDICT) {
          xblk=1L;
          xoff=2L;
        }else {
          upif_process_new_key(pst,&xblk,&xoff);
        }
#else /* !LIND 12 */
        xblk=0L; xoff=1L;
#endif /* !LIND 12 */

        root=upif_init_index(dbxp,treecase,key,xblk,xoff);
  }
}
