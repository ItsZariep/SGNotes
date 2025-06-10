#include "events.h"

gchar *program_icon;

#ifdef _WIN32
int is_dark_theme_enabled()
{
	HKEY hKey;
	DWORD value = 1;
	DWORD valueSize = sizeof(value);
	LONG result;

	result = RegOpenKeyExA(HKEY_CURRENT_USER,
		"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", 0, KEY_READ, &hKey);

	if (result != ERROR_SUCCESS) { return 0; }

	result = RegGetValueA(hKey, NULL, "AppsUseLightTheme", RRF_RT_REG_DWORD, NULL, &value, &valueSize);
	RegCloseKey(hKey);

	if (result != ERROR_SUCCESS) { return 0; }
	return (value == 0);
}

#endif

void updateuistyle(void)
{
	if (current_file[0] != '\0') { gtk_widget_set_visible(imgbox, !hideimgs);}
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), (wordwrap == 0) ? GTK_WRAP_NONE : GTK_WRAP_WORD_CHAR);
		GtkCssProvider *provider = gtk_css_provider_new();
		gchar *css = g_strdup_printf("textview { font-family: \"%s\"; font-size: %dpt; font-style: %s; font-weight: %s; }", 
			fontfamily, fontsize, fontstyle, fontweight);
		gtk_css_provider_load_from_data(provider, css, -1, NULL);
		g_free(css);
		gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
		GTK_STYLE_PROVIDER(provider),
		GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
		g_object_unref(provider);

	#ifndef WITHOUTSOURCEVIEW
		GtkSourceStyleSchemeManager *style_scheme_manager;
		GtkSourceStyleScheme *style_scheme;
		style_scheme_manager = gtk_source_style_scheme_manager_get_default();
		style_scheme = gtk_source_style_scheme_manager_get_scheme(style_scheme_manager, editortheme);
		gtk_source_buffer_set_style_scheme(GTK_SOURCE_BUFFER(buffer), style_scheme);
			gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(text_view), linenumbers);
			gtk_source_view_set_highlight_current_line(GTK_SOURCE_VIEW(text_view), highlightline);
	#endif

	#ifdef _WIN32
	gchar *theme_name = NULL;

	if (wtheme == 0)
	{
		theme_name = is_dark_theme_enabled() ? "Windows-10-Dark" : "Windows-10";
	}
	else if (wtheme == 1)
	{
		theme_name = "Windows-10";
	}
	else
	{
		theme_name = "Windows-10-Dark";
	}

	if (theme_name != NULL)
	{
		g_object_set(gtk_settings_get_default(), "gtk-theme-name", theme_name, NULL);
	}
	gtk_style_context_reset_widgets(gdk_screen_get_default());

	#endif

}

gchar* probe_icons_from_theme(GPtrArray *icon_names)
{
	theme = gtk_icon_theme_get_default();
	guint i;

	for (i = 0; i < icon_names->len; i++)
	{
		gchar *icon_name = g_ptr_array_index(icon_names, i);
		info = gtk_icon_theme_lookup_icon(theme, icon_name, 48, 0);
		if (info != NULL)
		{
			icon = gtk_icon_info_load_icon(info, NULL);
			if (icon != NULL)
			{
				g_custom_message("Events:", "Program icon found as: %s", icon_name);
				g_object_unref(icon);
				g_object_unref(info);
				return g_strdup(icon_name);
			}
			g_object_unref(info);
		}
	}
	return g_strdup("\0");
}

void window_set_icon(GtkWindow *target, gchar *iconname)
{
	info = gtk_icon_theme_lookup_icon(theme, iconname, 16, 16);
		icon = gtk_icon_info_load_icon(info, NULL);
		gtk_window_set_icon(target, icon);
		g_object_unref(icon);
		g_object_unref(info);
}

gint show_warning_dialog(const gchar *message)
{
	dialog = gtk_message_dialog_new(NULL, 
		GTK_DIALOG_MODAL, 
		GTK_MESSAGE_WARNING, 
		GTK_BUTTONS_OK_CANCEL, 
		"%s", 
		message);

	gtk_window_set_title(GTK_WINDOW(dialog), "Warning");
	gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);

	gint result,  response = gtk_dialog_run(GTK_DIALOG(dialog));

	if (response == GTK_RESPONSE_OK)
	{
		result = 1;
	}
	else
	{
		result = 0;
	}

	gtk_widget_destroy(dialog);
	return result;
}

