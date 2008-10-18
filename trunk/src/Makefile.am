INCLUDES = $(GLIB_CFLAGS) \
		   $(GTK_CFLAGS) \
		   $(GLADE_CFLAGS) \
		   $(HAL_CFLAGS) \
		   $(DBUS_CFLAGS)

AM_CFLAGS = -Wall -g

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

guboot_LDADD = $(GLIB_LIBS) \
			   $(GTK_LIBS) \
			   $(GLADE_LIBS) \
			   $(HAL_LIBS) \
			   $(DBUS_LIBS)
