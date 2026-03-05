#!/bin/bash
# Problem1.sh

# Input file
file="test2-1.txt"

# Array to hold dates
dates=()

# Regex for dates in formats: MM/DD/YYYY, MM-DD-YYYY, MM.DD.YYYY
regex='[0-9]{2}[./-][0-9]{2}[./-][0-9]{4}'

# Read file line by line
while IFS= read -r line; do
    # Extract all matching dates using grep
    matches=$(echo "$line" | grep -oE "$regex")
    # Add each date to the array
    if [[ -n "$matches" ]]; then
        while IFS= read -r date; do
            dates+=("$date")
        done <<< "$matches"
    fi
done < "$file"

# Sort the dates by converting them to YYYYMMDD for comparison, but keep original format
sorted_dates=$(printf "%s\n" "${dates[@]}" | \
    awk '{
        orig=$0
        gsub(/[\/.-]/," ",$0)
        split($0,a," ")
        printf "%04d%02d%02d %s\n", a[3], a[1], a[2], orig
    }' | sort | cut -d' ' -f2-)

# Print sorted results
printf "%s\n" "$sorted_dates"
