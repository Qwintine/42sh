# Name of the libaries
lib_LIBRARIES = libast.a

# List of file to compile in libast.a
libast_a_SOURCES =  \
  ast.c \
  ast.h

libast_a_CPPFLAGS = -I$(top_srcdir)/src

libast_a_CFLAGS = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla


noinst_LIBRARIES = libast.a
