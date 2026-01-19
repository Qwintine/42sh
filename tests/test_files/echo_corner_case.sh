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
testcase "multiple quotes" "-c" "echo ''''''hello''''''"

testcase "dquote: escape quote" "-c" "echo \"hello\\\"world\""
testcase "dquote: escape backslash" "-c" "echo \"hello\\\\world\""
testcase "dquote: escape dollar" "-c" "echo '\$USER'"
testcase "dquote: \\n stays literal" "-c" "echo \"\\n\""
testcase "no quote: escape space" "-c" "echo hello\\ world"
testcase "no quote: backslash newline" "-c" "echo hello\\
world"
testcase "no quote: double backslash" "-c" "echo \\\\\\\\"
testcase "squote: all literal" "-c" "echo '\\n\\t\\\"'"
testcase "mixed quotes" "-c" "echo \"\\n\"'\\n'"
testcase "complex escape" "-c" "echo \"a\\\"b\\\\c\""

testcase "tower" "" "tests/test_files/from_files/tower.sh"
