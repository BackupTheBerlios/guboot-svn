/*
 *      guboot.c
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

#include "guboot.h"
#include "gui.h"

/* need to include bk.h for access to bkisofs functions and structures */
#include <stdio.h>
#include "bk/bk.h"
#include <gio/gio.h>
#include <string.h>

#include <stdlib.h>
#include <gtk-2.0/gtk/gtkmessagedialog.h>


// Löscht die Dateien im übergebenen Verzeichnis
gboolean guboot_delete_files (gchar *path)
{
	gint ret;
	gchar *command;

	// Fehler abfangen, falls kein Pfad angegeben wurde
	if (!path) {
		gui_msg_dialog ("Falscher Pfad!", GTK_MESSAGE_WARNING);
		return FALSE;
	}
	
	command = g_strdup_printf ("rm -fr %s/*", path);
	g_debug ("Kommando: %s", command);
	ret = system (command);
	g_free (command);
	
	//g_debug ("Return Wert von system Aufruf: %i", ret);
	
	/*command = g_strdup_printf ("rm -f %s/.*", path);
	g_debug ("Kommando: %s", command);
	ret = system (command);
	g_free (command);
	
	g_debug ("Return Wert von system Aufruf: %i", ret);*/
	
	if (ret == 0) {
		return TRUE;
	} else {
		return FALSE;
	}
}


// Installiert den syslinux bootloader
gboolean guboot_install_bootloader (gchar *device)
{
	gint ret;
	gchar *command;
	
	command = g_strdup_printf ("gksudo \"syslinux %s\"", device);
	g_debug ("Kommando: %s", command);
	ret = system (command);
											 
	g_free (command);
											 
	//g_debug ("Return Wert von system Aufruf: %i", ret);
	
	if (ret == 0) {
		return TRUE;
	} else {
		return FALSE;
	}
}


// Modifiziere Syslinux Config Dateien
gboolean guboot_modify_files (gchar *mntpoint)
{
	gint ret;
	gchar *command;
	
	command = g_strdup_printf ("mv %s/isolinux.cfg %s/syslinux.cfg", mntpoint, mntpoint);
	g_debug ("Kommando: %s", command);
	ret = system (command);						 
	g_free (command);
	g_debug ("Return Wert von system Aufruf: %i", ret);									 
	
	command = g_strdup_printf ("mv %s/isolinux/isolinux.cfg %s/isolinux/syslinux.cfg", mntpoint, mntpoint);
	g_debug ("Kommando: %s", command);
	ret = system (command);						 
	g_free (command);
	g_debug ("Return Wert von system Aufruf: %i", ret);
	
	command = g_strdup_printf ("mv %s/isolinux %s/syslinux", mntpoint, mntpoint);
	g_debug ("Kommando: %s", command);
	ret = system (command);						 
	g_free (command);
	g_debug ("Return Wert von system Aufruf: %i", ret);
	
	command = g_strdup_printf ("mv %s/boot/isolinux/isolinux.cfg %s/boot/isolinux/syslinux.cfg", mntpoint, mntpoint);
	g_debug ("Kommando: %s", command);
	ret = system (command);						 
	g_free (command);
	g_debug ("Return Wert von system Aufruf: %i", ret);
	
	command = g_strdup_printf ("mv %s/boot/isolinux %s/boot/syslinux", mntpoint, mntpoint);
	g_debug ("Kommando: %s", command);
	ret = system (command);						 
	g_free (command);
	g_debug ("Return Wert von system Aufruf: %i", ret);
	
	return TRUE;
}


/* you can use this to update a progress bar or something */
void readProgressUpdaterCbk(VolInfo* volInfo)
{
    g_message ("Read progress updater");
	
	// Update UI
	while (gtk_events_pending()) {
		gtk_main_iteration();
	}
}