gint show_file_warning(void)
{
	gint result;
	gchar *warning_message;

	warning_message = g_strdup_printf("\"%s\" file is modified, if you close it your changes will be lost", current_file);

	GtkWidget *warning_dialog = gtk_message_dialog_new(NULL, 
		GTK_DIALOG_MODAL, 
		GTK_MESSAGE_QUESTION, 
		GTK_BUTTONS_NONE, 
		"%s", 
		warning_message);

	GtkWidget *save_button = gtk_dialog_add_button(GTK_DIALOG(warning_dialog), "_Save", GTK_RESPONSE_ACCEPT);
	GtkStyleContext *savebtn_context = gtk_widget_get_style_context(save_button);
	gtk_style_context_add_class(savebtn_context, "suggested-action");

	gtk_dialog_add_button(GTK_DIALOG(warning_dialog), "_Cancel", GTK_RESPONSE_CANCEL);

	GtkWidget *close_button = gtk_dialog_add_button(GTK_DIALOG(warning_dialog), "_Close without Saving", GTK_RESPONSE_REJECT);
	GtkStyleContext *closebtn_context = gtk_widget_get_style_context(close_button);
	gtk_style_context_add_class(closebtn_context, "destructive-action");

	gtk_window_set_title(GTK_WINDOW(warning_dialog), "Confirmation");
	gtk_window_set_position(GTK_WINDOW(warning_dialog), GTK_WIN_POS_CENTER);

	gint response;

	while (TRUE)
	{
		response = gtk_dialog_run(GTK_DIALOG(warning_dialog));

		switch (response)
		{
			case GTK_RESPONSE_ACCEPT:
				result = 1;
				gtk_widget_destroy(warning_dialog);
				return result;

			case GTK_RESPONSE_REJECT:
				if (show_warning_dialog("Are you sure?\nALL YOUR CHANGES WILL BE LOST") == 1)
				{
					result = 2;
					gtk_widget_destroy(warning_dialog);
					return result;
				}
				break;

			case GTK_RESPONSE_CANCEL:
			default:
				result = 0;
				gtk_widget_destroy(warning_dialog);
				return result;
		}
	}
}

void show_error_dialog(const gchar *message)
{
	dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "%s", message);
	gtk_window_set_title(GTK_WINDOW(dialog), "ERROR");
	gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}

void togglesave(GtkWidget *widget, gpointer data)
{
	(void)widget;
	if ((autosave == 1 && autosaverate != 0) || autosave == 0) 
	{
		saved = GPOINTER_TO_INT(data);
		if (!cooldown)
		{
			update_status(buffer, GTK_LABEL(status_label));
			if (saved)
			{
				markup_buffer = g_strdup_printf("%s/%s - SGNotes", current_workspace, current_file);
			}
			else
			{
				markup_buffer = g_strdup_printf("*%s/%s - SGNotes", current_workspace, current_file);
			}

			gtk_window_set_title(GTK_WINDOW(window), markup_buffer);
			g_free(markup_buffer);

			//gchar *css;
			//GtkCssProvider *provider = gtk_css_provider_new();
			//if (!saved)
			//{
				//css = g_strdup_printf("treeview:selected {color: #FF0000; }");
			//}
			//else
			//{
				//css = g_strdup_printf("treeview:selected {color: inherit; }");
			//}

			//gtk_css_provider_load_from_data(provider, css, -1, NULL);
			//g_free(css);
			//GtkStyleContext *context = gtk_widget_get_style_context(GTK_WIDGET(text_view));
			//gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
			//GTK_STYLE_PROVIDER(provider),
			//GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
			//gtk_widget_queue_draw(filelist);
		}
	}
}

gboolean timeout_callback(gpointer user_data)
{
	if (current_file[0] != '\0')
	{
		on_save_button_clicked(NULL, user_data);
		togglesave(text_view, GINT_TO_POINTER(1));
	}
	return G_SOURCE_CONTINUE;
}

