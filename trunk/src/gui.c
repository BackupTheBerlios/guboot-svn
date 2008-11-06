/*
 *      gui.c
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

#include "gui.h"
#include "guboot.h"

#include <string.h>
#include <glade/glade.h>

#define INSTALLED_GLADE  GUBOOT_DIR"/guboot.glade"
#define INSTALLED_ICON   ICON_DIR"/guboot.png"


enum 
{
	COL_DEV_NAME,
	COL_DEV_ID,
	COLS_DEV
};


GladeXML *glade;
GtkWidget *window_main;
GtkButton *button_create;
GtkFileChooserButton *chooser;
GtkComboBox *combo_devs;
GtkCheckButton *check_delete;
GtkListStore *model_devs;
gint devices_count;


// Get Widget from UI
GtkWidget* gui_get_widget (gchar *name)
{
	GtkWidget *widget;
	widget = glade_xml_get_widget(glade, name);
	if (widget == NULL) {
		g_error ("Das Widget '%s' konnte nicht geholt werden!", name);
	}
	return widget;
}


// Enabled oder disabled den Startbutton
void gui_createbutton_set_active (gboolean active)
{
	if (active) {
		gtk_widget_set_state (GTK_WIDGET (button_create), GTK_STATE_NORMAL);
	} else {
		gtk_widget_set_state (GTK_WIDGET (button_create), GTK_STATE_INSENSITIVE);
	}
}


// Zeige einen Meldungsdialog
void gui_msg_dialog (gchar *msg, GtkMessageType type)
{
	GtkWidget *dlg;
	dlg = gtk_message_dialog_new (GTK_WINDOW (window_main), GTK_DIALOG_MODAL,
								  type, GTK_BUTTONS_OK, msg);
	gtk_dialog_run (GTK_DIALOG (dlg));
	gtk_widget_destroy (dlg);
}

// Das GUI initialisieren
void gui_init (int argc, char *argv[])
{
	// GTK initialisieren
	gtk_init(&argc, &argv);

	// Variablen initialisieren
	devices_count = 0;
	
	// Glade initialisieren
	glade_init();
	
	// UI Datei laden
	GString* gladefile = g_string_new (g_get_current_dir ());
	gladefile = g_string_append (gladefile, "/data/guboot.glade");
	if (g_file_test (gladefile->str, G_FILE_TEST_EXISTS) == FALSE) {
		gladefile = g_string_assign (gladefile, INSTALLED_GLADE);
		if (g_file_test (gladefile->str, G_FILE_TEST_EXISTS) == FALSE) {
			g_error ("Glade Datei ist nicht vorhanden!");
		}
	}
	glade = glade_xml_new (gladefile->str, NULL, NULL);
	if (glade == NULL) {
		g_error ("Fehler beim Laden der Glade Datei!");
	}
	
	// Widgets holen
	window_main = gui_get_widget ("window_main");
	combo_devs = GTK_COMBO_BOX (gui_get_widget ("combobox_devices"));
	chooser = GTK_FILE_CHOOSER_BUTTON (gui_get_widget ("chooser"));
	check_delete = GTK_CHECK_BUTTON (gui_get_widget ("checkbutton_delete"));
	button_create = GTK_BUTTON (gui_get_widget ("button_create"));

	// Icon setzen
	const gchar* icon = INSTALLED_ICON;
	if (g_file_test(icon, G_FILE_TEST_EXISTS) == TRUE) {
		gtk_window_set_icon_from_file (GTK_WINDOW (window_main), icon, NULL);
	}

	// Verbinde die Signale automatisch mit dem GUI
	glade_xml_signal_autoconnect(glade);

	// Filter für den Image Auswahldialog
	GtkFileFilter *filter;
	filter = gtk_file_filter_new ();
	gtk_file_filter_add_pattern (filter, "*.iso");
	gtk_file_filter_add_pattern (filter, "*.img");
	gtk_file_chooser_set_filter (GTK_FILE_CHOOSER (chooser), filter);
	
	// Falls eine Datei dem Programm übergeben wurde, diese verwenden
	if (argc > 1) {
		g_debug ("Übergebene Datei: %s", argv[1]);
		gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (chooser), argv[1]);
	}
	
	// Devices Combobox initialisieren
	model_devs = gtk_list_store_new (COLS_DEV, G_TYPE_STRING, G_TYPE_STRING);
	gtk_combo_box_set_model (GTK_COMBO_BOX(combo_devs), GTK_TREE_MODEL(model_devs));
}


// Allgemeiner Destroy-Event
void on_window_main_destroy(GtkWidget *widget, gpointer user_data)
{
	gtk_main_quit();
}


// Gibt das ausgewählte Image zurück
gchar* gui_image_get_file (void)
{
	return gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (chooser));
}


// Fügt ein Device in die Auswahlbox ein
void gui_device_insert (const gchar *name, const gchar *uuid)
{
	GtkTreeIter iter;
	
	if (name == NULL || !strcmp (name, "") || uuid == NULL || !strcmp (uuid, "")) {
		g_warning ("Daten sind nicht komplett!");
		return;
	}
		
	gtk_list_store_append (GTK_LIST_STORE (model_devs), &iter);
	gtk_list_store_set (GTK_LIST_STORE (model_devs), &iter, COL_DEV_NAME, name,
										   					COL_DEV_ID, uuid, -1);
	gtk_combo_box_set_active_iter (GTK_COMBO_BOX (combo_devs), &iter);
	
	devices_count = devices_count + 1;
}


// Entfernt ein Device aus der Auswahlbox
void gui_device_remove (gulong id)
{
	//GtkTreeIter iter;
	
	if (id == 0) {
		return;
	}
		
	/*gtk_list_store_append (GTK_LIST_STORE (model_devs), &iter);
	gtk_list_store_set (GTK_LIST_STORE (model_devs), &iter, COL_DEV_NAME, label,
										   					COL_DEV_ID, id, -1);
	gtk_combo_box_set_active_iter (GTK_COMBO_BOX (combo_devs), &iter);
	g_free (label);*/
	
	devices_count = devices_count - 1;
}


