#!/bin/bash


if [ $# -ne 3 ];
then
	echo "Not enough arguments"
	exit 1
else
	if [ ! -f "$1" ];
	then
		echo "second argument must be a file"
		exit 1
	fi

	re='^[0-9]+$'

	if ! [[ $3 =~ $re ]];
	then
		echo "fourth argument must be an integer"
		exit 1
	fi


fi

declare -A countryFile

countryArray=()

mkdir "$2"

while read line; do
# reading each line
readarray -d ' ' -t strarr <<<"$line" #split a string based on the delimiter ':'  

if [[ "${countryArray[@]}" =~ "${strarr[3]}" ]];
then
	cd "$2"
	cd "${strarr[3]}"
	if [ -v countryFile[${strarr[3]}] ];
	then
		let "countryFile[${strarr[3]}]+=1"
		if [[ countryFile[${strarr[3]}] -gt "$3" ]];
		then
			countryFile[${strarr[3]}]=1
		fi

		temp=$( printf '%d' ${countryFile[${strarr[3]}]} )

		filename="${strarr[3]}-$temp.txt"

	fi

	echo "$line" >> "$filename"
	cd ".."
	cd ".."

else
	cd "$2"
	mkdir "${strarr[3]}"
	cd "${strarr[3]}"
	
	countryFile[${strarr[3]}]=1
	temp=$( printf '%d' ${countryFile[${strarr[3]}]} )

	filename="${strarr[3]}-$temp.txt"

	echo "$line" >> "$filename"
	chmod 777 "$filename"
	countryArray+="${strarr[3]}"
	cd ".."
	cd ".."
fi


done < "$1"