install:
	test -d $(libdir)/Tools || $(MKDIRHIER) $(libdir)/Tools
	set -x; \
	for f in *.lf *.exp README *.doc; do \
	  $(INSTALL) -m 744 $$f $(libdir)/Tools; \
	done

dist:
	test -d $(distdir)/Tools || $(MKDIRHIER) $(distdir)/Tools
	-$(CP) *.lf *.exp README *.doc Makefile $(distdir)/Tools/
