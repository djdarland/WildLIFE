# Makefile made manually from studying Mkefile.in - did not use configure
# There is much obsolete - It should be cleaned up. Don't have time yet. 
SHELL		= /bin/sh
CC		= gcc
CFLAGS		= -g -O
# CPPFLAGS	= 
# LDFLAGS		= 
LIBS		=  -lm
# DEFS		=  -DNEEDXLIBINT=1 -DGARBCOL=1 -DALLOC_WORDS=20971520 -DUNIF_DEBUG=1 -DNORAW=1 -DREV401PLUS=1  
# DEFS		=  -DX11=1 -DNEEDXLIBINT=1 -DGARBCOL=1 -DALLOC_WORDS=20971520 -DUNIF_DEBUG=1 -DNORAW=1 -DREV401PLUS=1
DEFS_EXT_MINT 		= -DREV401PLUS=1 -DMINT -DEXTERN=extern -DGARBCOL=1 -DALLOC_WORDS=20971520 -DUNIF_DEBUG=1 -DNORAW=1 
DEFS_EXT_MINT_X 		= -DREV401PLUS=1 -DMINT -DEXTERN=extern -DGARBCOL=1 -DALLOC_WORDS=20971520 -DUNIF_DEBUG=1 -DNORAW=1 -DX11=1 -DNEEDXLIBINT=1 
DEFS_MAIN_MINT 		= -DREV401PLUS=1  -DMINT -DGARBCOL=1 -DALLOC_WORDS=20971520 -DUNIF_DEBUG=1 -DNORAW=1
DEFS_MAIN_MINT_X 		= -DREV401PLUS=1  -DMINT -DGARBCOL=1 -DALLOC_WORDS=20971520 -DUNIF_DEBUG=1 -DNORAW=1 -DX11=1 -DNEEDXLIBINT=1 
# RANLIB		= ranlib
# INSTALL		= /usr/bin/install -c
# MKDIRHIER	= mkdirhier
prefix		= ~/life_local
exec_prefix	= ${prefix}
bindir		= ${exec_prefix}/bin
libdir		= ${prefix}/lib/life
srcdir		= .
# X_CFLAGS	= 
# X_LIBS		=  -L/usr/lib64
# X_EXTRA_LIBS	= 
# X_PRE_LIBS	=  -lSM -lICE
# X_PRE_LIBS      =
# XLIBS		= ${X_PRE_LIBS} ${X_LIBS} -lX11 ${X_EXTRA_LIBS}
# XLIBRARIES	= ${XLIBS}
# MKDEP		= makedepend
SETUPDIR	= ${libdir}
SETUP		= ${prefix}/Source/.set_up
# INCLUDE_PATHS	= 
# XSOURCE		= ${XSRC}
# XOBJECT		= ${XOBJ}
# XINCLUDE	= ${XINC}
# DBMSOURCE	= 
# DBMOBJECT	= 

#=====================================================================
# For Linux:
# The following variable should contain the pathname of the GCC include
# directory.  Run 'gcc -v' to find where gcc's directories are, then edit
# the sample pathname below to be correct.
#INCLUDE_PATHS	= -I/usr/lib/gcc-lib/i[34]86-linux/<version>/include
#
# For Linux (continued):
# This is an example of a definition that works on a Gateway 2000 machine:
#INCLUDE_PATHS = -I/usr/lib/gcc-lib/i486-linux/2.4.5/include -I/usr/include/bsd
#
# For Solaris
#INCLUDE_PATHS = -I/usr/openwin/include
#=====================================================================

#=====================================================================
# -DNORAW
# Add this if the low-level (unbuffered) terminal input file (raw.c) won't
# compile on your system.  On SunOS for some reason this file will include some
# library files twice. This does not seem to make any difference.
#
# -DNEED_TRUNC
# If defined, adds a definition of the 'trunc' function.
# 
# -DX11
# If defined, will cause the X interface to be linked in.  If it is not
# defined then the system will contain no X interface.  -lX11 should be
# removed from LOADFLAGS in that case (see below).
#
# -DGARBCOL
# If defined will cause the garbage collector to be called when memory is low.
#
# -DUNIF_DEBUG
# If defined causes the 'debug' mode to show matching and unification sub-goals.
#=====================================================================

# COMPILE.c	= $(CC) $(CFLAGS) $(CPPFLAGS) $(DEFS) -c
# COMPILE_EXT_MINT.c	= $(CC) $(CFLAGS) $(CPPFLAGS) $(DEFS_EXT_MINT) -c
# COMPILE_MAIN_MINT.c	= $(CC) $(CFLAGS) $(CPPFLAGS) $(DEFS_MAIN_MINT) -c
# XCOMPILE.c	= $(CC) $(CFLAGS) $(X_CFLAGS) $(CPPFLAGS) $(DEFS) -c

