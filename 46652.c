static int md_open(struct block_device *bdev, fmode_t mode)
{
	/*
	 * Succeed if we can lock the mddev, which confirms that
	 * it isn't being stopped right now.
	 */
	struct mddev *mddev = mddev_find(bdev->bd_dev);
	int err;

	if (!mddev)
		return -ENODEV;

	if (mddev->gendisk != bdev->bd_disk) {
		/* we are racing with mddev_put which is discarding this
		 * bd_disk.
		 */
		mddev_put(mddev);
		/* Wait until bdev->bd_disk is definitely gone */
		flush_workqueue(md_misc_wq);
		/* Then retry the open from the top */
		return -ERESTARTSYS;
	}
	BUG_ON(mddev != bdev->bd_disk->private_data);

	if ((err = mutex_lock_interruptible(&mddev->open_mutex)))
		goto out;

	err = 0;
	atomic_inc(&mddev->openers);
	clear_bit(MD_STILL_CLOSED, &mddev->flags);
	mutex_unlock(&mddev->open_mutex);

	check_disk_change(bdev);
 out:
	return err;
}
