#!/bin/bash

# Vamos fazer manobras simples com todas as versoes de CISIS:
#	id2i CDS.id create=CDS
#	mx CDS now -all
#	mx CDS iso=CDS.iso now -all
#	mx iso=CDS.iso create=CDS now -all
#	mx CDS "fst=@" fullinv=CDS
#	mx dict=CDS now -all
#	mx CDS "water * plants" now
#	mx CDS "text=water" now
#	wxis IsisScript=teste01.xis db=CDS
#		mfnrange
#		keyrange
#		search

# Codigo .xis utilizado
#
#<IsisScript name="teste01">
#<section name="main">
#
# <field action="cgi" tag="5000">db</field>
#
# <display>Percorrendo 10 registros...
#</display>
# <do task="mfnrange">
#	<parm name="db"><pft>v5000</pft></parm>
#	<parm name="from">1</parm>
#	<parm name="to">10</parm>
#	<loop>
#		<!-- display><pft>mfn(3),' '</pft></display -->
#	</loop>
# </do>
# <display>Percorrendo o IF...
#</display>
# <do task="keyrange">
#	<field action="define" tag="1000">Isis_Key</field>
#	<field action="define" tag="1001">Isis_Current</field>
#	<field action="define" tag="1002">Isis_Total</field>
#	<field action="define" tag="1031">Isis_From</field>
#	<field action="define" tag="1091">Isis_Status</field>
#	<field action="define" tag="1092">Isis_ErrorInfo</field>
#	<parm name="db"><pft>v5000</pft></parm>
#	<parm name="from">A</parm>
#	<parm name="to">ZZZZ</parm>
#	<loop>
#		<display><pft>v1000/</pft></display>
#	</loop>
# </do>
#
# <display>Efetuando uma busca...
#</display>
# <do task="search">
#	<field action="define" tag="1001">Isis_Current</field>
#	<field action="define" tag="1002">Isis_Total</field>
#	<field action="define" tag="1031">Isis_From</field>
#	<field action="define" tag="1091">Isis_Status</field>
#	<field action="define" tag="1092">Isis_ErrorInfo</field>
#	<parm name="db"><pft>v5000</pft></parm>
#	<parm name="expression">water * plants</parm>
#	<loop>
#		<display><pft>mfn(3)/</pft></display>
#	</loop>
# </do>
#
#</section>
#</IsisScript>
#

# Ajusta versão em teste
#for i in BigIsis ffi ffi1660 ffi512 ffi512G4 ffiG4 ffiG4_4 isis isis1660 isisG lind lind512 lind512G4 lindG4
#for i in isis isis1660 lind ffi lindG4 ffiG4

#  entrada: parametros com o nome das versoes a serem testadas
#    saida: console out com o resultado de execucao
# corrente: .
#  chamada: teste03.sh [-h|-V|--changelog] [parm1] [parm2] [parm3] .. [parmN]
#  exemplo: ./teste03.sh isis isis1660 lind ffi1660 ffi lindG4 ffiG4
#    notas: deve estar disponivel no diretorio corrente o arquivo CDS.id e
#           as versoes a serem testadas (conforme parmX) no diretorio VUT
# -------------------------------------------------------------------------- #
#  Centro Latino-Americano e do Caribe de Informação em Ciências da Saúde
#     é um centro especialidado da Organização Pan-Americana da Saúde,
#           escritório regional da Organização Mundial da Saúde
#                        BIREME / OPS / OMS (P)2014
# -------------------------------------------------------------------------- #
# Historico
# versao data, Responsavel
#	- Descricao
cat > /dev/null <<HISTORICO
vrs:  0.00 20140515, FJLopes
	- Edicao original
vrs:  0.01 20140516, FJLopes
	- Aprimoramento da chamada do comando
vrs:  0.02 20140520, FJLopes
	- Correcao de erro de sintaxe no codigo fonte
HISTORICO

# ========================================================================== #
#                                  Funcoes                                   #
# ========================================================================== #
# isNumber - Determina se o parametro eh numerico
# PARM $1  - String a verificar se eh numerica ou nao
# Obs.    `-eq` soh opera bem com numeros portanto se nao for numero da erro
#
isNumber() {
	[ "$1" -eq "$1" ] 2> /dev/null
	return $?
}

