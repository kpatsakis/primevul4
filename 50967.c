static void ffs_data_opened(struct ffs_data *ffs)
{
	ENTER();

	atomic_inc(&ffs->ref);
	if (atomic_add_return(1, &ffs->opened) == 1 &&
			ffs->state == FFS_DEACTIVATED) {
		ffs->state = FFS_CLOSING;
		ffs_data_reset(ffs);
	}
}
