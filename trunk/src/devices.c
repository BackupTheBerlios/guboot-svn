/*
 *      devices.c
 *      
 *      Copyright 2008 Patrik Obrist <padx@gmx.net>
 *      
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *      
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *      
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */

#include <dbus/dbus-glib.h>
#include <libhal.h>
#include <libhal-storage.h>
#include <string.h>

#include "devices.h"
#include "gui.h"

DBusGConnection *conn;
DBusError err;
LibHalContext *ctx;
LibHalDrive *drive;


// Geräte initialisieren
void devices_init(void)
{	
	dbus_error_init (&err);	
	if ((ctx = libhal_ctx_new ()) == NULL) {
		g_error ("Error 1");
	}
	if (!libhal_ctx_set_dbus_connection (ctx, dbus_bus_get (DBUS_BUS_SYSTEM, &err))) {
		g_error ("Error 2");
	}
	if (!libhal_ctx_init (ctx, &err)) {
		g_error ("Error 3");
	}
}


// Füge die Geräte dem GUI hinzu
void devices_fill_gui (void)
{
	gint num_udis;
	char **udis;
	gchar *udi, *storage, *mntpoint, *partition;
	LibHalPropertySet *ps;
	
	udis = libhal_find_device_by_capability (ctx, "volume", &num_udis, &err);
	if (dbus_error_is_set (&err)) {
		g_error ("DBus error!");
	}

	//g_debug ("found %d possible devices", num_udis);
	
	gint i;
	gint count = 0;
	gchar *name;
	for (i = 0; i < num_udis; i++) {
		udi = g_strdup (udis[i]);
		//g_debug ("device %i udi: %s", i+1, udi);
		
		storage = libhal_device_get_property_string (ctx, udi, "block.storage_device", &err);
		//g_debug ("storage: %s", storage);
		
		ps = libhal_device_get_all_properties (ctx, storage, &err);
		if (!libhal_ps_get_bool (ps, "storage.removable.media_available")) {
			//g_debug ("Kriterium 2");
			continue;
		}
		if (libhal_ps_get_bool (ps, "block.is_volume")) {
			//g_debug ("Kriterium 3");
			continue;
		}
		if (strcmp(libhal_ps_get_string (ps, "storage.bus"), "usb")) {
			//g_debug ("Kriterium 4");
			continue;
		}		
		if (libhal_ps_get_bool (ps, "volume.is_mounted_read_only")) {
			//g_debug ("Kriterium 5");
			continue;
		}
		
		name = g_strdup_printf ("%s %s", libhal_ps_get_string (ps, "storage.vendor"),
										 libhal_ps_get_string (ps, "info.product"));						
		mntpoint = libhal_device_get_property_string (ctx, udi, "volume.mount_point", &err);
		partition = libhal_device_get_property_string (ctx, udi, "block.device", &err);
		
		gui_device_insert (name, mntpoint, partition);
		
		g_free (name);
		
		count++;
	}
	
	if (count == 0) {
		gui_device_empty_list ();
	}
	
	libhal_free_string_array (udis);
}
