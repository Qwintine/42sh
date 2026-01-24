#!/bin/sh

testcase "export to subshell" "-c" "export a=ok; sh -c 'echo \$a'"
testcase "no export not in subshell" "-c" "a=ok; sh -c 'echo not ok'"
testcase "export vs no export" "-c" "a=not_ok; export b=ok; sh -c 'echo \$b'"

testcase "basic export" "-c" "export test=hello; sh -c 'echo \$test'"
testcase "export with spaces" "-c" "export var='hello world'; sh -c 'echo \$var'"
testcase "export empty value" "-c" "export empty=; sh -c 'echo :\$empty:'"

testcase "export existing variable" "-c" "var=ok; export var; sh -c 'echo \$var'"
testcase "set then export" "-c" "a=test; sh -c 'echo not ok'; export a; sh -c 'echo \$a'"

testcase "export nonexistent no crash" "-c" "export nonexist; echo ok"
testcase "export nonexistent to subshell" "-c" "export nonexist; sh -c 'echo :\$nonexist:'"

testcase "export multiple" "-c" "export a=a b=b c=c; sh -c 'echo \$a \$b \$c'"
testcase "mixed export no export" "-c" "export x=ok; y=not_ok; export z=ok; sh -c 'echo \$x \$z'"

testcase "export modify no re-export" "-c" "export var=first; var=second; sh -c 'echo \$var'"
testcase "export modify then export" "-c" "export var=not_ok; var=ok; export var; sh -c 'echo \$var'"

testcase "unset then export" "-c" "export var=not_ok; unset var; export var=ok; sh -c 'echo \$var'"

testcase "export special chars" "-c" "export sp='a@b#c'; sh -c 'echo \$sp'"
# testcase "export in subshell" "-c" "(export a=ok); sh -c 'echo not ok'"
testcase "export with numbers" "-c" "export var123=ok; sh -c 'echo \$var123'"
testcase "export equals in value" "-c" "export eq='a=b'; sh -c 'echo \$eq'"
testcase "export invalid name" "-c" "export 123=val 2>/dev/null; echo \$?"
