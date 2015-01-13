rm nohup.out
rm *.o
rm mk*.sh
rm xmk.sh
rm xls
make -f mx.mak SIXTY_FOUR=1
rm *.o
rm -fr utl
rm -fr wxis
sh ./makemake64.xsh
