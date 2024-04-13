get_supported_image_extensions(void)
{
	GSList *extensions = NULL;
	GSList *formats = gdk_pixbuf_get_formats ();
	GSList *l;

	for (l = formats; l != NULL; l = l->next) {
		int i;
		gchar **ext = gdk_pixbuf_format_get_extensions (l->data);

		for (i = 0; ext[i] != NULL; i++) {
			extensions = g_slist_append (extensions,
						     g_strdup (ext[i]));
		}

		g_strfreev (ext);
	}

	g_slist_free (formats);
	return extensions;
}
