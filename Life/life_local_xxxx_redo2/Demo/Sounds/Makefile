install:
	test -d $(libdir)/Demo/Sounds || $(MKDIRHIER) $(libdir)/Demo/Sounds
	for f in *.au ; do \
	  $(INSTALL) -m 744 $$f $(libdir)/Demo/Sounds; \
	done

dist:
	test -d $(distdir)/Demo/Sounds || $(MKDIRHIER) $(distdir)/Demo/Sounds
	$(CP) *.au Makefile $(distdir)/Demo/Sounds
