#!/bin/sh

testcase "not true" "-c" "! true"
testcase "not false" "-c" "! false"
testcase "double negation" "-c" "! ! true"
testcase "negation with output" "-c" "! echo test"
testcase "negation in if true" "-c" "if ! true; then echo ok; fi"
testcase "negation in if false" "-c" "if ! false; then echo ok; fi"
testcase "negation then command" "-c" "! true; echo after"
testcase "negation with pipe" "-c" "! true | echo test"
testcase "negation failing command" "-c" "! ls /nonexistent_dir_42sh"
testcase "triple negation" "-c" "! ! ! false"
testcase "negation with multiple spaces" "-c" "!    false"

testcase "negation of pipeline" "-c" "! echo a | echo b"
testcase "negation in elif" "-c" "if false; then echo ko; elif ! false; then echo ok; fi"
testcase "negation in else" "-c" "if false; then echo ko; else ! true; echo after; fi"
# testcase "negation with newline" "-c" "!
# true"
testcase "negation with semicolon" "-c" "! true ; ! false"
testcase "multiple negations separate" "-c" "! false; ! true; ! false"
testcase "negation nonexistent command" "-c" "! nonexistent_cmd_42sh"
testcase "negation of if" "-c" "! if true; then true; fi"
# testcase "negation with comment" "-c" "! # comment
# false"
testcase "pipe then negation" "-c" "echo test | ! false"
