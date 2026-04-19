#!/bin/sh
cd count_test
for i in * ;
do
	if [ -e  ../count_test_ref/"$i" ] ; then
		../../yasss -c < "$i" | diff - ../count_test_ref/"$i";
		if [ $? != "0" ]; then 
			echo -n 'output of `../../yasss -c` < '"$i"' and '
			echo "its reference file differ!"
			exit 1
		fi
	fi
done
