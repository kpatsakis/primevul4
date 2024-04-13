gsf_infile_tar_num_children (GsfInfile *infile)
{
	GsfInfileTar *tar = GSF_INFILE_TAR (infile);

	return tar->children->len;
}
