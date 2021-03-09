distname = Life-2.03REL
disttop = /home/dennis/src/lang
distdir = $(disttop)/$(distname)
MKDIRHIER = mkdir
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
#	cd EmacsMode;	$(MKDIST)
	cd Examples;	$(MKDIST)
	cd Lib;		$(MKDIST)
#	cp -r Source /home/dennis/src
#	cd Source;	$(MKDIST)
	cd Tests;	$(MKDIST)
	cd Tools;	$(MKDIST)
	cd Demo;	$(MKDIST)
	cd $(disttop); \
	$(TAR) cf $(distname).tar $(distname); \
	$(RM) -rf $(distname); \
	$(COMPRESS) $(distname).tar
