#!/bin/bash

# check if there are enough arguments
# if there are less than 3 arguments or path to exe is missing then print out error message
# and the script does nothing else
if [ $# -lt 3 ] || [ ${1:0:2} != "./" ]
then
	echo "Invalid argument list."
	echo "Proper usage: myScript <path to executable> m1 [m2 [m3 [...]]]"

# other wise script will continue	
else
	#variable max to represent the highest number in argument list
	max=0
	# tranverse through the arugment list skipping the first argument 
	# since the first argument is the program executable name
	# loop to check for the highest int in the argument list
	for count in ${@:2}
	do 
		if [ $count -gt $max ]
		then
			max=$count
		fi
	done
	
	# after highest value is found run the prog executable
	# with the highest int in the argument list
	$1 $max

    # variable count to help control the loop
    # intialized as 2 because want to tranverse through argument list skipping the first argument
	count=2
    
    # for loop to get all possible int pairs
	for i in ${@:$count}
	do
		# increment so that program will not launch
		# with the same argument twice
		((count=count+1))
		# tranverse through list starting at the next argument 
		# after argument i 
		for j in ${@:$count}
		do
			# launch the prog executable with int i and j
			$1 $i $j
		done
	done
fi
