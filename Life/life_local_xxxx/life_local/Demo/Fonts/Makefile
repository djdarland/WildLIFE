install:
	test -d $(libdir)/Demo/Fonts || $(MKDIRHIER) $(libdir)/Demo/Fonts
	for f in *.pcf.Z fonts.* ; do \
	  $(INSTALL) -m 744 $$f $(libdir)/Demo/Fonts; \
	done

dist:
	test -d $(distdir)/Demo/Fonts || $(MKDIRHIER) $(distdir)/Demo/Fonts
	$(CP) *.pcf.Z fonts.* Makefile $(distdir)/Demo/Fonts
