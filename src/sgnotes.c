#include "sgnotes.h"

gchar **remaining_args = NULL;
GOptionContext *context;

gboolean verbose;

GOptionEntry entries[] =
{
	{"nocsd", 0, 0, G_OPTION_ARG_NONE, &nocsd, "Disable CSD Decoration", NULL},
	{"verbose", 0, 0, G_OPTION_ARG_NONE, &verbose, "Show verbose program messages", NULL},
	{G_OPTION_REMAINING, 0, 0, G_OPTION_ARG_STRING_ARRAY, &remaining_args, "Workspace name or other arguments", "[Workspace name]"},
	{NULL}
};


int main(int argc, char *argv[])
{
	const gchar *env_sgcsd = g_getenv("SGCSD");
	notes_dir = g_build_filename(g_get_user_data_dir(), "sgnotes", NULL);
	workspaces_path = g_build_filename(notes_dir, NULL);

	nocsd = (env_sgcsd != NULL) ? atoi(env_sgcsd) == 0 : 0;
	fcsd = (env_sgcsd != NULL) ? 1 : 0;

	context = g_option_context_new("- SGNotes");
	g_option_context_add_main_entries(context, entries, NULL);

	GError *error = NULL;
	if (!g_option_context_parse(context, &argc, &argv, &error))
	{
		g_printerr("Error parsing options: %s\n", error->message);
		g_error_free(error);
		g_option_context_free(context);
		return 1;
	}

	if (remaining_args && remaining_args[0])
	{
		g_strlcpy(current_workspace, remaining_args[0], sizeof(current_workspace));
		initialized = 1;
	}

	if (nocsd)
	{fcsd = TRUE;}

	g_option_context_free(context);
	g_strfreev(remaining_args); 

	readconf();
	gtk_init(&argc, &argv);
	create_window();
}
