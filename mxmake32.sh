
 export LC_ALL=POSIX
 rm *.o
 rm $1/$2

 make -f $2.mak CC=gcc             $3 $4 $5 $6 $7 $8 $9 ${10} ${11} ${12} ${13}
 mv $2 $1
 $1/$2 | head -3

