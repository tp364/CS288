#!/bin/bash
#Problem1.sh
#Ask for directroy and verify, list all of the files in the directory and display as a menu, select a file and display the last 10 lines, ask if they want to see more and if they reply yes, show it to them 

# Prompt for directory
read -p "Enter directory path: " dir

# Verify directory
if [[ ! -d "$dir" ]]; then
    echo "Directory does not exist."
    exit 1
fi

cd "$dir" || exit 1

# List files
files=(*)
if [[ ${#files[@]} -eq 0 ]]; then
    echo "No files in directory."
    exit 1
fi

echo "Select a file:"
select file in "${files[@]}"; do
    if [[ -n "$file" && -f "$file" ]]; then
        break
    else
        echo "Invalid choice."
    fi
done

# Display file from bottom upwards
lines=$(wc -l < "$file")
offset=$lines

while (( offset > 0 )); do
    start=$(( offset - 9 ))
    if (( start < 1 )); then
        start=1
    fi
    sed -n "${start},${offset}p" "$file"
    offset=$(( start - 1 ))

    if (( offset < 1 )); then
        echo "Reached beginning of file."
        break
    fi

    read -p "Display 10 more lines? (yes/no): " ans
    [[ "$ans" != "yes" ]] && break
done

