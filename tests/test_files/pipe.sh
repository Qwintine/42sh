#!/bin/sh

testcase "pipe echo" "-c" "echo a | echo b"
testcase "pipe EOF" "-c" "echo a |"
testcase "pipe syntax error" "-c" "echo a | ;"
testcase "pipe lot of pipe" "-c" "echo hello | tr h p | tr e a | tr l m | tr o e"
testcase "pipe with if" "-c" "echo a | if true; then echo b; fi"
testcase "pipe with false" "-c" "false | echo after_false"
testcase "pipe with true" "-c" "true | echo after_true"
testcase "pipe multiple commands" "-c" "echo start | echo middle ; echo end"
testcase "pipe multiple commands 2" "-c" "echo start ; echo middle | echo end"
testcase "pipe syntax error multiple pipes" "-c" "echo a ||| echo b"
testcase "pipe syntax error no cmd start" "-c" "| echo a"
testcase "pipe syntax error start" "-c" " ;| echo a"

testcase "pipe no spaces" "-c" "echo a|echo b|echo c"
testcase "pipe with newlines" "-c" "echo a |
echo b"
testcase "pipe with comment" "-c" "echo a | # comment
echo b"
testcase "pipe in else" "-c" "if false; then echo ko; else echo a | echo b; fi"
testcase "pipe empty echo" "-c" "echo | echo after_empty"
testcase "pipe exit codes" "-c" "false | false | true"
testcase "pipe after if" "-c" "if true; then echo a; fi | echo b"
