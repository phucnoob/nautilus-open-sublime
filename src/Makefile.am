# Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
	"-DPACKAGE_LOCALE_DIR=\"$(localedir)\"" \
	"-DPACKAGE_SRC_DIR=\"$(srcdir)\"" \
	"-DPACKAGE_DATA_DIR=\"$(pkgdatadir)\"" \
	"-DG_LOG_DOMAIN=\"NautilusOpenSublime\""

AM_CFLAGS = \
	-Wall\
	-g \
	-pedantic \
	$(WARN_CFLAGS) \
	$(NAUTILUS_CFLAGS) \
	$(GLIB_CFLAGS) \
	$(NAUTILUS_OPEN_SUBLIME_CFLAGS)

nautilus_extensiondir = $(NAUTILUS_EXTENSION_DIR)

nautilus_extension_LTLIBRARIES = \
	libnautilus-open-sublime.la

libnautilus_open_sublime_la_SOURCES = \
	nautilus-open-sublime.c

libnautilus_open_sublime_la_LDFLAGS = \
	-module \
	-avoid-version

libnautilus_open_sublime_la_LIBADD = \
	$(NAUTILUS_OPEN_SUBLIME_LIBS)

# EOF

