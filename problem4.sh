#!/bin/bash
#Problem4.sh
#This builds a table of counters for the files under /bin based on the first character in their filenames

declare -A counts

# Initialize counts
for c in {a..z}; do
    counts[$c]=0
done

# Count files
for f in /bin/*; do
    base=$(basename "$f")
    first=${base:0:1}
    if [[ $first =~ [a-z] ]]; then
        ((counts[$first]++))
    fi
done

# Print results
for c in {a..z}; do
    echo "$c ${counts[$c]}"
done


