#!/bin/bash

# echo hello world
g++ schedule.cpp -o schedule
# ./schedule <<< '10'

# # ./schedule; echo $?
# a=`./schedule <<< 10`; echo $a;

a1=0
n=10
for i in {1..10}
do	
	read -a value <<< `./schedule <<< '10'` $i;
	echo $i;
	for element in "${value[@]}"
	do
		echo "$element ";
	done
	echo ${value[0]}
	a1=`python -c "print("%.6f" % ($a1+${value[0]}))"`
	echo $a1;
	# a1=`awk "BEGIN {print 1+3.5; exit}"`
	# unset values
done
z=`python -c "print($a1/$n)"`
echo $z
