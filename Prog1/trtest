#!/bin/bash
# This script converts a row file ./tempinputfile into a column file ./tempCol,
# then back into a row file ./tempRow<PID>

input_file="tempinputfile"
tempRow="temprowfile"

# Make the input row file
echo -e "1\t2\t3\t4\t5" > $input_file

# Append each number onto the end of a column file
cut -c 1 $input_file > tempCol
cut -c 3 $input_file >> tempCol
cut -c 5 $input_file >> tempCol
cut -c 7 $input_file >> tempCol
cut -c 9 $input_file >> tempCol

# Convert the column file back into a row file
cat tempCol | tr '\n' '\t' > "$tempRow$$"

# Add a newline char to the end of the row file, for easier printing
echo -e "" >> "$tempRow$$"
