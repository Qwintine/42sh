#!/bin/sh

testcase "if true then" "-c" "if true; then echo ok; fi"
testcase "if false else" "-c" "if false; then echo ko; else echo ok; fi"
testcase "if elif else" "-c" "if false; then echo 1; elif true; then echo 2; else echo 3; fi"
testcase "if with newlines" "-c" "if true
then
echo ok
fi"
testcase "nested if" "-c" "if true; then if false; then echo ko; else echo ok; fi; fi"

testcase "basic one line if" "-c" " if true; then echo ok; fi"
testcase "basic else test" "-c" "if false; then echo not ok; else echo ok; fi"
testcase "then multiple commands" "-c" "if true; then echo a; echo b; echo c; fi"
testcase "else multpile commands" "-c" "if false; then echo not ok; else echo ok1; echo ok2; fi"
testcase "single elif" "-c" "if false; then echo not ok; elif true; then echo ok; fi"
testcase "chain elif" "-c" "if false; then echo not ok; elif false; then echo not ok2; elif true;then echo ok; fi"
testcase "elif + else" "-c" "if false; then echo not ok1; elif false; then echo not ok2; else echo ok; fi"
testcase "if in if" "-c" "
if true; then
	if false; then echo inner not ok; else echo inner ok; fi
fi
"
testcase "if in else" "-c" "if false; then echo not ok; else if true; then echo inner else ok; fi; fi"
testcase "true list for condition" "-c" "if echo first_condition; echo second_condition; true; then echo ok; fi"
testcase "false list for condition" "-c" "if echo first_condition; echo second_condition; false; theen echo not ok; fi"
testcase "keywords in quotes" "-c" "if true; then echo 'then fi else elif'; fi" #rajouter guillemets
testcase "else in quotes" "-c" "if false; then echo not ok; else echo 'else'; fi"
testcase "fi in quotes" "-c" "if true; then echo 'fi'; fi"
testcase "elif without then" "-c" "if false; then echo not ok1; elif true; echo not ok2; fi"
testcase "else before elif" "-c" " if false; then echo not ok1; else echo not ok2; elif true; then echo not ok3; fi"
testcase "missing then" "-c" "if true; echo not ok; fi"
testcase "if without condition" "-c" "if then echo not ok; fi"
testcase "no then" "-c" "if true; fi; echo not ok"
testcase "no if else" "-c" "else eecho not ok"
testcase "no if elif" "-c" "elif true; then echo not ok; fi"
testcase "test if + NEWLINES" "-c" "
if true
then
echo ok
else
echo not ok
fi
"
testcase "builtin false condition" "-c" "if false; then echo not ok; else echo ok; fi"
testcase "empty then ( comment )" "-c" "if true; then #echo not ok
fi"
testcase "test else comment" "-c" "if false; then echo not ok1; else # echo not ok2
echo ok
fi"

testcase "test_files: test_if_in_if.sh" "" "tests/test_files/from_files/test_if_in_if.sh"
testcase "test_files: test_if_elif_else.sh" "" "tests/test_files/from_files/test_if_elif_else.sh"
testcase "test_files: test_empty_then.sh" "" "tests/test_files/from_files/test_empty_then.sh"
testcase "test_files: test_elif_without_then.sh" "" "tests/test_files/from_files/test_elif_without_then.sh"
testcase "test_files: test_multiple_elif.sh" "" "tests/test_files/from_files/test_multiple_elif.sh"
testcase "test_files: test_mix_echo_if.sh" "" "tests/test_files/from_files/test_mix_echo_if.sh"
testcase "test_files: test_if_in_if.sh" "<" "tests/test_files/from_files/test_if_in_if.sh"
testcase "test_files: test_if_elif_else.sh" "<" "tests/test_files/from_files/test_if_elif_else.sh"
testcase "test_files: test_empty_then.sh" "<" "tests/test_files/from_files/test_empty_then.sh"
testcase "test_files: test_elif_without_then.sh" "<" "tests/test_files/from_files/test_elif_without_then.sh"
testcase "test_files: test_multiple_elif.sh" "<" "tests/test_files/from_files/test_multiple_elif.sh"
testcase "test_files: test_mix_echo_if.sh" "<" "tests/test_files/from_files/test_mix_echo_if.sh"