#=====================================================================
# MEMORY is the number of words that the system will allocate.
# Because of the half-space GC, Wild-LIFE will actually allocate twice that
# many words. So on a 32-bit system, multiply by 8 to obtain the real memory
# usage in bytes.  The default size results in about 17MB virtual memory size.
# If this is too big for your system then redefine MEMORY as suggested below.
#
# The memory size given by MEMORY is overridden by
# the command line argument: "-memoryNNNNNNNN"
#
# Normal version (totals about 17 MB virtual)
#MEMORY = 2097152
#
# Smaller versions--take your pick.
# The smallest totals about 5 MB virtual.
# Under Linux, one of the smaller versions may run better--it depends on
# how much RAM and swap space your machine has.
#MEMORY = 1048576
#MEMORY = 524288
#=====================================================================

XSRC = xpred.c xdisplaylist.c
XOBJ = xpred.o xdisplaylist.o
XINC = xpred.h xdisplaylist.h

LIBSOURCE = \
	arity.c bi_math.c bi_sys.c bi_type.c built_ins.c copy.c error.c \
	hash_table.c info.c interrupt.c lefun.c lib.c list.c login.c \
	lub.c memory.c modules.c parser.c print.c raw.c sys.c templates.c \
	token.c trees.c types.c $(XSOURCE) $(DBMSOURCE)

LIBOBJECT_X = \
	arity_x.o bi_math_x.o bi_sys_x.o bi_type_x.o built_ins_x.o copy_x.o \
	error_x.o hash_table_x.o info2_x.o interrupt_x.o lefun_x.o \
	lib2_x.o list_x.o login_x.o lub_x.o memory_x.o modules_x.o parser_x.o \
	print_x.o raw_x.o sys_x.o templates_x.o \
	token_x.o trees_x.o types_x.o xpred.o xdisplaylist.o

LIBOBJECT = \
	arity.o bi_math.o bi_sys.o bi_type.o built_ins.o copy.o error.o \
	hash_table.o info2.o interrupt.o lefun.o lib2.o list.o login.o \
	lub.o memory.o modules.o parser.o print.o raw.o sys.o templates.o \
	token.o trees.o types.o

# $(XOBJECT) $(DBMOBJECT)

LIBINCLUDE = def_const.h def_glob.h def_macro.h def_proto.h \
	defs.h def_struct.h


REINCLUDE = regexp.h

PROGSOURCELIFE = built_ins.lf term_expansion.lf onlinedoc.lf

MANPAGE = $(PROGRAM).1

# Please don't change this so we can tell which release you are using.
# DATE        = Fri Jul  7 13:57:49 PDT 1995
# VERSION     = 1.02
# Saved above to use above for test suite
DATE = `date`
VERSION     = 4.20

# $(LIBOBJECT) regexp/regexp.o regexp/regsub.o

all: wild_life wild_life_X
	@echo
	@echo "Life generation done"
	@echo

Demo:
	cd ../Demo; $(MAKE) SETUPDIR="$(SETUPDIR)"

life2.o : life2.c $(LIBINCLUDE)
	$(CC) life2.c -o life2.o $(CFLAGS) $(CPPFLAGS) $(DEFS_MAIN_MINT) -c

arity.o : arity.c $(LIBINCLUDE)
	$(CC) arity.c -o arity.o $(CFLAGS) $(CPPFLAGS) $(DEFS_EXT_MINT) -c

bi_math.o : bi_math.c  $(LIBINCLUDE)
	$(CC) bi_math.c -o bi_math.o $(CFLAGS) $(CPPFLAGS) $(DEFS_EXT_MINT) -c

bi_sys.o : bi_sys.c  $(LIBINCLUDE)
	$(CC) bi_sys.c -o bi_sys.o $(CFLAGS) $(CPPFLAGS) $(DEFS_EXT_MINT) -c

bi_type.o : bi_type.c  $(LIBINCLUDE)
	$(CC) bi_type.c -o bi_type.o $(CFLAGS) $(CPPFLAGS) $(DEFS_EXT_MINT) -c

built_ins.o : built_ins.c  $(LIBINCLUDE)
	$(CC) built_ins.c -o built_ins.o $(CFLAGS) $(CPPFLAGS) $(DEFS_EXT_MINT) -c

