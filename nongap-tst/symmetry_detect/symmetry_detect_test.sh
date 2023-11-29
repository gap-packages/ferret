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
	json=${2}
	 $(dirname $0)/jsonToGap.py <(symmetry_detect --json "$json")  "$3" | gap -q | grep 'true' > /dev/null
	status=$?
elif [ "$1" = "-eprime" ]  ; then
	savilerow "$2" -var-sym-breaking > /dev/null
	if [ $? -ne 0 ] ; then
		echo "error in running savilerow on input file $2"
		exit 2
	fi
	json=${2}.json
	$(dirname $0)/jsonToGap.py <(symmetry_detect --json "$json") "$3" | gap -q | grep 'true' > /dev/null
	status=$?
	else
		echo $usage
		exit 1
fi

if [ $status -eq 0 ] ; then
	echo "passed for input file $2 compared against permutation group $3."
	if [ "$1" = "-eprime" ] ; then
		rm -f ${2}.aux ${2}.json ${2}.minion
	fi
else
	echo "failed test with input file $2 Compared against permutation group $3."
	echo "symmetry groups being compared:"
	symmetry_detect "$json" -q
	echo "--------"
	cat "$3"
		echo "--------"
fi

exit $status
