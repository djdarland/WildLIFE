if test -f OUT/$1.out.tmp
then
cp OUT/$1.out.tmp REFOUT-X/$1.refout
else
rm REFOUT-X/$1.refout
touch REFOUT-X/$1.refout
fi
if test -f ERR/$1.err.tmp
then
cp ERR/$1.err.tmp REFERR-X/$1.referr
else
rm REFERR-X/$1.referr
touch REFERR-X/$1.referr
fi
echo "$1 approved" >APPROVED-X/$1.approved
echo "$1 approved"

