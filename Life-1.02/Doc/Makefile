install:
	test -d $(libdir)/Doc || $(MKDIRHIER) $(libdir)/Doc
	set -x; \
	for f in *.doc; do \
		$(INSTALL) -m 744 $$f $(libdir)/Doc; \
	done

dist:
	test -d $(distdir)/Doc || $(MKDIRHIER) $(distdir)/Doc
	-$(CP) Makefile *.doc *.ps *.tex *.dvi *.1 $(distdir)/Doc/
