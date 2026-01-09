#include <stdio.h>
#include "io/io.h"
#include "parser/parser.h"
#include "utils/prettyprint.h"
#include "ast/ast.h"

int main(int argc, char **argv)
{
    int prettyprint = 0;
    FILE *entry = arg_file(argc, argv, &prettyprint);
    if (!entry)
        return 2;
    struct ast *ast = parser(entry);

    if (!ast)
        return 2;

    if (prettyprint)
        print_ast(ast);
    else
    {
        int res = run_ast(ast);
        free_ast(ast);
        return res;
    }

    free_ast(ast);
    return 0;
}