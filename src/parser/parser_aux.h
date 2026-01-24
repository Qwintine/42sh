#ifndef PARSER_AUX_H
#define PARSER_AUX_H

#include <stddef.h>

#include "../lexer/lexer.h"
#include "parser.h"

// parser_utils.c
struct token *peek(struct lex *lex);
struct token *pop(struct lex *lex);
int discard_token(struct token *tok);
int is_redir(enum type type);

// parser_redirections.c
int parser_redir(struct lex *lex, struct ast_cmd *ast_cmd);
int parser_element(struct lex *lex, struct ast_cmd *ast_cmd, size_t *w);
int parser_prefix(struct lex *lex, struct ast_cmd *ast_cmd);

// parser_rules.c
struct ast *parser_elif(struct lex *lex, struct dictionnary *dict);
struct ast *parser_else_clause(struct lex *lex, struct dictionnary *dict);
struct ast *parser_rule_if(struct lex *lex, struct dictionnary *dict);
struct ast *parser_rule_while(struct lex *lex, struct dictionnary *dict);
struct ast *parser_rule_until(struct lex *lex, struct dictionnary *dict);
struct ast *parser_rule_for(struct lex *lex, struct dictionnary *dict);
struct ast *parser_rule_command_block(struct lex *lex, struct dictionnary *dict);

// parser_commands.c
struct ast *parser_shell_command(struct lex *lex, struct dictionnary *dict);
struct ast *parser_simple_command(struct lex *lex);
struct ast *parser_command(struct lex *lex, struct dictionnary *dict);

// parser_pipelines.c
struct ast *parser_pipeline(struct lex *lex, struct dictionnary *dict);
struct ast *parser_and_or(struct lex *lex, struct dictionnary *dict);

// parser_lists.c
struct ast *parser_compound_list(struct lex *lex, struct dictionnary *dict);
struct ast *parser_list(struct lex *lex, struct dictionnary *dict);

#endif /* PARSER_AUX_H */
