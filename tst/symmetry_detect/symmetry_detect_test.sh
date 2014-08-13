#!/usr/bin/env bash

#validate arguments
usage='usage:  [-eprime eprime_file]|[-json json_graph] permutation_group_json_file'
if [ $# -ne 3 ] ; then
	echo $usage
	exit 1
fi

tempfile=".symmetry_tester_temp_file"
#run savilerow if eprime symmetry detect if json, exit if invalid args
if [ "$1" = "-json" ] ; then
	symmetry_detect "$2" -q > $tempfile  && $(dirname $0)/jsonToGap.py $tempfile  "$3" | gap.sh -q | grep 'true' > /dev/null
	status=$?
elif [ "$1" = "-eprime" ]  ; then
	savilerow "$2" -json > /dev/null
	if [ $? -ne 0 ] ; then
		echo "error in running savilerow on input file $2"
		exit 2
	fi
	json=${2}.json
	symmetry_detect "$json" -q > $tempfile  && $(dirname $0)/jsonToGap.py $tempfile  "$3" | tee test.gap |  gap.sh -q | grep 'true' > /dev/null
	status=$?
	rm -f ${2}.aux ${2}.json ${2}.minion
	else
		echo $usage
		exit 1
fi

if [ $status -eq 0 ] ; then
	echo "passed for input file $2 compared against permutation group $3."
else
	echo "failed test with input file $2 Compared against permutation group $3."
	echo "symmetry groups being compared:"
	cat $tempfile
	echo "--------"
	cat "$3"
		echo "--------"
fi

rm $tempfile
exit $status