copy.o : copy.c  $(LIBINCLUDE)
	$(CC) copy.c -o copy.o $(CFLAGS) $(CPPFLAGS) $(DEFS_EXT_MINT) -c

error.o : error.c  $(LIBINCLUDE)
	$(CC) error.c -o error.o $(CFLAGS) $(CPPFLAGS) $(DEFS_EXT_MINT) -c

hash_table.o : hash_table.c  $(LIBINCLUDE)
	$(CC) hash_table.c -o hash_table.o $(CFLAGS) $(CPPFLAGS) $(DEFS_EXT_MINT) -c

info2.o : info2.c  $(LIBINCLUDE)
	$(CC) info2.c -o info2.o $(CFLAGS) $(CPPFLAGS) $(DEFS_EXT_MINT) -c

interrupt.o : interrupt.c  $(LIBINCLUDE)
	$(CC) interrupt.c -o interrupt.o $(CFLAGS) $(CPPFLAGS) $(DEFS_EXT_MINT) -c

lefun.o : lefun.c  $(LIBINCLUDE)
	$(CC) lefun.c -o lefun.o $(CFLAGS) $(CPPFLAGS) $(DEFS_EXT_MINT) -c

lib2.o : lib2.c  $(LIBINCLUDE)
	$(CC) lib2.c -o lib2.o $(CFLAGS) $(CPPFLAGS) $(DEFS_EXT_MINT) -c

list.o : list.c  $(LIBINCLUDE)
	$(CC) list.c -o list.o $(CFLAGS) $(CPPFLAGS) $(DEFS_EXT_MINT) -c

login.o : login.c  $(LIBINCLUDE)
	$(CC) login.c -o login.o $(CFLAGS) $(CPPFLAGS) $(DEFS_EXT_MINT) -c

lub.o : lub.c  $(LIBINCLUDE)
	$(CC) lub.c -o lub.o $(CFLAGS) $(CPPFLAGS) $(DEFS_EXT_MINT) -c

memory.o : memory.c  $(LIBINCLUDE)
	$(CC) memory.c -o memory.o $(CFLAGS) $(CPPFLAGS) $(DEFS_EXT_MINT) -c

modules.o : modules.c  $(LIBINCLUDE)
	$(CC) modules.c -o modules.o $(CFLAGS) $(CPPFLAGS) $(DEFS_EXT_MINT) -c

parser.o : parser.c  $(LIBINCLUDE)
	$(CC) parser.c -o parser.o $(CFLAGS) $(CPPFLAGS) $(DEFS_EXT_MINT) -c

print.o : print.c  $(LIBINCLUDE)
	$(CC) print.c -o print.o $(CFLAGS) $(CPPFLAGS) $(DEFS_EXT_MINT) -c

raw.o : raw.c  $(LIBINCLUDE)
	$(CC) raw.c -o raw.o $(CFLAGS) $(CPPFLAGS) $(DEFS_EXT_MINT) -c

sys.o : sys.c  $(LIBINCLUDE)
	$(CC) sys.c -o sys.o $(CFLAGS) $(CPPFLAGS) $(DEFS_EXT_MINT) -c

templates.o : templates.c  $(LIBINCLUDE)
	$(CC) templates.c -o templates.o $(CFLAGS) $(CPPFLAGS) $(DEFS_EXT_MINT) -c

token.o : token.c  $(LIBINCLUDE)
	$(CC) token.c -o token.o $(CFLAGS) $(CPPFLAGS) $(DEFS_EXT_MINT) -c

trees.o : trees.c  $(LIBINCLUDE)
	$(CC) trees.c -o trees.o $(CFLAGS) $(CPPFLAGS) $(DEFS_EXT_MINT) -c

types.o : types.c  $(LIBINCLUDE)
	$(CC) types.c -o types.o $(CFLAGS) $(CPPFLAGS) $(DEFS_EXT_MINT) -c

# End regular Life

# Start X Life

life2_x.o : life2.c $(LIBINCLUDE)
	echo $(CC) life2.c -o life2_x.o $(CFLAGS) $(CPPFLAGS) $(DEFS_MAIN_MINT_X) -c
	$(CC) life2.c -o life2_x.o $(CFLAGS) $(CPPFLAGS) $(DEFS_MAIN_MINT_X) -c

arity_x.o : arity.c $(LIBINCLUDE)
	$(CC) arity.c -o arity_x.o $(CFLAGS) $(CPPFLAGS) $(DEFS_EXT_MINT_X) -c

