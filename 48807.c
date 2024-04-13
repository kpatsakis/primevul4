static void flush_disk(struct block_device *bdev, bool kill_dirty)
{
	if (__invalidate_device(bdev, kill_dirty)) {
		char name[BDEVNAME_SIZE] = "";

		if (bdev->bd_disk)
			disk_name(bdev->bd_disk, 0, name);
		printk(KERN_WARNING "VFS: busy inodes on changed media or "
		       "resized disk %s\n", name);
	}

	if (!bdev->bd_disk)
		return;
	if (disk_part_scan_enabled(bdev->bd_disk))
		bdev->bd_invalidated = 1;
}