gboolean on_workspace_button_press(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
	(void)user_data;
	if (event->type == GDK_BUTTON_PRESS && event->button == 3)
	{
		GtkTreeView *tree_view = GTK_TREE_VIEW(widget);
		GtkTreePath *path;
		GtkTreeSelection *selection;
		GtkTreeModel *model;
		GtkTreeIter iter;

		if (gtk_tree_view_get_path_at_pos(tree_view, (gint)event->x, (gint)event->y, &path, NULL, NULL, NULL))
		{
			selection = gtk_tree_view_get_selection(tree_view);
			gtk_tree_selection_select_path(selection, path);
			gtk_tree_view_set_cursor(tree_view, path, NULL, FALSE);

			model = gtk_tree_view_get_model(tree_view);

			if (gtk_tree_model_get_iter(model, &iter, path))
			{
				if (clicked_workspace != NULL)
				{
					g_free(clicked_workspace);
				}
				gtk_tree_model_get(model, &iter, 0, &clicked_workspace, -1);
			}

			gtk_tree_path_free(path);

			GtkWidget *menu = gtk_menu_new();
			GtkWidget *menu_item = gtk_menu_item_new_with_label("Delete Workspace");
			gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
			gtk_widget_show_all(menu);

			g_signal_connect(menu_item, "activate", G_CALLBACK(on_workspace_menu_item_activate), NULL);

			gtk_menu_popup_at_pointer(GTK_MENU(menu), (GdkEvent*)event);

			return TRUE;
		}
	}
	return FALSE;
}

gboolean filelist_element_showmenu(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
	(void)user_data;
	if (event->type == GDK_BUTTON_PRESS && event->button == 3)
	{
		GtkTreeView *tree_view = GTK_TREE_VIEW(widget);
		GtkTreePath *path;
		GtkTreeSelection *selection;
		GtkTreeModel *model;
		GtkTreeIter iter;

		if (gtk_tree_view_get_path_at_pos(tree_view, (gint)event->x, (gint)event->y, &path, NULL, NULL, NULL))
		{
			selection = gtk_tree_view_get_selection(tree_view);
			gtk_tree_selection_select_path(selection, path);
			gtk_tree_view_set_cursor(tree_view, path, NULL, FALSE);

			model = gtk_tree_view_get_model(tree_view);

			if (gtk_tree_model_get_iter(model, &iter, path))
			{
				if (clicked_workspace != NULL)
				{
					g_free(clicked_workspace);
				}
				gtk_tree_model_get(model, &iter, 0, &clicked_workspace, -1);
			}

			gtk_tree_path_free(path);

			GtkWidget *menu = gtk_menu_new();
			GtkWidget *menu_item_rename = gtk_menu_item_new_with_label("Rename");
			GtkWidget *menu_item_delete = gtk_menu_item_new_with_label("Delete");
			GtkWidget *menu_item_export = gtk_menu_item_new_with_label("Export");
			gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item_rename);
			gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item_delete);
			gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item_export);
			gtk_widget_show_all(menu);

			g_signal_connect(menu_item_delete, "activate", G_CALLBACK(on_delete_button_clicked), NULL);
			g_signal_connect(menu_item_rename, "activate", G_CALLBACK(on_rename_button_clicked), NULL);
			g_signal_connect(menu_item_export, "activate", G_CALLBACK(on_export_button_clicked), NULL);

			gtk_menu_popup_at_pointer(GTK_MENU(menu), (GdkEvent*)event);

			return TRUE;
		}
		else
		{
			gtk_menu_popup_at_pointer(GTK_MENU(submenu_filelist), NULL);
		}
	}
	return FALSE;
}

void on_workspace_row_activated(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer output)
{
	(void)column;

	GtkTreeModel *model;
	GtkTreeIter iter;
	gchar *selected_workspace;

	model = gtk_tree_view_get_model(tree_view);

	if (gtk_tree_model_get_iter(model, &iter, path))
	{
		gtk_tree_model_get(model, &iter, 0, &selected_workspace, -1);
		g_print("Selected Workspace: %s\n", selected_workspace);
		strncpy(current_workspace, selected_workspace, sizeof(current_workspace) - 1);
		g_free(selected_workspace);

		gtk_widget_destroy(output);
		restart_program();
	}
}

