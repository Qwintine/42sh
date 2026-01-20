#define _POSIX_C_SOURCE 200809L

#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <criterion/redirect.h>

#include <stdlib.h>

#include "../../src/io/io.h"
#include "../../src/utils/token.h"
#include "../../src/lexer/lexer.h"
#include "../../src/lexer/lexer_aux.h"
#include "../../src/expand/expand.h"
#include "../../src/utils/redir.h"

TestSuite(Test42sh);

Test(Test42sh, lex_simple, .init = cr_redirect_stdout)
{
    char *buff;
    FILE *f = arg_file(3, (char*[]){"program", "-c", "echo hello\n"}, NULL, &buff);
    cr_assert_not_null(f);

    struct lex *lx = init_lex(f);
    cr_assert_not_null(lx);

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, WORD));
    cr_expect(eq(str, lx->current_token->value, "echo"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, WORD));
    cr_expect(eq(str, lx->current_token->value, "hello"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, NEWLINE));
    cr_expect(eq(str, lx->current_token->value, "\n"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, END));

    free_lex(lx);
}

Test(Test42sh, lex_medium_1, .init = cr_redirect_stdout)
{
    char *buff;
    FILE *f = arg_file(3, (char*[]){"program", "-c", "echo 'W  o'   \n   \\n 'r   ld'     !"}, NULL, &buff);
    cr_assert_not_null(f);

    struct lex *lx = init_lex(f);
    cr_assert_not_null(lx);

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, WORD));
    cr_expect(eq(str, lx->current_token->value, "echo"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, WORD));
    cr_expect(eq(str, lx->current_token->value, "'W  o'"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, NEWLINE));
    cr_expect(eq(str, lx->current_token->value, "\n"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, WORD));
    cr_expect(eq(str, lx->current_token->value, "n"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, WORD));
    cr_expect(eq(str, lx->current_token->value, "'r   ld'"));

    cr_expect(eq(int, lexer(lx), 0));  
    cr_expect(eq(int, lx->current_token->token_type, WORD));
    cr_expect(eq(str, lx->current_token->value, "!"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, END));

    free_lex(lx);
}

Test(Test42sh, lex_medium_2, .init = cr_redirect_stdout)
{
    char *buff;
    FILE *f = arg_file(3, (char*[]){"program", "-c", "echo hello; cat"}, NULL, &buff);
    cr_assert_not_null(f);

    struct lex *lx = init_lex(f);
    cr_assert_not_null(lx);

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, WORD));
    cr_expect(eq(str, lx->current_token->value, "echo"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, WORD));
    cr_expect(eq(str, lx->current_token->value, "hello"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, SEMI_COLON));
    cr_expect(eq(str, lx->current_token->value, ";"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, WORD));
    cr_expect(eq(str, lx->current_token->value, "cat"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, END));

    free_lex(lx);
}

Test(Test42sh, lex_medium_operator, .init = cr_redirect_stdout)
{
    char *buff;
    FILE *f = arg_file(3, (char*[]){"program", "-c", "if ! false && false; then echo ok& fi"}, NULL, &buff);
    cr_assert_not_null(f);

    struct lex *lx = init_lex(f);
    cr_assert_not_null(lx);
    lx->context = KEYWORD;

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, IF));
    cr_expect(eq(str, lx->current_token->value, "if"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, NEGATION));
    cr_expect(eq(str, lx->current_token->value, "!"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, WORD));
    cr_expect(eq(str, lx->current_token->value, "false"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, AND));
    cr_expect(eq(str, lx->current_token->value, "&&"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, WORD));
    cr_expect(eq(str, lx->current_token->value, "false"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, SEMI_COLON));
    cr_expect(eq(str, lx->current_token->value, ";"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, THEN));
    cr_expect(eq(str, lx->current_token->value, "then"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, WORD));
    cr_expect(eq(str, lx->current_token->value, "echo"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, WORD));
    cr_expect(eq(str, lx->current_token->value, "ok"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, SEMI_COLON));
    cr_expect(eq(str, lx->current_token->value, "&"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, FI));
    cr_expect(eq(str, lx->current_token->value, "fi"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, END));

    free_lex(lx);
}

Test(Test42sh, lex_syntax_error, .init = cr_redirect_stdout)
{
    char *buff;
    FILE *f = arg_file(3, (char*[]){"program", "-c", "echo 'a"}, NULL, &buff);
    cr_assert_not_null(f);

    struct lex *lx = init_lex(f);
    cr_assert_not_null(lx);

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, WORD));
    cr_expect(eq(str, lx->current_token->value, "echo"));

    cr_expect(eq(int, lexer(lx), 1));

    free_lex(lx);
}

Test(Test42sh, lex_loop, .init = cr_redirect_stdout)
{
    char *buff;
    FILE *f = arg_file(3, (char*[]){"program", "-c", "while true; do echo a; done"}, NULL, &buff);
    cr_assert_not_null(f);

    struct lex *lx = init_lex(f);
    cr_assert_not_null(lx);
    lx->context = KEYWORD;

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, WHILE));
    cr_expect(eq(str, lx->current_token->value, "while"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, WORD));
    cr_expect(eq(str, lx->current_token->value, "true"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, SEMI_COLON));
    cr_expect(eq(str, lx->current_token->value, ";"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, DO));
    cr_expect(eq(str, lx->current_token->value, "do"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, WORD));
    cr_expect(eq(str, lx->current_token->value, "echo"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, WORD));
    cr_expect(eq(str, lx->current_token->value, "a"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, SEMI_COLON));
    cr_expect(eq(str, lx->current_token->value, ";"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, DONE));
    cr_expect(eq(str, lx->current_token->value, "done"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, END));

    free_lex(lx);
}

Test(Test42sh, lex_redir, .init = cr_redirect_stdout)
{
    char *buff;
    FILE *f = arg_file(3, (char*[]){"program", "-c", "test > 2 < 4>> ok42>& 1234<&    ok >|  1<>2"}, NULL, &buff);
    cr_assert_not_null(f);

    struct lex *lx = init_lex(f);
    cr_assert_not_null(lx);
    lx->context = KEYWORD;

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, WORD));
    cr_expect(eq(str, lx->current_token->value, "test"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, REDIR_OUT));
    cr_expect(eq(str, lx->current_token->value, ">"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, WORD));
    cr_expect(eq(str, lx->current_token->value, "2"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, REDIR_IN));
    cr_expect(eq(str, lx->current_token->value, "<"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, IO_NUMBER));
    cr_expect(eq(str, lx->current_token->value, "4"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, REDIR_APPEND));
    cr_expect(eq(str, lx->current_token->value, ">>"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, WORD));
    cr_expect(eq(str, lx->current_token->value, "ok42"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, REDIR_DUP_OUT));
    cr_expect(eq(str, lx->current_token->value, ">&"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, IO_NUMBER));
    cr_expect(eq(str, lx->current_token->value, "1234"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, REDIR_DUP_IN));
    cr_expect(eq(str, lx->current_token->value, "<&"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, WORD));
    cr_expect(eq(str, lx->current_token->value, "ok"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, REDIR_NO_CLOBB));
    cr_expect(eq(str, lx->current_token->value, ">|"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, IO_NUMBER));
    cr_expect(eq(str, lx->current_token->value, "1"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, REDIR_IO));
    cr_expect(eq(str, lx->current_token->value, "<>"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, WORD));
    cr_expect(eq(str, lx->current_token->value, "2"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, END));

    free_lex(lx);
}

Test(Test42sh, lex_expand, .init = cr_redirect_stdout)
{
    char *buff;
    FILE *f = arg_file(3, (char*[]){"program", "-c", "test=ok; echo $test ${test}"}, NULL, &buff);
    cr_assert_not_null(f);

    struct lex *lx = init_lex(f);
    cr_assert_not_null(lx);

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, ASSIGNMENT));
    cr_expect(eq(str, lx->current_token->value, "test=ok"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, SEMI_COLON));
    cr_expect(eq(str, lx->current_token->value, ";"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, WORD));
    cr_expect(eq(str, lx->current_token->value, "echo"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, WORD));
    cr_expect(eq(str, lx->current_token->value, "$test"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, WORD));
    cr_expect(eq(str, lx->current_token->value, "${test}"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, END));

    free_lex(lx);
}

Test(Test42sh, lex_quotes, .init = cr_redirect_stdout)
{
    char *buff;
    FILE *f = arg_file(3, (char*[]){"program", "-c", "echo 'a b c' \"d $e f\""}, NULL, &buff);
    cr_assert_not_null(f);

    struct lex *lx = init_lex(f);
    cr_assert_not_null(lx);

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, WORD));
    cr_expect(eq(str, lx->current_token->value, "echo"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, WORD));
    cr_expect(eq(str, lx->current_token->value, "'a b c'"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, WORD));
    cr_expect(eq(str, lx->current_token->value, "\"d $e f\""));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, END));

    free_lex(lx);
}

Test(Test42sh, lex_backslash, .init = cr_redirect_stdout)
{
    char *buff;
    FILE *f = arg_file(3, (char*[]){"program", "-c", "echo a\\ b \\n \\\\"}, NULL, &buff);
    cr_assert_not_null(f);

    struct lex *lx = init_lex(f);
    cr_assert_not_null(lx);

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, WORD));
    cr_expect(eq(str, lx->current_token->value, "echo"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, WORD));
    cr_expect(eq(str, lx->current_token->value, "a b"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, WORD));
    cr_expect(eq(str, lx->current_token->value, "n"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, WORD));
    cr_expect(eq(str, lx->current_token->value, "\\"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, END));

    free_lex(lx);
}

Test(Test42sh, lex_single_quotes, .init = cr_redirect_stdout)
{
    char *buff;
    FILE *f = arg_file(3, (char*[]){"program", "-c", "echo a'b'c"}, NULL, &buff);
    cr_assert_not_null(f);

    struct lex *lx = init_lex(f);
    cr_assert_not_null(lx);

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, WORD));
    cr_expect(eq(str, lx->current_token->value, "echo"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, WORD));
    cr_expect(eq(str, lx->current_token->value, "a'b'c"));
    
    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, END));

    free_lex(lx);
}

Test(Test42sh, expand_dictionnary, .init = cr_redirect_stdout)
{
    struct dictionnary *dict = init_dict();
    cr_assert_not_null(dict);

    cr_expect(eq(int, add_var(dict, "test=hello"), 0));
    
    char **val = get_var(dict, "test");
    cr_assert_not_null(val);
    cr_expect(eq(str, val[0], "hello"));

    cr_expect(eq(int, add_var(dict, "name=world"), 0));
    val = get_var(dict, "name");
    cr_assert_not_null(val);
    cr_expect(eq(str, val[0], "world"));

    val = get_var(dict, "notexist");
    cr_expect(eq(ptr, val[0], NULL));

    free_dict(dict);
}

Test(Test42sh, expand_empty_value, .init = cr_redirect_stdout)
{
    struct dictionnary *dict = init_dict();
    cr_assert_not_null(dict);

    cr_expect(eq(int, add_var(dict, "empty="), 0));
    
    char **val = get_var(dict, "empty");
    cr_assert_not_null(val);
    cr_expect(eq(str, val[0], ""));

    free_dict(dict);
}

Test(Test42sh, expand_update_variable, .init = cr_redirect_stdout)
{
    struct dictionnary *dict = init_dict();
    cr_assert_not_null(dict);

    cr_expect(eq(int, add_var(dict, "var=first"), 0));
    char **val = get_var(dict, "var");
    cr_assert_not_null(val);
    cr_expect(eq(str, val[0], "first"));

    cr_expect(eq(int, add_var(dict, "var=second"), 0));
    val = get_var(dict, "var");
    cr_assert_not_null(val);
    cr_expect(eq(str, val[0], "second"));

    free_dict(dict);
}

Test(Test42sh, redir_init, .init = cr_redirect_stdout)
{
    struct redir *redir = init_redir();
    cr_assert_not_null(redir);
    
    cr_expect(eq(ptr, redir->io_num, NULL));
    cr_expect(eq(ptr, redir->target, NULL));
    cr_expect(eq(int, redir->type, REDIR_OUT));

    free_redir(redir);
}

Test(Test42sh, token_concat, .init = cr_redirect_stdout)
{
    char *str = strdup("hello");
    cr_assert_not_null(str);

    str = concat(str, ' ');
    cr_assert_not_null(str);
    cr_expect(eq(str, str, "hello "));

    str = concat(str, 'w');
    cr_assert_not_null(str);
    cr_expect(eq(str, str, "hello w"));

    free(str);
}

Test(Test42sh, lex_pipe_operator, .init = cr_redirect_stdout)
{
    char *buff;
    FILE *f = arg_file(3, (char*[]){"program", "-c", "echo a | cat"}, NULL, &buff);
    cr_assert_not_null(f);

    struct lex *lx = init_lex(f);
    cr_assert_not_null(lx);

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, WORD));
    cr_expect(eq(str, lx->current_token->value, "echo"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, WORD));
    cr_expect(eq(str, lx->current_token->value, "a"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, PIPE));
    cr_expect(eq(str, lx->current_token->value, "|"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, WORD));
    cr_expect(eq(str, lx->current_token->value, "cat"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, END));

    free_lex(lx);
}

Test(Test42sh, lex_or_operator, .init = cr_redirect_stdout)
{
    char *buff;
    FILE *f = arg_file(3, (char*[]){"program", "-c", "false || true"}, NULL, &buff);
    cr_assert_not_null(f);

    struct lex *lx = init_lex(f);
    cr_assert_not_null(lx);

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, WORD));
    cr_expect(eq(str, lx->current_token->value, "false"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, OR));
    cr_expect(eq(str, lx->current_token->value, "||"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, WORD));
    cr_expect(eq(str, lx->current_token->value, "true"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, END));

    free_lex(lx);
}

Test(Test42sh, lex_comment, .init = cr_redirect_stdout)
{
    char *buff;
    FILE *f = arg_file(3, (char*[]){"program", "-c", "echo test # this is a comment"}, NULL, &buff);
    cr_assert_not_null(f);

    struct lex *lx = init_lex(f);
    cr_assert_not_null(lx);

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, WORD));
    cr_expect(eq(str, lx->current_token->value, "echo"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, WORD));
    cr_expect(eq(str, lx->current_token->value, "test"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, END));

    free_lex(lx);
}