# docs/Makefile.am
EXTRA_DIST = ../Doxyfile

.PHONY: doxygen
doxygen:
	doxygen ../Doxyfile
	xdg-open doxygen/html/index.html >/dev/null 2>&1 &

clean-local:
	rm -rf html latex

