#ifndef EVENTS_H
#define EVENTS_H

#include "sgnotes.h"
#include "fmanip.h"
#include "settings.h"
#include "editor.h"
#include "img.h"
#include "mainwindow.h"

extern gchar *program_icon;

void updateuistyle(void);
gchar* probe_icons_from_theme(GPtrArray *icon_names);
void window_set_icon(GtkWindow *target, gchar *iconname);
gint show_warning_dialog(const gchar *message);
gint show_file_warning(void);
void show_error_dialog(const gchar *message);
void togglesave(GtkWidget *widget, gpointer data);
gboolean timeout_callback(gpointer user_data);
gboolean on_workspace_button_press(GtkWidget *widget, GdkEventButton *event, gpointer user_data);
gboolean filelist_element_showmenu(GtkWidget *widget, GdkEventButton *event, gpointer user_data);
void on_workspace_row_activated(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer output);
void adjust_font_size(GtkWidget *widget, gpointer data);
void on_close_file(GtkWidget *widget, gpointer data);
gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer data);
gboolean on_scroll_event(GtkWidget *widget, GdkEventScroll *event, gpointer user_data);
gboolean on_button_press(GtkWidget *widget, GdkEventButton *event, gpointer data);
gboolean on_list_press(GtkWidget *widget, GdkEventButton *event, gpointer data);
gboolean on_entry_key_press(GtkWidget *widget, GdkEventKey *event, GtkDialog *input);
void on_entry_changed(GtkEditable *editable, gpointer user_data);
void on_filelist_item_selected(gpointer user_data);
gboolean on_treeview_clicked(GtkWidget *input, GdkEventButton *event, gpointer data);
gboolean on_listbox_clicked(GtkWidget *listbox, GdkEventButton *event, gpointer data);
gboolean quit_handler(void);

void custom_log_handler(const gchar *log_domain, GLogLevelFlags log_level, const gchar *message, gpointer user_data);
void g_custom_message(const gchar *prefix, const gchar *format, ...);

#endif
