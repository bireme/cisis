export LC_ALL=POSIX

echo crunchif.mak  > xls
echo crunchmf.mak >> xls
echo ctlmfn.mak   >> xls
echo i2id.mak     >> xls
echo id2i.mak     >> xls
echo ifkeys.mak   >> xls
echo ifload.mak   >> xls
echo ifmerge.mak  >> xls
echo ifupd.mak    >> xls
echo mkiy0.mak    >> xls
echo mkxrf.mak    >> xls
echo msrt.mak     >> xls
echo mx.mak       >> xls
echo mxcp.mak     >> xls
echo mxf0.mak     >> xls
echo mxtb.mak     >> xls
echo mys.mak      >> xls
echo retag.mak    >> xls
echo wxis.mak     >> xls

echo export LC_ALL=POSIX    >xmk.sh
echo mkdir  utl            >>xmk.sh
echo mkdir  utl/linux64    >>xmk.sh

echo mkdir utl/linux64/isis       >mkisis_64.sh
echo mkdir utl/linux64/isis1660   >mkisis1660_64.sh
echo mkdir utl/linux64/isisG      >mkisisG_64.sh
echo mkdir utl/linux64/lind       >mklind_64.sh
echo mkdir utl/linux64/lindG4     >mklindG4_64.sh
echo mkdir utl/linux64/lind512    >mklind512_64.sh
echo mkdir utl/linux64/lind512G4  >mklind512G4_64.sh
echo mkdir utl/linux64/ffi        >mkffi_64.sh
echo mkdir utl/linux64/ffiG4      >mkffiG4_64.sh
echo mkdir utl/linux64/ffi512     >mkffi512_64.sh
echo mkdir utl/linux64/ffi512G4   >mkffi512G4_64.sh
echo mkdir utl/linux64/ffi1660    >mkffi1660_64.sh

echo mkdir utl/linux64/BigIsis    >mkBigIsis_64.sh

