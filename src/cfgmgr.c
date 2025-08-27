#include "cfgmgr.h"

GtkWidget *scrolled_treeview;
GtkWidget *treeview;
GtkWidget *wintitle;
GtkWidget *search_entry;
GtkWidget *next_button;
GtkWidget *prev_button;
GtkWidget *gwordwrap;
GtkWidget *gfont;
GtkWidget *gdefworkspace;
GtkWidget *gpermitoverwrite;
GtkWidget *gautosave;
GtkWidget *gautosaverate;
GtkWidget *gautosaverate_label;
GtkWidget *gusecsd;
GtkWidget *gresizablewidgets;
GtkWidget *ghideimgs;
GtkWidget *gwrapfilelist;

GtkWidget *glinenumbers;
GtkWidget *ghighlightline;
GtkWidget *geditortheme;

gchar notes_path[8192];

#ifndef WITHOUTSOURCEVIEW

GtkWidget *geditortheme_apply;
gchar *tmpeditortheme;
GtkWidget *geditortheme_dialog;
GtkWidget *gwtheme;

void on_themebutton_clicked(void)
{
	tmpeditortheme = g_object_get_data(G_OBJECT(geditortheme_apply), "chtheme");
		gchar *geditortheme_buttonlabel = g_strdup_printf("To Apply: %s", tmpeditortheme);
	gtk_button_set_label(GTK_BUTTON(geditortheme), geditortheme_buttonlabel);
		g_object_set_data(G_OBJECT(geditortheme), "chtheme", g_strdup(tmpeditortheme));
	gtk_widget_destroy(geditortheme_dialog);
}

gboolean on_source_view_button_press(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
	(void)user_data;
	(void)event;
	const gchar *scheme_id = (const gchar *)g_object_get_data(G_OBJECT(widget), "scheme-id");
	const gchar *scheme_name = (const gchar *)g_object_get_data(G_OBJECT(widget), "scheme-name");
	gchar *buttontext = g_strdup_printf("Apply theme: %s", scheme_name);
	gtk_button_set_label(GTK_BUTTON(geditortheme_apply), buttontext);
	g_object_set_data(G_OBJECT(geditortheme_apply), "chtheme", g_strdup(scheme_id));
		gtk_widget_set_sensitive(geditortheme_apply, TRUE);
	return FALSE;
}

