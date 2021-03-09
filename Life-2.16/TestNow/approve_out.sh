if test -f OUT/$1.out
then
cp OUT/$1.out REFOUT_UPDATED/$1.refout
else
rm REFOUT_UPDATED/$1.refout
touch REFOUT_UPDATED/$1.refout
fi
if test -f ERR/$1.err
then
cp ERR/$1.err REFERR_UPDATED/$1.referr
else
rm REFERR_UPDATED/$1.referr
touch REFERR_UPDATED/$1.referr
fi
echo "$1 approved" >APPROVED/$1.approved

