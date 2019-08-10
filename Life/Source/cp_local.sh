if test -f ~/life_local
then
    rm -r ~/life_local
fi
cp -r ../life_local ~
if test -f ~/life_local_xxxx
then
    rm -r ~/life_local_xxxx
fi
cp -r ../life_local_xxxx ~
cd
chmod -R u+r life_local