gboolean guboot_extract (VolInfo* volInfo, BkFileBase* base, gchar *dir)
{
    if (IS_DIR (base->posixFileMode)) {
		/* print name of the directory */
		//g_print ("%s (directory)\n", base->name);
		
		if (dir) {
			dir = g_strdup_printf ("%s/%s", dir, base->name);
			
			g_debug ("dir: %s", dir);
			
			// create destination directory
			gchar *destdir;
			destdir = g_strdup_printf ("%s%s", gui_device_get_mntpoint (), dir);
			g_debug ("destdir: %s", destdir);
			
			GFile *file;
			file = g_file_new_for_path (destdir);
			if (!g_file_make_directory (file, NULL, NULL)) {
				//return FALSE;
			}
			
		} else {
			dir = g_strdup ("");
			g_debug ("initial dir: %s", dir);
		}
		
		

		/* print all the directory's children */
		BkFileBase* child = BK_DIR_PTR (base)->children;
		while (child != NULL) {
			if (!guboot_extract (volInfo, child, dir)) {
				return FALSE;
			}
			child = child->next;
		}
    } else if (IS_REG_FILE (base->posixFileMode)) {
        // extract this file
        gint rc;
        gchar *input, *output;
        input = g_strdup_printf ("%s/%s", dir, base->name);
        output = g_strdup_printf ("%s%s", gui_device_get_mntpoint (), dir);
        g_debug ("extract: input: %s   output: %s", input, output);
        rc = bk_extract (volInfo, input, output, TRUE, readProgressUpdaterCbk);
		if (rc <= 0) {
			g_warning ("%s", bk_get_error_string (rc));
			return FALSE;
		}
    } else if (IS_SYMLINK (base->posixFileMode)) {
    	g_debug ("Hier Symlink behandeln!");
	}
    
    return TRUE;
}


// Extrahiert eine ISO Datei ins angegebene Verzeichnis
gboolean guboot_extract_iso (gchar *isofile, gchar *destination)
{
	VolInfo volInfo;
	gint rc;
    
    /* initialise volInfo, set it up to scan for duplicate files */
    rc = bk_init_vol_info (&volInfo, TRUE);
    if(rc <= 0) {
		g_warning ("%s", bk_get_error_string (rc));
		return FALSE;
	}
	
	/* open the iso file (supplied as argument 1) */
    rc = bk_open_image (&volInfo, isofile);
    if(rc <= 0) {
		g_warning ("%s", bk_get_error_string (rc));
		return FALSE;
	}
    
    /* read information about the volume (required before reading directory tree) */
    rc = bk_read_vol_info (&volInfo);
    if(rc <= 0) {
		g_warning ("%s", bk_get_error_string (rc));
		return FALSE;
	}
	
	/* read the directory tree */
    if (volInfo.filenameTypes & FNTYPE_ROCKRIDGE) {
        rc = bk_read_dir_tree (&volInfo, FNTYPE_ROCKRIDGE, true, readProgressUpdaterCbk);
    } else if (volInfo.filenameTypes & FNTYPE_JOLIET) {
        rc = bk_read_dir_tree (&volInfo, FNTYPE_JOLIET, false, readProgressUpdaterCbk);
    } else {
        rc = bk_read_dir_tree (&volInfo, FNTYPE_9660, false, readProgressUpdaterCbk);
    }
    if (rc <= 0) {
		g_warning (bk_get_error_string (rc));
		return FALSE;
	}
	
	/* extract the entire directory tree */
    if (!guboot_extract (&volInfo, BK_BASE_PTR (&(volInfo.dirTree)), NULL)) {
    	return FALSE;
	}
	
	/* we're finished with this ISO, so clean up */
    bk_destroy_vol_info (&volInfo);
	
	g_message ("Soweit alles ok!");
	
	return TRUE;
}


// Hier passiert die eigentliche Arbeit
gboolean guboot_create ()
{
	gchar *mntpoint;
	gchar *partition;
	gchar *iso;
	gboolean ok;
	
	mntpoint = gui_device_get_mntpoint ();
	iso = gui_image_get_file ();
	partition = gui_device_get_partition ();
	
	g_debug ("<-- START WORK -->");
	g_debug ("Input:  %s, Output: %s", iso, mntpoint);
	
	// Sollen Dateien zuerst gelöscht werden?
	if (gui_delete_checked ()) {
		guboot_delete_files (mntpoint);
	}
	
	ok = guboot_extract_iso (iso, mntpoint);
	if (!ok) {
		gui_msg_dialog ("Die Daten konnten nicht extrahiert werden!", GTK_MESSAGE_WARNING);
		return FALSE;
	}
	
	ok = guboot_modify_files (mntpoint);
	if (!ok) {
		gui_msg_dialog ("Die Syslinux Konfigurationsdateien konnten nicht angepasst werden!", GTK_MESSAGE_WARNING);
		return FALSE;
	}
	
	ok = guboot_install_bootloader (partition);
	if (!ok) {
		gui_msg_dialog ("Der Syslinux Bootloader konnte nicht installiert werden!", GTK_MESSAGE_WARNING);
		return FALSE;
	}
	
	g_debug ("<-- END WORK -->");
	
	return TRUE;
}
