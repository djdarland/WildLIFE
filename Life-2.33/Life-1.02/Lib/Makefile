install:
	test -d $(libdir)/Lib || $(MKDIRHIER) $(libdir)/Lib
	for f in *.lf *.exp; do \
	  $(INSTALL) -m 744 $$f $(libdir)/Lib; \
	done

dist:
	test -d $(distdir)/Lib || $(MKDIRHIER) $(distdir)/Lib
	$(CP) *.lf *.exp Makefile $(distdir)/Lib/
