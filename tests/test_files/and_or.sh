#!/bin/sh

testcase "and echo" "-c" "echo a && echo b"
testcase "and true" "-c" "true && echo ok"
testcase "and false" "-c" "false && echo should_not_print"
testcase "and multiple ok" "-c" "echo a && echo b && echo c"
testcase "and multiple not ok" "-c" "echo a && false && echo should_not_print"
testcase "and multiple bool echo" "-c" "true && true && echo ok"

testcase "or echo" "-c" "echo a || echo b"
testcase "or true" "-c" "true || echo should_not_print"
testcase "or false" "-c" "false || echo ok"
testcase "or multiple ok" "-c" "echo a || echo should_not_print || echo should_not_print_either"
testcase "or multiple not ok" "-c" "false || false || echo ok"

testcase "and or 1" "-c" "true && echo ok || echo not_ok"
testcase "and or 2" "-c" "false && echo not_ok || echo ok"
testcase "and or 3" "-c" "false || echo ok && echo ok2"
testcase "and or 4" "-c" "true || echo not_ok && echo ok"

testcase "and semicolon" "-c" "echo a && echo b ; echo c"
testcase "or semicolon" "-c" "false || echo ok ; echo after"
testcase "and or multiple" "-c" "echo a && echo b ; false || echo c"

testcase "and with newline" "-c" "echo a &&
echo b"
testcase "or with newline" "-c" "false ||
echo ok"

testcase "and no spaces" "-c" "echo a&&echo b"
testcase "or no spaces" "-c" "false||echo ok"
testcase "and with multiple spaces" "-c" "echo a    &&    echo b"

testcase "and if condition" "-c" "if true && true; then echo ok; fi"
testcase "or if condition" "-c" "if false || true; then echo ok; fi"
testcase "and or if condition" "-c" "if false || true && true; then echo ok; fi"

testcase "and then" "-c" "if true; then echo a && echo b; fi"
testcase "or else" "-c" "if false; then echo ko; else false || echo ok; fi"

testcase "and EOF syntax error" "-c" "echo a &&"
testcase "or EOF syntax error" "-c" "echo a ||"
testcase "and syntax error" "-c" "echo a && ;"
testcase "or syntax error" "-c" "echo a || ;"

testcase "and nonexistent command" "-c" "true && nonexistent_cmd_42sh"
testcase "or nonexistent command" "-c" "false || nonexistent_cmd_42sh"

testcase "and or mix" "-c" "echo 1 && echo 2 || echo 3 && echo 4"
testcase "and or echo" "-c" "echo start && echo middle || echo end"

testcase "and with comment" "-c" "echo a && # comment
echo b"
testcase "or with comment" "-c" "false || # comment  
echo ok"
testcase "and in if with comment" "-c" "if true && # comment
true; then echo ok; fi"
testcase "or in if with comment" "-c" "if false || # comment
true; then echo ok; fi"
