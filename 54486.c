comics_document_render_pixbuf (EvDocument      *document,
			       EvRenderContext *rc)
{
	GdkPixbufLoader *loader;
	GdkPixbuf *rotated_pixbuf, *tmp_pixbuf;
	char **argv;
	guchar buf[4096];
	gboolean success;
	gint outpipe = -1;
	GPid child_pid;
	gssize bytes;
	gint width, height;
	gchar *filename;
	ComicsDocument *comics_document = COMICS_DOCUMENT (document);
	
	if (!comics_document->decompress_tmp) {
		argv = extract_argv (document, rc->page->index);
		success = g_spawn_async_with_pipes (NULL, argv, NULL,
						    G_SPAWN_SEARCH_PATH | 
						    G_SPAWN_STDERR_TO_DEV_NULL,
						    NULL, NULL,
						    &child_pid,
						    NULL, &outpipe, NULL, NULL);
		g_strfreev (argv);
		g_return_val_if_fail (success == TRUE, NULL);

		loader = gdk_pixbuf_loader_new ();
		g_signal_connect (loader, "size-prepared",
				  G_CALLBACK (render_pixbuf_size_prepared_cb), 
				  rc);

		while (outpipe >= 0) {
			bytes = read (outpipe, buf, 4096);

			if (bytes > 0) {
				gdk_pixbuf_loader_write (loader, buf, bytes, 
				NULL);
			} else if (bytes <= 0) {
				close (outpipe);
				gdk_pixbuf_loader_close (loader, NULL);
				outpipe = -1;
			}
		}
		tmp_pixbuf = gdk_pixbuf_loader_get_pixbuf (loader);
		rotated_pixbuf =
			gdk_pixbuf_rotate_simple (tmp_pixbuf,
						  360 - rc->rotation);
		g_spawn_close_pid (child_pid);
		g_object_unref (loader);
	} else {
		int scaled_width, scaled_height;

		filename = 
			g_build_filename (comics_document->dir,
                                          (char *) comics_document->page_names->pdata[rc->page->index],
					  NULL);
	   
		gdk_pixbuf_get_file_info (filename, &width, &height);

		ev_render_context_compute_scaled_size (rc, width, height,
						       &scaled_width, &scaled_height);
		
		tmp_pixbuf =
			gdk_pixbuf_new_from_file_at_size (
				    filename, scaled_width, scaled_height, NULL);
		rotated_pixbuf =
			gdk_pixbuf_rotate_simple (tmp_pixbuf,
						  360 - rc->rotation);
		g_free (filename);
		g_object_unref (tmp_pixbuf);
	}
	return rotated_pixbuf;
}
