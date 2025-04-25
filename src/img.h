#ifndef IMG_H
#define IMG_H

#include "sgnotes.h"
#include "settings.h"

void add_images_from_directory(GtkWidget *widget, gpointer user_data);
void add_image(GtkWidget *widget, gpointer user_data);
void on_submenu_imglist_item1_selected(void);
gint numeric_file_compare(const void *a, const void *b);
void on_submenu_imglist_item2_selected(GtkWidget *widget, gpointer user_data);
#endif