void adjust_font_size(GtkWidget *widget, gpointer data)
{
	(void)widget;
	(void)data;
	gint adjustment = GPOINTER_TO_INT(data);
	if (adjustment == 0)
	{
		rfontsize = fontsize;
	}
	else
	{
		rfontsize = (rfontsize > 100) ? 100 : (rfontsize < 4) ? 4 : rfontsize;
		rfontsize += adjustment;
	}

	GtkCssProvider *provider = gtk_css_provider_new();
	gchar *css = g_strdup_printf("textview { font-size: %dpt; }", rfontsize);
	gtk_css_provider_load_from_data(provider, css, -1, NULL);
	g_free(css);

	GtkStyleContext *currentcontext = gtk_widget_get_style_context(GTK_WIDGET(text_view));
	gtk_style_context_add_provider(currentcontext, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

	g_object_unref(provider);
}

void on_close_file(GtkWidget *widget, gpointer data)
{
	(void)widget;
	(void)data;

	if (!saved)
	{
		gint opt = show_file_warning();
		if (opt == 1)
		{
			timeout_callback(NULL);
		}
		else if (opt == 2)
		{
			g_warning("File was closed without saving modifications. All changes were lost.");
			togglesave(text_view, GINT_TO_POINTER(1));
		}
		else
		{
			return ;
		}
	}

	current_file[0] = '\0';
	gtk_widget_hide(textbox_container);
	gtk_widget_hide(imgbox);
	gtk_widget_set_sensitive(submenu_item_save, FALSE);
	gtk_widget_set_sensitive(submenu_item_closefile, FALSE);
	gtk_widget_set_sensitive(submenu_imglist_item3, FALSE);
	gtk_widget_set_sensitive(submenu_filelist_item2, FALSE);
	gtk_widget_set_sensitive(submenu_filelist_item3, FALSE);
	gtk_widget_set_sensitive(submenu_item_zoomin, FALSE);
	gtk_widget_set_sensitive(submenu_item_zoomreset, FALSE);
	gtk_widget_set_sensitive(submenu_item_zoomout, FALSE);

	markup_buffer = g_strdup_printf("%s - SGNotes", current_workspace);
	gtk_window_set_title(GTK_WINDOW(window), markup_buffer);

	gtk_widget_hide(textbox_grid);
	gtk_widget_set_hexpand(scrolled_list, TRUE);
	gtk_widget_set_hexpand(filelist, TRUE);
	if (resizablewidgets)
	{
		gtk_paned_set_position(GTK_PANED(paned_horiz1), -1);
	}
	g_object_set(filelist_renderer, "wrap-width", 999, NULL);
	fromfile = 0;
}


gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
	(void)widget;
	(void)data;
	if ((event->state & GDK_CONTROL_MASK) && (event->keyval == GDK_KEY_f))
	{
		if (current_file[0] == '\0')
		{
		}
		else
		{
			if (showfind == 0)
			{
				gtk_widget_show(textbox_grid);
				gtk_widget_grab_focus(GTK_WIDGET(search_entry));
				showfind = 1;
			}
			else if (showfind == 1)
			{
				gtk_widget_hide(textbox_grid);
				gtk_widget_grab_focus(GTK_WIDGET(text_view));
				showfind = 0;
			}
		}
		return TRUE;
	}
	else if (event->keyval == GDK_KEY_Escape && showfind == 1)
	{
		gtk_widget_hide(textbox_grid);
		gtk_widget_grab_focus(GTK_WIDGET(text_view));
		showfind = 0;
	}
	if ((event->keyval == GDK_KEY_Escape && showfind == 0))
	{
		on_close_file(text_view, NULL);
	}
	return FALSE;
}

gboolean on_scroll_event(GtkWidget *widget, GdkEventScroll *event, gpointer user_data)
{
	(void)widget;
	(void)user_data;
	if (event->state & GDK_CONTROL_MASK)
	{
		if (event->direction == GDK_SCROLL_UP)
		{
			adjust_font_size(text_view, GINT_TO_POINTER(2));
			return TRUE;
		}
		else if (event->direction == GDK_SCROLL_DOWN)
		{
			adjust_font_size(text_view, GINT_TO_POINTER(-2));
			return TRUE;
		}
	}
	return FALSE;
}

