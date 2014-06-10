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

echo export LC_ALL=POSIX  >xmk.xsh
echo mkdir  utl          >>xmk.xsh
echo mkdir  utl/linux    >>xmk.xsh

echo mkdir utl/linux/isis       >mkisis.xsh
echo mkdir utl/linux/isis1660   >mkisis1660.xsh
echo mkdir utl/linux/isisG      >mkisisG.xsh
echo mkdir utl/linux/lind       >mklind.xsh
echo mkdir utl/linux/lindG4     >mklindG4.xsh
echo mkdir utl/linux/lind512    >mklind512.xsh
echo mkdir utl/linux/lind512G4  >mklind512G4.xsh
echo mkdir utl/linux/ffi        >mkffi.xsh
echo mkdir utl/linux/ffiG4      >mkffiG4.xsh
echo mkdir utl/linux/ffi512     >mkffi512.xsh
echo mkdir utl/linux/ffi512G4   >mkffi512G4.xsh
echo mkdir utl/linux/ffi1660    >mkffi1660.xsh

echo mkdir utl/linux/BigIsis    >mkBigIsis.xsh

./mx seq=xls "pft='sh -x mxmake32.xsh utl/linux/isis       'left(v1,size(v1)-4)' CIFFI=0 LIND=0 LIND4=0 ISISXL=0 ISISXL512=0 _FILE_OFFSET_BITS=00 _LARGEFILE64_SOURCE=0'/" lw=0 now >>mkisis.xsh
./mx seq=xls "pft='sh -x mxmake32.xsh utl/linux/isis1660   'left(v1,size(v1)-4)' CIFFI=0 LIND=0 LIND4=0 ISISXL=1 ISISXL512=0 _FILE_OFFSET_BITS=00 _LARGEFILE64_SOURCE=0'/" lw=0 now >>mkisis1660.xsh
./mx seq=xls "pft='sh -x mxmake32.xsh utl/linux/isisG      'left(v1,size(v1)-4)' CIFFI=0 LIND=0 LIND4=0 ISISXL=0 ISISXL512=0 _FILE_OFFSET_BITS=64 _LARGEFILE64_SOURCE=1'/" lw=0 now >>mkisisG.xsh
./mx seq=xls "pft='sh -x mxmake32.xsh utl/linux/lind       'left(v1,size(v1)-4)' CIFFI=0 LIND=1 LIND4=0 ISISXL=1 ISISXL512=0 _FILE_OFFSET_BITS=00 _LARGEFILE64_SOURCE=0'/" lw=0 now >>mklind.xsh
./mx seq=xls "pft='sh -x mxmake32.xsh utl/linux/lindG4     'left(v1,size(v1)-4)' CIFFI=0 LIND=1 LIND4=1 ISISXL=1 ISISXL512=0 _FILE_OFFSET_BITS=64 _LARGEFILE64_SOURCE=1'/" lw=0 now >>mklindG4.xsh
./mx seq=xls "pft='sh -x mxmake32.xsh utl/linux/lind512    'left(v1,size(v1)-4)' CIFFI=0 LIND=1 LIND4=0 ISISXL=1 ISISXL512=1 _FILE_OFFSET_BITS=00 _LARGEFILE64_SOURCE=0'/" lw=0 now >>mklind512.xsh
./mx seq=xls "pft='sh -x mxmake32.xsh utl/linux/lind512G4  'left(v1,size(v1)-4)' CIFFI=0 LIND=1 LIND4=1 ISISXL=1 ISISXL512=1 _FILE_OFFSET_BITS=64 _LARGEFILE64_SOURCE=1'/" lw=0 now >>mklind512G4.xsh
./mx seq=xls "pft='sh -x mxmake32.xsh utl/linux/ffi        'left(v1,size(v1)-4)' CIFFI=1 LIND=1 LIND4=0 ISISXL=1 ISISXL512=0 _FILE_OFFSET_BITS=00 _LARGEFILE64_SOURCE=0'/" lw=0 now >>mkffi.xsh
./mx seq=xls "pft='sh -x mxmake32.xsh utl/linux/ffiG4      'left(v1,size(v1)-4)' CIFFI=1 LIND=1 LIND4=1 ISISXL=1 ISISXL512=0 _FILE_OFFSET_BITS=64 _LARGEFILE64_SOURCE=1'/" lw=0 now >>mkffiG4.xsh
./mx seq=xls "pft='sh -x mxmake32.xsh utl/linux/ffi512     'left(v1,size(v1)-4)' CIFFI=1 LIND=1 LIND4=0 ISISXL=1 ISISXL512=1 _FILE_OFFSET_BITS=00 _LARGEFILE64_SOURCE=0'/" lw=0 now >>mkffi512.xsh
./mx seq=xls "pft='sh -x mxmake32.xsh utl/linux/ffi512G4   'left(v1,size(v1)-4)' CIFFI=1 LIND=1 LIND4=1 ISISXL=1 ISISXL512=1 _FILE_OFFSET_BITS=64 _LARGEFILE64_SOURCE=1'/" lw=0 now >>mkffi512G4.xsh
./mx seq=xls "pft='sh -x mxmake32.xsh utl/linux/ffi1660    'left(v1,size(v1)-4)' CIFFI=1 LIND=0 LIND4=0 ISISXL=1 ISISXL512=0 _FILE_OFFSET_BITS=00 _LARGEFILE64_SOURCE=0'/" lw=0 now >>mkffi1660.xsh