# ========================================================================== #
#   ciclo - Ciclo basico de teste
# PARM $1 - Sujeito do teste
# Obs.      
ciclo() {
	export CISIS_DIR=$VUT/$i
	echo "-------------------------------------------------------"
	echo "Iniciando teste com $CISIS_DIR"
	echo 
	$CISIS_DIR/mx what
	echo "Carga de .id"
	$CISIS_DIR/id2i CDS.id create=CDS
	if [ $? != 0 ]; then echo "Erro $i. Carregando .ID"; exit 1; fi
	echo "Leitura de MF"
	$CISIS_DIR/mx CDS now -all
	if [ $? != 0 ]; then echo "Erro $i. Lendo MF"; exit 1; fi
	echo "Geracao de ISO-2709"
	$CISIS_DIR/mx CDS iso=CDS.iso now -all
	if [ $? != 0 ]; then echo "Erro $i. Gerando ISO"; exit 1; fi
	echo "Carga de ISO-2709"
	$CISIS_DIR/mx iso=CDS.iso create=CDS now -all
	if [ $? != 0 ]; then echo "Erro $i. Carregando ISO"; exit 1; fi
	echo "Geracao de IF"
	$CISIS_DIR/mx CDS "fst=@" fullinv=CDS
	if [ $? != 0 ]; then echo "Erro $i. Gerando IF"; exit 1; fi
	echo "Leitura de IF"
	$CISIS_DIR/mx dict=CDS now -all
	if [ $? != 0 ]; then echo "Erro $i. Lendo IF"; exit 1; fi
	echo "Busca booleana com mx"
	$CISIS_DIR/mx CDS "water * plants" "pft=mfn/"  now
	if [ $? != 0 ]; then echo "Erro $i. Busca booleana"; exit 1; fi
	echo "Busca livre por 'water' com mx"
	$CISIS_DIR/mx CDS "text=water" "pft=mfn/" now
	if [ $? != 0 ]; then echo "Erro $i. Busca livre"; exit 1; fi
	echo "====================="
	echo "Testes usando WWWISIS"
	echo "====================="
	$CISIS_DIR/wxis IsisScript=teste01.xis db=CDS
	if [ $? != 0 ]; then echo "Erro $i. Uso do WWWISIS"; exit 1; fi
	echo
	unset CISIS_DIR
}

# Valores default
unset LISTA
VUT=../utl/linux64

# Anotacoes de inicio de processamento
CURRD=$(pwd)
HINIC=$(date '+%s')
HRINI=$(date '+%Y.%m.%d %H:%M:%S')
_DIA_=$(date '+%d')
_MES_=$(date '+%m')
_ANO_=$(date '+%Y')
TREXE=$(basename $0)
PRGDR=$(dirname $0)
LCORI=$*

#        1         2         3         4         5         6         7         8
#2345678901234567890123456789012345678901234567890123456789012345678901234567890
AJUDA_USO="
Uso: $TREXE [-h|-V|--changelog] [-f FILENAME] | [PARM1] [PARM2] .. [PARMn]

OPCOES:
 -h, --help              Exibe este texto de ajuda e para a execucao
     --info              Exibe texto sobre a operacao do teste
 -V, --version           Exibe a versao corrente do comando e para a execucao
 -d, --debug NIVEL       Define nivel de depuracao com valor numerico positivo
     --changelog         Exibe o historico de alteracoes
 -f, --file-in FILENAME  Testa as versoes contidas no arquivo indicado
 -32                     Efetua teste de compilacoes de 32 bits
 -64                     Efetua teste de compilacoes de 64 bits (DEFAULT)

PARAMETROS:
 PARM1   diretorio com a versao 1 a testar
 PARM2   diretorio com a versao 2 a testar
 PARMn   diretorio com a versao n a testar"

#        1         2         3         4         5         6         7         8
#2345678901234567890123456789012345678901234567890123456789012345678901234567890
INFORMATIVO="
O teste efetua algumas operacoes basicas com o CISIS e tres operacoes com o
WWWISIS, na ordem relacionada a seguir, para cada versao informada:
	1-Carga da base CDS a partir di arquivo CDS.id 
	2-Leitura do MASTER-FILE CDS
	3-Geracao do arquivo ISO-2709 CDS.iso
	4-Carga do arquivo ISO-28709 CDS.iso
	5-Geracao do INVERTED-FILE CDS
	6-Leitura do INVERTED-FILE CDS
	7-Busca booleana com a expressao 'WATER * PLANTS'
	8-Execucao de IsisScript que efetua tres operacoes:
		A-MFNRANGE da base CDS
		B-KETRANGE do invertido CDS
		C-SEARCH na base CDS por 'WATER * PLANTS'

A estrutura de diretorios eh tal que exista um diretorio linux64 contendo um
diretorio para cada versao de compilacao dos utilitarios e WWWISIS, cujo nome
do diretorio eh o mesmo do parametro informado na chamada para teste.
"

