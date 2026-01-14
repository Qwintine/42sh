#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
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
            if (!peek(lex))
                return NULL;
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

/* Helper pour savoir si token de type Redir */
static int is_redir(enum type type)
{
	return type == REDIR_OUT || type == REDIR_IN || type == REDIR_APPEND 
		||  type == REDIR_DUP_OUT || type == REDIR_DUP_IN 
		|| type == REDIR_NO_CLOBB || type == REDIR_IO;
}

//possible de supprimer si on merge les type de tok et de redir
static int tok_to_redir(enum type token_type, enum redir_type *out)
{
	if(out)
	{
		switch(token_type)
		{
			case REDIR_OUT:
				*out = R_OUT;
				break;
			case REDIR_IN:
				*out = R_IN;
				break;
			case REDIR_APPEND:
				*out = R_APPEND;
				break;
			case REDIR_DUP_OUT:
				*out = R_DUP_OUT;
				break;
			case REDIR_DUP_IN:
				*out = R_DUP_IN;
				break;
			case REDIR_NO_CLOBB:
				*out = R_NO_CLOBB;
				break;
			case REDIR_IO:
				*out = R_IO;
				break;
			default:
				return 1;
		}
		return 0;
	}
	return 1; 
}

//a faire dans lexer
static int all_digits(const char *string) //io number build only with unseparated digits
{
	if(!string || !*string)
	{
		return 0;
	}
	for(size_t i = 0; string[i]; i++)
	{
		if(!isdigit(string[i]))
		{
			return 0; 
		}
	}
	return 1;
}

//============================ Parser Grammar =================================

static struct ast *parser_and_or(struct lex *lex);
static struct ast *parser_else_clause(struct lex *lex);
static struct ast *parser_list(struct lex *lex);


static struct redir *parse_redirection(struct lex *lex)
{
	if(!peek(lex) || !is_redir(peek(lex)->token_type))
		return NULL;
	struct token *op = pop(lex); // <, >, etc... not && or ||

	if(!peek(lex) || peek(lex)->token_type != WORD) // no target for redir
	{
		free_token(op);
		return NULL;
	}

	struct token *target = pop(lex);
	struct redir *redir = malloc(sizeof(struct redir )); // créer init redir dans redir.c 
	if(!redir)
	{
		free_token(op);
		free_token(target);
		return NULL;
	}
	redir->io_num = -1;
	redir->target = target->value;
	free(target); // didn't free value because used by redir
	if(tok_to_redir(op->token_type, &redir->type))
	{
		free(redir->target);
		free(redir); //créer free_redir dans redir.c
		free_token(op);
		return NULL;
	}
	free_token(op);
	return redir;
}

/*
 * Description:
 * 	Absorb a chain of and_or separeted by ';' or '\n'
 * Return:
 * 	*ast -> chained list of  parser_and_or result ast
 * Verbose:
 * 	Grammar:
 * 		{'\n'} and_or { ( ';' | '\n' ) {'\n'} and_or } [';'] {'\n'} ;
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
    if (!current->elt)
    {
        free_ast((struct ast *)current);
        return NULL;
    }

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
        if (!new_node->elt)
        {
            free_ast((struct ast *)new_node);
            return NULL;
        }
        current->next = new_node;
        current = new_node;
    }
    return (struct ast *)head;
}

static struct ast *parser_elif(struct lex *lex)
{
    if (!peek(lex) || peek(lex)->token_type != ELIF)
        return NULL;
    discard_token(pop(lex));

    struct ast_if *ast_if = (struct ast_if *)init_ast_if();

    ast_if->condition = parser_compound_list(lex);

    if (!peek(lex) || peek(lex)->token_type != THEN || !ast_if->condition)
    {
        goto ERROR;
    }
    discard_token(pop(lex));

    ast_if->then_body = parser_compound_list(lex);
    if (!ast_if->then_body)
    {
        goto ERROR;
    }

    if (peek(lex)
        && (peek(lex)->token_type == ELSE || peek(lex)->token_type == ELIF))
    {
        ast_if->else_body = parser_else_clause(lex);
        if (!ast_if->else_body)
        {
            goto ERROR;
        }
    }

    return (struct ast *)ast_if;
ERROR:
    free_ast((struct ast *)ast_if);
    return NULL;
}

/*
 * Description:
 * 	Define which parser to use depending on the context
 * Verbose:
 * 	Grammar:
 * 		else_clause =
 * 			'else' compound_list
 * 		      | 'elif' compound_list 'then' compound_list [else_clause]
 * 		      ;
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
 * Description:
 * 	Handle a 'if' block by calling corresponding parser at each step
 * Return:
 *
 * Verbose:
 * 	Grammar:
 * 		'if' compound_list 'then' compound_list [else_clause] 'fi' ;
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
    if (!ast_if->condition)
    {
        goto ERROR;
    }

    if (!peek(lex) || peek(lex)->token_type != THEN)
    {
        goto ERROR;
    }
    discard_token(pop(lex));
    ast_if->then_body = parser_compound_list(lex);
    if (!ast_if->then_body)
    {
        goto ERROR;
    }

    if (peek(lex)
        && (peek(lex)->token_type == ELSE || peek(lex)->token_type == ELIF))
    {
        ast_if->else_body = parser_else_clause(lex);
        if (!ast_if->else_body)
        {
            goto ERROR;
        }
    }

    if (!peek(lex) || peek(lex)->token_type != FI)
    {
        goto ERROR;
    }
    discard_token(pop(lex));

    return (struct ast *)ast_if;

ERROR:
    free_ast((struct ast *)ast_if);
    return NULL;
}

// See parser_rule_if ( for now )
static struct ast *parser_shell_command(struct lex *lex)
{
    return parser_rule_if(lex);
}

/*
 * Description:
 * 	Group words in a command in order to form a list of commands
 * Return:
 * 	*ast -> ast containing a command to execute
 * Verbose:
 *
 */