./mx seq=xls "pft='sh -x mxmake32.xsh utl/linux/BigIsis    'left(v1,size(v1)-4)' CIFFI=1 LIND=0 LIND4=0 ISISXL=1 ISISXL512=0 _FILE_OFFSET_BITS=64 _LARGEFILE64_SOURCE=1 SUPERISIS=1'/" lw=0 now >>mkBigIsis.xsh


cat           mkisis.xsh       >>xmk.xsh
cat           mkisis1660.xsh   >>xmk.xsh
cat           mkisisG.xsh      >>xmk.xsh
cat           mklind.xsh       >>xmk.xsh
cat           mklindG4.xsh     >>xmk.xsh
cat           mklind512.xsh    >>xmk.xsh
cat           mklind512G4.xsh  >>xmk.xsh
cat           mkffi.xsh        >>xmk.xsh
cat           mkffiG4.xsh      >>xmk.xsh
cat           mkffi512.xsh     >>xmk.xsh
cat           mkffi512G4.xsh   >>xmk.xsh
cat           mkffi1660.xsh    >>xmk.xsh

cat           mkBigIsis.xsh   >>xmk.xsh

echo          cp utl/linux/lindG4/crunchmf utl/linux/isis              >>xmk.xsh
echo          cp utl/linux/lindG4/crunchmf utl/linux/isis1660          >>xmk.xsh
echo          cp utl/linux/lindG4/crunchmf utl/linux/lind              >>xmk.xsh
echo          cp utl/linux/lindG4/crunchmf utl/linux/lind512           >>xmk.xsh

echo          cp utl/linux/ffiG4/crunchmf utl/linux/ffi                >>xmk.xsh
echo          cp utl/linux/ffiG4/crunchmf utl/linux/ffi512             >>xmk.xsh
echo          cp utl/linux/ffiG4/crunchmf utl/linux/ffi1660            >>xmk.xsh

echo          mv utl/linux/lind/ifload utl/linux/lind/iflind           >>xmk.xsh
echo          mv utl/linux/lindG4/ifload utl/linux/lindG4/iflind       >>xmk.xsh
echo          mv utl/linux/lind512/ifload utl/linux/lind512/iflind     >>xmk.xsh
echo          mv utl/linux/lind512G4/ifload utl/linux/lind512G4/iflind >>xmk.xsh
echo          mv utl/linux/ffi/ifload utl/linux/ffi/iflind             >>xmk.xsh
echo          mv utl/linux/ffiG4/ifload utl/linux/ffiG4/iflind         >>xmk.xsh
echo          mv utl/linux/ffi512/ifload utl/linux/ffi512/iflind       >>xmk.xsh
echo          mv utl/linux/ffi512G4/ifload utl/linux/ffi512G4/iflind   >>xmk.xsh
echo          mv utl/linux/ffi1660/ifload utl/linux/ffi1660/iflind     >>xmk.xsh

