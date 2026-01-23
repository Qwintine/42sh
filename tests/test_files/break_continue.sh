#!/bin/sh

testcase "break in while loop" "-c" "while true; do echo test; break; done"
testcase "break in for loop" "-c" "for i in 1 2 3 4 5; do echo \$i; break; done"
testcase "break exits immediately" "-c" "while true; do echo before; break; echo after; done"

testcase "continue in for loop" "-c" "for i in 1 2 3; do echo before; continue; echo after; done"
testcase "break in while" "-c" "while true; do echo a; if true; then break; fi; echo not_ok; done"

testcase "break in 2 loop" "-c" "for i in a b; do for j in 1 2; do echo \$i\$j; break 2; done; echo not_ok; done"
testcase "continue in 2 loop" "-c" "for i in a b; do for j in 1 2; do continue 2; echo not_ok; done; echo ok; done"

testcase "break with invalid argument" "-c" "while true; do break abc; done"
testcase "continue with invalid argument" "-c" "while true; do continue xyz; break; done"