void on_geditortheme_clicked(void)
{
	if (GTK_IS_DIALOG(geditortheme_dialog))
	{
		return;
	}

	geditortheme_dialog = gtk_dialog_new();
	gtk_window_set_title(GTK_WINDOW(geditortheme_dialog), "Editor theme selector - SGNotes");
	gtk_container_set_border_width(GTK_CONTAINER(geditortheme_dialog), 5);
	window_set_icon(GTK_WINDOW(geditortheme_dialog), program_icon);
	gtk_window_set_default_size(GTK_WINDOW(geditortheme_dialog), 500, 400);
	gtk_window_set_position(GTK_WINDOW(geditortheme_dialog), GTK_WIN_POS_CENTER);
	g_signal_connect(geditortheme_dialog, "destroy", G_CALLBACK(gtk_widget_destroy), geditortheme_dialog);

	GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(geditortheme_dialog));

	GtkWidget *main_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add(GTK_CONTAINER(content_area), main_vbox);

	GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_propagate_natural_height(GTK_SCROLLED_WINDOW(scrolled_window), TRUE);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start(GTK_BOX(main_vbox), scrolled_window, TRUE, TRUE, 0);

	GtkWidget *sourceviews_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add(GTK_CONTAINER(scrolled_window), sourceviews_vbox);

	geditortheme_apply = gtk_button_new_with_label("Choose a theme");
		gtk_widget_set_sensitive(geditortheme_apply, FALSE);
	gtk_box_pack_end(GTK_BOX(main_vbox), geditortheme_apply, FALSE, FALSE, 0);

	GtkSourceStyleSchemeManager *style_scheme_manager;
	const gchar * const *scheme_ids;

	style_scheme_manager = gtk_source_style_scheme_manager_get_default();
	scheme_ids = gtk_source_style_scheme_manager_get_scheme_ids(style_scheme_manager);

	for (int i = 0; scheme_ids[i] != NULL; i++)
	{
		GtkSourceStyleScheme *scheme = gtk_source_style_scheme_manager_get_scheme(
			style_scheme_manager, scheme_ids[i]);
		const gchar *scheme_name = gtk_source_style_scheme_get_name(scheme);

		GtkWidget *source_view = gtk_source_view_new();
		GtkSourceBuffer *source_buffer = GTK_SOURCE_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW(source_view)));

		gtk_source_buffer_set_style_scheme(source_buffer, scheme);

		gchar *buffertext = g_strdup_printf("# %s \n- SGNotes (using GtkSourceView4) [in GTK3] 😎️", scheme_name);
		gtk_text_buffer_set_text(GTK_TEXT_BUFFER(source_buffer), buffertext, -1);
		g_object_set_data_full(G_OBJECT(source_view), "scheme-id", g_strdup(scheme_ids[i]), g_free);
		g_object_set_data_full(G_OBJECT(source_view), "scheme-name", g_strdup(scheme_name), g_free);
		gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(source_view), GTK_WRAP_WORD_CHAR);
		gtk_widget_set_name(source_view, scheme_ids[i]);
		g_free(buffertext);

		GtkSourceLanguageManager *language_manager = gtk_source_language_manager_get_default();
		GtkSourceLanguage *language = gtk_source_language_manager_get_language(language_manager, "markdown");
		guint tmplinenumbers = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(glinenumbers));
		guint tmphighlightline = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ghighlightline));
			gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(source_view), tmplinenumbers);
			gtk_source_view_set_highlight_current_line(GTK_SOURCE_VIEW(source_view), tmphighlightline);
		gtk_source_buffer_set_language(source_buffer, language);

		gtk_widget_set_size_request(source_view, -1, 40);

		gtk_box_pack_start(GTK_BOX(sourceviews_vbox), source_view, FALSE, FALSE, 5);
		g_print("Available style scheme: %s (ID: %s)\n", scheme_name, scheme_ids[i]);
		g_signal_connect(source_view, "button-press-event", G_CALLBACK(on_source_view_button_press), NULL);
		g_signal_connect(geditortheme_apply, "clicked", G_CALLBACK(on_themebutton_clicked), NULL);
	}
	gtk_widget_show_all(geditortheme_dialog);
}
#endif

void toggleautosave(void)
{
	if (gtk_widget_get_visible(gautosaverate)) 
	{
		gtk_widget_hide(gautosaverate);
		gtk_widget_hide(gautosaverate_label);
	}
	else
	{
		gtk_widget_show(gautosaverate);
		gtk_widget_show(gautosaverate_label);
	}
}

void closecfg(void)
{
	visiblecfgmgr = 0;
	gtk_widget_set_sensitive(submenu_item_settings, TRUE);
	gtk_widget_destroy(dialog);
}

