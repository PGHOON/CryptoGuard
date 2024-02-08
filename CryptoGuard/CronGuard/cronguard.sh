#!/bin/bash

DELETE_PATH=$1

if [ -z "$DELETE_PATH" ]; then
    echo "Usage: $0 <path_to_delete>"
    exit 1
fi

for USER_NAME in $(cut -f1 -d: /etc/passwd); do
    CRONTAB_FILE=$(mktemp)
    if crontab -u "$USER_NAME" -l > "$CRONTAB_FILE" 2>/dev/null; then

        grep -v "$DELETE_PATH" "$CRONTAB_FILE" > "${CRONTAB_FILE}.tmp" && mv "${CRONTAB_FILE}.tmp" "$CRONTAB_FILE"
        
        crontab -u "$USER_NAME" "$CRONTAB_FILE"
        
        echo "Crontab entries containing '$DELETE_PATH' have been removed for user $USER_NAME."
    fi
    
    rm "$CRONTAB_FILE"
done