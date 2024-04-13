comics_generate_command_lines (ComicsDocument *comics_document, 
			       GError         **error)
{
	gchar *quoted_file, *quoted_file_aux;
	gchar *quoted_command;
	ComicBookDecompressType type;
	
	type = comics_document->command_usage;
	comics_document->regex_arg = command_usage_def[type].regex_arg;
	quoted_command = g_shell_quote (comics_document->selected_command);
	if (comics_document->regex_arg) {
		quoted_file = comics_regex_quote (comics_document->archive);
		quoted_file_aux = g_shell_quote (comics_document->archive);
		comics_document->list_command =
			   g_strdup_printf (command_usage_def[type].list,
			                    comics_document->alternative_command,
			                    quoted_file_aux);
		g_free (quoted_file_aux);
	} else {
		quoted_file = g_shell_quote (comics_document->archive);
		comics_document->list_command =
				g_strdup_printf (command_usage_def[type].list,
				                 quoted_command, quoted_file);
	}
	comics_document->extract_command =
			    g_strdup_printf (command_usage_def[type].extract,
				             quoted_command);
	comics_document->offset = command_usage_def[type].offset;
	if (command_usage_def[type].decompress_tmp) {
		comics_document->dir = ev_mkdtemp ("evince-comics-XXXXXX", error);
                if (comics_document->dir == NULL)
                        return FALSE;

		/* unrar-free can't create directories, but ev_mkdtemp already created the dir */

		comics_document->decompress_tmp =
			g_strdup_printf (command_usage_def[type].decompress_tmp, 
					 quoted_command, quoted_file,
					 comics_document->dir);
		g_free (quoted_file);
		g_free (quoted_command);

		if (!comics_decompress_temp_dir (comics_document->decompress_tmp,
		    comics_document->selected_command, error))
			return FALSE;
		else
			return TRUE;
	} else {
		g_free (quoted_file);
		g_free (quoted_command);
		return TRUE;
	}

}
