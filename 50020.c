gsf_infile_tar_name_by_index (GsfInfile *infile, int target)
{
	GsfInfileTar *tar = GSF_INFILE_TAR (infile);

	if (target < 0 || (unsigned)target >= tar->children->len)
		return NULL;

	return g_array_index (tar->children, TarChild, target).name;
}
