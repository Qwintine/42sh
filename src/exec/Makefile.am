noinst_LIBRARIES = libexec.a

libexec_a_SOURCES =  \
  exec.c \
  exec.h \
  ../builtin/echo.c

libexec_a_CPPFLAGS = -I$(top_srcdir)/src

libexec_a_CFLAGS = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla