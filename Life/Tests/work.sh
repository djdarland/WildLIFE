echo "WildLife Test Suite Utility"
echo "Examine Test Results For LF/$1.lf ?"
echo "enter 0 to proceed 888 to next, or crtl-c to quit"
read answer
while [ $answer -ne 888 ]
do
    echo "1 LF/$1.lf"
    if test -f IN/$1.in 
    then
	echo "2 IN/$1.in"
    fi
    if test -f REFDIFF/$1.refdiff.tmp
    then
	echo "3 REFDIFF/$1.refdiff.tmp"
	echo "4 REFOUT_UPDATED/$1.refout OUT/$1.out.tmp"
    fi
    if test -f ERRDIFF/$1.errdiff.tmp 
    then
	echo "5 ERRDIFF/$1.errdiff.tmp"
	echo "6 REFERR_UPDATED/$1.referr ERR/$1.err.tmp"
    fi
    echo "10 Rerun wild_life on $1"

    echo "11 Approve New $1 Output To Reference"
    echo "12 Restore $1 Original 1.02 Output Reference"
    echo "13 Cannot expect match - move LF/$1.lf to LFSAV/$1.lf"
    echo "14 Run wild_life interactively"
    echo "16 edit Port.txt"

    echo "888 NEXT FILE SET"
    echo "ctrl-c QUIT"
    echo "select"
    read select
    case $select in
	1) $EDITOR LF/$1.lf;;
	2) $EDITOR IN/$1.in;;
	3) $EDITOR REFDIFF/$1.refdiff.tmp;;
	4) $EDITOR REFOUT_UPDATED/$1.refout OUT/$1.out.tmp;;
	5) $EDITOR ERRDIFF/$1.errdiff.tmp;;
	6) $EDITOR REFERR_UPDATED/$1.referr ERR/$1.err.tmp;;
	10) ./check_edit_all "2" $1;;
	11) ./approve_out.sh $1;;
	12) ./rest_out.sh $1;;
	13) mv LF/$1.lf LFSAV/$1.lf ;;
	14) wild_life;;
	16) $EDITOR ../Source/Port.txt;;
	888) exit 2;;
    esac
done


