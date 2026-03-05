#!/bin/bash
# Problem2.sh

# Input file
file="test2_2.txt"

# Current date (for age calculation)
current_year=2023
current_month=10
current_day=01

# Use awk to handle parsing, filtering, and age calculation
awk -F', ' -v y=$current_year -v m=$current_month -v d=$current_day '
{
    name = $1
    split($2, dob, "-")
    city = $3
    country = $4

    # Check if city has two or more words
    if (city ~ /[[:alpha:]]+[[:space:]]+[[:alpha:]]+/) {
        birth_year = dob[1]
        birth_month = dob[2]
        birth_day = dob[3]

        age = y - birth_year
        if (birth_month > m || (birth_month == m && birth_day > d)) {
            age -= 1
        }

	# Store in a sortable form
        printf "%d\t%s\n", age, name
    }
}' "$file" | sort -nr -k1,1 | awk -F'\t' '{ print $2 " is " $1 }'