bi_math_x.o : bi_math.c  $(LIBINCLUDE)
	$(CC) bi_math.c -o bi_math_x.o $(CFLAGS) $(CPPFLAGS) $(DEFS_EXT_MINT_X) -c

bi_sys_x.o : bi_sys.c  $(LIBINCLUDE)
	$(CC) bi_sys.c -o bi_sys_x.o $(CFLAGS) $(CPPFLAGS) $(DEFS_EXT_MINT_X) -c

bi_type_x.o : bi_type.c  $(LIBINCLUDE)
	$(CC) bi_type.c -o bi_type_x.o $(CFLAGS) $(CPPFLAGS) $(DEFS_EXT_MINT_X) -c

built_ins_x.o : built_ins.c  $(LIBINCLUDE)
	$(CC) built_ins.c -o built_ins_x.o $(CFLAGS) $(CPPFLAGS) $(DEFS_EXT_MINT_X) -c

copy_x.o : copy.c  $(LIBINCLUDE)
	$(CC) copy.c -o copy_x.o $(CFLAGS) $(CPPFLAGS) $(DEFS_EXT_MINT_X) -c

error_x.o : error.c  $(LIBINCLUDE)
	$(CC) error.c -o error_x.o $(CFLAGS) $(CPPFLAGS) $(DEFS_EXT_MINT_X) -c

hash_table_x.o : hash_table.c  $(LIBINCLUDE)
	$(CC) hash_table.c -o hash_table_x.o $(CFLAGS) $(CPPFLAGS) $(DEFS_EXT_MINT_X) -c

info2_x.o : info2.c  $(LIBINCLUDE)
	$(CC) info2.c -o info2_x.o $(CFLAGS) $(CPPFLAGS) $(DEFS_EXT_MINT_X) -c

interrupt_x.o : interrupt.c  $(LIBINCLUDE)
	$(CC) interrupt.c -o interrupt_x.o $(CFLAGS) $(CPPFLAGS) $(DEFS_EXT_MINT_X) -c

lefun_x.o : lefun.c  $(LIBINCLUDE)
	$(CC) lefun.c -o lefun_x.o $(CFLAGS) $(CPPFLAGS) $(DEFS_EXT_MINT_X) -c

lib2_x.o : lib2.c  $(LIBINCLUDE)
	$(CC) lib2.c -o lib2_x.o $(CFLAGS) $(CPPFLAGS) $(DEFS_EXT_MINT_X) -c

list_x.o : list.c  $(LIBINCLUDE)
	$(CC) list.c -o list_x.o $(CFLAGS) $(CPPFLAGS) $(DEFS_EXT_MINT_X) -c

login_x.o : login.c  $(LIBINCLUDE)
	$(CC) login.c -o login_x.o $(CFLAGS) $(CPPFLAGS) $(DEFS_EXT_MINT_X) -c

lub_x.o : lub.c  $(LIBINCLUDE)
	$(CC) lub.c -o lub_x.o $(CFLAGS) $(CPPFLAGS) $(DEFS_EXT_MINT_X) -c

memory_x.o : memory.c  $(LIBINCLUDE)
	$(CC) memory.c -o memory_x.o $(CFLAGS) $(CPPFLAGS) $(DEFS_EXT_MINT_X) -c

modules_x.o : modules.c  $(LIBINCLUDE)
	$(CC) modules.c -o modules_x.o $(CFLAGS) $(CPPFLAGS) $(DEFS_EXT_MINT_X) -c

parser_x.o : parser.c  $(LIBINCLUDE)
	$(CC) parser.c -o parser_x.o $(CFLAGS) $(CPPFLAGS) $(DEFS_EXT_MINT_X) -c

print_x.o : print.c  $(LIBINCLUDE)
	$(CC) print.c -o print_x.o $(CFLAGS) $(CPPFLAGS) $(DEFS_EXT_MINT_X) -c

raw_x.o : raw.c  $(LIBINCLUDE)
	$(CC) raw.c -o raw_x.o $(CFLAGS) $(CPPFLAGS) $(DEFS_EXT_MINT_X) -c

sys_x.o : sys.c  $(LIBINCLUDE)
	$(CC) sys.c -o sys_x.o $(CFLAGS) $(CPPFLAGS) $(DEFS_EXT_MINT_X) -c

templates_x.o : templates.c  $(LIBINCLUDE)
	$(CC) templates.c -o templates_x.o $(CFLAGS) $(CPPFLAGS) $(DEFS_EXT_MINT_X) -c

token_x.o : token.c  $(LIBINCLUDE)
	$(CC) token.c -o token_x.o $(CFLAGS) $(CPPFLAGS) $(DEFS_EXT_MINT_X) -c

