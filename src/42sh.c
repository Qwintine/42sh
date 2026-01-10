#include <stdio.h>
#include "io/io.h"
#include "parser/parser.h"
#include "utils/prettyprint.h"
#include "ast/ast.h"

/*
Truc a corriger:
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
