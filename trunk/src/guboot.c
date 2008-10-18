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

#include <stdlib.h>


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
	
	return TRUE;
}


// Extrahiert eine ISO Datei ins angegebene Verzeichnis
gboolean guboot_extract_iso (gchar *isofile, gchar *destination)
{
	gint ret;
	gchar *command;
	
	command = g_strdup_printf ("7z -bd -y -o\"%s\" x \"%s\"", destination, isofile);
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
