if test -f REFOUT/$1.refout
then
cp REFOUT-1.02/$1.refout REFOUT/$1.refout
fi
if test -f REFERR/$1.referr
then
cp REFERR-1.02/$1.referr REFERR/$1.referr
fi
echo "$1 Restored" >RESTORED/$1.restored
