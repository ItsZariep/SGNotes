#include "editor.h"

void update_status(GtkTextBuffer *input, GtkLabel *label)
{
	GtkTextIter iter;
	//gint offset;
	gint total_chars;
	gint line;
	gint column;
	gchar *status;
	gchar *savedtext;

	gtk_text_buffer_get_iter_at_mark(input, &iter, gtk_text_buffer_get_insert(input));

	const gchar *buffername = g_object_get_data(G_OBJECT(text_view), "filename");

	//offset = gtk_text_iter_get_offset(&iter);
	total_chars = gtk_text_buffer_get_char_count(input);
	line = gtk_text_iter_get_line(&iter);
	column = gtk_text_iter_get_line_offset(&iter);

	if (saved == 1)
	{
		savedtext = "<span foreground=\"green\">Saved</span>";
	}
	else
	{
		savedtext = "<span foreground=\"red\">Unsaved</span>";
	}

	status = g_strdup_printf("%s (%s)\nLine: %d  Col: %d  Total Chars: %d", buffername, savedtext, line + 1, column + 1, total_chars);
	gtk_label_set_markup(label, status);
	g_free(status);
}

void on_cursor_moved(GtkTextBuffer *aa, gpointer user_data)
{
	update_status(buffer, GTK_LABEL(status_label));
}

void find(GtkTextView *input, const gchar *text, GtkTextIter *iter)
{
	GtkTextIter mstart, mend;
	gboolean found;
	GtkTextBuffer *currentbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(input));
	GtkTextMark *last_pos;

	found = gtk_text_iter_forward_search(
		iter,
		text,
		GTK_TEXT_SEARCH_CASE_INSENSITIVE,
		&mstart,
		&mend,
		NULL
	);

	if (found)
	{
		gtk_text_buffer_select_range(currentbuffer, &mstart, &mend);
		last_pos = gtk_text_buffer_create_mark(currentbuffer, "last_pos", &mend, FALSE);
		gtk_text_view_scroll_mark_onscreen(input, last_pos);
	}
}


void search_entry_changed(GtkEditable *editable, gpointer user_data)
{
	const gchar *text;
	GtkTextIter iter;

	text = gtk_entry_get_text(GTK_ENTRY(editable));

	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
	gtk_text_buffer_get_start_iter(buffer, &iter);

	find(GTK_TEXT_VIEW(text_view), text, &iter);
}

void prev_button_clicked(GtkWidget *input)
{
	const gchar *text;
	GtkTextIter iter, start, end;

	text = gtk_entry_get_text(GTK_ENTRY(search_entry));
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));

	if (buffer == NULL || text == NULL || *text == '\0')
	{
		return;
	}

	GtkTextMark *last_pos = gtk_text_buffer_get_mark(buffer, "last_pos");

	if (last_pos != NULL)
	{
		gtk_text_buffer_get_iter_at_mark(buffer, &iter, last_pos);
		gtk_text_iter_backward_char(&iter);
	}
	else
	{
		gtk_text_buffer_get_end_iter(buffer, &iter);
	}

	if (!gtk_text_iter_backward_search(&iter, text, GTK_TEXT_SEARCH_CASE_INSENSITIVE, &start, &end, NULL))
	{
		gtk_text_buffer_get_end_iter(buffer, &iter);

		if (!gtk_text_iter_backward_search(&iter, text, GTK_TEXT_SEARCH_CASE_INSENSITIVE, &start, &end, NULL))
		{
			return;
		}
	}

	gtk_text_buffer_select_range(buffer, &start, &end);
	gtk_text_buffer_delete_mark(buffer, gtk_text_buffer_get_mark(buffer, "last_pos"));
	last_found = gtk_text_buffer_create_mark(buffer, "last_pos", &start, FALSE);
	gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(text_view), last_found, 0.0, TRUE, 0.5, 0.5);
}


void next_button_clicked(GtkWidget *input)
{
	const gchar *text;
	GtkTextMark *last_pos;
	GtkTextIter iter;

	text = gtk_entry_get_text(GTK_ENTRY(search_entry));
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));

	if (buffer == NULL || text == NULL || *text == '\0')
	{
		return;
	}

	last_pos = gtk_text_buffer_get_mark(buffer, "last_pos");

	if (last_pos == NULL)
	{
		gtk_text_buffer_get_start_iter(buffer, &iter);
	}
	else
	{
		gtk_text_buffer_get_iter_at_mark(buffer, &iter, last_pos);
		gtk_text_iter_forward_char(&iter);
	}

	if (!gtk_text_iter_forward_search(&iter, text, GTK_TEXT_SEARCH_CASE_INSENSITIVE, NULL, NULL, NULL))
	{
		gtk_text_buffer_get_start_iter(buffer, &iter);

		if (!gtk_text_iter_forward_search(&iter, text, GTK_TEXT_SEARCH_CASE_INSENSITIVE, NULL, NULL, NULL))
		{
			return;
		}
	}

	find(GTK_TEXT_VIEW(text_view), text, &iter);
}

void load_file_content(gchar *filename)
{
	gchar *file_path = g_build_filename(notes_dir, current_workspace, filename, NULL);

	GError *error = NULL;
	GIOChannel *channel = g_io_channel_new_file(file_path, "r", &error);

	if (channel == NULL)
	{
		g_printerr("Error opening file: %s\n", error->message);
		g_clear_error(&error);
		return;
	}

	gtk_text_buffer_set_text(GTK_TEXT_BUFFER(buffer), "", -1);

	gchar *line = NULL;
	gsize length;
	GIOStatus status;

	while ((status = g_io_channel_read_line(channel, &line, &length, NULL, &error)) == G_IO_STATUS_NORMAL)
	{
		gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer), line, length);
		g_free(line);
		line = NULL;
	}

	if (error != NULL)
	{
		g_printerr("Error reading file: %s\n", error->message);
		g_clear_error(&error);
	}

	saved = 1;
	g_io_channel_shutdown(channel, TRUE, NULL);
	g_io_channel_unref(channel);

	g_custom_message("FManip [READ]", "File opened: %s", filename);
	showfind = 0;
	gtk_widget_hide(textbox_grid);
	gtk_widget_grab_focus(GTK_WIDGET(text_view));

		//GtkSourceStyleSchemeManager *style_scheme_manager;
		//GtkSourceStyleScheme *style_scheme;
		//style_scheme_manager = gtk_source_style_scheme_manager_get_default();
		//style_scheme = gtk_source_style_scheme_manager_get_scheme(style_scheme_manager, "oblivion");
		//gtk_source_buffer_set_style_scheme(GTK_SOURCE_BUFFER(buffer), style_scheme);
			//gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(text_view), TRUE);
			//gtk_source_view_set_highlight_current_line(GTK_SOURCE_VIEW(text_view), TRUE);
}
