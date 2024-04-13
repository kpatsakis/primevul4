static void ffs_closed(struct ffs_data *ffs)
{
	struct ffs_dev *ffs_obj;
	struct f_fs_opts *opts;

	ENTER();
	ffs_dev_lock();

	ffs_obj = ffs->private_data;
	if (!ffs_obj)
		goto done;

	ffs_obj->desc_ready = false;

	if (test_and_clear_bit(FFS_FL_CALL_CLOSED_CALLBACK, &ffs->flags) &&
	    ffs_obj->ffs_closed_callback)
		ffs_obj->ffs_closed_callback(ffs);

	if (ffs_obj->opts)
		opts = ffs_obj->opts;
	else
		goto done;

	if (opts->no_configfs || !opts->func_inst.group.cg_item.ci_parent
	    || !atomic_read(&opts->func_inst.group.cg_item.ci_kref.refcount))
		goto done;

	unregister_gadget_item(ffs_obj->opts->
			       func_inst.group.cg_item.ci_parent->ci_parent);
done:
	ffs_dev_unlock();
}
