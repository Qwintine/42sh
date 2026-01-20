#!/bin/sh

testcase "brandone" "-c" "brandone=pierre.brandone; echo -n Files \$brandone; brandone=brando_p; echo and \$brandone differ"
testcase "multiple_defs_on_line" "-c" "A=YA "
testcase "assignement after expansion" "-c" "A=YA B=K C=A echo \$A \$B \$C"
testcase "with \" " "-c" "A=YA B=K C=A; echo \"\$A\" \"\$B\" \"\$C\""
testcase "pwd" "-c" "echo \$PWD"
testcase "bad var" "-c" "echo \$bad_var"
testcase "sharp" "-c" "echo \$#"
testcase "hard_var_3" "-c" "a=takebon; a=\"\$a le 42sh\"; echo \$a"
testcase "simple_var_multiple_words" "-c" "a=titi; echo  \$a"
testcase "hard_var_1" "-c" "a=beau; echo \$a"
testcase "questonmark" "-c" "false; echo \$?"
testcase "uid" "-c" "echo \$UID"
testcase "random" "-c" "echo \$RANDOM > diff1; echo \$RANDOM > diff2; diff diff1 diff2 > /dev/null || echo Files diff1 and diff 2 differ"
rm -f diff1 diff2
testcase "simple_var_bracket" "-c" "a=titi; echo \${a}\${a}"
testcase "ifs2" "-c" "echo \"\$IFS\"
echo 
IFS=newifs
echo \$IFS"
#modif testsuite pour accepter plus d'args
testcase "special_args" "-c" "echo @:\$@ \n *:\$* \n {\\#}:\${\\#} \n arg1:\$1 arg2:\$ \n"
testcase "test for" "-c" "for arg in \$@; do echo \$arg; done " 
