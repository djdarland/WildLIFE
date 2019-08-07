MKINSTALL = \
 $(MAKE) MKDIRHIER="$(MKDIRHIER)" libdir="$(libdir)" INSTALL="$(INSTALL)" install

install:
	test -d $(libdir)/Examples || $(MKDIRHIER) $(libdir)/Examples
	set -x; \
	for f in *.lf *README *.doc *.c; do \
		$(INSTALL) -m 744 $$f $(libdir)/Examples; \
	done
	cd SuperLint; $(MKINSTALL)

MKDIST = $(MAKE) distdir="$(distdir)" MKDIRHIER="$(MKDIRHIER)" CP="$(CP)" dist

dist:
	test -d $(distdir)/Examples || $(MKDIRHIER) $(distdir)/Examples
	-$(CP) Makefile *.lf *README *.doc *.c $(distdir)/Examples/
	cd SuperLint; $(MKDIST)
