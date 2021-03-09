LIFE_LIB = ../Source/c_life.a
LDFLAGS = -lm -lX11
CFLAGS = -O2
DISTSOURCE = Makefile c_life.doc demo.c

demo: demo.c $(LIFE_LIB)
	cc $(CFLAGS) -o demo demo.c $(LIFE_LIB) $(LDFLAGS)

dist:
	test -d $(distdir)/CLife || $(MKDIRHIER) $(distdir)/CLife
	-$(CP) $(DISTSOURCE) $(distdir)/CLife
