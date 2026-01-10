#!/bin/sh

REF_SHELL=/bin/sh
BIN="${BIN_PATH:-./src/42sh}"
OUTFILE="${OUTPUT_FILE:-./out}"

REF_OUT=".expected"
TEST_OUT=".got"

PASS=0
TOTAL=0

testcase() {
  name="$1"
  flag="$2"
  cmd="$3"

  TOTAL=$((TOTAL + 1))

  timeout 5 $REF_SHELL $flag "$cmd" >"$REF_OUT" 2>/dev/null
  ref_status=$?
  printf "\n[exit:%d]\n" "$ref_status" >>"$REF_OUT"

  timeout 5 "$BIN" $flag "$cmd" >"$TEST_OUT" 2>/dev/null
  test_status=$?
  printf "\n[exit:%d]\n" "$test_status" >>"$TEST_OUT"

  if diff -u "$REF_OUT" "$TEST_OUT" 2>/dev/null; then
    echo "$name ==> OK"
    PASS=$((PASS + 1))
  else
    echo "$name ==> FAIL"
    diff -u "$REF_OUT" "$TEST_OUT" | grep -E '^(@@|[-+])' | grep -vE '^(---|\+\+\+)' || true
  fi
  echo
}

run_criterion() {
  if ! command -v pkg-config >/dev/null 2>&1 || ! pkg-config --exists criterion >/dev/null 2>&1; then
    echo "criterion ==> SKIP (not installed)"
    return 0
  fi

  gcc -std=c99 -Wall -Wextra -Werror \
    $(pkg-config --cflags criterion) \
    tests/tests_unitaires/tests.c \
    src/io/io.c \
    src/utils/token.c \
    src/lexer/lexer.c \
    $(pkg-config --libs criterion) \
    -o tests/crit_tests >/dev/null 2>&1

  if [ $? -ne 0 ]; then
    echo "criterion ==> SKIP (compile failed)"
    rm -f tests/crit_tests
    return 0;
  fi

  crit_output=$(./tests/crit_tests 2>&1)
  crit_exit=$?
  
  crit_tested=$(echo "$crit_output" | grep -E "Synthesis:" | sed -n 's/.*Tested: \([0-9]*\).*/\1/p')
  crit_passing=$(echo "$crit_output" | grep -E "Synthesis:" | sed -n 's/.*Passing: \([0-9]*\).*/\1/p')
  
  if [ -n "$crit_tested" ] && [ -n "$crit_passing" ]; then
    TOTAL=$((TOTAL + crit_tested))
    PASS=$((PASS + crit_passing))
    
    echo "criterion ==> Tested: $crit_tested | Passing: $crit_passing"
    
    if [ "$crit_exit" -ne 0 ]; then
      echo "$crit_output"
    fi
    
  else
    TOTAL=$((TOTAL + 1))
    if [ "$crit_exit" -eq 0 ]; then
      PASS=$((PASS + 1))
      echo "criterion ==> OK"
    else
      echo "criterion ==> FAIL"
      echo "$crit_output"
    fi
  fi

  rm -f tests/crit_tests
}

testcase "echo Hello World!" "-c" "echo Hello World!"
testcase "true" "-c" "true"
testcase "false" "-c" "false"
testcase "ls" "-c" "ls"
testcase "source file" "" "tests/test_files/simple_command.sh"
testcase "one line if err" "" "tests/test_files/test1.sh"
#testcase "stdin" "" "< tests/tests_files/test_stdin.sh" #faire .sh
testcase "unknown option" "--unknown"

testcase "if true then" "-c" "if true; then echo ok; fi"
testcase "if false else" "-c" "if false; then echo ko; else echo ok; fi"
testcase "if elif else" "-c" "if false; then echo 1; elif true; then echo 2; else echo 3; fi"
testcase "if with newlines" "-c" "if true
then
echo ok
fi"
testcase "nested if" "-c" "if true; then if false; then echo ko; else echo ok; fi; fi"

testcase "two commands" "-c" "echo a; echo b"
testcase "three commands" "-c" "echo a; echo b; echo c"
testcase "trailing semicolon" "-c" "echo test;"

testcase "empty command" "-c" ""
testcase "only semicolon" "-c" ";"
testcase "only newline" "-c" "
"
testcase "command with newline" "-c" "echo a
echo b"
testcase "ignore second arg" "-c" "echo from_c" "tests/tests_files/should_not_print.sh"
testcase "too much -c" "-c" "-c" "echo not ok"

testcase "echo fi" "-c" "echo fi"
testcase "echo then else" "-c" "echo then else"
testcase "echo if" "-c" "echo if"

testcase "unclosed if" "-c" "if true; then echo ok"
testcase "if without then" "-c" "if true echo ok fi"
testcase "fi without if" "-c" "echo ok; fi"

