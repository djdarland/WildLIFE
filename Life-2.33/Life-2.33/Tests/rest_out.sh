if test -f REFOUT/$1.refout
then
cp REFOUT/$1.refout REFOUT_UPDATED/$1.refout
fi
if test -f REFERR/$1.referr
then
cp REFERR/$1.referr REFERR_UPDATED/$1.referr
fi
echo "$1 Restored" >RESTORED/$1.restored
