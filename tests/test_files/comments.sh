#!/bin/sh

testcase "line commented" "-c" "
# echo should not be printed
echo ok
"

testcase "comment after command" "-c" "echo ok # echo should not be printed
echo after"

testcase "# in word" "-c" "echo no#comment"

testcase "escaped #" "-c" "echo \#escaped"
testcase "# in quotes" "-c" "
echo '#quoted'
echo # not quoted"
testcase "concat #" "-c" "echo a'#'b"
testcase "after spaces" "-c" "echo ok        #echo not ok
echo after"

testcase "in if" "-c" "if true then # echo not ok
echo ok
fi"

testcase "commented fi" "-c" "if true; then echo ok #fi
fi"

testcase "comment in condition" "-c" "if true #false
then echo ok
else echo not ok
fi
"

testcase "comment with ;" "-c" "#a; b  ;c; echo not ok
echo ok"

testcase "lastcomment" "-c" "#lastcomment"
testcase "only comment" "-c" "# only comment"
