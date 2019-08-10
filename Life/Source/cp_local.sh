if test -f ~/life_local_redo
then
    rm -r ~/life_local_redo
fi
cp -r ../life_local_redo ~
if test -f ~/life_local_xxxx
then
    rm -r ~/life_local_xxxx
fi
cp -r ../life_local_xxxx ~
cd
chmod -R u+r life_local
