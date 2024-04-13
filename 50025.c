tar_create_dir (GsfInfileTar *dir, const char *name)
{
	TarChild c;

	c.offset = 0;
	c.length = 0;
	c.name = g_strdup (name);
	c.modtime = NULL;
	c.dir = g_object_new (GSF_INFILE_TAR_TYPE, NULL);

	/*
	 * We set the source here, so gsf_infile_tar_constructor doesn't
	 * start reading the tarfile recursively.
	 */
	gsf_infile_tar_set_source (c.dir, dir->source);

	gsf_input_set_name (GSF_INPUT (c.dir), name);

	g_array_append_val (dir->children, c);

	return c.dir;
}
