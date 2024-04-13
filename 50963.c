static void ffs_data_clear(struct ffs_data *ffs)
{
	ENTER();

	ffs_closed(ffs);

	BUG_ON(ffs->gadget);

	if (ffs->epfiles)
		ffs_epfiles_destroy(ffs->epfiles, ffs->eps_count);

	if (ffs->ffs_eventfd)
		eventfd_ctx_put(ffs->ffs_eventfd);

	kfree(ffs->raw_descs_data);
	kfree(ffs->raw_strings);
	kfree(ffs->stringtabs);
}