// Gibt den Namen (z.B. Sony Storage Media) des selektierten Device zurück
gchar* gui_device_get_name (void)
{
	gchar *name;
	GtkTreeIter iter;
	GtkListStore *model;
	
	model = (GtkListStore*)gtk_combo_box_get_model (GTK_COMBO_BOX(combo_devs));
	gtk_combo_box_get_active_iter (GTK_COMBO_BOX(combo_devs), &iter);	
	gtk_tree_model_get (GTK_TREE_MODEL(model), &iter, 0, &name, -1);
	
	return g_strdup (name);
}


// Gibt den Mountpunkt (z.B. /media/disk) des selektierten Device zurück
gchar* gui_device_get_mntpoint (void)
{
	/*gchar *mntpoint;
	GtkTreeIter iter;
	GtkListStore *model;
	
	model = (GtkListStore*)gtk_combo_box_get_model (GTK_COMBO_BOX(combo_devs));
	gtk_combo_box_get_active_iter (GTK_COMBO_BOX(combo_devs), &iter);	
	gtk_tree_model_get (GTK_TREE_MODEL(model), &iter, 1, &mntpoint, -1);*/
	
	return g_strdup ("none");
}


// Gibt den Partitionsnamen (z.B. /dev/sdf1) des selektierten Device zurück
gchar* gui_device_get_partition (void)
{
	/*gchar *partition;
	GtkTreeIter iter;
	
	gtk_combo_box_get_active_iter (GTK_COMBO_BOX (combo_devs), &iter);	
	gtk_tree_model_get (GTK_TREE_MODEL (model_devs), &iter, COL_DEV_PART, &partition, -1);*/
	
	return g_strdup ("none");
}


// Leer das Device Feld und setzt ein Defaultwert
void gui_device_empty_list (void)
{
	GtkTreeIter iter;
	
	gtk_list_store_clear (GTK_LIST_STORE (model_devs));
	gtk_list_store_append (GTK_LIST_STORE (model_devs), &iter);
	gtk_list_store_set (GTK_LIST_STORE (model_devs), &iter, COL_DEV_NAME, "Kein USB Stick vorhanden!",
										   					COL_DEV_ID, 0, -1);
	gtk_combo_box_set_active_iter (GTK_COMBO_BOX (combo_devs), &iter);
	
	devices_count = 0;
}


// Sollen vorhandene Daten gelöscht werden?
gboolean gui_delete_checked (void)
{
	return gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (check_delete));
}


// Event-Handler für den Erstellen Button
void on_button_create_clicked (GtkWidget *widget, gpointer user_data)
{
	if (!gui_image_get_file () || devices_count < 1) {
		gui_msg_dialog ("Kein Image oder kein USB Stick vorhanden!", GTK_MESSAGE_INFO);
		return;
	}
	
	if (guboot_create ()) {
		gui_msg_dialog ("Die USB-Boot-Stick Erstellung war erfolgreich!", GTK_MESSAGE_INFO);
	}
}
