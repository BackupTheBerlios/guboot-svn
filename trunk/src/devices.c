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


void mount_added (GVolumeMonitor *monitor, GMount *mount, gpointer data)
{
	gchar *name;
	gchar *uuid;
	
	name = g_mount_get_name (mount);
	uuid = g_mount_get_uuid (mount);
	
	g_debug ("Mount '%s' added!", name);
	gui_device_insert (name, uuid);
}


void mount_removed (GVolumeMonitor *monitor, GMount *mount, gpointer data)
{
	gchar *name;
	gchar *uuid;
	
	name = g_mount_get_name (mount);
	uuid = g_mount_get_uuid (mount);
	
	g_debug ("Mount '%s' removed!", name);
	//gui_device_remove (uuid);
}


// Geräte initialisieren
void devices_init(void)
{
	monitor = g_volume_monitor_get ();
	
	g_signal_connect (monitor, "mount-added", G_CALLBACK(mount_added), NULL);
	g_signal_connect (monitor, "mount-removed", G_CALLBACK(mount_removed), NULL);
}


// Füge die Geräte dem GUI hinzu
void devices_fill_gui (void)
{
	GList *l, *mounts;
	GMount *mount;
	//GDrive *drive;
	//gchar *drive_name;
	gchar *mount_name;
	gchar *mount_uuid;
	//gchar *fs;

	mounts = g_volume_monitor_get_mounts (monitor);
	
	for (l = mounts; l != NULL; l = l->next) {
		mount = l->data;
		//if (gnome_vfs_volume_is_user_visible (volume)) {
			//if (!gnome_vfs_volume_is_read_only (volume)) {
				//fs = gnome_vfs_volume_get_filesystem_type (volume);
				//if (!strcmp (fs, "fat") || !strcmp (fs, "vfat")) {
					//drive = g_mount_get_drive (mount);
					//drive_name = g_drive_get_name  (drive);					
					mount_name = g_mount_get_name (mount);
					mount_uuid = g_mount_get_uuid (mount);
					g_debug ("Volume: %s (UUID: %s)", mount_name, mount_uuid);
					gui_device_insert (mount_name, mount_uuid);
				//}
			//}
		//}
		g_object_unref (mount);
	}
	
	g_list_free (mounts);
}



