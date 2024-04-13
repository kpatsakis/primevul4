gsf_infile_tar_dup (GsfInput *src_input, GError **err)
{
	GsfInfileTar *res, *src;
	unsigned ui;

	src = GSF_INFILE_TAR (src_input);
	if (src->err) {
		if (err)
			*err = g_error_copy (src->err);
		return NULL;
	}

	res = (GsfInfileTar *)g_object_new (GSF_INFILE_TAR_TYPE, NULL);
	gsf_infile_tar_set_source (res, src->source);

	for (ui = 0; ui < src->children->len; ui++) {
		/* This copies the structure.  */
		TarChild c = g_array_index (src->children, TarChild, ui);
		c.name = g_strdup (c.name);
		if (c.modtime) g_date_time_ref (c.modtime);
		if (c.dir) g_object_ref (c.dir);
		g_array_append_val (res->children, c);
	}

	return NULL;
}
