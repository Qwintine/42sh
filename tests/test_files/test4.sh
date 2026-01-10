#!/bin/sh

if true; then
    if true; then
        echo "niveau 2 ok"
    fi
    echo "niveau 1 ok"
fi

if true; then
    if false; then
        echo "ko"
    else
        echo "else niveau 2 ok"
    fi
fi
