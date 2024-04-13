static int cuse_channel_open(struct inode *inode, struct file *file)
{
	struct fuse_dev *fud;
	struct cuse_conn *cc;
	int rc;

	/* set up cuse_conn */
	cc = kzalloc(sizeof(*cc), GFP_KERNEL);
	if (!cc)
		return -ENOMEM;

	fuse_conn_init(&cc->fc);

	fud = fuse_dev_alloc(&cc->fc);
	if (!fud) {
		kfree(cc);
		return -ENOMEM;
	}

	INIT_LIST_HEAD(&cc->list);
	cc->fc.release = cuse_fc_release;

	cc->fc.initialized = 1;
	rc = cuse_send_init(cc);
	if (rc) {
		fuse_dev_free(fud);
		return rc;
	}
	file->private_data = fud;

	return 0;
}
