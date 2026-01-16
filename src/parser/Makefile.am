noinst_LIBRARIES = libparser.a

libparser_a_SOURCES =  \
  parser.c \
  parser.h \
  parser_utils.c \
  parser_aux.h \
  parser_redirections.c \
  parser_rules.c \
  parser_commands.c \
  parser_pipelines.c \
  parser_lists.c \
  ../lexer/lexer.c \
  ../lexer/lexer_utils.c \
  ../lexer/lexer_handlers.c \
  ../lexer/lexer_tokens.c \
  ../lexer/lexer_operators.c \
  ../ast/ast.c

libparser_a_CPPFLAGS = -I$(top_srcdir)/src

libparser_a_CFLAGS = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla