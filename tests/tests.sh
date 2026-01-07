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
  cmd="$2"

  TOTAL=$((TOTAL + 1))

  $REF_SHELL -c "$cmd" >"$REF_OUT" 2>&1
  ref_status=$?
  printf "\n[exit:%d]\n" "$ref_status" >>"$REF_OUT"

  "$BIN" -c "$cmd" >"$TEST_OUT" 2>&1
  test_status=$?
  printf "\n[exit:%d]\n" "$test_status" >>"$TEST_OUT"

  if diff -u "$REF_OUT" "$TEST_OUT" >/dev/null; then
    echo "$name ==> OK"
    PASS=$((PASS + 1))
  else
    echo "$name ==> FAIL"
    diff -u "$REF_OUT" "$TEST_OUT" | grep -E '^(@@|[-+])' | grep -vE '^(---|\+\+\+)' || true
  fi
  echo
}

testcase "echo Hello World!" "echo Hello World!"
testcase "exit 42" "exit 42"
testcase "ls" "ls"

if [ "$TOTAL" -gt 0 ]; then
  SCORE=$(( (100 * PASS) / TOTAL ))
else
  SCORE=0
fi

printf "%d" "$SCORE" > "$OUTFILE"

rm -f "$REF_OUT" "$TEST_OUT"

exit 0
