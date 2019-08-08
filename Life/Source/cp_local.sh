if test -f ~/life_local
then
    rm -r ~/life_local
fi
cp -r ../life_local ~
cd
chmod -R u+r life_local
