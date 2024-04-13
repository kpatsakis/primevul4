int revalidate_disk(struct gendisk *disk)
{
	struct block_device *bdev;
	int ret = 0;

	if (disk->fops->revalidate_disk)
		ret = disk->fops->revalidate_disk(disk);

	bdev = bdget_disk(disk, 0);
	if (!bdev)
		return ret;

	mutex_lock(&bdev->bd_mutex);
	check_disk_size_change(disk, bdev);
	bdev->bd_invalidated = 0;
	mutex_unlock(&bdev->bd_mutex);
	bdput(bdev);
	return ret;
}