./mx seq=xls "pft='sh -x mxmake64.sh utl/linux64/isis     'left(v1,size(v1)-4)' CIFFI=0 LIND=0 LIND4=0 ISISXL=0 ISISXL512=0  SIXTY_FOUR=1 _FILE_OFFSET_BITS=00 _LARGEFILE64_SOURCE=0'/" lw=0 now >>mkisis_64.sh
./mx seq=xls "pft='sh -x mxmake64.sh utl/linux64/isis1660 'left(v1,size(v1)-4)' CIFFI=0 LIND=0 LIND4=0 ISISXL=1 ISISXL512=0  SIXTY_FOUR=1 _FILE_OFFSET_BITS=00 _LARGEFILE64_SOURCE=0'/" lw=0 now >>mkisis1660_64.sh
./mx seq=xls "pft='sh -x mxmake64.sh utl/linux64/isisG    'left(v1,size(v1)-4)' CIFFI=0 LIND=0 LIND4=0 ISISXL=0 ISISXL512=0  SIXTY_FOUR=1 _FILE_OFFSET_BITS=64 _LARGEFILE64_SOURCE=1'/" lw=0 now >>mkisisG_64.sh
./mx seq=xls "pft='sh -x mxmake64.sh utl/linux64/lind     'left(v1,size(v1)-4)' CIFFI=0 LIND=1 LIND4=0 ISISXL=1 ISISXL512=0  SIXTY_FOUR=1 _FILE_OFFSET_BITS=00 _LARGEFILE64_SOURCE=0'/" lw=0 now >>mklind_64.sh
./mx seq=xls "pft='sh -x mxmake64.sh utl/linux64/lindG4   'left(v1,size(v1)-4)' CIFFI=0 LIND=1 LIND4=1 ISISXL=1 ISISXL512=0  SIXTY_FOUR=1 _FILE_OFFSET_BITS=64 _LARGEFILE64_SOURCE=1'/" lw=0 now >>mklindG4_64.sh
./mx seq=xls "pft='sh -x mxmake64.sh utl/linux64/lind512  'left(v1,size(v1)-4)' CIFFI=0 LIND=1 LIND4=0 ISISXL=1 ISISXL512=1  SIXTY_FOUR=1 _FILE_OFFSET_BITS=00 _LARGEFILE64_SOURCE=0'/" lw=0 now >>mklind512_64.sh
./mx seq=xls "pft='sh -x mxmake64.sh utl/linux64/lind512G4  'left(v1,size(v1)-4)' CIFFI=0 LIND=1 LIND4=1 ISISXL=1 ISISXL512=1  SIXTY_FOUR=1 _FILE_OFFSET_BITS=64 _LARGEFILE64_SOURCE=1'/" lw=0 now >>mklind512G4_64.sh
./mx seq=xls "pft='sh -x mxmake64.sh utl/linux64/ffi      'left(v1,size(v1)-4)' CIFFI=1 LIND=1 LIND4=0 ISISXL=1 ISISXL512=0  SIXTY_FOUR=1 _FILE_OFFSET_BITS=00 _LARGEFILE64_SOURCE=0'/" lw=0 now >>mkffi_64.sh
./mx seq=xls "pft='sh -x mxmake64.sh utl/linux64/ffiG4    'left(v1,size(v1)-4)' CIFFI=1 LIND=1 LIND4=1 ISISXL=1 ISISXL512=0  SIXTY_FOUR=1 _FILE_OFFSET_BITS=64 _LARGEFILE64_SOURCE=1'/" lw=0 now >>mkffiG4_64.sh
./mx seq=xls "pft='sh -x mxmake64.sh utl/linux64/ffi512   'left(v1,size(v1)-4)' CIFFI=1 LIND=1 LIND4=0 ISISXL=1 ISISXL512=1  SIXTY_FOUR=1 _FILE_OFFSET_BITS=00 _LARGEFILE64_SOURCE=0'/" lw=0 now >>mkffi512_64.sh
./mx seq=xls "pft='sh -x mxmake64.sh utl/linux64/ffi512G4 'left(v1,size(v1)-4)' CIFFI=1 LIND=1 LIND4=1 ISISXL=1 ISISXL512=1  SIXTY_FOUR=1 _FILE_OFFSET_BITS=64 _LARGEFILE64_SOURCE=1'/" lw=0 now >>mkffi512G4_64.sh
./mx seq=xls "pft='sh -x mxmake64.sh utl/linux64/ffi1660  'left(v1,size(v1)-4)' CIFFI=1 LIND=0 LIND4=0 ISISXL=1 ISISXL512=0  SIXTY_FOUR=1 _FILE_OFFSET_BITS=00 _LARGEFILE64_SOURCE=0'/" lw=0 now >>mkffi1660_64.sh

./mx seq=xls "pft='sh -x mxmake64.sh utl/linux64/BigIsis    'left(v1,size(v1)-4)' CIFFI=1 LIND=0 LIND4=0 ISISXL=1 ISISXL512=0 SIXTY_FOUR=1  _FILE_OFFSET_BITS=64 _LARGEFILE64_SOURCE=1 SUPERISIS=1'/" lw=0 now >>mkBigIsis_64.sh


cat           mkisis_64.sh       >>xmk.sh
cat           mkisis1660_64.sh   >>xmk.sh
cat           mkisisG_64.sh      >>xmk.sh
cat           mklind_64.sh       >>xmk.sh
cat           mklindG4_64.sh     >>xmk.sh
cat           mklind512_64.sh    >>xmk.sh
cat           mklind512G4_64.sh  >>xmk.sh
cat           mkffi_64.sh        >>xmk.sh
cat           mkffiG4_64.sh      >>xmk.sh
cat           mkffi512_64.sh     >>xmk.sh
cat           mkffi512G4_64.sh   >>xmk.sh
cat           mkffi1660_64.sh    >>xmk.sh

cat           mkBigIsis_64.sh   >>xmk.sh

echo          cp utl/linux64/lindG4/crunchmf utl/linux64/isis            >>xmk.sh
echo          cp utl/linux64/lindG4/crunchmf utl/linux64/isis1660        >>xmk.sh
echo          cp utl/linux64/lindG4/crunchmf utl/linux64/lind            >>xmk.sh
echo          cp utl/linux64/lindG4/crunchmf utl/linux64/lind512         >>xmk.sh

echo          cp utl/linux64/ffiG4/crunchmf utl/linux64/ffi              >>xmk.sh
echo          cp utl/linux64/ffiG4/crunchmf utl/linux64/ffi512           >>xmk.sh
echo          cp utl/linux64/ffiG4/crunchmf utl/linux64/ffi1660        >>xmk.sh

