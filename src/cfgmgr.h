#ifndef CFGMGR_H
#define CFGMGR_H

#include <gtk/gtk.h>
#include "settings.h"
#include "mainwindow.h"

#define XA 0.0f
#define XM 4

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
extern GtkWidget *gwrapfilelist;
extern GtkWidget *ghideimgs;
extern GtkWidget *gwtheme;

#ifndef WITHOUTSOURCEVIEW
	extern GtkWidget *glinenumbers;
	extern GtkWidget *ghighlightline;
	extern GtkWidget *geditortheme;
#endif

void toggleautosave(void);
void closecfg(void);
void showcfg(void);
#endif
