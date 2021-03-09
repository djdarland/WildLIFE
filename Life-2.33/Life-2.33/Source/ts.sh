cd ../Tests
./check_edit_all 1
if [ $1 -eq 1 ]
then 
echo "enter 1 to edit the temporary results script." 
echo "you can eliminate some parts you do not wish to look at now."
read choice
if [ $choice -eq 1 ]
then
$EDITOR editdiffs.sh;
fi
fi
./editdiffs.sh
