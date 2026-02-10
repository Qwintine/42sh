#include <stdio.h>
#include <stdlib.h>

#include "ast/ast.h"
#include "expand/expand.h"
#include "io/io.h"
#include "parser/parser.h"
#include "utils/prettyprint.h"

/* *
 * @brief Get file entry from args or stdin
 *
 * @param int argc, char **argv -> binary's args
 * @param int *prettyprint -> prettyprint flag
 * @param struct dictionnary *vars -> dictionnary of vars
 * @return int -> exit status
 */
int main(int argc, char **argv)
{
    struct dictionnary *vars = init_dict();
    int prettyprint = 0;
    FILE *entry = arg_file(argc, argv, &prettyprint, vars);
    if (!entry)
    {
        fprintf(stderr, "42sh: error file entry\n");
        free_dict(vars);
        return 2;
    }

    int eof = 0;
    int res = 0;
    int exit = 0;

    while (!eof)
    {
        struct ast *ast = parser(entry, &eof, vars);

        if (!ast)
        {
            fclose(entry);
            free_dict(vars);
            fprintf(stderr, "42sh: grammar/syntax error\n");
            free_stdin_buffer();
            return 2;
        }

        if (prettyprint)
            print_ast(ast);
        else
        {
            if (ast->type != AST_LIST || ((struct ast_list *)ast)->elt != NULL)
                res = run_ast(ast, vars, &exit); // derniere valeur de retour
            if (exit)
            {
                free_ast(ast);
                break;
            }
        }

        free_ast(ast);
    }

    fclose(entry);
    free_dict(vars);
    free_stdin_buffer();

    return res;
}
