static void ffs_data_get(struct ffs_data *ffs)
{
	ENTER();

	atomic_inc(&ffs->ref);
}
