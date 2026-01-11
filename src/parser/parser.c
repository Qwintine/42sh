#include "parser.h"

#include <stdio.h>
#include <stdlib.h>

#include "../lexer/lexer.h"
#include "../utils/token.h"

/*
 * Description:
 * 	Peek on the lexer to see the current token
 * Arguments:
 * 	lex -> struct lexer being used
 * Return:
 * 	Current_token if success, NULL otherwise
 */
static struct token *peek(struct lex *lex)
{
    if (lex)
    {
        if (!lex->current_token)
        {
            int res = lexer(lex);
            if (res)
                return NULL;
        }
        return lex->current_token;
    }
    return NULL;
}

/*
 * Description:
 * 	Pop the current token from lex
 * Arguments:
 * 	lex -> struct lexer being used
 * Return:
 * 	Copy of current_token if success, NULL otherwise
 */
static struct token *pop(struct lex *lex)
{
    if (lex)
    {
        if (!lex->current_token)
        {
            peek(lex);
        }
        struct token *tok = lex->current_token;
        lex->current_token = NULL;
        return tok;
    }
    return NULL;
}

/*
 * Description:
 * 		Free a discarded token
 * Arguments:
 * 		Token to free
 * Return:
 * 		1 Succes / 0 Error
 */
int discard_token(struct token *tok)
{
    if (!tok)
    {
        return 0;
    }
    free_token(tok);
    return 1;
}

//============================ Parser Grammar =================================

static struct ast *parser_and_or(struct lex *lex);
static struct ast *parser_else_clause(struct lex *lex);
static struct ast *parser_list(struct lex *lex);

/*
 * TODO
 */
static struct ast *parser_compound_list(struct lex *lex)
{
    if (!peek(lex) || peek(lex)->token_type == END)
    {
        return NULL;
    }
    while (peek(lex) && peek(lex)->token_type == NEWLINE)
    {
        discard_token(pop(lex));
    }

    struct ast_list *head = (struct ast_list *)init_ast_list();
    struct ast_list *current = head;

    current->elt = parser_and_or(lex);

    while (peek(lex)
           && (peek(lex)->token_type == SEMI_COLON
               || peek(lex)->token_type == NEWLINE))
    {
        discard_token(pop(lex));

        while (peek(lex) && peek(lex)->token_type == NEWLINE)
        {
            discard_token(pop(lex));
        }

        if (!peek(lex) || peek(lex)->token_type == END
            || peek(lex)->token_type == THEN || peek(lex)->token_type == ELSE
            || peek(lex)->token_type == ELIF || peek(lex)->token_type == FI)
        {
            break;
        }

        struct ast_list *new_node = (struct ast_list *)init_ast_list();
        new_node->elt = parser_and_or(lex);
        current->next = new_node;
        current = new_node;
    }

    return (struct ast *)head;
}

/*
 * TODO
 */
static struct ast *parser_elif(struct lex *lex)
{
    if (!peek(lex) || peek(lex)->token_type != ELIF)
        return NULL;
    discard_token(pop(lex));

    struct ast_if *ast_if = (struct ast_if *)init_ast_if();

    ast_if->condition = parser_compound_list(lex);

    if (!peek(lex) || peek(lex)->token_type != THEN)
    {
        free_ast((struct ast *)ast_if);
        return NULL;
    }
    discard_token(pop(lex));

    ast_if->then_body = parser_compound_list(lex);

    if (peek(lex)
        && (peek(lex)->token_type == ELSE || peek(lex)->token_type == ELIF))
    {
        ast_if->else_body = parser_else_clause(lex);
    }

    return (struct ast *)ast_if;
}

/*
 * TODO
 */
static struct ast *parser_else_clause(struct lex *lex)
{
    if (!peek(lex) || peek(lex)->token_type == END)
        return NULL;

    if (peek(lex)->token_type == ELIF)
    {
        return parser_elif(lex);
    }

    if (peek(lex)->token_type == ELSE)
    {
        discard_token(pop(lex));
        return parser_compound_list(lex);
    }

    return NULL;
}

/*
 * TODO
 */
static struct ast *parser_rule_if(struct lex *lex)
{
    if (!peek(lex)
        || peek(lex)->token_type != IF) // si autre keyword (pas con wlh, bien
                                        // joué Victor1 (désolé bébou Victor2))
        return NULL;
    discard_token(pop(lex)); // consomme if

    struct ast_if *ast_if = (struct ast_if *)init_ast_if();

    ast_if->condition = parser_compound_list(lex);

    if (!peek(lex) || peek(lex)->token_type != THEN)
    {
        free_ast((struct ast *)ast_if);
        return NULL;
    }
    discard_token(pop(lex));
    ast_if->then_body = parser_compound_list(lex);

    if (peek(lex)
        && (peek(lex)->token_type == ELSE || peek(lex)->token_type == ELIF))
    {
        ast_if->else_body = parser_else_clause(lex);
    }

    if (!peek(lex) || peek(lex)->token_type != FI)
    {
        free_ast((struct ast *)ast_if);
        return NULL;
    }
    discard_token(pop(lex));

    return (struct ast *)ast_if;
}

//See parser_rule_if ( for now )
static struct ast *parser_shell_command(struct lex *lex)
{
    return parser_rule_if(lex);
}