testcase "test_files: test1.sh" "" "tests/test_files/test1.sh"
testcase "test_files: test2.sh" "" "tests/test_files/test2.sh"
testcase "test_files: test3.sh" "" "tests/test_files/test3.sh"
testcase "test_files: test4.sh" "" "tests/test_files/test4.sh"
testcase "test_files: test5.sh" "" "tests/test_files/test5.sh"
testcase "test_files: test6.sh" "" "tests/test_files/test6.sh"
testcase "test_files: test7.sh" "" "tests/test_files/test7.sh"
testcase "test_files: test8.sh" "" "tests/test_files/test8.sh"
testcase "test_files: test9.sh" "" "tests/test_files/test9.sh"
testcase "test_files: test10.sh" "" "tests/test_files/test10.sh"
testcase "test_files: test11.sh" "" "tests/test_files/test11.sh"
testcase "test_files: test12.sh" "" "tests/test_files/test12.sh"


#=========================== Simple commands =================================

testcase "true \n echo after" "-c" "true
echo after"

testcase "nonexistent command" "-c" "unknown_command
echo after"

testcase "multiple arguments echo" "-c" "echo 1 2 3"

testcase "echo vide" "-c" "echo ''
echo after_empty"

testcase "echo special characters" "-c" "echo a-B_c.1"

testcase "echo tabs + spaces" "-c" "echo	tab            spaces"

testcase "echo after newline" "-c" "
echo ok"

testcase "trailing spaces" "-c" "echo ok    "

testcase "multiple lines" "-c" "
echo 1
echo 2
echo 3
"

#====================== Command lists + Semi-colons ===========================

testcase "; between commands" "-c" "echo a; echo b"
testcase "; between commands + end" "-c" "echo a; echo b;"
testcase "; + different blank characters positions" "-c" "echo a	;echo b ; echo c; "
testcase "double ;; error" "-c" "echo a;; echo b"
testcase "grammar error no command before ;" "-c" ";
echo not ok" #vérifier comportement attendu par le binaire dans ce cas
testcase "trailing semicolon" "-c" "echo test;"
testcase "mix ;and NEWLINE" "-c" "echo a;
echo b"
testcase "spaces between ; and keywords" "-c" "if true ;  then echo ok  ; fi"
testcase "empty command" "-c" "echo not ok 1; ;echo not ok 2"
testcase "many trailing ;" "-c" "echo ok;;;"
testcase "if followed by ;" "-c" "if true;  then echo ok; fi;
echo after"
testcase "then as word" "-c" "echo then; echo ok"
testcase "not kewords" "-c" "echo if; echo then; echo fi"

#========================= NEWLINE + Compound list ============================

testcase "if + NEWLINE" "-c" "
if true
then
echo ok
fi
"

testcase "multiple NEWLINE" "-c" "
if true



then

echo ok


fi
"

testcase "condition on multpile lines" "-c" "
if false
true
then
echo OK
else
echo NOT OK
fi
"

testcase "mix NEWLINE + ;" "-c" "if true; then
echo a
echo b; echo c
fi "

testcase "then onee line after newline" "-c" "if true
then echo ok
fi"

testcase "fi before mult NEWLINE" "-c" "if true; then echo ok; fi


echo after"

testcase "empty if" "-c" "
if true; then
fi
echo not ok
" #a vérifier

testcase "then without body" "-c" "
if true; then
else
echo not ok
fi"

testcase "else without body" "-c" "
if false; then
echo not ok1
else
fi

echo not ok2"

testcase "elif with newline" "-c" "
if false
then echo not ok
elif true
then echo ok
fi"

testcase "missing then" "-c" "
if true
echo not ok
fi"

testcase "missing fi" "-c" "if true; then echo not ok"
testcase "kewordsas words" "-c" "
echo if
echo then
echo fi
echo ok"

testcase "NEWLINE delimiter" "-c" "
echo a
echo b; echo c
echo d
"

#============================ IF - ELIF - ELSE ================================

testcase "basic one line if" "-c" " if true; then echo ok; fi"
testcase "basic else test" "-c" "if false; then echo not ok; else echo ok; fi"
testcase "then multiple commands" "-c" "if true; then echo a; echo b; echo c; fi"
testcase "else multpile commands" "-c" "if false; then echo not ok; else echo ok1; echo ok2; fi"
testcase "single elif" "-c" "if false; then echo not ok; elif true; then echo ok; fi"
testcase "chain elif" "-c" "if false; then echo not ok; elif false; then echo not ok2; elif true;then echo ok; fi"
testcase "elif + else" "-c" "if false; then echo not ok1; elif false; then echo not ok2; else echo ok; fi"
testcase "if in if" "-c" "
if true; then
	if false; then echo inner not ok; else echo inner ok; fi
