noinst_LIBRARIES = libexec.a

libexec_a_SOURCES =  \
  exec.c \
  exec.h \
  redir_exec.c \
  ../builtin/echo.c \
  ../builtin/exit.c \
  ../builtin/cd.c \
  ../builtin/dot.c \
  ../builtin/unset.c
  ../utils/itoa.c

libexec_a_CPPFLAGS = -I$(top_srcdir)/src

libexec_a_CFLAGS = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla
