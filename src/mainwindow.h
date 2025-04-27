#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <gtk/gtk.h>
#include "events.h"
#include "editor.h"

extern GtkWidget *window;
extern GtkWidget *dialog;
extern GtkWidget *submenu_item_wordwrap;
extern GtkWidget *text_view;
extern GtkWidget *textbox_container;
extern GtkWidget *paned_horiz1;
extern GtkWidget *paned_horiz2;
extern GtkWidget *filelist;
extern GtkWidget *scrolled_list;
extern GtkWidget *textbox_grid;
extern GtkWidget *submenu_filelist;
extern GtkWidget *submenu_item_zoomin;
extern GtkWidget *submenu_item_zoomout;
extern GtkWidget *submenu_item_zoomreset;
extern GtkWidget *submenu_item_toggleimgs;
extern GtkWidget *submenu_item_settings;
extern GtkWidget *imgbox;
extern GtkWidget *submenu_item_save;
extern GtkWidget *submenu_item_closefile;
extern GtkWidget *submenu_imglist_item3;
extern GtkWidget *submenu_imglist_item2;
extern GtkWidget *submenu_filelist_item2;
extern GtkWidget *submenu_filelist_item3;
extern GtkWidget *workspaces_dialog;
extern GtkWidget *scrolled_txt;
extern GtkWidget *pic_button;
extern GtkWidget *newnote_button;

extern GtkWidget *scrolled_treeview;
extern GtkWidget *treeview;
extern GtkWidget *wintitle;
extern GtkWidget *search_entry;
extern GtkWidget *next_button;
extern GtkWidget *prev_button;
extern GtkWidget *gwordwrap;
extern GtkWidget *gfont;
extern GtkWidget *gdefworkspace;
extern GtkWidget *gpermitoverwrite;
extern GtkWidget *gautosave;
extern GtkWidget *gautosaverate;
extern GtkWidget *gautosaverate_label;
extern GtkWidget *gusecsd;
extern GtkWidget *gresizablewidgets;
extern GtkWidget *dialog;
extern GtkWidget *status_label;

extern gchar *markup_buffer;


void on_submenu_item_about_selected(GtkMenuItem *menuitem, gpointer userdata);
void help_handler(void);
void create_window(void);
void restart_program(void);

#endif
