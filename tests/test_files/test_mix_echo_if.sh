#!/bin/sh

echo "debut du script"

if true; then
    echo "dans if 1"
    if false; then
        echo "ko"
    else
        echo "dans else imbriqué"
    fi
    echo "fin if 1"
fi

echo "entre les if"

if false; then
    echo "ko"
elif true; then
    echo "elif ok"
    true
    false
    echo "apres false"
else
    echo "ko"
fi

echo "fin du script"
