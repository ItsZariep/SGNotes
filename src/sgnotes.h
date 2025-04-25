#ifndef SGNOTES_H
#define SGNOTES_H

#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gtk/gtk.h>
#include <glib/gstdio.h>

#ifndef WITHOUTSOURCEVIEW
	#include <gtksourceview/gtksource.h>
#endif

#include <sys/stat.h>

#define ML 256

extern GtkIconTheme *theme;
extern GtkIconInfo *info;
extern GdkPixbuf *icon;
extern GtkAccelGroup *accel_group;
extern GtkCellRenderer *filelist_renderer;
extern GtkListStore *imglist_store;
extern GtkTreeStore *filelist_store;
extern GtkTextMark *last_found;
extern GtkTextBuffer *buffer;

extern gboolean verbose;

extern gchar current_workspace[1024];
extern gchar current_file[1024];
extern gchar *clicked_workspace;

typedef struct
{
	GdkPixbuf *pixbuf;
	gchar *path;
}ImageInfo;

#ifdef WITHOUTSOURCEVIEW
#define gtk_source_view_new gtk_text_view_new
#endif

#include "settings.h"
#include "mainwindow.h"

#endif
