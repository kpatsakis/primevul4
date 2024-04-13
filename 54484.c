comics_document_load (EvDocument *document,
		      const char *uri,
		      GError    **error)
{
	ComicsDocument *comics_document = COMICS_DOCUMENT (document);
	GSList *supported_extensions;
	gchar *std_out;
	gchar *mime_type;
	gchar **cb_files, *cb_file;
	gboolean success;
	int i, retval;
	GError *err = NULL;

	comics_document->archive = g_filename_from_uri (uri, NULL, error);
	if (!comics_document->archive)
		return FALSE;

	mime_type = ev_file_get_mime_type (uri, FALSE, &err);
	if (mime_type == NULL)
		return FALSE;
	
	if (!comics_check_decompress_command (mime_type, comics_document, 
	error)) {	
		g_free (mime_type);
		return FALSE;
	} else if (!comics_generate_command_lines (comics_document, error)) {
		   g_free (mime_type);
		return FALSE;
	}

	g_free (mime_type);

	/* Get list of files in archive */
	success = g_spawn_command_line_sync (comics_document->list_command,
					     &std_out, NULL, &retval, error);

	if (!success) {
		return FALSE;
	} else if (!WIFEXITED(retval) || WEXITSTATUS(retval) != EXIT_SUCCESS) {
		g_set_error_literal (error,
                                     EV_DOCUMENT_ERROR,
                                     EV_DOCUMENT_ERROR_INVALID,
                                     _("File corrupted"));
		return FALSE;
	}

	/* FIXME: is this safe against filenames containing \n in the archive ? */
	cb_files = g_strsplit (std_out, EV_EOL, 0);

	g_free (std_out);

	if (!cb_files) {
		g_set_error_literal (error,
				     EV_DOCUMENT_ERROR,
				     EV_DOCUMENT_ERROR_INVALID,
				     _("No files in archive"));
		return FALSE;
	}

        comics_document->page_names = g_ptr_array_sized_new (64);

	supported_extensions = get_supported_image_extensions ();
	for (i = 0; cb_files[i] != NULL; i++) {
		if (comics_document->offset != NO_OFFSET) {
			if (g_utf8_strlen (cb_files[i],-1) > 
			    comics_document->offset) {
				cb_file = 
					g_utf8_offset_to_pointer (cb_files[i], 
						       comics_document->offset);
			} else {
				continue;
			}
		} else {
			cb_file = cb_files[i];
		}
		gchar *suffix = g_strrstr (cb_file, ".");
		if (!suffix)
			continue;
		suffix = g_ascii_strdown (suffix + 1, -1);
		if (g_slist_find_custom (supported_extensions, suffix,
					 (GCompareFunc) strcmp) != NULL) {
                        g_ptr_array_add (comics_document->page_names,
                                         g_strstrip (g_strdup (cb_file)));
		}
		g_free (suffix);
	}
	g_strfreev (cb_files);
	g_slist_foreach (supported_extensions, (GFunc) g_free, NULL);
	g_slist_free (supported_extensions);

	if (comics_document->page_names->len == 0) {
		g_set_error (error,
			     EV_DOCUMENT_ERROR,
			     EV_DOCUMENT_ERROR_INVALID,
			     _("No images found in archive %s"),
			     uri);
		return FALSE;
	}

        /* Now sort the pages */
        g_ptr_array_sort (comics_document->page_names, sort_page_names);

	return TRUE;
}
