static void ffs_reset_work(struct work_struct *work)
{
	struct ffs_data *ffs = container_of(work,
		struct ffs_data, reset_work);
	ffs_data_reset(ffs);
}
