static void md_release(struct gendisk *disk, fmode_t mode)
{
	struct mddev *mddev = disk->private_data;

	BUG_ON(!mddev);
	atomic_dec(&mddev->openers);
	mddev_put(mddev);
}
