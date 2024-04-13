static int cuse_open(struct inode *inode, struct file *file)
{
	dev_t devt = inode->i_cdev->dev;
	struct cuse_conn *cc = NULL, *pos;
	int rc;

	/* look up and get the connection */
	mutex_lock(&cuse_lock);
	list_for_each_entry(pos, cuse_conntbl_head(devt), list)
		if (pos->dev->devt == devt) {
			fuse_conn_get(&pos->fc);
			cc = pos;
			break;
		}
	mutex_unlock(&cuse_lock);

	/* dead? */
	if (!cc)
		return -ENODEV;

	/*
	 * Generic permission check is already done against the chrdev
	 * file, proceed to open.
	 */
	rc = fuse_do_open(&cc->fc, 0, file, 0);
	if (rc)
		fuse_conn_put(&cc->fc);
	return rc;
}
