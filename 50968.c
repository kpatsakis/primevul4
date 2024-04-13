static void ffs_data_put(struct ffs_data *ffs)
{
	ENTER();

	if (unlikely(atomic_dec_and_test(&ffs->ref))) {
		pr_info("%s(): freeing\n", __func__);
		ffs_data_clear(ffs);
		BUG_ON(waitqueue_active(&ffs->ev.waitq) ||
		       waitqueue_active(&ffs->ep0req_completion.wait));
		kfree(ffs->dev_name);
		kfree(ffs);
	}
}
