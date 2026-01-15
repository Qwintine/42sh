#!/bin/sh

testcase "two commands" "-c" "echo a; echo b"
testcase "three commands" "-c" "echo a; echo b; echo c"
testcase "trailing semicolon" "-c" "echo test;"

testcase "; between commands" "-c" "echo a; echo b"
testcase "; between commands + end" "-c" "echo a; echo b;"
testcase "; + different blank characters positions" "-c" "echo a	;echo b ; echo c; "
testcase "double ;; error" "-c" "echo a;; echo b"
testcase "grammar error no command before ;" "-c" ";
echo not ok" #vérifier comportement attendu par le binaire dans ce cas
testcase "trailing semicolon" "-c" "echo test;"
testcase "mix ;and NEWLINE" "-c" "echo a;
echo b"
testcase "spaces between ; and keywords" "-c" "if true ;  then echo ok  ; fi"
testcase "empty command" "-c" "echo not ok 1; ;echo not ok 2"
testcase "many trailing ;" "-c" "echo ok;;;"
testcase "if followed by ;" "-c" "if true;  then echo ok; fi;
echo after"
testcase "then as word" "-c" "echo then; echo ok"
testcase "not kewords" "-c" "echo if; echo then; echo fi"

testcase "test_files: test_multiple_command_per_line.sh" "" "tests/test_files/from_files/test_multiple_command_per_line.sh"
testcase "test_files: test_multiple_command.sh" "" "tests/test_files/from_files/test_multiple_command.sh"
testcase "test_files: test_multiple_command_per_line.sh" "<" "tests/test_files/from_files/test_multiple_command_per_line.sh"
testcase "test_files: test_multiple_command.sh" "<" "tests/test_files/from_files/test_multiple_command.sh"