gboolean on_button_press(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
	(void)widget;
	if (event->type == GDK_BUTTON_PRESS && event->button == 3)
	{
		GtkWidget *submenu = GTK_WIDGET(data);
		gtk_menu_popup_at_pointer(GTK_MENU(submenu), NULL);
		return TRUE;
	}
	
	return FALSE;
}
gboolean on_list_press(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
	(void)widget;
	(void)data;
	if (event->type == GDK_BUTTON_PRESS && event->button == 3)
	{
		return TRUE;
	}
	return FALSE;
}
// Entry dialogs behavior
gboolean on_entry_key_press(GtkWidget *widget, GdkEventKey *event, GtkDialog *input)
{
	(void)widget;
	if (event->keyval == GDK_KEY_Return || event->keyval == GDK_KEY_KP_Enter)
	{
		gtk_dialog_response(input, GTK_RESPONSE_OK);
		return TRUE;
	}
	return FALSE;
}

void on_entry_changed(GtkEditable *editable, gpointer user_data)
{
	(void)user_data;
	GtkWidget *entry = GTK_WIDGET(editable);
	const gchar *text = gtk_entry_get_text(GTK_ENTRY(entry));
	gchar *cleaned_text = g_strdelimit(g_strdup(text), "/\\\n", '\0');
	gtk_entry_set_text(GTK_ENTRY(entry), cleaned_text);
	g_free(cleaned_text);
}

void on_filelist_item_selected(gpointer user_data)
{
	if (!saved)
	{
		gint opt = show_file_warning();
		if (opt == 1)
		{
			timeout_callback(NULL);
		}
		else if (opt == 2)
		{
			g_warning("File was closed without saving modifications. All changes were lost.");
			togglesave(text_view, GINT_TO_POINTER(1));
		}
		else
		{
			return;
		}
	}

	cooldown = 1;
	GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(filelist));
	GtkTreeModel *model;
	GtkTreeIter iter;
	gchar *aaaa;
	gchar *filename;

	if (gtk_tree_selection_get_selected(selection, &model, &iter))
	{
		gtk_tree_model_get(model, &iter, 0, &aaaa, 1, &filename, -1);

		gtk_widget_show(textbox_container);
		gtk_widget_hide(textbox_grid);
		gtk_widget_set_visible(imgbox, !hideimgs);

		gtk_widget_set_sensitive(submenu_item_save, TRUE);
		gtk_widget_set_sensitive(submenu_imglist_item3, TRUE);
		gtk_widget_set_sensitive(submenu_filelist_item2, TRUE);
		gtk_widget_set_sensitive(submenu_filelist_item3, TRUE);
		gtk_widget_set_sensitive(submenu_item_closefile, TRUE);
		gtk_widget_set_sensitive(submenu_item_zoomin, TRUE);
		gtk_widget_set_sensitive(submenu_item_zoomreset, TRUE);
		gtk_widget_set_sensitive(submenu_item_zoomout, TRUE);
		gtk_widget_set_size_request(scrolled_list, 100, 150);
		gtk_widget_set_hexpand(scrolled_list, FALSE);

		load_file_content(filename);

		g_strlcpy(current_file, filename, sizeof(current_file));

		add_images_from_directory(GTK_WIDGET(treeview), user_data);

			markup_buffer = g_strdup_printf("%s/%s - SGNotes", current_workspace, current_file);
			g_object_set_data(G_OBJECT(text_view), "filename", current_file);

			gtk_window_set_title(GTK_WINDOW(window), markup_buffer);

		if (resizablewidgets && !fromfile)
		{
			gtk_paned_set_position(GTK_PANED(paned_horiz1), 32);
			gtk_paned_set_position(GTK_PANED(paned_horiz2), 999);
		}

		g_free(filename);
	}
	saved = 1;
	fromfile = 1;
	cooldown = 0;
	update_status(buffer, GTK_LABEL(status_label));
}

