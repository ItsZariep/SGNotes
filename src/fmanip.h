#ifndef FMANIP_H
#define FMANIP_H

#include "sgnotes.h"
#include "settings.h"
#include "mainwindow.h"
#include "editor.h"

void submenu_item_newnote_selected(GtkWidget *widget, gpointer data);
void on_rename_button_clicked(GtkButton *button, gpointer user_data);
void saveToFile(const gchar *text);
void save_file(const gchar *filename);
void on_export_button_clicked(GtkButton *button, gpointer user_data);
void on_delete_button_clicked(GtkButton *button, gpointer user_data);
void delete_current_file(gchar* file_path);
gint remove_recursive(const gchar *path);
void load_file_list(void);
void on_submenu_item_workspace_selected(void);
void on_workspace_menu_item_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_create_new_workspace(GtkButton *button, gpointer voiddialog);
void on_save_button_clicked(GtkButton *button, gpointer user_data);
void on_submenu_item_workspace_selected(void);
gboolean timeout_callback(gpointer user_data);

#endif
