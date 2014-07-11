export LC_ALL=POSIX

ls *.mak >xls

echo export LC_ALL=POSIX  >xmk.xsh
echo mkdir  utl          >>xmk.xsh
echo mkdir  utl/linux64  >>xmk.xsh

echo mkdir utl/linux64/isis      >mkisis_64.xsh
echo mkdir utl/linux64/isis1660  >mkisis1660_64.xsh
echo mkdir utl/linux64/isisG     >mkisisG_64.xsh
echo mkdir utl/linux64/lind      >mklind_64.xsh
echo mkdir utl/linux64/lindG4    >mklindG4_64.xsh
echo mkdir utl/linux64/lind512   >mklind512_64.xsh
echo mkdir utl/linux64/lind512G4 >mklind512G4_64.xsh
echo mkdir utl/linux64/ffi       >mkffi_64.xsh
echo mkdir utl/linux64/ffiG4     >mkffiG4_64.xsh
echo mkdir utl/linux64/ffiG4_4   >mkffiG4_4_64.xsh
echo mkdir utl/linux64/ffi512    >mkffi512_64.xsh
echo mkdir utl/linux64/ffi512G4  >mkffi512G4_64.xsh
echo mkdir utl/linux64/ffi1660   >mkffi1660_64.xsh

echo mkdir utl/linux64/BigIsis   >mkBigIsis_64.xsh

./mx seq=xls "pft='sh -x mxmake64.xsh utl/linux64/isis     'left(v1,size(v1)-4)' CIFFI=0 LIND=0 LIND4=0 ISISXL=0 ISISXL512=0  SIXTY_FOUR=1 _FILE_OFFSET_BITS=00 _LARGEFILE64_SOURCE=0'/" lw=0 now >>mkisis_64.xsh
./mx seq=xls "pft='sh -x mxmake64.xsh utl/linux64/isis1660 'left(v1,size(v1)-4)' CIFFI=0 LIND=0 LIND4=0 ISISXL=1 ISISXL512=0  SIXTY_FOUR=1 _FILE_OFFSET_BITS=00 _LARGEFILE64_SOURCE=0'/" lw=0 now >>mkisis1660_64.xsh
./mx seq=xls "pft='sh -x mxmake64.xsh utl/linux64/isisG    'left(v1,size(v1)-4)' CIFFI=0 LIND=0 LIND4=0 ISISXL=0 ISISXL512=0  SIXTY_FOUR=1 _FILE_OFFSET_BITS=64 _LARGEFILE64_SOURCE=1'/" lw=0 now >>mkisisG_64.xsh
./mx seq=xls "pft='sh -x mxmake64.xsh utl/linux64/lind     'left(v1,size(v1)-4)' CIFFI=0 LIND=1 LIND4=0 ISISXL=1 ISISXL512=0  SIXTY_FOUR=1 _FILE_OFFSET_BITS=00 _LARGEFILE64_SOURCE=0'/" lw=0 now >>mklind_64.xsh
./mx seq=xls "pft='sh -x mxmake64.xsh utl/linux64/lindG4   'left(v1,size(v1)-4)' CIFFI=0 LIND=1 LIND4=1 ISISXL=1 ISISXL512=0  SIXTY_FOUR=1 _FILE_OFFSET_BITS=64 _LARGEFILE64_SOURCE=1'/" lw=0 now >>mklindG4_64.xsh
./mx seq=xls "pft='sh -x mxmake64.xsh utl/linux64/lind512  'left(v1,size(v1)-4)' CIFFI=0 LIND=1 LIND4=0 ISISXL=1 ISISXL512=1  SIXTY_FOUR=1 _FILE_OFFSET_BITS=00 _LARGEFILE64_SOURCE=0'/" lw=0 now >>mklind512_64.xsh
./mx seq=xls "pft='sh -x mxmake64.xsh utl/linux64/lind512G4  'left(v1,size(v1)-4)' CIFFI=0 LIND=1 LIND4=1 ISISXL=1 ISISXL512=1  SIXTY_FOUR=1 _FILE_OFFSET_BITS=64 _LARGEFILE64_SOURCE=1'/" lw=0 now >>mklind512G4_64.xsh
./mx seq=xls "pft='sh -x mxmake64.xsh utl/linux64/ffi      'left(v1,size(v1)-4)' CIFFI=1 LIND=1 LIND4=0 ISISXL=1 ISISXL512=0  SIXTY_FOUR=1 _FILE_OFFSET_BITS=00 _LARGEFILE64_SOURCE=0'/" lw=0 now >>mkffi_64.xsh
./mx seq=xls "pft='sh -x mxmake64.xsh utl/linux64/ffiG4    'left(v1,size(v1)-4)' CIFFI=1 LIND=1 LIND4=1 ISISXL=1 ISISXL512=0  SIXTY_FOUR=1 _FILE_OFFSET_BITS=64 _LARGEFILE64_SOURCE=1'/" lw=0 now >>mkffiG4_64.xsh
./mx seq=xls "pft='sh -x mxmake64.xsh utl/linux64/ffiG4_4   'left(v1,size(v1)-4)' CIFFI=1 LIND=1 LIND4=1 ISISXL=1 ISISXL512=0  SIXTY_FOUR=1 _FILE_OFFSET_BITS=64 _LARGEFILE64_SOURCE=1 MAXMFRL=4194304'/" lw=0 now >>mkffiG4_4_64.xsh
./mx seq=xls "pft='sh -x mxmake64.xsh utl/linux64/ffi512   'left(v1,size(v1)-4)' CIFFI=1 LIND=1 LIND4=0 ISISXL=1 ISISXL512=1  SIXTY_FOUR=1 _FILE_OFFSET_BITS=00 _LARGEFILE64_SOURCE=0'/" lw=0 now >>mkffi512_64.xsh
./mx seq=xls "pft='sh -x mxmake64.xsh utl/linux64/ffi512G4 'left(v1,size(v1)-4)' CIFFI=1 LIND=1 LIND4=1 ISISXL=1 ISISXL512=1  SIXTY_FOUR=1 _FILE_OFFSET_BITS=64 _LARGEFILE64_SOURCE=1'/" lw=0 now >>mkffi512G4_64.xsh
./mx seq=xls "pft='sh -x mxmake64.xsh utl/linux64/ffi1660  'left(v1,size(v1)-4)' CIFFI=1 LIND=0 LIND4=0 ISISXL=1 ISISXL512=0  SIXTY_FOUR=1 _FILE_OFFSET_BITS=00 _LARGEFILE64_SOURCE=0'/" lw=0 now >>mkffi1660_64.xsh