gboolean on_treeview_clicked(GtkWidget *input, GdkEventButton *event, gpointer data)
{
	if (event->button == 3)
	{
		GtkTreePath *path = NULL;
		GtkTreeViewColumn *column = NULL;
		if (gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(input), (gint)event->x, (gint)event->y, &path, &column, NULL, NULL))
		{
			gchar *path_string = gtk_tree_path_to_string(path);
			selfromtreeview = atoi(path_string);
			g_custom_message("Events", "Picture selected: %d", selfromtreeview);
			GtkWidget *submenu = GTK_WIDGET(data);
			gtk_menu_popup_at_pointer(GTK_MENU(submenu), NULL);
			g_free(path_string);
			gtk_tree_path_free(path);
			return TRUE;
		}
		else
		{
		#ifndef _WIN32 //temporally disabled on windows because it doesn't work
			GtkWidget *submenu = gtk_menu_new();
				GtkWidget *submenu_item1 = gtk_menu_item_new_with_label("Show pictures folder in file manager");
				gtk_menu_shell_append(GTK_MENU_SHELL(submenu), submenu_item1);
				g_signal_connect(submenu_item1, "activate", G_CALLBACK(on_img_empty_selected), NULL);
			gtk_widget_show_all(submenu);
			gtk_menu_popup_at_pointer(GTK_MENU(submenu), NULL);
		#endif
		}
	}
	else if (event->button == 1)
	{
		GtkTreePath *path = NULL;
		GtkTreeViewColumn *column = NULL;
		if (gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(input), (gint)event->x, (gint)event->y, &path, &column, NULL, NULL))
		{
			gchar *path_string = gtk_tree_path_to_string(path);
			selfromtreeview = atoi(path_string);
			g_custom_message("Events", "Picture selected: %d", selfromtreeview);
			on_submenu_imglist_item1_selected();
			g_free(path_string);
			gtk_tree_path_free(path);
			return TRUE;
		}
	}

	return TRUE;
}

gboolean on_listbox_clicked(GtkWidget *listbox, GdkEventButton *event, gpointer data)
{
	(void)listbox;
	if (event->button == 3)
	{
		GtkListBoxRow *row = gtk_list_box_get_row_at_y(GTK_LIST_BOX(filelist), (gint)event->y);
		if (row != NULL)
		{
			selfromlistbox = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(row), "index"));
			g_custom_message("Events", "Picture selected: %d", selfromtreeview);
			GtkWidget *submenu = GTK_WIDGET(data);
			gtk_menu_popup_at_pointer(GTK_MENU(submenu), NULL);
			return TRUE;
		}
	}
	else if (event->button == 1)
	{
		GtkListBoxRow *row = gtk_list_box_get_row_at_y(GTK_LIST_BOX(filelist), (gint)event->y);
		if (row != NULL)
		{
			selfromlistbox = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(row), "index"));
			g_custom_message("Events", "Picture selected: %d", selfromtreeview);
			on_submenu_imglist_item1_selected();
			return TRUE;
		}
	}
	return TRUE;
}

gboolean quit_handler(void)
{
	if (!saved)
	{
		gint opt = show_file_warning();
		if (opt == 1)
		{
			timeout_callback(NULL);
			gtk_main_quit();
		}
		else if (opt == 2)
		{
			g_warning("File was closed without saving modifications. All changes were lost.");
			togglesave(text_view, GINT_TO_POINTER(1));
			gtk_main_quit();
		}
		else
		{
			return 1;
		}
	}
	else
	{
		gtk_main_quit();
		exit(0);
		return FALSE;
	}
	return FALSE;
}

void custom_log_handler(const gchar *log_domain, GLogLevelFlags log_level, const gchar *message, gpointer user_data)
{
	(void)log_level;
	(void)user_data;
	(void)log_domain;

	gint64 now_us = g_get_real_time();

	time_t seconds = (time_t)(now_us / G_USEC_PER_SEC);
	gint milliseconds = (gint)((now_us % G_USEC_PER_SEC) / 1000);

	struct tm *tm_info = localtime(&seconds);

	gchar time_buffer[32];

	if (tm_info != NULL)
	{
		strftime(time_buffer, sizeof(time_buffer), "%H:%M:%S", tm_info);

		size_t len = strlen(time_buffer);
		snprintf(time_buffer + len, sizeof(time_buffer) - len, ".%03d", milliseconds);
	}
	else
	{
		strncpy(time_buffer, "??:??:??.???", sizeof(time_buffer));
		time_buffer[sizeof(time_buffer) - 1] = '\0';
	}

	const gchar *prefix = (const gchar *)user_data;

	g_print("** \033[0;32m%s\033[0m: \033[0;34m%s\033[0m: %s\n", prefix, time_buffer, message);
}

void g_custom_message(const gchar *prefix, const gchar *format, ...)
{
	if (verbose == 1)
	{
		va_list args;
		va_start(args, format);

		gchar *formatted_message = g_strdup_vprintf(format, args);
		va_end(args);

		g_log_set_handler(NULL, G_LOG_LEVEL_MESSAGE, (GLogFunc) custom_log_handler, (gpointer) prefix);
		g_message("%s", formatted_message);
		g_free(formatted_message);
	}
}
