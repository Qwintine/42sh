SUBDIRS = src
EXTRA_DIST = configure.ac

TESTS = tests/tests.sh

# flag clean-all pour tout supprimer
MAINTAINERCLEANFILES = \
	aclocal.m4 \
	configure \
	compile \
	depcomp \
	install-sh \
	missing \
	ar-lib

clean-all:
	$(MAKE) maintainer-clean
	-rm -rf autom4te.cache
	-rm -rf src/**/.deps
	-rm -rf src/.deps
	-rm -f src/**/Makefile.in
	-rm -f src/Makefile.in
	-rm -f Makefile.in
	-rm -f test-driver
	-rm -f out

debug:
	$(MAKE) clean
	CFLAGS="-g -O0" $(MAKE)


.PHONY: doxygen
doxygen:
	$(MAKE) -C docs doxygen || true
