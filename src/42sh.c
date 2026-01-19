#include <stdio.h>
#include <stdlib.h>

#include "ast/ast.h"
#include "expand/expand.h"
#include "io/io.h"
#include "parser/parser.h"
#include "utils/prettyprint.h"

/*
Truc a corriger:
    -> Msg erreur surr stderr dans endroit code adapté ( syntax dans lexer,
grammaire dans parser, builtin dans fct builtin, etc...)
    -> echo (sans argument) plante
    -> execvp ne s'arrete pas si la commande n'existe pas
    -> ;; ne renvoie pas d'erreur
    -> if true; then fi doit planter (sans cmd apres then) pareil pour else
    -> if sans condition marche (ne devrais pas (return 2))
    -> verifier que les quotes se ferment bien (lexer)
    -> \\ n'affiche rien (lexer) (il devrais afficher \)
    -> echo -e -E prendre le dernier en compte seulement (builtin/echo.c)
    -> echo -(unknown) doit afficher -(unknown) (builtin/echo.c)
    -> echo "test#commentaire" doit afficher test#commentaire (lexer)
    -> echo \#commentaire doit afficher #commentaire (lexer)
*/

int main(int argc, char **argv)
{
    struct dictionnary *vars = init_dict();
    int prettyprint = 0;
    char *buff = NULL;
    FILE *entry = arg_file(argc, argv, &prettyprint, &buff);
    if (!entry)
    {
        fprintf(stderr, "42sh: error file entry\n");
        free_dict(vars);
        return 2;
    }

    int eof = 0;
    int res = 0;
    int exit= 0;

    while (!eof)
    {
        struct ast *ast = parser(entry, &eof);

        if (!ast)
        {
            fclose(entry);
            free(buff);
            free_dict(vars);
            fprintf(stderr, "42sh: grammar/syntax error\n");
            return 2;
        }

        if (prettyprint)
            print_ast(ast);
        else
        {
            if (ast->type != AST_LIST
                || ((struct ast_list *)ast)->elt != NULL)
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
    free(buff);
    free_dict(vars);

    return res;
}