echo          rm utl/linux/lind/ifupd           >>xmk.xsh
echo          rm utl/linux/lindG4/ifupd         >>xmk.xsh
echo          rm utl/linux/lind512/ifupd        >>xmk.xsh
echo          rm utl/linux/lind512G4/ifupd      >>xmk.xsh
echo          rm utl/linux/ffi/ifupd            >>xmk.xsh
echo          rm utl/linux/ffiG4/ifupd          >>xmk.xsh
echo          rm utl/linux/ffi512/ifupd         >>xmk.xsh
echo          rm utl/linux/ffi512G4/ifupd       >>xmk.xsh

echo          rm utl/linux/isis/wtrig*          >>xmk.xsh
echo          rm utl/linux/isis1660/wtrig*      >>xmk.xsh
echo          rm utl/linux/isisG/wtrig*         >>xmk.xsh
echo          rm utl/linux/lind/wtring*         >>xmk.xsh
echo          rm utl/linux/lindG4/wtrig*        >>xmk.xsh
echo          rm utl/linux/lind512/wtrig*       >>xmk.xsh
echo          rm utl/linux/lind512G4/wtrig*     >>xmk.xsh

echo          mkdir wxis                        >>xmk.xsh
echo          mkdir wxis/linux                  >>xmk.xsh
echo          mkdir wxis/linux/isis             >>xmk.xsh
echo          mkdir wxis/linux/isis1660         >>xmk.xsh
echo          mkdir wxis/linux/isisG            >>xmk.xsh
echo          mkdir wxis/linux/lind             >>xmk.xsh
echo          mkdir wxis/linux/lindG4           >>xmk.xsh
echo          mkdir wxis/linux/lind512          >>xmk.xsh
echo          mkdir wxis/linux/lind512G4        >>xmk.xsh
echo          mkdir wxis/linux/ffi              >>xmk.xsh
echo          mkdir wxis/linux/ffiG4            >>xmk.xsh
echo          mkdir wxis/linux/ffi512           >>xmk.xsh
echo          mkdir wxis/linux/ffi512G4         >>xmk.xsh
echo          mkdir wxis/linux/ffi1660          >>xmk.xsh
echo          mkdir wxis/linux/BigIsis          >>xmk.xsh

echo          cp utl/linux/isis/wxis wxis/linux/isis            >>xmk.xsh
echo          cp utl/linux/isis1660/wxis wxis/linux/isis1660    >>xmk.xsh
echo          cp utl/linux/isis1660/wxis wxis/linux/isisG       >>xmk.xsh
echo          cp utl/linux/lind/wxis wxis/linux/lind            >>xmk.xsh
echo          cp utl/linux/lindG4/wxis wxis/linux/lindG4        >>xmk.xsh
echo          cp utl/linux/lind512/wxis wxis/linux/lind512      >>xmk.xsh
echo          cp utl/linux/lind512G4/wxis wxis/linux/lind512G4  >>xmk.xsh
echo          cp utl/linux/ffi/wxis wxis/linux/ffi              >>xmk.xsh
echo          cp utl/linux/ffiG4/wxis wxis/linux/ffiG4          >>xmk.xsh
echo          cp utl/linux/ffi512/wxis wxis/linux/ffi512        >>xmk.xsh
echo          cp utl/linux/ffi512G4/wxis wxis/linux/ffi512G4    >>xmk.xsh
echo          cp utl/linux/ffi1660/wxis wxis/linux/ffi1660      >>xmk.xsh
echo          cp utl/linux/BigIsis/wxis wxis/linux/BigIsis      >>xmk.xsh

echo "nohup time sh -x xmk.xsh &"
nohup time sh -x xmk.xsh &

echo "compilation finished !!!"
exit
