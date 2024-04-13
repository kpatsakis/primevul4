gsf_infile_tar_child_by_name (GsfInfile *infile, char const *name, GError **err)
{
	GsfInfileTar *tar = GSF_INFILE_TAR (infile);
	unsigned ui;

	for (ui = 0; ui < tar->children->len; ui++) {
		const TarChild *c = &g_array_index (tar->children,
						    TarChild,
						    ui);
		if (strcmp (name, c->name) == 0)
			return gsf_infile_tar_child_by_index (infile, ui, err);
	}

	return NULL;
}
