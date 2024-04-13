static void md_free(struct kobject *ko)
{
	struct mddev *mddev = container_of(ko, struct mddev, kobj);

	if (mddev->sysfs_state)
		sysfs_put(mddev->sysfs_state);

	if (mddev->queue)
		blk_cleanup_queue(mddev->queue);
	if (mddev->gendisk) {
		del_gendisk(mddev->gendisk);
		put_disk(mddev->gendisk);
	}

	kfree(mddev);
}