// prochaine step -> ajouter gestion des préfixes ( cf. Trove Shell Syntax )
/*static struct ast *parser_simple_command(struct lex *lex)
{
    struct ast_cmd *ast_cmd = (struct ast_cmd *)init_ast_cmd();
    size_t ind = 0;

    if (peek(lex) && peek(lex)->token_type == WORD)
    {
        struct token *tok = pop(lex);
        if (!tok)
            goto ERROR;
        ast_cmd->words[ind] = tok->value;
        free(tok);
        ind++;
        ast_cmd->words = realloc(ast_cmd->words, (ind + 1) * sizeof(char *));
        if (!ast_cmd->words)
        {
            free_ast((struct ast *)ast_cmd);
            return NULL;
        }
        // ajouter test realloc pour sécu -> peu probable mais pas pro
        ast_cmd->words[ind] = NULL;

        lex->context = WORD;
        while (peek(lex) != NULL && peek(lex)->token_type == WORD)
        {
            tok = pop(lex);
            if (!tok)
                goto ERROR;
            ast_cmd->words[ind] = tok->value;
            free(tok);
            ind++;
            ast_cmd->words =
                realloc(ast_cmd->words, (ind + 1) * sizeof(char *));
            if (!ast_cmd->words)
                goto ERROR;
            ast_cmd->words[ind] = NULL;
        }
        lex->context = KEYWORD;
        if (!peek(lex))
            goto ERROR;
        return (struct ast *)ast_cmd;
    }
    lex->context = KEYWORD;

ERROR:
    free_ast((struct ast *)ast_cmd);
    return NULL;
}*/

static struct ast *parser_simple_command(struct lex *lex)
{
    struct ast_cmd *ast_cmd = (struct ast_cmd *)init_ast_cmd();
    size_t w = 0;
    size_t r = 0;

    //  WORD or redir (prefix-only)
    if (!peek(lex) || (peek(lex)->token_type != WORD && !is_redir(peek(lex)->token_type)))
        goto ERROR;

    lex->context = WORD;

    while (peek(lex) && (peek(lex)->token_type == WORD || is_redir(peek(lex)->token_type)))
    {
        if (peek(lex)->token_type == WORD) // word
        {
		struct token *tok = pop(lex);
		if(tok && all_digits(tok->value))
		{
			if(peek(lex) && is_redir(peek(lex)->token_type)) //  io num 
			{
				int io = atoi(tok->value);
				free_token(tok);
				struct redir *redir = parse_redirection(lex);
				if(!redir)
					goto ERROR;
				redir->io_num = io;
				ast_cmd->redirs[r] = redir;
				r++;
				ast_cmd->redirs = realloc(ast_cmd->redirs, (r+1) * sizeof(struct redir ));
				if(!ast_cmd->redirs)
				{
					goto ERROR;
				}
				ast_cmd->redirs[r] = NULL;
				continue;
			}
			// normal word
			ast_cmd->words[w] = tok->value;
			w++;
			free(tok);
			ast_cmd->words = realloc(ast_cmd->words, (w+1) * sizeof(char *));
			if(!ast_cmd->words)
				goto ERROR;
			ast_cmd->words[w] = NULL;
		}
		else
		{	
			if (!tok) 
				goto ERROR;

			ast_cmd->words[w] = tok->value;
			free(tok);
			w++;

			ast_cmd->words = realloc(ast_cmd->words, (w + 1) * sizeof(char *));
			if (!ast_cmd->words) 
				goto ERROR;
			ast_cmd->words[w] = NULL;
		}
        }
        else // redir
        {
            struct redir *redir = parse_redirection(lex);
            if (!redir) 
		    goto ERROR;

            ast_cmd->redirs[r] = redir;
            r++;

            ast_cmd->redirs = realloc(ast_cmd->redirs, (r + 1) * sizeof(struct redir *));
            if (!ast_cmd->redirs) 
		    goto ERROR;
            ast_cmd->redirs[r] = NULL;
        }
    }

    lex->context = KEYWORD;

    // at least one must exist
    if (w == 0 && r == 0)
        goto ERROR;

    return (struct ast *)ast_cmd;

ERROR:
    lex->context = KEYWORD;
    free_ast((struct ast *)ast_cmd);
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
    if (!peek(lex)
        || peek(lex)->token_type == END) // can't start with EOF -> Error
        return NULL;

    lex->context =
        KEYWORD; // list starts with keyword (command, operator, if, etc...)
    struct ast_list *head = (struct ast_list *)init_ast_list();
    struct ast_list *current = head;

    current->elt = parser_and_or(
        lex); // récursion sur ast type and_or ( cf. parser_and_or )
    if (!current->elt)
    {
        free(head);
        return NULL;
    }

    while (peek(lex) && (peek(lex)->token_type == SEMI_COLON)) // séparateurs
    {
        discard_token(pop(lex));

        if (!peek(lex) || peek(lex)->token_type == END)
            break;

        struct ast_list *new_node = (struct ast_list *)
            init_ast_list(); // éléments liste de block de and_or
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
struct ast *parser(FILE *entry, int *eof)
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
        *eof = 1;
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

    if (!peek(lex)
        || (peek(lex)->token_type != NEWLINE && peek(lex)->token_type != END))
    {
        free_lex(lex);
        free_ast(ast);
        return NULL;
    }

    *eof = (pop(lex)->token_type == END);

    free_lex(lex);
    return ast;
}
