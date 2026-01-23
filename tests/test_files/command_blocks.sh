#!/bin/sh

testcase "simple command block" "-c" "{ echo inside; }"
testcase "command block with semicolon" "-c" "{ echo first; echo second; }"
testcase "multiple command blocks" "-c" "{ echo outer; { echo inner; }; }"
testcase "command block with redirection" "-c" "{ echo redirected > tmp.txt; } && cat tmp.txt"
testcase "empty command block" "-c" "{}"
testcase "with pipes" "-c" "{ echo a; echo b; } | tr b h"
