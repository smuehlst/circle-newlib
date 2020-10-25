## Process this file with automake to generate Makefile.in

AUTOMAKE_OPTIONS = cygnus

INCLUDES = $(NEWLIB_CFLAGS) $(CROSS_CFLAGS) $(TARGET_CFLAGS)

AM_CCASFLAGS = $(INCLUDES)

noinst_LIBRARIES = lib.a

# We only want to install dirent.h, but the build system
# seems to force the compilation of a module here
lib_a_SOURCES = dummy.c 
lib_a_CCASFLAGS = $(AM_CCASFLAGS)
lib_a_CFLAGS = $(AM_CFLAGS)

ACLOCAL_AMFLAGS = -I ../../.. -I ../../../..
CONFIG_STATUS_DEPENDENCIES = $(newlib_basedir)/configure.host

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(tooldir)/include/arpa; \
	for i in $(srcdir)/include/arpa/*.h; do \
	  $(INSTALL_DATA) $$i $(DESTDIR)$(tooldir)/include/arpa/`basename $$i`; \
	done; \
	$(mkinstalldirs) $(DESTDIR)$(tooldir)/include/net; \
	for i in $(srcdir)/include/net/*.h; do \
	  $(INSTALL_DATA) $$i $(DESTDIR)$(tooldir)/include/net/`basename $$i`; \
	done; \
	$(mkinstalldirs) $(DESTDIR)$(tooldir)/include/netinet; \
	for i in $(srcdir)/include/netinet/*.h; do \
	  $(INSTALL_DATA) $$i $(DESTDIR)$(tooldir)/include/netinet/`basename $$i`; \
	done;

# LATER IPv6
#	$(mkinstalldirs) $(DESTDIR)$(tooldir)/include/netinet6; \
#	for i in $(srcdir)/include/netinet6/*.h; do \
#	  $(INSTALL_DATA) $$i $(DESTDIR)$(tooldir)/include/netinet6/`basename $$i`; \
#	done
