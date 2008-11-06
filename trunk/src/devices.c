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

#include <string.h>
#include <gio/gio.h>

#include "devices.h"
#include "gui.h"

GVolumeMonitor *monitor;

#define MOUNT_ADD    1
#define MOUNT_REMOVE 2


void mount_change (GVolumeMonitor *monitor, GMount *mount, gpointer data)
{
	gchar *name;
	gchar *uuid;
	
	name = g_mount_get_name (mount);
	uuid = g_mount_get_uuid (mount);
	
	if ((gint)data == MOUNT_ADD) {
		g_debug ("%s added!", name);
		gui_device_insert (uuid, name);
	} else {
		g_debug ("%s removed!", name);
		gui_device_remove (uuid);
	}
}


// Geräte initialisieren
void devices_init(void)
{
	monitor = g_volume_monitor_get ();
	
	g_signal_connect (monitor, "mount-added", G_CALLBACK(mount_change), (gpointer)MOUNT_ADD);
	g_signal_connect (monitor, "mount-removed", G_CALLBACK(mount_change), (gpointer)MOUNT_REMOVE);
}


// Füge die Geräte dem GUI hinzu
void devices_fill_gui (void)
{
	GList *l, *mounts;
	GMount *mount;

	mounts = g_volume_monitor_get_mounts (monitor);
	
	for (l = mounts; l != NULL; l = l->next) {
		mount = l->data;
		mount_change (monitor, mount, (gpointer)MOUNT_ADD);
		g_object_unref (mount);
	}
	
	g_list_free (mounts);
}



