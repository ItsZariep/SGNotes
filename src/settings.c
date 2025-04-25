#include "settings.h"

guint fontsize=12;
guint rfontsize=12;
guint wordwrap=1;
guint autosave=1;
guint autosaverate=1;
guint permitoverwrite=0;
guint nautosaverate;
guint fix;
guint showfind=0;
guint nocsd=0;
guint fcsd=0;
guint selfromtreeview;
guint selfromlistbox;
guint nohome=0;
guint initialized=0;
guint usecsd=1;
guint resizablewidgets=0;
guint saved=1;
guint cooldown=0;
guint visiblecfgmgr=0;
guint timeout_id=0;
guint fromfile=0;

gchar *pm;
gchar *notes_dir;
gchar *fontfamily;
gchar *fontstyle;
gchar *fontweight;
gchar *defworkspace;
gchar *config_file_path;
gchar *workspaces_path;

//GtkWidget *scrolled_treeview,*treeview,*wintitle,*search_entry,*next_button,*prev_button;
void readconf(void)
{
	g_custom_message("SGNotes", "version: %s", pver);


	config_file_path = g_build_filename(g_get_user_config_dir(), "sgnotes.conf", NULL);
	g_custom_message("Settings [READ]", "Reading file: %s", config_file_path);

	GKeyFile *key_file;
	GError *error = NULL;

	key_file = g_key_file_new();

	if (!g_key_file_load_from_file(key_file, config_file_path, G_KEY_FILE_NONE, &error))
	{
		g_warning("Failed to load configuration file: %s", error->message);
		g_error_free(error);
		g_key_file_free(key_file);
		return;
	}

		wordwrap = g_key_file_get_integer(key_file, "View", "wordwrap", NULL);
			g_custom_message("Settings [LOAD]", "wordwrap: %d", wordwrap);
		fontfamily = g_key_file_get_string(key_file, "View", "fontfamily", NULL);
			g_custom_message("Settings [LOAD]", "fontfamily: %s", fontfamily);
		fontsize = g_key_file_get_integer(key_file, "View", "fontsize", NULL);
			g_custom_message("Settings [LOAD]", "fontsize: %d", fontsize);
			rfontsize = fontsize;
		fontstyle = g_key_file_get_string(key_file, "View", "fontstyle", NULL);
			g_custom_message("Settings [LOAD]", "fontstyle: %s", fontstyle);
		fontweight = g_key_file_get_string(key_file, "View", "fontweight", NULL);
			g_custom_message("Settings [LOAD]", "fontweight: %s", fontweight);

		defworkspace = g_key_file_get_string(key_file, "File", "defworkspace", NULL);
			g_custom_message("Settings [LOAD]", "defworkspace: %s", defworkspace);
		permitoverwrite = g_key_file_get_integer(key_file, "File", "permitoverwrite", NULL);
			g_custom_message("Settings [LOAD]", "permitoverwrite: %d", permitoverwrite);
		autosave = g_key_file_get_integer(key_file, "File", "autosave", NULL);
			g_custom_message("Settings [LOAD]", "autosave: %d", autosave);

		autosaverate = g_key_file_get_integer(key_file, "File", "autosaverate", NULL);
			g_custom_message("Settings [LOAD]", "autosaverate: %d", autosaverate);


	if (!initialized && defworkspace != NULL)
	{
		initialized = 1;
		strncpy(current_workspace, defworkspace, sizeof(current_workspace) - 1);
		current_workspace[sizeof(current_workspace) - 1] = '\0';
	}
		usecsd = g_key_file_get_integer(key_file, "Window", "usecsd", NULL);
			g_custom_message("Settings [LOAD]", "usecsd: %d", usecsd);

	if (fcsd)
	{
		g_custom_message("Settings:", "--nocsd is present, disabling CSD");
	}
	else
	{
		nocsd = (usecsd == 0);
	}


	resizablewidgets = g_key_file_get_integer(key_file, "Window", "resizablewidgets", NULL);
		g_custom_message("Settings [LOAD]", "resizablewidgets: %d", resizablewidgets);

	g_key_file_free(key_file);
	//g_custom_message("SGNotes", "End of function: Readconf");
}

