#!/bin/sh

testcase "unclosed if" "-c" "if true; then echo ok"
testcase "if without then" "-c" "if true echo ok fi"
testcase "fi without if" "-c" "echo ok; fi"

testcase "then without if" "-c" "then echo not ok"
testcase "lonely fi" "-c" "fi"
testcase "if without fi" "-c" "if true; then echo not ok;"
testcase "if without then" "-c" "if true; echo not ok; fi"
testcase "bad position elif" "-c" "if true; then echo not ok1; else echo not ok2; elif true; then echo not ok3; fi"
testcase "non closed quotes" "-c" "echo 'not ok"
testcase "multiple ;" "-c" "echo a;;;;;; echo b"
testcase "empty compound list" "-c" "
if true; then
#echo not ok
fi
"
testcase "semi colon after newline" "" "tests/test_files/from_files/test_semi_colon_alone_after_command_and_newline.sh"

testcase "test_files: test_syntax_error_separator.sh" "" "tests/test_files/from_files/test_syntax_error_separator.sh"
testcase "test_files: test_if_syntax_error.sh" "" "tests/test_files/from_files/test_if_syntax_error.sh"
testcase "test_files: test_syntax_error_separator.sh" "<" "tests/test_files/from_files/test_syntax_error_separator.sh"
testcase "test_files: test_if_syntax_error.sh" "<" "tests/test_files/from_files/test_if_syntax_error.sh"
