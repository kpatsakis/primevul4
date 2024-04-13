gsf_infile_tar_dispose (GObject *obj)
{
	GsfInfileTar *tar = GSF_INFILE_TAR (obj);
	unsigned ui;

	gsf_infile_tar_set_source (tar, NULL);
	g_clear_error (&tar->err);

	for (ui = 0; ui < tar->children->len; ui++) {
		TarChild *c = &g_array_index (tar->children, TarChild, ui);
		g_free (c->name);
		if (c->modtime)
			g_date_time_unref (c->modtime);
		if (c->dir)
			g_object_unref (c->dir);
	}
	g_array_set_size (tar->children, 0);

	parent_class->dispose (obj);
}
