static int ffs_ready(struct ffs_data *ffs)
{
	struct ffs_dev *ffs_obj;
	int ret = 0;

	ENTER();
	ffs_dev_lock();

	ffs_obj = ffs->private_data;
	if (!ffs_obj) {
		ret = -EINVAL;
		goto done;
	}
	if (WARN_ON(ffs_obj->desc_ready)) {
		ret = -EBUSY;
		goto done;
	}

	ffs_obj->desc_ready = true;
	ffs_obj->ffs_data = ffs;

	if (ffs_obj->ffs_ready_callback) {
		ret = ffs_obj->ffs_ready_callback(ffs);
		if (ret)
			goto done;
	}

	set_bit(FFS_FL_CALL_CLOSED_CALLBACK, &ffs->flags);
done:
	ffs_dev_unlock();
	return ret;
}
