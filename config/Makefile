
all: ../configure ac.h.in

ac.h.in: configure.in acconfig.h
	rm -f ac.h.in
	autoheader configure.in >ac.h.in

../configure: configure.in aclocal.m4
	rm -f ../configure
	autoconf configure.in >../configure
	chmod a+x ../configure

clean:

distclean:
	-rm -f ../config.status ../config.cache ../config.log mc.pl ac.pl sc.pl ac.h ach2acpl

realclean: distclean
	-rm -f ../configure ac.h.in

