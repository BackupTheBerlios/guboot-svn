INCLUDES = $(GTK_CFLAGS) \
		   $(GLADE_CFLAGS) \
		   $(GIO_CFLAGS)

AM_CFLAGS = -Wall -Werror -g

AM_CPPFLAGS = -DINSTALL_PREFIX=\"$(prefix)\" \
			  -DGUBOOT_DIR\=\"$(datadir)/guboot\" \
			  -DICON_DIR=\"$(datadir)/pixmaps\"

bin_PROGRAMS = guboot

guboot_SOURCES = main.c \
				 gui.c \
				 gui.h \
				 devices.c \
				 devices.h \
				 guboot.c \
				 guboot.h

guboot_LDFLAGS = -export-dynamic

guboot_LDADD = $(GTK_LIBS) \
			   $(GLADE_LIBS) \
			   $(GIO_LIBS) \
			   bk/libbk.la
