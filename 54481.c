comics_decompress_temp_dir (const gchar *command_decompress_tmp,
			    const gchar *command, 
			    GError      **error)
{
	gboolean success;
	gchar *std_out, *basename;
	GError *err = NULL;
	gint retval;
	
	success = g_spawn_command_line_sync (command_decompress_tmp, &std_out, 
					     NULL, &retval, &err);
	basename = g_path_get_basename (command);
	if (!success) {
		g_set_error (error,
			     EV_DOCUMENT_ERROR, 
			     EV_DOCUMENT_ERROR_INVALID,
			     _("Error launching the command “%s” in order to "
			     "decompress the comic book: %s"),
			     basename,
			     err->message);
		g_error_free (err);
	} else if (WIFEXITED (retval)) {
		if (WEXITSTATUS (retval) == EXIT_SUCCESS) {
			g_free (std_out);
			g_free (basename);
			return TRUE;
		} else {
			g_set_error (error,
				     EV_DOCUMENT_ERROR,
				     EV_DOCUMENT_ERROR_INVALID,
				     _("The command “%s” failed at "
				     "decompressing the comic book."),
				     basename);
			g_free (std_out);
		}
	} else {
		g_set_error (error,
			     EV_DOCUMENT_ERROR,
			     EV_DOCUMENT_ERROR_INVALID,
			     _("The command “%s” did not end normally."),
			     basename);
		g_free (std_out);
	}
	g_free (basename);
	return FALSE;
}
