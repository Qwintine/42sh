#!/bin/sh
# Helper file for dot tests - nested dot
nested1=ok
. tests/test_files/dot_test_helper6.sh
echo \$nested1 \$nested2
