#!/bin/sh

testcase "echo fi" "-c" "echo fi"
testcase "echo then else" "-c" "echo then else"
testcase "echo if" "-c" "echo if"

testcase "no args" "-c" "echo"
testcase "-n" "-c" "echo -n hello
echo world"
testcase "-n many args" "-c" "echo -n a b c
echo d"
testcase "-e + NEWLINE" "-c" "echo -e \"a\nb\"" 
testcase "-e + tab" "-c" "echo -e \"a\tb\"" 
testcase "-e + \\ " "-c" "echo -e \" \\ \"" 
testcase "-E" "-c" "echo -E \"a\nb\"" 
testcase "-e then -E" "-c" "echo -e -E \"a\nb\"" 
testcase "-E then -e" "-c" "echo -E -e \"a\nb\"" 
testcase "unknown option" "-c" "echo -u ok"
testcase "unknown option + n" "-c" "echo -un ok"
testcase "-n as argument" "-c" "echo '-n' ok"
