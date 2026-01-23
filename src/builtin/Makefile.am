noinst_LIBRARIES = libbuiltin.a

libbuiltin_a_SOURCES =  \
  echo.c \
  echo.h \
  exit.c \
  exit.h \
  cd.c \
  cd.h \
  dot.c \
  dot.h \
  unset.c \
  unset.c

libbuiltin_a_CPPFLAGS = -I$(top_srcdir)/src

libbuiltin_a_CFLAGS = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla
