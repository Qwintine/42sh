#!/bin/sh

testcase "echo Hello World!" "-c" "echo Hello World!"
testcase "ls" "-c" "ls"
testcase "source file" "" "tests/test_files/from_files/simple_command.sh"

testcase "true \\\n echo after" "-c" "true
echo after"

testcase "nonexistent command" "-c" "unknown_command
echo after"

testcase "multiple arguments echo" "-c" "echo 1 2 3"

testcase "echo vide" "-c" "echo ''
echo after_empty"

testcase "echo special characters" "-c" "echo a-B_c.1"

testcase "echo tabs + spaces" "-c" "echo	tab            spaces"

testcase "echo after newline" "-c" "
echo ok"

testcase "trailing spaces" "-c" "echo ok    "

testcase "multiple lines" "-c" "
echo 1
echo 2
echo 3
"

testcase "test_files: test_multiple_echo.sh" "" "tests/test_files/from_files/test_multiple_echo.sh"
testcase "test_files: test_multiple_echo.sh" "<" "tests/test_files/from_files/test_multiple_echo.sh"

testcase "PWD" "-c" "echo \$PWD"
testcase "echo only" "-c" "echo"
testcase "echo with -n" "-c" "echo -n test"
testcase "true command" "-c" "true"
testcase "false command" "-c" "false"
testcase "command with pipe char in quotes" "-c" "echo '|'"
testcase "command with semicolon in quotes" "-c" "echo ';'"
testcase "echo with equals" "-c" "echo a=b"
testcase "echo multiple spaces" "-c" "echo a    b    c"
testcase "command substitution simple" "-c" "echo test"

testcase "sort files" "" "tests/test_files/from_files/sort_files.sh"
