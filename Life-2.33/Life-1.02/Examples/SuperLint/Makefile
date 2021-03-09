install:
	test -d $(libdir)/Examples/SuperLint || \
		$(MKDIRHIER) $(libdir)/Examples/SuperLint
	set -x; \
	for f in *.lf README *.doc *.c; do \
		$(INSTALL) -m 744 $$f $(libdir)/Examples/SuperLint; \
	done; \
	for f in sl slc wild_life; do \
		$(INSTALL) -m 755 $$f $(libdir)/Examples/SuperLint; \
	done

dist:
	test -d $(distdir)/Examples/SuperLint || \
		$(MKDIRHIER) $(distdir)/Examples/SuperLint
	-$(CP) Makefile README *.lf *.doc *.c sl slc wild_life \
		$(distdir)/Examples/SuperLint