/*
 * TODO
 */
// prochaine step -> ajouter gestion des préfixes ( cf. Trove Shell Syntax )
static struct ast *parser_simple_command(struct lex *lex)
{
    struct ast_cmd *ast_cmd = (struct ast_cmd *)init_ast_cmd();
    size_t ind = 0;

    if (peek(lex) && peek(lex)->token_type == WORD)
    {
        struct token *tok = pop(lex);
        ast_cmd->words[ind] = tok->value;
        free(tok);
        ind++;
        ast_cmd->words = realloc(ast_cmd->words, (ind + 1) * sizeof(char *));
        // ajouter test realloc pour sécu -> peu probable mais pas pro
        ast_cmd->words[ind] = NULL;

        lex->context = WORD;
        while (peek(lex) && peek(lex)->token_type == WORD)
        {
            tok = pop(lex);
            ast_cmd->words[ind] = tok->value;
            free(tok);
            ind++;
            ast_cmd->words =
                realloc(ast_cmd->words, (ind + 1) * sizeof(char *));
            ast_cmd->words[ind] = NULL;
        }
        lex->context = KEYWORD;
        return (struct ast *)ast_cmd;
    }
    lex->context = KEYWORD;
    free(ast_cmd->words);
    free(ast_cmd);
    return NULL;
}


// prochaine step -> ajouter gestion  { redirections } après shell_command
static struct ast *parser_command(struct lex *lex)
{
    if (peek(lex) && peek(lex)->token_type == IF)
    {
        return parser_shell_command(lex);
    }
    return parser_simple_command(lex);
}

// See parser_command ( for now )
static struct ast *parser_pipeline(struct lex *lex)
{
    return parser_command(lex);
}

// See parser_command ( for now )
static struct ast *parser_and_or(struct lex *lex)
{
    return parser_pipeline(lex);
}

/*
 * Description:
 * 	Parse list block into sub and_or block(s) separated by semi-colons
 * Arguments:
 * 	*lex -> struct of the lexer:
 * 		entry -> input stream
 * 		current_token -> last token returned by lexer
 * 		context -> context of the parser when retrieving a token
 * Return:
 * 	*ast:
 * 		NULL: Grammar/Syntax error
 * 		| *ast: ast of a list ( cf. Verbose )
 * Verbose:
 * 	Grammar:
 *		list = and_or { ';' and_or } [ ';' ]
 *	TODO
 */
static struct ast *parser_list(struct lex *lex)
{
    if (!peek(lex) || peek(lex)->token_type == END)// can't start with EOF -> Error
        return NULL;

    lex->context = KEYWORD; // list starts with keyword (command, operator, if, etc...)
    struct ast_list *head = (struct ast_list *)init_ast_list();
    struct ast_list *current = head;

    current->elt = parser_and_or(lex); // récursion sur ast type and_or ( cf. parser_and_or )
    if (!current->elt)
    {
        free(head);
        return NULL;
    }

    while (peek(lex)
           && (peek(lex)->token_type == SEMI_COLON
               || peek(lex)->token_type == NEWLINE)) // séparateurs
    {
        discard_token(pop(lex));
        while (peek(lex) && peek(lex)->token_type == NEWLINE) 
        {
            discard_token(pop(lex));
        }
        if (!peek(lex) || peek(lex)->token_type == END)
            break;

        struct ast_list *new_node = (struct ast_list *)init_ast_list(); // éléments liste de block de and_or 
        new_node->elt = parser_and_or(lex); // récursion sur ast type and_or
        if (!new_node->elt)
        {
            free(new_node);
            break;
        }
        current->next = new_node; // liste de and_or
        current = new_node;
    }
    return (struct ast *)head; // 1er elem liste 
}

/*
 * Description:
 * 	Parse the input of our shell into list
 * Arguments:
 * 	FILE *entry -> used to create struct lexer
 * Return:
 * 	struct ast * representing an Abstract Syntax Tree of our input / NULL on
 * Error
 *
 * Verbose:
 * 	Grammar:
 * 		list '\n'
 * 	      | list EOF
 * 	      | '\n'
 * 	      | EOF
 * 	      ;
 * 	TODO
 */
struct ast *parser(FILE *entry)
{
    struct lex *lex = init_lex(entry);
    lex->context = KEYWORD;
    if (!peek(lex))
    {
        free_lex(lex);
        return NULL;
    }

    while (peek(lex)
           && peek(lex)->token_type
               == NEWLINE) // consomme newline until début code évaluable
    {
        discard_token(pop(lex));
    }

    if (!peek(lex)
        || peek(lex)->token_type == END) // fichier sans code évaluable
    {
        free_lex(lex);
        return init_ast_list();
    }

    struct ast *ast =
        parser_list(lex); // récursion sur ast type list ( cf. parser_list )

    if (!ast) // remontée erreur syntax / grammaire
    {
        free_lex(lex);
        return NULL;
    }

    while (peek(lex) && peek(lex)->token_type == NEWLINE)
    {
        discard_token(pop(lex));
    }

    struct token *tok = peek(lex);

    if (!tok || tok->token_type != END) // erreur de grammaire
    {
        free_lex(lex);
        free_ast(ast);
        return NULL;
    }

    free_lex(lex);
    return ast;
}
