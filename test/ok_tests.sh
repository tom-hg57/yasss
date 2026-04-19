#!/bin/sh
cd ok_tests
for i in * ;
do
	if [ -e  ../ok_test_ref/"$i" ] ; then
		../../yasss < "$i" | diff - ../ok_test_ref/"$i";
		if [ $? != "0" ]; then 
			echo -n 'output of `../../yasss` < '"$i"' and its reference'
			echo " file differ!"
			exit 1
		fi
	fi
done
