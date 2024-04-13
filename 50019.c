gsf_infile_tar_finalize (GObject *obj)
{
	GsfInfileTar *tar = GSF_INFILE_TAR (obj);
	g_array_free (tar->children, TRUE);
	parent_class->finalize (obj);
}
