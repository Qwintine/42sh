noinst_LIBRARIES = libparser.a

libparser_a_SOURCES =  \
  parser.c \
  parser.h \
  ../lexer/lexer.c \
  ../ast/ast.c

libparser_a_CPPFLAGS = -I$(top_srcdir)/src

libparser_a_CFLAGS = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla