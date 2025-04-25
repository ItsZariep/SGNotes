#ifndef EDITOR_H
#define EDITOR_H

#include "sgnotes.h"

void update_status(GtkTextBuffer *buffer, GtkLabel *label);
void on_cursor_moved(GtkTextBuffer *buffer, gpointer user_data);
void find (GtkTextView *input, const gchar *text, GtkTextIter *iter);
void search_entry_changed(GtkEditable *editable, gpointer user_data);
void next_button_clicked(GtkWidget *input);
void prev_button_clicked(GtkWidget *input);
void load_file_content(gchar *filename);
#endif