trees_x.o : trees.c  $(LIBINCLUDE)
	$(CC) trees.c -o trees_x.o $(CFLAGS) $(CPPFLAGS) $(DEFS_EXT_MINT_X) -c

types_x.o : types.c  $(LIBINCLUDE)
	$(CC) types.c -o types_x.o $(CFLAGS) $(CPPFLAGS) $(DEFS_EXT_MINT_X) -c

xpred.o : xpred.c  $(LIBINCLUDE) $(XINC)
	$(CC) xpred.c -o xpred.o $(CFLAGS) $(CPPFLAGS) $(DEFS_EXT_MINT_X) -c

xdisplaylist.o : xdisplaylist.c  $(LIBINCLUDE) $(XINC)
	$(CC) xdisplaylist.c -o xdisplaylist.o $(CFLAGS) $(CPPFLAGS) $(DEFS_EXT_MINT_X) -c




regexp/regexp.o regexp/regsub.o: RegExp
	@echo "Regular Expression Package: $@ done"
RegExp:
	cd regexp; $(MAKE) r CC="$(CC)" CFLAGS="$(CFLAGS)"

wild_life: life2.o $(LIBOBJECT) regexp/regexp.o regexp/regsub.o
	$(CC) -o wild_life life2.o $(LIBOBJECT) $(LIBS) regexp/regexp.o regexp/regsub.o

wild_life_X: life2_x.o $(LIBOBJECT_X) regexp/regexp.o regexp/regsub.o
	$(CC) -o wild_life_X life2_x.o $(LIBOBJECT_X) $(LIBS)  $(XLIBRARIES) regexp/regexp.o regexp/regsub.o -lX11

# $(PROGRAM): life2.o $(LIBOBJECT) regexp/regexp.o regexp/regsub.o
#	$(CC) -o $(PROGRAM) life2.o $(LDFLAGS) $(LIBS) $(XLIBRARIES) $(LIBOBJECT) regexp/regexp.o regexp/regsub.o

.set_up: .set_up_basis
	sed -e "s%+SETUPDIR+%$(SETUPDIR)%g" < .set_up_basis > .set_up

clean:
	-rm *~ *.o.set_up life2.c lib2.c $(PROGRAM)
	cd regexp; $(MAKE) clean

veryclean: clean
	-rm *.cache *.status Makefile
	cd regexp; $(MAKE) veryclean

life2.c: life.c Makefile
	sed -e "s%+SETUP+%$(SETUP)%" < life.c > life2.c

lib2.c:	lib.c Makefile
	sed -e "s%+SETUP+%$(SETUP)%" < lib.c > lib2.c

info2.c: info.c  Makefile
	@echo Don\'t be surprised, info is compiled "for ${DATE} because" something has changed
	sed -e "s/+DATE+/$(DATE)/" \
	    -e "s/+VERSION+/$(VERSION)/" < info.c > info2.c

# xpred.o: xpred.c
# 	$(XCOMPILE.c) -o $@ $<

# xdisplaylist.o: xdisplaylist.c
#	$(XCOMPILE.c) -o $@ $<

MKINSTALL = $(MAKE) MKDIRHIER="$(MKDIRHIER)"\
 libdir="$(libdir)" INSTALL="$(INSTALL)" srcdir="$(srcdir)" install

install: $(PROGRAM) .set_up
	test -d $(bindir)          || $(MKDIRHIER) $(bindir)
	$(INSTALL) -m 755 $(PROGRAM) $(bindir)/$(PROGRAM)
	test -d $(libdir)          || $(MKDIRHIER) $(libdir)
	test -d $(libdir)/Source   || $(MKDIRHIER) $(libdir)/Source
	$(INSTALL) -m 744 .set_up           $(libdir)/Source
	$(INSTALL) -m 744 built_ins.lf      $(libdir)/Source
	$(INSTALL) -m 744 onlinedoc.lf      $(libdir)/Source
	$(INSTALL) -m 744 term_expansion.lf $(libdir)/Source
	cd $(srcdir)/../Lib;      $(MKINSTALL)
	cd $(srcdir)/../Tools;    $(MKINSTALL)
	cd $(srcdir)/../Examples; $(MKINSTALL)
	cd $(srcdir)/../Doc;      $(MKINSTALL)
	cd $(srcdir)/../Demo;     $(MKINSTALL)

#=====================================================================
# Dependencies

depend:
	$(MKDEP) $(CPPFLAGS) $(DEFS) $(INCLUDE_PATHS) $(PROGSOURCE)
