/*
 *      gui.h
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

#ifndef GUI_H
#define GUI_H

// Includes
#include <gtk/gtk.h>

// Functions
void gui_init (int argc, char *argv[]);

gchar* gui_image_get_file (void);

void gui_device_insert (const gchar *uuid, const gchar *name);
void gui_device_remove (const gchar *uuid);
void gui_device_empty_list (void);

gchar* gui_device_get_name (void);
gchar* gui_device_get_mntpoint (void);
gchar* gui_device_get_partition (void);

gboolean gui_delete_checked (void);

void gui_msg_dialog (gchar *msg, GtkMessageType type);


#endif
