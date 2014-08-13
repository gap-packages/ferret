cd $(dirname $0)
passes=0
fails=0
for x in savilerow_symmetry_tests/*.eprime
do
	./symmetry_detect_test.sh -eprime $x savilerow_symmetry_tests/$(basename $x .eprime).expectedGroup.json
	if [ $? -eq 0 ] ; then
		((passes += 1))
	else
		((fails += 1))
	fi
done

echo "number of test passed: $passes"
echo "number of tests failed: $fails"

cd - > /dev/null