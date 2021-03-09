all: life_demo.lf check_fp INSTALL

life_demo.lf: life_demo.lf.in
	sed -e "s%+SETUPDIR+%$(SETUPDIR)%g" < life_demo.lf.in > life_demo.lf

tutorial.lf: tutorial.lf.in
	sed -e "s%+SETUPDIR+%$(SETUPDIR)%g" < tutorial.lf.in > tutorial.lf

check_fp: check_fp.in
	sed -e "s%+SETUPDIR+%$(SETUPDIR)%g" < check_fp.in > check_fp

INSTALL: INSTALL.in
	sed -e "s%+SETUPDIR+%$(SETUPDIR)%g" < INSTALL.in > INSTALL

MKINSTALL = $(MAKE) MKDIRHIER="$(MKDIRHIER)"\
 libdir="$(libdir)" INSTALL="$(INSTALL)" srcdir="$(srcdir)" install

install:
	test -d $(libdir)/Demo || $(MKDIRHIER) $(libdir)/Demo
	for f in *.lf *.ps; do \
		$(INSTALL) -m 644 $$f $(libdir)/Demo; \
	done
	$(INSTALL) -m 755 INSTALL $(libdir)/Demo
	$(INSTALL) -m 755 check_fp $(libdir)/Demo
	cd Fonts; $(MKINSTALL)
	cd Sounds; $(MKINSTALL)

MKDIST = $(MAKE) distdir="$(distdir)" MKDIRHIER="$(MKDIRHIER)" CP="$(CP)" dist

DEMOFILES = Makefile check_fp.in color_edit.lf crypt.lf life_demo.lf.in \
slides.ps tutorial.lf.in x_mathwiz.lf x_schedule.lf x_zebra.lf \
xtools_utils.lf INSTALL.in

dist:
	test -d $(distdir)/Demo || $(MKDIRHIER) $(distdir)/Demo
	$(CP) $(DEMOFILES) $(distdir)/Demo/
	cd Fonts; $(MKDIST)
	cd Sounds; $(MKDIST)
