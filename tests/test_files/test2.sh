#!/bin/sh

if true; then
    echo "premier test ok"
fi

if false; then
    echo "ne devrait pas apparaitre"
else
    echo "deuxieme test ok"
fi

if false; then
    echo "ko"
elif true; then
    echo "troisieme test ok"
fi
