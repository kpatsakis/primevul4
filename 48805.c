void check_disk_size_change(struct gendisk *disk, struct block_device *bdev)
{
	loff_t disk_size, bdev_size;

	disk_size = (loff_t)get_capacity(disk) << 9;
	bdev_size = i_size_read(bdev->bd_inode);
	if (disk_size != bdev_size) {
		char name[BDEVNAME_SIZE];

		disk_name(disk, 0, name);
		printk(KERN_INFO
		       "%s: detected capacity change from %lld to %lld\n",
		       name, bdev_size, disk_size);
		i_size_write(bdev->bd_inode, disk_size);
		flush_disk(bdev, false);
	}
}
