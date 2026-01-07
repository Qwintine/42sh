SUBDIRS = io lexer

bin_PROGRAMS = 42sh
42sh_SOURCES = 42sh.c

AM_CFLAGS = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla
AM_CPPFLAGS = -I$(top_srcdir)/src

42sh_LDADD = \
  io/libio.a \
  lexer/liblexer.a
