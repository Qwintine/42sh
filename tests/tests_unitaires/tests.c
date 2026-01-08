#define _POSIX_C_SOURCE 200809L

#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <criterion/redirect.h>

#include <stdlib.h>

#include "../../src/io/io.h"
#include "../../src/utils/token.h"
#include "../../src/lexer/lexer.h"

TestSuite(Test42sh);

Test(Test42sh, lex_simple, .init = cr_redirect_stdout)
{
    FILE *f = arg_file(3, (char*[]){"program", "-c", "echo hello\n"});
	cr_assert_not_null(f);

    struct lex *lx = init_lex(f);
    cr_assert_not_null(lx);

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, WORD));
    cr_expect(eq(str, lx->current_token->value, "echo"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, WORD));
    cr_expect(eq(str, lx->current_token->value, "hello\n"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, END));

    free_lex(lx);
}

Test(Test42sh, lex_medium, .init = cr_redirect_stdout)
{
    FILE *f = arg_file(3, (char*[]){"program", "-c", "test 'W  o'   \n   \\n 'r   ld'     !"});
	cr_assert_not_null(f);

    struct lex *lx = init_lex(f);
    cr_assert_not_null(lx);

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, WORD));
    cr_expect(eq(str, lx->current_token->value, "test"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, WORD));
    cr_expect(eq(str, lx->current_token->value, "W  o"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, WORD));
    cr_expect(eq(str, lx->current_token->value, "\n"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, WORD));
    cr_expect(eq(str, lx->current_token->value, "\\n"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, WORD));
    cr_expect(eq(str, lx->current_token->value, "r   ld"));

    cr_expect(eq(int, lexer(lx), 0));  
    cr_expect(eq(int, lx->current_token->token_type, WORD));
    cr_expect(eq(str, lx->current_token->value, "!"));

    cr_expect(eq(int, lexer(lx), 0));
    cr_expect(eq(int, lx->current_token->token_type, END));

    free_lex(lx);
}
