static void __blkdev_put(struct block_device *bdev, fmode_t mode, int for_part)
{
	struct gendisk *disk = bdev->bd_disk;
	struct block_device *victim = NULL;

	mutex_lock_nested(&bdev->bd_mutex, for_part);
	if (for_part)
		bdev->bd_part_count--;

	if (!--bdev->bd_openers) {
		WARN_ON_ONCE(bdev->bd_holders);
		sync_blockdev(bdev);
		kill_bdev(bdev);
		/* ->release can cause the old bdi to disappear,
		 * so must switch it out first
		 */
		bdev_inode_switch_bdi(bdev->bd_inode,
					&default_backing_dev_info);
	}
	if (bdev->bd_contains == bdev) {
		if (disk->fops->release)
			disk->fops->release(disk, mode);
	}
	if (!bdev->bd_openers) {
		struct module *owner = disk->fops->owner;

		disk_put_part(bdev->bd_part);
		bdev->bd_part = NULL;
		bdev->bd_disk = NULL;
		if (bdev != bdev->bd_contains)
			victim = bdev->bd_contains;
		bdev->bd_contains = NULL;

		put_disk(disk);
		module_put(owner);
	}
	mutex_unlock(&bdev->bd_mutex);
	bdput(bdev);
	if (victim)
		__blkdev_put(victim, mode, 1);
}
