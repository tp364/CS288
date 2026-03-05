#!/bin/bash
#Problem5.sh
#This traverses a directory (provided as a command-line argument) and finds all duplicate filenames (files having the same name, not necessarily the same directory or contents)

dir="$1"

# Check argument validity
if [[ -z "$dir" || ! -d "$dir" ]]; then
    echo "Usage: $0 <directory>"
    exit 1
fi

declare -A seen

# Traverse directory and check for duplicates
while IFS= read -r -d '' file; do
    name=$(basename "$file")
    if [[ -n "${seen[$name]}" ]]; then
        echo "Duplicate found:"
        echo "  Original : ${seen[$name]}"
        echo "  Duplicate: $file"

        # Read user input from terminal
        read -rp "Do you want to delete $file? (yes/no): " ans </dev/tty

        case "$ans" in
            yes|y|Y)
                rm -- "$file" && echo "Deleted $file"
                ;;
            *)
                echo "Kept $file"
                ;;
        esac
    else
        seen[$name]="$file"
    fi
done < <(find "$dir" -type f -print0)