./mx seq=xls "pft='sh -x mxmake64.xsh utl/linux64/BigIsis    'left(v1,size(v1)-4)' CIFFI=1 LIND=0 LIND4=0 ISISXL=1 ISISXL512=0 SIXTY_FOUR=1  _FILE_OFFSET_BITS=64 _LARGEFILE64_SOURCE=1 SUPERISIS=1'/" lw=0 now >>mkBigIsis_64.xsh


cat           mkisis_64.xsh     >>xmk.xsh
cat           mkisis1660_64.xsh >>xmk.xsh
cat           mkisisG_64.xsh    >>xmk.xsh
cat           mklind_64.xsh     >>xmk.xsh
cat           mklindG4_64.xsh   >>xmk.xsh
cat           mklind512_64.xsh  >>xmk.xsh
cat           mklind512G4_64.xsh  >>xmk.xsh
cat           mkffi_64.xsh      >>xmk.xsh
cat           mkffiG4_64.xsh    >>xmk.xsh
cat           mkffiG4_4_64.xsh  >>xmk.xsh
cat           mkffi512_64.xsh   >>xmk.xsh
cat           mkffi512G4_64.xsh >>xmk.xsh
cat           mkffi1660_64.xsh  >>xmk.xsh

cat           mkBigIsis_64.xsh  >>xmk.xsh

echo          cp utl/linux64/lindG4/crunchmf utl/linux64/isis            >>xmk.xsh
echo          cp utl/linux64/lindG4/crunchmf utl/linux64/isis1660        >>xmk.xsh
echo          cp utl/linux64/lindG4/crunchmf utl/linux64/lind            >>xmk.xsh
echo          cp utl/linux64/lindG4/crunchmf utl/linux64/lind512         >>xmk.xsh

echo          cp utl/linux64/ffiG4/crunchmf utl/linux64/ffi              >>xmk.xsh
echo          cp utl/linux64/ffiG4/crunchmf utl/linux64/ffi512           >>xmk.xsh
echo          cp utl/linux64/ffiG4/crunchmf utl/linux64/ffi1660          >>xmk.xsh

echo          mv utl/linux64/lind/ifload utl/linux64/lind/iflind             >>xmk.xsh
echo          mv utl/linux64/lindG4/ifload utl/linux64/lindG4/iflind         >>xmk.xsh
echo          mv utl/linux64/lind512/ifload utl/linux64/lind512/iflind       >>xmk.xsh
echo          mv utl/linux64/lind512G4/ifload utl/linux64/lind512G4/iflind   >>xmk.xsh
echo          mv utl/linux64/ffi/ifload utl/linux64/ffi/iflind               >>xmk.xsh
echo          mv utl/linux64/ffiG4/ifload utl/linux64/ffiG4/iflind           >>xmk.xsh
echo          mv utl/linux64/ffiG4_4/ifload utl/linux64/ffiG4_4/iflind       >>xmk.xsh
echo          mv utl/linux64/ffi512/ifload utl/linux64/ffi512/iflind         >>xmk.xsh
echo          mv utl/linux64/ffi512G4/ifload utl/linux64/ffi512G4/iflind     >>xmk.xsh
echo          mv utl/linux64/ffi1660/ifload utl/linux64/ffi1660/iflind       >>xmk.xsh