void updateconf(GtkWidget *widget, gpointer data)
{
	gint isdefault = GPOINTER_TO_INT(data);

	if (!isdefault && visiblecfgmgr)
	{
		wordwrap = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gwordwrap));

		fontfamily = g_strdup(
			pango_font_family_get_name(
				gtk_font_chooser_get_font_family(GTK_FONT_CHOOSER(gfont))
			)
		);

		PangoFontDescription *font_desc = gtk_font_chooser_get_font_desc(GTK_FONT_CHOOSER(gfont));

		PangoStyle font_style = pango_font_description_get_style(font_desc);
		switch (font_style)
		{
			case PANGO_STYLE_ITALIC:
				fontstyle = "Italic";
				break;
			case PANGO_STYLE_OBLIQUE:
				fontstyle = "Oblique";
				break;
			case PANGO_STYLE_NORMAL:
			default:
				fontstyle = "Normal";
				break;
		}

		PangoWeight font_weight = pango_font_description_get_weight(font_desc);
		switch (font_weight)
		{
			case PANGO_WEIGHT_THIN:
				fontweight = "100";
				break;
			case PANGO_WEIGHT_ULTRALIGHT:
			case PANGO_WEIGHT_LIGHT:
				fontweight = "200";
				break;
			case PANGO_WEIGHT_SEMILIGHT:
				fontweight = "300";
				break;
			case PANGO_WEIGHT_BOOK:
			case PANGO_WEIGHT_NORMAL:
				fontweight = "400";
				break;
			case PANGO_WEIGHT_MEDIUM:
				fontweight = "500";
				break;
			case PANGO_WEIGHT_SEMIBOLD:
				fontweight = "600";
				break;
			case PANGO_WEIGHT_BOLD:
				fontweight = "700";
				break;
			case PANGO_WEIGHT_ULTRABOLD:
			case PANGO_WEIGHT_HEAVY:
				fontweight = "800";
				break;
			case PANGO_WEIGHT_ULTRAHEAVY:
				fontweight = "900";
				break;
			default:
				fontweight = "400";
				break;
		}

		gint font_size = pango_font_description_get_size(font_desc);
		if (!pango_font_description_get_size_is_absolute(font_desc))
		{
			font_size = font_size / PANGO_SCALE;
		}

		rfontsize = font_size;
		fontsize = font_size;

		defworkspace = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(gdefworkspace));

		permitoverwrite = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gpermitoverwrite));
		autosave = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gautosave));
		autosaverate = gtk_spin_button_get_value(GTK_SPIN_BUTTON(gautosaverate));
		usecsd = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gusecsd));
		resizablewidgets = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gresizablewidgets));
	}
	else if (!visiblecfgmgr)
	{
		wordwrap = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(submenu_item_wordwrap));
	}
	else
	{
		g_custom_message("Settings [SAVE]", "Restoring all values to default");
		wordwrap = 1;
		fontfamily = "";
		fontsize = 10;
		fontstyle = "Regular";
		fontweight = "400";
		defworkspace = "Default";
		permitoverwrite = 0;
		autosave = 1;
		autosaverate = 0;
		usecsd = 1;
		resizablewidgets = 1;
	}

	if (!nohome)
	{
		GKeyFile *key_file = g_key_file_new();

		g_key_file_set_integer(key_file, "View", "wordwrap", wordwrap);
			g_custom_message("Settings [SAVE]", "wordwrap: %d", wordwrap);
		g_key_file_set_string(key_file, "View", "fontfamily", fontfamily);
			g_custom_message("Settings [SAVE]", "fontfamily: %s", fontfamily);
		g_key_file_set_integer(key_file, "View", "fontsize", fontsize);
			g_custom_message("Settings [SAVE]", "fontsize: %d", fontsize);
		g_key_file_set_string(key_file, "View", "fontstyle", fontstyle);
			g_custom_message("Settings [SAVE]", "fontstyle: %s", fontstyle);
		g_key_file_set_string(key_file, "View", "fontweight", fontweight);
			g_custom_message("Settings [SAVE]", "fontweight: %s", fontweight);

		g_key_file_set_string(key_file, "File", "defworkspace", defworkspace);
			g_custom_message("Settings [SAVE]", "defworkspace: %s", defworkspace);
		g_key_file_set_integer(key_file, "File", "permitoverwrite", permitoverwrite);
			g_custom_message("Settings [SAVE]", "permitoverwrite: %d", permitoverwrite);
		g_key_file_set_integer(key_file, "File", "autosave", autosave);
			g_custom_message("Settings [SAVE]", "autosave: %d", autosave);
		g_key_file_set_integer(key_file, "File", "autosaverate", autosaverate);
			g_custom_message("Settings [SAVE]", "autosaverate: %d", autosaverate);

		g_key_file_set_integer(key_file, "Window", "usecsd", usecsd);
			g_custom_message("Settings [SAVE]", "usecsd: %d", usecsd);
		g_key_file_set_integer(key_file, "Window", "resizablewidgets", resizablewidgets);
			g_custom_message("Settings [SAVE]", "resizablewidgets: %d", resizablewidgets);

		GError *error = NULL;
		gchar *filedata = g_key_file_to_data(key_file, NULL, &error);

		if (filedata != NULL)
		{
			if (!g_file_set_contents(config_file_path, filedata, -1, &error))
			{
				g_warning("Failed to write config: %s", error->message);
				g_clear_error(&error);
			}

			g_free(filedata);
		}
		else
		{
			g_warning("Failed to serialize key file: %s", error->message);
			g_clear_error(&error);
		}

		g_key_file_unref(key_file);
	}

	updateuistyle();

	if (visiblecfgmgr)
	{
		if (!autosave)
		{
			if (timeout_id != 0)
			{
				g_source_remove(timeout_id);
				timeout_id = 0;
			}
		}
		else
		{
			if (timeout_id != 0)
			{
				g_source_remove(timeout_id);
			}

			if (autosaverate == 0)
			{
				timeout_id = g_timeout_add(100, timeout_callback, NULL);
			}
			else
			{
				timeout_id = g_timeout_add_seconds(autosaverate, timeout_callback, NULL);
			}
		}
		gtk_widget_destroy(dialog);
	}

	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(submenu_item_wordwrap), wordwrap);
	visiblecfgmgr = 0;
}
