#include "prettyprint.h"

static void ast_print_if(struct ast *ast)
{
	if(!ast)
		return;
    struct ast_if *ast_if = (struct ast_if *)ast;
    printf("if { ");
    print_ast(ast_if->condition);
    printf(" } then { ");
    print_ast(ast_if->then_body);
    printf(" }");
    if (ast_if->else_body)
    {
        printf(" else { ");
        print_ast(ast_if->else_body);
        printf(" }");
    }
}

static void ast_print_cmd(struct ast *ast)
{
    struct ast_cmd *ast_cmd = (struct ast_cmd *)ast;
	if(!ast_cmd->words)
		return;
    printf("command: ");
    for (int i = 0; ast_cmd->words[i] != NULL; i++)
    {
        printf("%s", ast_cmd->words[i]);
        if (ast_cmd->words[i + 1] != NULL)
        {
            printf(" ");
        }
    }
    printf(";");
}

static void ast_print_list(struct ast *ast)
{
	if(!ast)
		return;
    struct ast_list *ast_list = (struct ast_list *)ast;
    print_ast(ast_list->elt);
    if (ast_list->next)
        ast_print_list((struct ast *)ast_list->next);
    printf("\n");
}

void print_ast(struct ast *ast)
{
    static const ast_handler_free functions[] = {
        [AST_CMD] = &ast_print_cmd,
        [AST_IF] = &ast_print_if,
        [AST_LIST] = &ast_print_list,
    };
    (*functions[ast->type])(ast);
}
