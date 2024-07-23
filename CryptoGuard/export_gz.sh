#!/bin/bash

# Check if an argument is provided
if [ -z "$1" ]; then
	echo "Usage: $0 <archive_name>"
	exit 1
fi

# Input argument
ARCHIVE_NUMBER=$1

# Directories
SOURCE_DIR="./buf_1"
DEST_DIR="../Model/CMS/dataset"

# Archive name
ARCHIVE_NAME="csv_files_${ARCHIVE_NUMBER}.tar.gz"

# Compress the CSV files
cd "$SOURCE_DIR" || { echo "Source directory not found"; exit 1; }
tar -czf "../${ARCHIVE_NAME}" ./*.csv
cd ".."
mv "${ARCHIVE_NAME}" "$DEST_DIR"

# Print completion message
echo "CSV files from $SOURCE_DIR have been compressed and saved to $DEST_DIR/$ARCHIVE_NAME"

