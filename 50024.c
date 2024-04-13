gsf_infile_tar_set_source (GsfInfileTar *tar, GsfInput *src)
{
	if (src)
		src = gsf_input_proxy_new (src);
	if (tar->source)
		g_object_unref (tar->source);
	tar->source = src;
}