echo          rm utl/linux64/lind/ifupd         >>xmk.xsh
echo          rm utl/linux64/lindG4/ifupd       >>xmk.xsh
echo          rm utl/linux64/lind512/ifupd      >>xmk.xsh
echo          rm utl/linux64/lind512G4/ifupd    >>xmk.xsh
echo          rm utl/linux64/ffi/ifupd          >>xmk.xsh
echo          rm utl/linux64/ffiG4/ifupd        >>xmk.xsh
echo          rm utl/linux64/ffiG4_4/ifupd      >>xmk.xsh
echo          rm utl/linux64/ffi512/ifupd       >>xmk.xsh
echo          rm utl/linux64/ffi512G4/ifupd     >>xmk.xsh

echo          rm utl/linux64/isis/wtrig*            >>xmk.xsh
echo          rm utl/linux64/isis1660/wtrig*        >>xmk.xsh
echo          rm utl/linux64/isisG/wtrig*           >>xmk.xsh
echo          rm utl/linux64/lind/wtring*           >>xmk.xsh
echo          rm utl/linux64/lindG4/wtrig*          >>xmk.xsh
echo          rm utl/linux64/lind512/wtrig*         >>xmk.xsh
echo          rm utl/linux64/lind512G4/wtrig*       >>xmk.xsh

echo          mkdir wxis                                     >>xmk.xsh
echo          mkdir wxis/linux64                             >>xmk.xsh
echo          mkdir wxis/linux64/isis                        >>xmk.xsh
echo          mkdir wxis/linux64/isis1660                    >>xmk.xsh
echo          mkdir wxis/linux64/isisG                       >>xmk.xsh
echo          mkdir wxis/linux64/lind                        >>xmk.xsh
echo          mkdir wxis/linux64/lindG4                      >>xmk.xsh
echo          mkdir wxis/linux64/lind512                     >>xmk.xsh
echo          mkdir wxis/linux64/lind512G4                   >>xmk.xsh
echo          mkdir wxis/linux64/ffi                         >>xmk.xsh
echo          mkdir wxis/linux64/ffiG4                       >>xmk.xsh
echo          mkdir wxis/linux64/ffiG4_4                     >>xmk.xsh
echo          mkdir wxis/linux64/ffi512                      >>xmk.xsh
echo          mkdir wxis/linux64/ffi512G4                    >>xmk.xsh
echo          mkdir wxis/linux64/ffi1660                     >>xmk.xsh
echo          mkdir wxis/linux64/BigIsis                     >>xmk.xsh

echo          cp utl/linux64/isis/wxis wxis/linux64/isis                >>xmk.xsh
echo          cp utl/linux64/isis1660/wxis wxis/linux64/isis1660        >>xmk.xsh
echo          cp utl/linux64/isisG/wxis wxis/linux64/isisG              >>xmk.xsh
echo          cp utl/linux64/lind/wxis wxis/linux64/lind                >>xmk.xsh
echo          cp utl/linux64/lindG4/wxis wxis/linux64/lindG4            >>xmk.xsh
echo          cp utl/linux64/lind512/wxis wxis/linux64/lind512          >>xmk.xsh
echo          cp utl/linux64/lind512G4/wxis wxis/linux64/lind512G4      >>xmk.xsh
echo          cp utl/linux64/ffi/wxis wxis/linux64/ffi                  >>xmk.xsh
echo          cp utl/linux64/ffiG4/wxis wxis/linux64/ffiG4              >>xmk.xsh
echo          cp utl/linux64/ffiG4_4/wxis wxis/linux64/ffiG4_4          >>xmk.xsh
echo          cp utl/linux64/ffi512/wxis wxis/linux64/ffi512            >>xmk.xsh
echo          cp utl/linux64/ffi512G4/wxis wxis/linux64/ffi512G4        >>xmk.xsh
echo          cp utl/linux64/ffi1660/wxis wxis/linux64/ffi1660          >>xmk.xsh
echo          cp utl/linux64/BigIsis/wxis wxis/linux64/BigIsis          >>xmk.xsh

echo "nohup time sh -x xmk.xsh &"
nohup time sh -x xmk.xsh &

echo "compilation finished !!!"
exit
