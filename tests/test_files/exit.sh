#!/bin/sh

testcase "exit no arg" "-c" "exit"
testcase "exit with 0" "-c" "exit 0"
testcase "exit with 1" "-c" "exit 1"
testcase "exit with 42" "-c" "exit 42"
testcase "exit 301" "-c" "exit 301"
testcase "exit -5" "-c" "exit -5"
testcase "exit -256" "-c" "exit -256"

testcase "exit stops next command" "-c" "exit 5; echo should not appear"
testcase "exit in if then" "-c" "if true; then exit 99; fi; echo should not appear"
testcase "exit in while loop" "-c" "while true; do exit 77; done; echo no"
testcase "exit in until loop" "-c" "until false; do exit 66; done; echo no"
testcase "exit with AND" "-c" "true && exit 15 && echo no"
testcase "exit with OR success" "-c" "false || exit 25 || echo no"
testcase "exit with OR fail" "-c" "true || exit 35"

testcase "exit after pipe" "-c" "echo test | cat; exit 55"
testcase "exit in if" "-c" "if true; then if true; then exit 111; fi; echo no; fi; echo also no"
testcase "exit with invalid arg" "-c" "exit abc"
testcase "exit with multiple args" "-c" "exit 1 2"
testcase "exit with empty string" "-c" "exit ''"
# testcase "exit with variable" "-c" "A=42; exit \$A"

testcase "exit from file *" "tests/test_files/from_files/test_exit.sh" ""
testcase "true then exit" "-c" "true; exit 8"
testcase "false then exit" "-c" "false; exit 9"

# testcase "exit in subshell" "-c" "(exit 100); echo \$?"