void showcfg(void)
{
	if (visiblecfgmgr == 1)
	{
		return;
	}
	else
	{
		gtk_widget_set_sensitive(submenu_item_settings, FALSE);
	}

	visiblecfgmgr = 1;
	dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(dialog), "Settings - SGNotes");
	gtk_container_set_border_width(GTK_CONTAINER(dialog), 10);
	gtk_widget_set_size_request(dialog, 400, 200);
	window_set_icon(GTK_WINDOW(dialog), program_icon);
	gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);
	g_signal_connect(dialog, "destroy", G_CALLBACK(closecfg), dialog);
	gtk_container_set_border_width(GTK_CONTAINER(dialog), 10);

	GtkWidget *defbtn;
	GtkWidget *okbtn;
	GtkWidget *cancelbtn;
	GtkWidget *applybtn;

	GtkWidget *notebook = gtk_notebook_new();

		GtkWidget *viewgrid = gtk_grid_new();
			gtk_grid_set_row_spacing(GTK_GRID(viewgrid), 5);
			gtk_grid_set_column_homogeneous(GTK_GRID(viewgrid), TRUE);

			GtkWidget *gfont_label = gtk_label_new("Font");
				gtk_label_set_xalign(GTK_LABEL(gfont_label), XA);
				gtk_widget_set_margin_start(GTK_WIDGET(gfont_label), XM);
			gfont = gtk_font_button_new();
				gtk_font_button_set_show_style(GTK_FONT_BUTTON(gfont), FALSE);
			gwordwrap = gtk_check_button_new_with_label("Word wrap");
				gtk_widget_set_direction(gwordwrap, GTK_TEXT_DIR_RTL);
			#ifndef WITHOUTSOURCEVIEW
				glinenumbers = gtk_check_button_new_with_label("Show line numbers");
					gtk_widget_set_direction(glinenumbers, GTK_TEXT_DIR_RTL);
				ghighlightline = gtk_check_button_new_with_label("Highlight the current line");
					gtk_widget_set_direction(ghighlightline, GTK_TEXT_DIR_RTL);
				GtkWidget *geditortheme_label = gtk_label_new("Editor theme");
					gtk_label_set_xalign(GTK_LABEL(geditortheme_label), XA);
					gtk_widget_set_margin_start(GTK_WIDGET(geditortheme_label), XM);
				geditortheme = gtk_button_new_with_label("");
					g_object_set_data(G_OBJECT(geditortheme), "chtheme", g_strdup(editortheme));
					gchar *geditortheme_buttonlabel = g_strdup_printf("Choose (Current: %s)", editortheme);
					gtk_button_set_label(GTK_BUTTON(geditortheme), geditortheme_buttonlabel);
			#endif

		GtkWidget *filegrid = gtk_grid_new();
			gtk_grid_set_row_spacing(GTK_GRID(filegrid), 5);
			gtk_grid_set_column_homogeneous(GTK_GRID(filegrid), TRUE);

			GtkWidget *gdefworkspace_label = gtk_label_new("Default Workspace");
				gtk_label_set_xalign(GTK_LABEL(gdefworkspace_label), XA);
				gtk_widget_set_margin_start(GTK_WIDGET(gdefworkspace_label), XM);
			gdefworkspace = gtk_combo_box_text_new();
				GDir *dir = g_dir_open(workspaces_path, 0, NULL);

				if (dir != NULL)
				{
					const gchar *filename;

					while ((filename = g_dir_read_name(dir)) != NULL)
					{
						gchar *fullpath = g_build_filename(workspaces_path, filename, NULL);

						if (g_file_test(fullpath, G_FILE_TEST_IS_DIR) &&
							g_strcmp0(filename, ".") != 0 &&
							g_strcmp0(filename, "..") != 0)
						{
							gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(gdefworkspace), NULL, filename);
						}

						g_free(fullpath);
					}

					g_dir_close(dir);
				}

		gtk_combo_box_set_active(GTK_COMBO_BOX(gdefworkspace), 0);
			gpermitoverwrite = gtk_check_button_new_with_label("Permit overwrite");
				gtk_widget_set_direction(gpermitoverwrite, GTK_TEXT_DIR_RTL);
			gautosave = gtk_check_button_new_with_label("Auto save");
				gtk_widget_set_direction(gautosave, GTK_TEXT_DIR_RTL);
			gautosaverate_label = gtk_label_new("Autosave Rate (seconds)");
				gtk_label_set_xalign(GTK_LABEL(gautosaverate_label), XA);
				gtk_widget_set_margin_start(GTK_WIDGET(gautosaverate_label), XM);
				GtkAdjustment *gautosaverate_adjustment = gtk_adjustment_new(1, 1, 3600, 1, 1, 0);
				gautosaverate = gtk_spin_button_new(gautosaverate_adjustment, 1, 0);

		GtkWidget *windowgrid = gtk_grid_new();
			gtk_grid_set_row_spacing(GTK_GRID(windowgrid), 10);
			gtk_grid_set_column_homogeneous(GTK_GRID(windowgrid), TRUE);

			gusecsd = gtk_check_button_new_with_label("Use CSD Decoration");
				gtk_widget_set_direction(gusecsd, GTK_TEXT_DIR_RTL);
			gresizablewidgets = gtk_check_button_new_with_label("Resizable elements (Experimental)");
				gtk_widget_set_direction(gresizablewidgets, GTK_TEXT_DIR_RTL);
			gwrapfilelist = gtk_check_button_new_with_label("wrap file list");
				gtk_widget_set_direction(gwrapfilelist, GTK_TEXT_DIR_RTL);
			#ifdef _WIN32
				GtkWidget *gwtheme_label = gtk_label_new("Theme");
				gtk_label_set_xalign(GTK_LABEL(gwtheme_label), XA);
				gtk_widget_set_margin_start(GTK_WIDGET(gwtheme_label), XM);
				gwtheme = gtk_combo_box_text_new();
					gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(gwtheme), NULL, "System");
					gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(gwtheme), NULL, "Light");
					gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(gwtheme), NULL, "Dark");
			#endif

			ghideimgs = gtk_check_button_new_with_label("Hide images");
				gtk_widget_set_direction(ghideimgs, GTK_TEXT_DIR_RTL);

			GtkWidget *applybox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1);
				defbtn = gtk_button_new_with_label("Default");
					GtkStyleContext *defbtn_context = gtk_widget_get_style_context(defbtn);
					gtk_style_context_add_class(defbtn_context, "destructive-action");
				okbtn  = gtk_button_new_with_label("OK");
					GtkStyleContext *okbtn_context = gtk_widget_get_style_context(okbtn);
					gtk_style_context_add_class(okbtn_context, "suggested-action");
				cancelbtn  = gtk_button_new_with_label("Cancel");
				applybtn  = gtk_button_new_with_label("Apply");

	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), viewgrid, gtk_label_new("View"));
		gtk_grid_attach(GTK_GRID(viewgrid), gfont_label, 0, 0, 1, 1);
			gtk_grid_attach(GTK_GRID(viewgrid), gfont, 1, 0, 1, 1);
		gtk_grid_attach(GTK_GRID(viewgrid), gwordwrap, 0, 1, 2, 1);
		#ifndef WITHOUTSOURCEVIEW
			gtk_grid_attach(GTK_GRID(viewgrid), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), 0, 2, 2, 1);
			gtk_grid_attach(GTK_GRID(viewgrid), gtk_label_new("Editor"), 0, 3, 2, 1);
			gtk_grid_attach(GTK_GRID(viewgrid), glinenumbers, 0, 4, 2, 1);
			gtk_grid_attach(GTK_GRID(viewgrid), ghighlightline, 0, 5, 2, 1);
			gtk_grid_attach(GTK_GRID(viewgrid), geditortheme_label, 0, 6, 2, 1);
			gtk_grid_attach(GTK_GRID(viewgrid), geditortheme, 1, 6, 1, 1);
		#endif

	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), filegrid, gtk_label_new("File"));
		gtk_grid_attach(GTK_GRID(filegrid),gdefworkspace_label, 0, 0, 1, 1);
			gtk_grid_attach(GTK_GRID(filegrid), gdefworkspace, 1, 0, 1, 1);
		gtk_grid_attach(GTK_GRID(filegrid), gpermitoverwrite, 0, 1, 2, 1);
		gtk_grid_attach(GTK_GRID(filegrid), gautosave, 0, 2, 2, 1);
		gtk_grid_attach(GTK_GRID(filegrid), gautosaverate_label, 0, 3, 1, 1);
			gtk_grid_attach(GTK_GRID(filegrid), gautosaverate, 1, 3, 1, 1);

	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), windowgrid, gtk_label_new("Window"));
		gtk_grid_attach(GTK_GRID(windowgrid), gtk_label_new("NOTE: These options require restarting SGNotes"), 0, 0, 2, 1);
			gtk_grid_attach(GTK_GRID(windowgrid), gusecsd, 0, 1, 2, 1);
		gtk_grid_attach(GTK_GRID(windowgrid), gresizablewidgets, 0, 2, 2, 1);
		gtk_grid_attach(GTK_GRID(windowgrid), gwrapfilelist, 0, 3, 2, 1);
		gtk_grid_attach(GTK_GRID(windowgrid), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), 0, 4, 2, 1);
		gtk_grid_attach(GTK_GRID(windowgrid), ghideimgs, 0, 5, 2, 1);
		#ifdef _WIN32
		gtk_grid_attach(GTK_GRID(windowgrid), gwtheme_label, 0, 6, 1, 1);
		gtk_grid_attach(GTK_GRID(windowgrid), gwtheme, 1, 6, 1, 1);
		#endif

		gtk_box_pack_start(GTK_BOX(applybox), defbtn, FALSE, FALSE, 2);
		gtk_box_pack_end(GTK_BOX(applybox), applybtn, FALSE, FALSE, 2);
		gtk_box_pack_end(GTK_BOX(applybox), cancelbtn, FALSE, FALSE, 2);
		gtk_box_pack_end(GTK_BOX(applybox), okbtn, FALSE, FALSE, 2);

	GtkWidget *confbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);
	gtk_box_pack_start(GTK_BOX(confbox), notebook, TRUE, TRUE, 2);
	gtk_box_pack_start(GTK_BOX(confbox), applybox, FALSE, FALSE, 2);

	gtk_container_add(GTK_CONTAINER(dialog), confbox);

	gtk_widget_show_all(dialog);

	if (autosave == 0)
	{
		gtk_widget_hide(gautosaverate);
		gtk_widget_hide(gautosaverate_label);
	}

	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gautosave), autosave);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gwordwrap), wordwrap);
