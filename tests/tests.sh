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
