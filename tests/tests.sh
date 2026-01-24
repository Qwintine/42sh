#!/bin/sh

REF_SHELL="bash --posix"
BIN="${BIN_PATH:-./src/42sh}"
OUTFILE="${OUTPUT_FILE:-./out}"

REF_OUT=".expected"
TEST_OUT=".got"
REF_ERR=".expected_err"
TEST_ERR=".got_err"

PASS=0
TOTAL=0

testcase() {
  name="$1"
  flag="$2"
  cmd="$3"

  TOTAL=$((TOTAL + 1))

  if [ "$flag" = "<" ]; then
    timeout 5 $REF_SHELL <"$cmd" >"$REF_OUT" 2>"$REF_ERR"
    ref_status=$?
    printf "\n[exit:%d]\n" "$ref_status" >>"$REF_OUT"
    rm -f tmp.txt

    timeout 5 "$BIN" <"$cmd" >"$TEST_OUT" 2>"$TEST_ERR"
    test_status=$?
    printf "\n[exit:%d]\n" "$test_status" >>"$TEST_OUT"
    rm -f tmp.txt
  else
    timeout 5 $REF_SHELL $flag "$cmd" >"$REF_OUT" 2>"$REF_ERR"
    ref_status=$?
    printf "\n[exit:%d]\n" "$ref_status" >>"$REF_OUT"
    rm -f tmp.txt

    timeout 5 "$BIN" $flag "$cmd" >"$TEST_OUT" 2>"$TEST_ERR"
    test_status=$?
    printf "\n[exit:%d]\n" "$test_status" >>"$TEST_OUT"
    rm -f tmp.txt
  fi

  ref_has_stderr=0
  test_has_stderr=0
  
  if [ -s "$REF_ERR" ]; then
    ref_has_stderr=1
  fi
  
  if [ -s "$TEST_ERR" ]; then
    test_has_stderr=1
  fi

  if diff -u "$REF_OUT" "$TEST_OUT" 2>/dev/null && [ "$ref_has_stderr" -eq "$test_has_stderr" ]; then
    # echo "$name ==> OK"
    PASS=$((PASS + 1))
  else
    echo "$name ==> FAIL"
    diff -u "$REF_OUT" "$TEST_OUT" | grep -E '^(@@|[-+])' | grep -vE '^(---|\+\+\+)' || true
    
    if [ "$ref_has_stderr" -ne "$test_has_stderr" ]; then
      if [ "$ref_has_stderr" -eq 1 ]; then
        echo "Expected stderr but got none"
      else
        echo "Got unexpected stderr"
      fi
    fi
    echo
  fi
}

run_criterion() {
  if ! command -v pkg-config >/dev/null 2>&1 || ! pkg-config --exists criterion >/dev/null 2>&1; then
    echo "criterion ==> SKIP (not installed)"
    return 0
  fi

  gcc -std=c99 -Wall -Wextra -Werror \
    $(pkg-config --cflags criterion) \
    tests/tests_unitaires/tests.c \
    src/*/*.c \
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
    
    echo "Unitaire ==> Total: $crit_tested | Passed: $crit_passing | Failed: $((crit_tested - crit_passing))\n"
    
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

#================================== Step 1 ====================================

echo "=========================== Simple commands =================================\n"
. tests/test_files/simple_commands.sh

echo "====================== Command lists + Semi-colons ===========================\n"
. tests/test_files/command_lists_semicolons.sh

echo "========================= NEWLINE + Compound list ============================\n"
. tests/test_files/new_line_cp_list.sh

echo "=========================== IF - ELIF - ELSE ================================\n"
. tests/test_files/if_elif_else.sh

echo "=============================== Single quotes ================================\n"
. tests/test_files/single_quotes.sh

echo "=============================== True/False ===================================\n"
. tests/test_files/true_false.sh

echo "============================ Echo Corner case ================================\n"
. tests/test_files/echo_corner_case.sh

echo "=============================== Comments =====================================\n"
. tests/test_files/comments.sh

echo "================================= Grammar ====================================\n"
. tests/test_files/grammar.sh

echo "=============================== Stress tests ================================\n"
. tests/test_files/stress_tests.sh

# =================================== Step 2 ====================================

echo "=============================== And Or ====================================\n"
. tests/test_files/and_or.sh

echo "================================= Pipe ==================================\n"
. tests/test_files/pipe.sh

echo "=============================== Negation ================================\n"
. tests/test_files/negation.sh

echo "================================= LOOP =================================\n"
. tests/test_files/loop.sh

echo "========================== Redirection ===========================\n"
. tests/test_files/redirection.sh

echo "========================== Expansion ===========================\n"
. tests/test_files/expansion.sh

echo "============================ Backslash ================================\n"
. tests/test_files/backslash.sh

echo "=========================== Variables ==================================\n"
. tests/test_files/variables.sh

#================================== Step 3 ====================================
echo "================================= Builtin exit =================================\n"
. tests/test_files/exit.sh

echo "============================= Builtin cd ==============================\n"
. tests/test_files/cd.sh

echo "=========================== Builtin break/continue ===========================\n"
. tests/test_files/break_continue.sh

echo "=========================== Command blocks ===========================\n"
. tests/test_files/command_blocks.sh

printf "Fonctionel => Total: %d | Passed: %d | Failed: %d\n\n" "$TOTAL" "$PASS" "$((TOTAL - PASS))"

if [ "${COVERAGE:-no}" = "yes" ]; then
  run_criterion
fi

if [ "$TOTAL" -gt 0 ]; then
  SCORE=$(( (100 * PASS) / TOTAL ))
else
  SCORE=0
fi

printf "%d" "$SCORE" > "$OUTFILE"

rm -f "$REF_OUT" "$TEST_OUT" "$REF_ERR" "$TEST_ERR"

exit 0
