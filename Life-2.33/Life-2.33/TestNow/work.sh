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
    if test -f REFDIFF/$1.refdiff
    then
	echo "3 REFDIFF/$1.refdiff"
	echo "4 OUT/$1.out REFOUT_UPDATED/$1.refout"
    fi
    if test -f ERRDIFF/$1.errdiff 
    then
	echo "5 ERRDIFF/$1.errdiff"
	echo "6 ERR/$1.err REFERR_UPDATED/$1.referr"
    fi
    echo "11 Approve New $1 Output To Reference"
    echo "12 Restore $1 Original 1.02 Output Reference"

    echo "888 NEXT FILE SET"
    echo "ctrl-c QUIT"
    echo "select"
    read select
    case $select in
	1) $EDITOR LF/$1.lf;;
	2) $EDITOR IN/$1.in;;
	3) $EDITOR REFDIFF/$1.refdiff;;
	4) $EDITOR OUT/$1.out REFOUT_UPDATED/$1.refout;;
	5) $EDITOR ERRDIFF/$1.errdiff;;
	6) $EDITOR ERR/$1.err REFERR_UPDATED/$1.referr;;
	11) ./approve_out.sh $1;;
	12) ./rest_out.sh $1;;
	888) exit 2;;
    esac
done