while test -n "$1"
do
	case "$1" in

		--info)
			echo -n "$TREXE "
			grep '^vrs: ' $PRGDR/$TREXE | tail -1
			echo "$AJUDA_USO"
			echo "$INFORMATIVO"
			exit
			;;

		-h | --help)
			echo "$AJUDA_USO"
			exit
			;;

		-V | --version)
			echo -e -n "\n$TREXE "
			grep '^vrs: ' $PRGDR/$TREXE | tail -1
			echo
			exit
			;;

		-d | --debug)
			shift
			isNumber $1
			[ $? -ne 0 ] && echo -e "\n$TREXE: O argumento da opcao DEBUG deve existir e ser numerico.\n$AJUDA_USO" && exit 2
			DEBUG=$1
			;;

		--changelog)
			TOTLN=$(wc -l $0 | awk '{ print $1 }')
			INILN=$(grep -n "<SPICEDHAM" $0 | tail -1 | cut -d ":" -f "1")
			LINHAI=$(expr $TOTLN - $INILN)
			LINHAF=$(expr $LINHAI - 2)
			echo -e -n "\n$TREXE "
			grep '^vrs: ' $PRGDR/$TREXE | tail -1
			echo -n "==> "
			tail -$LINHAI $0 | head -$LINHAF
			echo
			unset	TOTLN	INILN	LINHAI	LINHAF
			exit
			;;

		-f | --file-in)
			shift
			LISTA=$1
			;;

		-32)
			VUT=../utl/linux
			;;

		-64)
			VUT=../utl/linux64
			;;

		*)
			if [ $(expr index "$1" "-") -ne 1 ]; then
				if test -z "$PARM1";  then PARM1=$1;  shift; continue; fi
				if test -z "$PARM2";  then PARM2=$1;  shift; continue; fi
				if test -z "$PARM3";  then PARM3=$1;  shift; continue; fi
				if test -z "$PARM4";  then PARM4=$1;  shift; continue; fi
				if test -z "$PARM5";  then PARM5=$1;  shift; continue; fi
				if test -z "$PARM6";  then PARM6=$1;  shift; continue; fi
				if test -z "$PARM7";  then PARM7=$1;  shift; continue; fi
				if test -z "$PARM8";  then PARM8=$1;  shift; continue; fi
				if test -z "$PARM9";  then PARM9=$1;  shift; continue; fi
				if test -z "$PARM10"; then PARM10=$1; shift; continue; fi
				if test -z "$PARM11"; then PARM11=$1; shift; continue; fi
				if test -z "$PARM12"; then PARM12=$1; shift; continue; fi
				if test -z "$PARM13"; then PARM13=$1; shift; continue; fi
				if test -z "$PARM14"; then PARM14=$1; shift; continue; fi
				if test -z "$PARM15"; then PARM15=$1; shift; continue; fi
				if test -z "$PARM16"; then PARM16=$1; shift; continue; fi
				if test -z "$PARM17"; then PARM17=$1; shift; continue; fi
				if test -z "$PARM18"; then PARM18=$1; shift; continue; fi
				if test -z "$PARM19"; then PARM19=$1; shift; continue; fi
				if test -z "$PARM20"; then PARM20=$1; shift; continue; fi
			else
				echo "Opcao nao valida: ($1)"
			fi
			;;
	esac
	# Argumento tratado, desloca os parametros e trata o proximo
	shift
done

if [ -z "$LISTA" ]; then
	[ -z "$PARM1" ] && PARM1="isis"
fi

if [ ! -z "$LISTA" ]; then
	for i in $(< $LISTA)
	do
		ciclo $1
	done
else
	for i in $PARM1 $PARM2 $PARM3 $PARM4 $PARM5 $PARM6 $PARM7 $PARM8 $PARM9 $PARM10 $PARM11 $PARM12 $PARM13 $PARM14 $PARM15 $PARM16 $PARM17 $PARM18 $PARM12 $PARM19 $PARM20
	do
		ciclo $1
	done
fi

# -------------------------------------------------------------------------- #
cat > /dev/null <<SPICEDHAM
CHANGELOG
20140515 Edicao original
20140515 Insercao de opcoes de execucao
	 -1 mini-help
	 -2 exibicao de Versao
	 -3 exibicao de changelog
	 -4 indicacao de arquivo com lista de versoes 
20140520 Correcao da sintaxe do teste na linha 301 ([ -z "$PARM1" ])
	 Adicao de mensagens de sinalizacao de operacao em execucao
SPICEDHAM

