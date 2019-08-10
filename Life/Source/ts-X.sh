rm ../Tests/REFDIFF-X/*.tmp
rm ../Tests/ERRDIFF-X/*.tmp
rm ../Tests/OUT/*.tmp
rm ../Tests/ERR/*.tmp
cd ../Tests
./check_edit_all-X 1
echo "enter 1 to edit the temporary results script." 
echo "you can eliminate some parts you do not wish to look at now."
read choice
if [ $choice -eq 1 ]
then
$EDITOR editdiffs.sh;
fi
./editdiffs.sh
