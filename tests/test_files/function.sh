#!/bin/sh

testcase "function definition and call" "-c" "foo() { echo Hello; }; foo"
testcase "nested function calls" "-c" "foo() { bar() { echo hello; }; bar; echo world; }; foo"
testcase "function with variable" "-c" "foo() { var=a; echo \$var; }; foo"
testcase "double function definition" "-c" "foo() { echo first; }; foo() { echo second; }; foo"
testcase "function with arguments" "-c" "foo() { echo \$1 \$2; }; foo hello world"