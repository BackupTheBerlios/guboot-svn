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
#include <libgnomevfs/gnome-vfs.h>

#include "devices.h"
#include "gui.h"

GnomeVFSVolumeMonitor *monitor;


void volume_mounted (GnomeVFSVolumeMonitor *volume_monitor, GnomeVFSVolume *volume, gpointer user_data)
{
	gchar *volume_name;
	gulong volume_id;
	
	volume_name = gnome_vfs_volume_get_display_name (volume);
	volume_id = gnome_vfs_volume_get_id (volume);
	
	g_debug ("Volume '%s' mounted!", volume_name);
	gui_device_insert (volume_name, volume_id);
}


void volume_unmounted (GnomeVFSVolumeMonitor *volume_monitor, GnomeVFSVolume *volume, gpointer user_data)
{
	gchar *volume_name;
	gulong volume_id;
	
	volume_name = gnome_vfs_volume_get_display_name (volume);
	volume_id = gnome_vfs_volume_get_id (volume);
	
	g_debug ("Volume '%s' unmounted!", volume_name);
	gui_device_remove (volume_id);
}


// Geräte initialisieren
void devices_init(void)
{
	if (!gnome_vfs_initialized ()) {
		if (!gnome_vfs_init ()) {
			g_error ("Konnte GNOMEVFS nicht initialisieren!");
		}
	}
	
	monitor = gnome_vfs_get_volume_monitor ();
	
	g_signal_connect (monitor, "volume-mounted", G_CALLBACK(volume_mounted), NULL);
	g_signal_connect (monitor, "volume-unmounted", G_CALLBACK(volume_unmounted), NULL);
}


// Füge die Geräte dem GUI hinzu
void devices_fill_gui (void)
{
	GList *l, *volumes;
	GnomeVFSVolume *volume;
	GnomeVFSDrive *drive;
	gchar *volume_name;
	gchar *drive_name;
	gchar *fs;
	gulong volume_id;

	volumes = gnome_vfs_volume_monitor_get_mounted_volumes (monitor);
	
	for (l = volumes; l != NULL; l = l->next) {
		volume = l->data;
		if (gnome_vfs_volume_is_user_visible (volume)) {
			if (!gnome_vfs_volume_is_read_only (volume)) {
				fs = gnome_vfs_volume_get_filesystem_type (volume);
				if (!strcmp (fs, "fat") || !strcmp (fs, "vfat")) {
					drive = gnome_vfs_volume_get_drive (volume);
					drive_name = gnome_vfs_drive_get_display_name (drive);					
					volume_name = gnome_vfs_volume_get_display_name (volume);
					volume_id = gnome_vfs_volume_get_id (volume);
					gui_device_insert (volume_name, volume_id);
					g_debug ("Volume: %s (Filesystem: %s, Drive: %s)", volume_name, fs, drive_name);
				}
			}
		}
		gnome_vfs_volume_unref (volume);
	}
	
	g_list_free (volumes);
}



