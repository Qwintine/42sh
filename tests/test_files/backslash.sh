#!/bin/sh

testcase "all_quotes_2" "-c" "echo \\\' \\n \\\" \\\" \\\' a b c d \\\\ \\| \\\""
testcase "hard_var_4" "-c" "echo JEsuisTONpere\\\$JEsuisTONpere\\=JEsuisTONpere-tuJEsuisTONpere\\%tiJEsuisTONpere\\^ta_"
#testcase "escape_newline4" "-c" ""
testcase "special_quote_1" "-c" "echo \\& \\; \\< \\> \\( \\) \\t \\% \\^ \\* \\- \\+ \\= \ \\\# \\~ \\! \\t \\t"

