if test -d ~/bin
then
    echo "~/bin already exists"
else
    echo "creating ~/bin"
    mkdir ~/bin
fi
echo "enter 0 clean all .o and generated .c files"
echo "enter 999 to skip"
read select
if [ $select -ne 999 ]
then
    if [ $select -eq 0 ]
       then
	   rm info2.c
	   rm lib2.c
	   rm life2.c
	   rm regexp/*.o
	   rm *.o
	   rm wild_life
	   rm wild_life_X
    fi
fi

cd regexp
make >djd3 2>djd4
cd ..
$EDITOR regexp/djd3 regexp/djd4
make >djd1 2>djd2
$EDITOR djd1 djd2
if test -f wild_life.exe
then	
    cp wild_life.exe ~/bin
fi
if test -f wild_life
then
    cp wild_life ~/bin
fi
if test -f wild_life_X
then
    cp wild_life_X ~/bin
fi
