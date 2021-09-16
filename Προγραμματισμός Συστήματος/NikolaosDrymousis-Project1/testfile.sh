#!/bin/bash


if [ $# -ne 4 ];
then
	echo "Not enough arguments"
	exit 1
else
	if [ ! -f "$1" ];
	then
		echo "second argument must be a file"
		exit 1
	fi
	
	if [ ! -f "$2" ];
	then
		echo "third argument must be a file"
		exit 1
	fi

	re='^[0-9]+$'

	if ! [[ $3 =~ $re ]];
	then
		echo "fourth argument must be an integer"
		exit 1
	fi

	re='^[0-1]+$'

	if ! [[ $4 =~ $re ]];
	then
		echo "fifth argument must be 0 or 1"
		exit 1
	fi


fi

p=($(shuf -i 0-"$3" -n "$3" | sort -n))

declare -A nameArray
declare -A lastArray
declare -A ageArray
declare -A countryArray

for r in "${p[@]}";
do
	id=($(shuf -i 1-9999 -n 1))

	if [ "$4" == 0 ];
	then
		if [ -v nameArray[$id] ];
		then
			name=${nameArray[$id]}
		else
			name=($(head -1 <(fold -w 20  <(tr -dc 'a-zA-Z' < /dev/urandom) | cut -c -"$((RANDOM%10+2))")))
			nameArray[$id]="$name"
		fi
		
		if [ -v lastArray[$id] ];
		then
			lastname=${lastArray[$id]}
		else
			lastname=($(head -1 <(fold -w 20  <(tr -dc 'a-zA-Z' < /dev/urandom) | cut -c -"$((RANDOM%10+2))")))
			lastArray[$id]="$lastname"
		fi	

		if [ -v countryArray[$id] ];
		then
			country=${countryArray[$id]}
		else
			country=($(shuf -n 1 "$2"))
			countryArray[$id]="$country"
		fi
		
		if [ -v ageArray[$id] ];
		then
			age=${ageArray[$id]}
		else
			age=($(shuf -i 1-120 -n 1))
			ageArray[$id]="$age"
		fi
	else
		name=($(head -1 <(fold -w 20  <(tr -dc 'a-zA-Z' < /dev/urandom) | cut -c -"$((RANDOM%10+5))")))
		lastname=($(head -1 <(fold -w 20  <(tr -dc 'a-zA-Z' < /dev/urandom) | cut -c -"$((RANDOM%10+5))")))
		country=($(shuf -n 1 "$2"))
		age=($(shuf -i 1-120 -n 1))
	fi

	virus=($(shuf -n 1 "$1"))

	if [ "$((RANDOM%2))" == 1 ];
	then
		answer="YES"
		vac_date=($(date -d "$(shuf -i 1960-2021 -n 1)-$((RANDOM%12+1))-$((RANDOM%29+1))" '+%d-%m-%Y'))
		echo "$id $name $lastname $country $age $virus $answer $vac_date" >> inputFile.txt
	else
		answer="NO"
		echo "$id $name $lastname $country $age $virus $answer" >> inputFile.txt
	fi
done
