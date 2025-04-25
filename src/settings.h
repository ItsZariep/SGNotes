#ifndef SETTINGS_H
#define SETTINGS_H

#include "sgnotes.h"
#include "cfgmgr.h"
#include "mainwindow.h"
#include "events.h"

extern guint fontsize;
extern guint rfontsize;
extern guint wordwrap;
extern guint autosave;
extern guint autosaverate;
extern guint permitoverwrite;
extern guint nautosaverate;
extern guint fix;
extern guint showfind;
extern guint nocsd;
extern guint fcsd;
extern guint selfromtreeview;
extern guint selfromlistbox;
extern guint nohome;
extern guint initialized;
extern guint usecsd;
extern guint resizablewidgets;
extern guint saved;
extern guint cooldown;
extern guint visiblecfgmgr;
extern guint timeout_id;
extern guint fromfile;
extern gchar *notes_dir;
extern gchar *workspaces_path;

extern gchar *pm;
extern gchar *notes_dir;
extern gchar *fontfamily;
extern gchar *fontstyle;
extern gchar *fontweight;
extern gchar *defworkspace;
extern gchar *program_icon;
extern gchar *config_file_path;
extern gchar *workspaces_path;

void updateuistyle(void);
void readconf(void);
void updateconf(GtkWidget *widget, gpointer data);



#endif
