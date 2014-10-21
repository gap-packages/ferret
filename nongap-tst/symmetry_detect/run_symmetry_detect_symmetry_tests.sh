cd $(dirname $0)
passes=0
fails=0
for x in symmetry_detect_tests/*.graph.json
do
	./symmetry_detect_test.sh -json $x symmetry_detect_tests/$(basename $x .graph.json).expectedGroup.json
	if [ $? -eq 0 ] ; then
		((passes += 1))
	else
		((fails += 1))
	fi
done

echo "number of test passed: $passes"
echo "number of tests failed: $fails"

cd - > /dev/null