fi
"
testcase "if in else" "-c" "if false; then echo not ok; else if true; then echo inner else ok; fi; fi"
testcase "true list for condition" "-c" "if echo first_condition; echo second_condition; true; then echo ok; fi"
testcase "false list for condition" "-c" "if echo first_condition; echo second_condition; false; theen echo not ok; fi"
testcase "keywords in quotes" "-c" "if true; then echo 'then fi else elif'; fi" #rajouter guillemets
testcase "else in quotes" "-c" "if false; then echo not ok; else echo 'else'; fi"
testcase "fi in quotes" "-c" "if true; then echo 'fi'; fi"
testcase "elif without then" "if false; then echo not ok1; elif true; echo not ok2; fi"
testcase "else before elif" "-c" " if false; then echo not ok1; else echo not ok2; elif true; then echo not ok3; fi"
testcase "missing then" "-c" "if true; echo not ok; fi"
testcase "if without condition" "-c" "if then echo not ok; fi"
testcase "no then" "-c" "if true; fi; echo not ok"
testcase "no if else" "-c" "else eecho not ok"
testcase "no if elif" "-c" "elif true; then echo not ok; fi"
testcase "test if + NEWLINES" "-c" "
if true
then
echo ok
else
echo not ok
fi
"
testcase "builtin false condition" "-c" "if false; then echo not ok; else echo ok; fi"
testcase "empty then ( comment )" "-c" "if true; then #echo not ok
fi"
testcase "test else comment" "-c" "if false; then echo not ok1; else # echo not ok2
echo ok
fi"

#=============================== Single quotes ================================

testcase "test single quotes" "-c" "echo 'ok'"
testcase "kept spaces" "-c" "echo 'a     spaces       c'  "
testcase "not a comment" "-c" "echo '#not a comment'"
testcase "; not a seperator" "-c" "echo 'a; b ;c'"
testcase "keywords in quotes" "-c" "echo 'if then ' 'elif' '    else fi'"
testcase "empty quotes" "-c" "''"
testcase "concat word + quotes" "-c" "echo a'b'c"
testcase "not closed quotes" "-c" "echo 'not ok"
testcase "backslash in quotes" "-c" "echo '\n \t \\'"
testcase "mix quotes spaces" "-c" "echo 'a' 'b' 'c'  "


#=============================== True/False ===================================

testcase "simple test true" "-c" "
true
echo after true"

testcase "simple test false" "-c" "
false
echo after_false"

testcase "if true" "-c" "if true; the echo ok; else echo not ok; fi"
testcase "if false" "-c" "if false; then echo not ok; else echo ok; fi"
testcase "exit status last command" "-c" "true ;false"
testcase "if false; true" "-c" "if false; true; then echo ok; else echo not ok; fi"

#============================ Echo Corner case ================================

testcase "no args" "-c" "echo"
testcase "-n" "-c" "echo -n hello
echo world"
testcase "-n many args" "-c" "echo -n a b c
echo d"
testcase "-e + NEWLINE" "-c" "echo -e \"a\nb\"" 
testcase "-e + tab" "-c" "echo -e \"a\tb\"" 
testcase "-e + \\ " "-c" "echo -e \" \\ \"" #
testcase "-E" "-c" "echo -E \"a\nb\"" #
testcase "-e then -E" "-c" "echo -e -E \"a\nb\"" #
testcase "-E then -e" "-c" "echo -E -e \"a\nb\"" #
testcase "unknown option" "-c" "echo -u ok"
testcase "unknown option + n" "-c" "echo -un ok"
testcase "-n as argument" "-c" "echo '-n' ok"

#=============================== Comments =====================================

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

#================================= Grammar ====================================

testcase "then without if" "-c" "then echo not ok"
testcase "lonely fi" "-c" "fi"
testcase "if without fi" "-c" "if true; then echo not ok;"
testcase "if without then" "-c" "if true; echo not ok; fi"
testcase "bad position elif" "-c" "if true; then echo not ok1; else echo not ok2; elif true; then echo not ok3; fi"
testcase "non closed quotes" "-c" "echo 'not ok"
testcase "multiple ;" "-c" "echo a;;;;;; echo b"
testcase "empty compound list" "-c" "
if true; then
#echo not ok
fi
"


if [ "${COVERAGE:-no}" = "yes" ]; then
  run_criterion
fi

if [ "$TOTAL" -gt 0 ]; then
  SCORE=$(( (100 * PASS) / TOTAL ))
else
  SCORE=0
fi

printf "%d" "$SCORE" > "$OUTFILE"

rm -f "$REF_OUT" "$TEST_OUT"

exit 0
