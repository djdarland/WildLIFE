distname = Life-1.02
disttop = /usr/local/Life
distdir = $(disttop)/$(distname)
MKDIRHIER = mkdirhier
MKDIST = $(MAKE) distdir="$(distdir)" MKDIRHIER="$(MKDIRHIER)" CP="$(CP)" dist
MKDIST2 = $(MAKE) -f Makedist distdir="$(distdir)" MKDIRHIER="$(MKDIRHIER)" CP="$(CP)" dist
DISTSOURCE = LICENSE Makefile README INSTALL
CP = cp
TAR = tar
COMPRESS = gzip

dist:
	if test -d $(distdir); then \
		echo "$(distdir) already exists"; \
	else \
		$(MKDIRHIER) $(distdir); \
	fi
	$(CP) $(DISTSOURCE) $(distdir)/
	cd CLife;	$(MKDIST)
	cd Doc;		$(MKDIST)
	cd EmacsMode;	$(MKDIST)
	cd Examples;	$(MKDIST)
	cd Lib;		$(MKDIST)
	cd Source;	$(MKDIST2)
	cd Tests;	$(MKDIST)
	cd Tools;	$(MKDIST)
	cd Demo;	$(MKDIST)
	cd $(disttop); \
	$(TAR) cf $(distname).tar $(distname); \
	$(RM) -rf $(distname); \
	$(COMPRESS) $(distname).tar
