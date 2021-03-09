cd DOXY
rm -r html
cd ..
rm info2.c
rm lib2.c
rm life2.c
doxygen
emacs DOXY.LOG.txt