echo          mv utl/linux64/lind/ifload utl/linux64/lind/iflind             >>xmk.sh
echo          mv utl/linux64/lindG4/ifload utl/linux64/lindG4/iflind         >>xmk.sh
echo          mv utl/linux64/lind512/ifload utl/linux64/lind512/iflind       >>xmk.sh
echo          mv utl/linux64/lind512G4/ifload utl/linux64/lind512G4/iflind   >>xmk.sh
echo          mv utl/linux64/ffi/ifload utl/linux64/ffi/iflind               >>xmk.sh
echo          mv utl/linux64/ffiG4/ifload utl/linux64/ffiG4/iflind           >>xmk.sh
echo          mv utl/linux64/ffi512/ifload utl/linux64/ffi512/iflind         >>xmk.sh
echo          mv utl/linux64/ffi512G4/ifload utl/linux64/ffi512G4/iflind     >>xmk.sh
echo          mv utl/linux64/ffi1660/ifload utl/linux64/ffi1660/iflind       >>xmk.sh

echo          rm utl/linux64/lind/ifupd       >>xmk.sh
echo          rm utl/linux64/lindG4/ifupd     >>xmk.sh
echo          rm utl/linux64/lind512/ifupd    >>xmk.sh
echo          rm utl/linux64/lind512G4/ifupd  >>xmk.sh
echo          rm utl/linux64/ffi/ifupd        >>xmk.sh
echo          rm utl/linux64/ffiG4/ifupd      >>xmk.sh
echo          rm utl/linux64/ffi512/ifupd     >>xmk.sh
echo          rm utl/linux64/ffi512G4/ifupd   >>xmk.sh

echo          mkdir wxis                                   >>xmk.sh
echo          mkdir wxis/linux64                           >>xmk.sh
echo          mkdir wxis/linux64/isis                      >>xmk.sh
echo          mkdir wxis/linux64/isis1660                  >>xmk.sh
echo          mkdir wxis/linux64/isisG                     >>xmk.sh
echo          mkdir wxis/linux64/lind                      >>xmk.sh
echo          mkdir wxis/linux64/lindG4                    >>xmk.sh
echo          mkdir wxis/linux64/lind512                   >>xmk.sh
echo          mkdir wxis/linux64/lind512G4                 >>xmk.sh
echo          mkdir wxis/linux64/ffi                       >>xmk.sh
echo          mkdir wxis/linux64/ffiG4                     >>xmk.sh
echo          mkdir wxis/linux64/ffi512                    >>xmk.sh
echo          mkdir wxis/linux64/ffi512G4                  >>xmk.sh
echo          mkdir wxis/linux64/ffi1660                   >>xmk.sh

echo          mkdir wxis/linux64/BigIsis       >>xmk.sh

echo          cp utl/linux64/isis/wxis wxis/linux64/isis                >>xmk.sh
echo          cp utl/linux64/isis1660/wxis wxis/linux64/isis1660        >>xmk.sh
echo          cp utl/linux64/isisG/wxis wxis/linux64/isisG
echo          cp utl/linux64/lind/wxis wxis/linux64/lind            >>xmk.sh
echo          cp utl/linux64/lindG4/wxis wxis/linux64/lindG4        >>xmk.sh
echo          cp utl/linux64/lind512/wxis wxis/linux64/lind512      >>xmk.sh
echo          cp utl/linux64/lind512G4/wxis wxis/linux64/lind512G4  >>xmk.sh
echo          cp utl/linux64/ffi/wxis wxis/linux64/ffi              >>xmk.sh
echo          cp utl/linux64/ffiG4/wxis wxis/linux64/ffiG4          >>xmk.sh
echo          cp utl/linux64/ffi512/wxis wxis/linux64/ffi512        >>xmk.sh
echo          cp utl/linux64/ffi512G4/wxis wxis/linux64/ffi512G4    >>xmk.sh
echo          cp utl/linux64/ffi1660/wxis wxis/linux64/ffi1660      >>xmk.sh

echo          cp utl/linux64/BigIsis wxis/linux64/BigIsis  >>xmk.sh

echo "nohup time sh -x xmk.sh &"
nohup time sh -x xmk.sh &

echo "compilation finished !!!"
exit
