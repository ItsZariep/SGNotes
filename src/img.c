#include "img.h"

GError *error = NULL;

gchar current_workspace[1024];
gchar current_file[1024];
gchar *clicked_workspace;

void add_images_from_directory(GtkWidget *widget, gpointer user_data)
{
	gtk_list_store_clear(GTK_LIST_STORE(imglist_store));

	gchar *pre_path = g_build_filename(notes_dir, current_workspace, NULL);
	gchar *dir_path = g_build_filename(pre_path, g_strdup_printf("%s_files", current_file), NULL);

	GFile *directory = g_file_new_for_path(dir_path);

	if (!g_file_query_exists(directory, NULL))
	{
		g_print("This note don't have pictures.\n");
		g_object_unref(directory);
		return;
	}

	GError *currenterror = NULL;
	GFileEnumerator *enumerator = g_file_enumerate_children(directory, G_FILE_ATTRIBUTE_STANDARD_NAME, G_FILE_QUERY_INFO_NONE, NULL, &currenterror);

	if (currenterror != NULL)
	{
		g_print("Failed to enum files on directory: %s\n", error->message);
		g_error_free(error);
		g_object_unref(directory);
		return;
	}

	GFileInfo *fileinfo;
	while ((fileinfo = g_file_enumerator_next_file(enumerator, NULL, &error)) != NULL)
	{
		const gchar *filename = g_file_info_get_name(fileinfo);
		gchar *file_path = g_build_filename(dir_path, filename, NULL);

		GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(file_path, NULL);
			if (pixbuf != NULL)
		{
		gint original_width = gdk_pixbuf_get_width(pixbuf);
		gint target_width = 90;
		gint target_height = gdk_pixbuf_get_height(pixbuf) * target_width / original_width;

		GdkPixbuf *scaled_pixbuf = gdk_pixbuf_scale_simple(pixbuf, target_width, target_height, GDK_INTERP_BILINEAR);

		GtkTreeIter iter;
		gtk_list_store_append(imglist_store, &iter);
		gtk_list_store_set(imglist_store, &iter, 0, scaled_pixbuf, -1);

		g_object_unref(pixbuf);
		g_object_unref(scaled_pixbuf);
		}
		g_free(file_path);
		g_object_unref(fileinfo);
	}
	g_file_enumerator_close(enumerator, NULL, NULL);
	g_object_unref(enumerator);
	g_object_unref(directory);
}

void add_image(GtkWidget *widget, gpointer user_data)
{
	GtkTreeIter iter;

	GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
	gint res;

	dialog = gtk_file_chooser_dialog_new("Open File",
		GTK_WINDOW(window),
		action,
		"_Cancel",
		GTK_RESPONSE_CANCEL,
		"_Open",
		GTK_RESPONSE_ACCEPT,
		NULL);

	res = gtk_dialog_run(GTK_DIALOG(dialog));
	if (res == GTK_RESPONSE_ACCEPT)
	{
		gchar *filename;
		GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
		filename = gtk_file_chooser_get_filename(chooser);
		GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(filename, NULL);
		if (pixbuf != NULL)
		{
			gint original_width = gdk_pixbuf_get_width(pixbuf);
			gint target_width = 100;
			gint target_height = gdk_pixbuf_get_height(pixbuf) * target_width / original_width;

			gchar *dir_path = g_build_filename(notes_dir, current_workspace, NULL);

			gchar *file_path = g_build_filename(dir_path,g_strdup_printf("%s_files", current_file),NULL);

			g_mkdir_with_parents(file_path, 0755);

			gchar *unique_filename;
			gint counter = 0;
			do
			{
				unique_filename = g_strdup_printf("%s/%d.png", file_path, counter);
				counter++;
			} while (g_file_test(unique_filename, G_FILE_TEST_EXISTS));

			GFile *source_file = g_file_new_for_path(filename);
			GFile *destination_file = g_file_new_for_path(unique_filename);

			if (g_file_copy(source_file, destination_file, G_FILE_COPY_OVERWRITE, NULL, NULL, NULL, NULL))
			{
				GdkPixbuf *scaled_pixbuf = gdk_pixbuf_scale_simple(pixbuf, target_width, target_height, GDK_INTERP_BILINEAR);
				gtk_list_store_append(imglist_store, &iter);
				gtk_list_store_set(imglist_store, &iter, 0, scaled_pixbuf, -1);
				g_object_unref(pixbuf);
				g_object_unref(scaled_pixbuf);
			}
			else
			{
				g_print("Error can't open output folder.\n");
			}
			g_object_unref(source_file);
			g_object_unref(destination_file);
		}
	}
	gtk_widget_destroy(dialog);
}