#ifndef WITHOUTSOURCEVIEW
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(glinenumbers), linenumbers);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ghighlightline), highlightline);
	gtk_combo_box_set_active(GTK_COMBO_BOX(gwtheme), wtheme);
#endif
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gpermitoverwrite), permitoverwrite);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gusecsd), usecsd);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gresizablewidgets), resizablewidgets);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ghideimgs), hideimgs);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gwrapfilelist), wrapfilelist);

	gchar *selectedfont = g_strdup_printf("%s %s %d", fontfamily, fontstyle, fontsize);

	gtk_font_chooser_set_font(GTK_FONT_CHOOSER(gfont), selectedfont);

	int num_items = gtk_tree_model_iter_n_children(gtk_combo_box_get_model(GTK_COMBO_BOX(gdefworkspace)), NULL);
	for (int i = 0; i < num_items; i++)
	{
		gtk_combo_box_set_active(GTK_COMBO_BOX(gdefworkspace), i);
		const gchar *active_text = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(gdefworkspace));
		if (g_strcmp0(active_text, defworkspace) == 0)
		{
			break;
		}
	}

	gtk_spin_button_set_value(GTK_SPIN_BUTTON(gautosaverate), autosaverate);

#ifndef WITHOUTSOURCEVIEW
	g_signal_connect(geditortheme, "clicked", G_CALLBACK(on_geditortheme_clicked), NULL);
#endif
	g_signal_connect(gautosave, "clicked", G_CALLBACK(toggleautosave), NULL);
	g_signal_connect(defbtn, "clicked", G_CALLBACK(updateconf), GINT_TO_POINTER(1));
	g_signal_connect(okbtn, "clicked", G_CALLBACK(updateconf), GINT_TO_POINTER(0));
	g_signal_connect(cancelbtn, "clicked", G_CALLBACK(closecfg), NULL);
	g_signal_connect(applybtn, "clicked", G_CALLBACK(updateconf), GINT_TO_POINTER(2));
}
