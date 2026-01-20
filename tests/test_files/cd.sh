#!/bin/sh

testcase "cd to home directory" "-c" "cd; ls"
testcase "cd to src" "-c" "cd src/; ls; cd -; ls"
testcase "cd to parent directory" "-c" "cd src/../; ls"
testcase "cd to racine" "-c" "cd /; ls"
testcase "cd to non-existent directory" "-c" "cd /nonexistent; ls"