void on_submenu_imglist_item1_selected(void)
{
	gchar *file_path = NULL;

	gchar *dir_path = g_build_filename(notes_dir, current_workspace, NULL);
	gchar *file_dir = g_strdup_printf("%s_files", current_file);
	gchar *full_dir_path = g_build_filename(dir_path, file_dir, NULL);
	gchar *filename = g_strdup_printf("%d.png", selfromtreeview);
	file_path = g_build_filename(full_dir_path, filename, NULL);

	error = NULL;
	gchar *uri = g_filename_to_uri(file_path, NULL, &error);

	if (uri != NULL)
	{
		if (!g_app_info_launch_default_for_uri(uri, NULL, &error))
		{
			g_printerr("Error opening image: %s\n", error->message);
			g_error_free(error);
		}

		g_free(uri);
	}
	else
	{
		g_printerr("Error converting file path to URI: %s\n", error->message);
		g_error_free(error);
	}

	g_free(dir_path);
	g_free(file_dir);
	g_free(full_dir_path);
	g_free(filename);
	g_free(file_path);
}

gint numeric_file_compare(const void *a, const void *b)
{
	const char *str1 = *(const char **)a;
	const char *str2 = *(const char **)b;

	gint num1 = atoi(str1);
	gint num2 = atoi(str2);

	return num1 - num2;
}

void on_submenu_imglist_item2_selected(GtkWidget *widget, gpointer user_data)
{
	gchar *file_imgs = g_strconcat(current_file, "_files", NULL);
	gchar *filename = g_strdup_printf("%d.png", selfromtreeview);
	gchar *file_path = g_build_filename(notes_dir, current_workspace, file_imgs, filename, NULL);
	g_free(file_imgs);

	gchar *warning_message = g_strdup_printf("Are you sure you want to delete \"%s\"?", filename);
		g_custom_message("Events", "Candidate file to delete: %s", file_path);
	gint confirmed = show_warning_dialog(warning_message);
	g_free(warning_message);

	if (confirmed)
	{
		if (g_remove(file_path) == 0)
		{
			g_custom_message("FManip [DELETE]", "File deleted: %s", file_path);

			gchar *folder_path = g_build_filename(notes_dir, current_workspace, current_file, NULL);

			GDir *dir = g_dir_open(folder_path, 0, NULL);
			if (dir != NULL)
			{
				GArray *file_names = g_array_new(TRUE, TRUE, sizeof(gchar *));
				const gchar *entry_name;

				while ((entry_name = g_dir_read_name(dir)) != NULL)
				{
					gchar *full_path = g_build_filename(folder_path, entry_name, NULL);
					if (g_file_test(full_path, G_FILE_TEST_IS_REGULAR))
					{
						gchar *dup_name = g_strdup(entry_name);
						g_array_append_val(file_names, dup_name);
					}
					g_free(full_path);
				}

				g_dir_close(dir);

				g_array_sort_with_data(file_names, (GCompareDataFunc)numeric_file_compare, NULL);

				for (guint i = 0; i < file_names->len; i++)
				{
					filename = g_array_index(file_names, gchar *, i);
					gchar *old_name = g_build_filename(folder_path, filename, NULL);
					gchar *new_name = g_strdup_printf("%s/%d.png", folder_path, i);
					g_rename(old_name, new_name);
					g_free(old_name);
					g_free(new_name);
					g_free(filename);
				}
				g_array_free(file_names, TRUE);
			}

			g_free(folder_path);
			gtk_list_store_clear(GTK_LIST_STORE(imglist_store));
			add_images_from_directory(GTK_WIDGET(treeview), user_data);
		}
		else
		{
			g_warning("Error deleting file: %s\n", file_path);
		}
	}
	else
	{
		g_custom_message("Events", "Operation cancelled, Keeping file: %s", file_path);
	}

	g_free(filename);
	g_free(file_path);
}


