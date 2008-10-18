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


#define INSTALLED_GLADE GUBOOT_DIR"/guboot.glade"
#define INSTALLED_ICON  ICON_DIR"/guboot.png"


enum 
{
	COL_DEV_NAME,
	COL_DEV_MNT,
	COL_DEV_PART,
	COLS_DEV
};


GladeXML *glade;
GtkWidget *window_main;
GtkButton *button_create;
GtkFileChooserButton *chooser;
GtkComboBox *combo_devs;
GtkCheckButton *check_delete;
GtkListStore *model_devs;
gboolean has_devices;


// Get Widget from Glade
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
	glade = NULL;
	has_devices = FALSE;
	
	// Glade initialisieren
	glade_init();
	
	// Glade Datei laden
	GString* gladefile = g_string_new(g_get_current_dir());
	gladefile = g_string_append(gladefile, "/data/guboot.glade");
	if (g_file_test(gladefile->str, G_FILE_TEST_EXISTS) == FALSE) {
		gladefile = g_string_assign(gladefile, INSTALLED_GLADE);
		if (g_file_test(gladefile->str, G_FILE_TEST_EXISTS) == FALSE) {
			g_error ("Fehler beim Laden der Glade Datei!");
		}
	}
	glade = glade_xml_new(gladefile->str, NULL, NULL);
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
	model_devs = gtk_list_store_new (COLS_DEV, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
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
void gui_device_insert (gchar *name, gchar *mntpoint, gchar *partition)
{
	GtkTreeIter iter;
	gchar *label;
	
	if (name == NULL && mntpoint == NULL && partition == NULL) {
		label = g_strdup ("Kein USB Stick gefunden");
	} else {
		label = g_strdup_printf ("%s (%s)", mntpoint, name);
		//g_debug ("Device '%s' added to the device list", name);
		has_devices = TRUE;
	}
	gtk_list_store_append (model_devs, &iter);
	gtk_list_store_set (model_devs, &iter, COL_DEV_NAME, g_strdup (label),
										   COL_DEV_MNT, g_strdup (mntpoint),
										   COL_DEV_PART, g_strdup (partition), -1);
	gtk_combo_box_set_active_iter (GTK_COMBO_BOX (combo_devs), &iter);
	g_free (label);
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
	gchar *mntpoint;
	GtkTreeIter iter;
	GtkListStore *model;
	
	model = (GtkListStore*)gtk_combo_box_get_model (GTK_COMBO_BOX(combo_devs));
	gtk_combo_box_get_active_iter (GTK_COMBO_BOX(combo_devs), &iter);	
	gtk_tree_model_get (GTK_TREE_MODEL(model), &iter, 1, &mntpoint, -1);
	
	return g_strdup (mntpoint);
}


// Gibt den Partitionsnamen (z.B. /dev/sdf1) des selektierten Device zurück
gchar* gui_device_get_partition (void)
{
	gchar *partition;
	GtkTreeIter iter;
	
	gtk_combo_box_get_active_iter (GTK_COMBO_BOX (combo_devs), &iter);	
	gtk_tree_model_get (GTK_TREE_MODEL (model_devs), &iter, COL_DEV_PART, &partition, -1);
	
	return g_strdup (partition);
}


// Leer das Device Feld und setzt ein Defaultwert
void gui_device_empty_list (void)
{
	gtk_list_store_clear (GTK_LIST_STORE (model_devs));
	gui_device_insert (NULL, NULL, NULL);
	has_devices = FALSE;
}


// Sollen vorhandene Daten gelöscht werden?
gboolean gui_delete_checked (void)
{
	return gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (check_delete));
}


// Event-Handler für den Erstellen Button
void on_button_create_clicked (GtkWidget *widget, gpointer user_data)
{
	if (!gui_image_get_file () || !has_devices) {
		gui_msg_dialog ("Kein Image oder kein USB Stick vorhanden!", GTK_MESSAGE_INFO);
		return;
	}
	
	if (guboot_create ()) {
		gui_msg_dialog ("Die USB-Boot-Stick Erstellung war erfolgreich!", GTK_MESSAGE_INFO);
	}
}
