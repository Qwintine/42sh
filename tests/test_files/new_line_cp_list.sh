#!/bin/sh

testcase "empty command" "-c" ""
testcase "only semicolon" "-c" ";"
testcase "only newline" "-c" "
"
testcase "command with newline" "-c" "echo a
echo b"
testcase "ignore second arg" "-c" "echo from_c" "tests/tests_files/should_not_print.sh"

testcase "if + NEWLINE" "-c" "
if true
then
echo ok
fi
"

testcase "multiple NEWLINE" "-c" "
if true



then

echo ok


fi
"

testcase "condition on multpile lines" "-c" "
if false
true
then
echo OK
else
echo NOT OK
fi
"

testcase "mix NEWLINE + ;" "-c" "if true; then
echo a
echo b; echo c
fi "

testcase "then one line after newline" "-c" "if true
then echo ok
fi"

testcase "fi before mult NEWLINE" "-c" "if true; then echo ok; fi


echo after"

testcase "empty if" "-c" "
if true; then
fi
echo not ok
"

testcase "then without body" "-c" "
if true; then
else
echo not ok
fi"

testcase "else without body" "-c" "
if false; then
echo not ok1
else
fi

echo not ok2"

testcase "elif with newline" "-c" "
if false
then echo not ok
elif true
then echo ok
fi"

testcase "missing then" "-c" "
if true
echo not ok
fi"

testcase "missing fi" "-c" "if true; then echo not ok"
testcase "kewordsas words" "-c" "
echo if
echo then
echo fi
echo ok"

testcase "NEWLINE delimiter" "-c" "
echo a
echo b; echo c
echo d
"

testcase "test_files: test_compound_list.sh" "" "tests/test_files/from_files/test_compound_list.sh"
testcase "test_files: test_if_separator_newline.sh" "" "tests/test_files/from_files/test_if_separator_newline.sh"
testcase "test_files: test_compound_list.sh" "<" "tests/test_files/from_files/test_compound_list.sh"
testcase "test_files: test_if_separator_newline.sh" "<" "tests/test_files/from_files/test_if_separator_newline.sh"
