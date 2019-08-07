rm info2.c
rm lib2.c
rm life2.c
rm regexp/*.o
rm *.o
rm *~
cd regexp
make >djd3 2>djd4
cd ..
make >djd1 2>djd2
if test -f wild_life.exe
then	
cp wild_life.exe ~/bin
fi
if test -f wild_life
then
cp wild_life ~/bin
fi
$EDITOR djd3 djd4
$EDITOR djd1 djd2
