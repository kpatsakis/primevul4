gsf_infile_tar_child_by_index (GsfInfile *infile, int target, GError **err)
{
	GsfInfileTar *tar = GSF_INFILE_TAR (infile);
	const TarChild *c;

	if (err)
		*err = NULL;

	if (target < 0 || (unsigned)target >= tar->children->len)
		return NULL;

	c = &g_array_index (tar->children, TarChild, target);
	if (c->dir)
		return g_object_ref (c->dir);
	else {
		GsfInput *input = gsf_input_proxy_new_section (tar->source,
							       c->offset,
							       c->length);
		gsf_input_set_modtime (input, c->modtime);
		gsf_input_set_name (input, c->name);
		return input;
	}
}
