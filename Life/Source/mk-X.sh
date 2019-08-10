# 1.02 compatible version
# no "END Abort" in built_ins.c
#
rm c_life.a
rm info2.c
rm lib2.c
rm life2.c
rm regexp/*.o
rm *.o
rm *~
cd regexp
make >djd3 2>djd4
cd ..
make -f Makefile.X >djd1 2>djd2
# check for cygwin
if test -f wild_life_X.exe
then	
cp wild_life_X.exe ~/bin
fi
if test -f wild_life_X
then
cp wild_life_X ~/bin
fi
$EDITOR djd3 djd4
$EDITOR djd1 djd2
