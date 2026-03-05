#!/bin/bash
#Problem2.sh
#This prints out the entries (including files and sub-directories) in a directory in a reversed order as what obtained using filename expansion


# Check arguments
if [[ $# -ne 1 ]]; then
    echo "Usage: $0 <directory>"
    exit 1
fi

dir=$1

# Validate directory
if [[ ! -d "$dir" ]]; then
    echo "Error: '$dir' is not a directory"
    exit 1
fi

# Collect entries using filename expansion
entries=("$dir"/*)

# Handle case when directory is empty
if [[ ! -e "${entries[0]}" ]]; then
    echo "Directory is empty"
    exit 0
fi

# Strip pathnames, keep only base names
for i in "${!entries[@]}"; do
    entries[$i]=$(basename "${entries[$i]}")
done

# Print in reverse order vertically
for (( i=${#entries[@]}-1; i>=0; i-- )); do
    printf '%s\n' "${entries[i]}"
done

