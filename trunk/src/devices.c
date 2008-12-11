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
	
	uuid = g_mount_get_uuid (mount);
	if (!uuid) {
		return;
	}
	name = g_mount_get_name (mount);
	
	g_debug ("mount_added uuid: %s / name: %s", uuid, name);
	gui_device_insert (uuid, name);
}


void mount_removed (GVolumeMonitor *monitor, GMount *mount, gpointer data)
{
	gchar *name;
	gchar *uuid;
	
	uuid = g_mount_get_uuid (mount);
	if (!uuid) {
		return;
	}
	name = g_mount_get_name (mount);
	
	g_debug ("mount_removed uuid: %s / name: %s", uuid, name);
	gui_device_remove (uuid);
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

	mounts = g_volume_monitor_get_mounts (monitor);
	
	for (l = mounts; l != NULL; l = l->next) {
		mount = l->data;
		mount_added (monitor, mount, NULL);
		g_object_unref (mount);
	}
	
	g_list_free (mounts);
}


// Gibt zB. /media/SuperTalent zurück
gchar* devices_get_mntpoint_for_uuid (gchar *uuid)
{
	GMount *mount;
	GFile *root;
	
	mount = g_volume_monitor_get_mount_for_uuid (monitor, uuid);
	root = g_mount_get_root (mount);
	
	return g_file_get_path (root);
}


// Gibt zB. /dev/sdf1 zurück
gchar* devices_get_partition_for_uuid (gchar *uuid)
{
	GMount *mount;
	GVolume *volume;
	
	mount = g_volume_monitor_get_mount_for_uuid (monitor, uuid);
	volume = g_mount_get_volume (mount);
	
	return g_volume_get_identifier (volume, G_VOLUME_IDENTIFIER_KIND_UNIX_DEVICE);
}
