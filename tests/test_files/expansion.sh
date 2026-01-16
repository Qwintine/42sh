#!/bin/sh

testcase "simple expansion" "-c" "test=hello; echo \$test"

testcase "expansion with word before" "-c" "var=world; echo hello \$var"

testcase "expansion with word after" "-c" "var=test; echo \$var ok"

testcase "multiple variables" "-c" "a=foo; b=bar; echo \$a \$b"

testcase "undefined variable" "-c" "echo \$undefined"

testcase "multiple expansions" "-c" "x=1; y=2; z=3; echo \$x \$y \$z"

testcase "variable with numbers" "-c" "var123=test123; echo \$var123"

testcase "variable with underscores" "-c" "my_var=underscore; echo \$my_var"

testcase "long string expansion" "-c" "long=this_is_a_very_long_string; echo \$long"

testcase "expansion in different positions" "-c" "a=start; b=middle; c=end; echo \$a \$b \$c"

testcase "numeric values" "-c" "num1=42; num2=100; echo \$num1 \$num2"

testcase "multiple assignments then expansion" "-c" "a1=t1; a2=t2; a3=t3; echo \$a1 \$a2 \$a3"

testcase "variable with dash" "-c" "special=test-value; echo \$special"

testcase "repeated expansion" "-c" "same=rep; echo \$same \$same \$same"

testcase "variable with slash" "-c" "path=/usr/bin; echo \$path"

testcase "mixed text and expansion" "-c" "pre=before; echo start \$pre end"

testcase "assignment in command" "-c" "quick=fast; echo \$quick test"

testcase "expansion with echo arguments" "-c" "arg=value; echo \$arg arg2 \$arg"


