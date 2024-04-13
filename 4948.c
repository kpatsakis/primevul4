void btrfs_scratch_superblocks(struct btrfs_fs_info *fs_info,
			       struct block_device *bdev,
			       const char *device_path)
{
	struct btrfs_super_block *disk_super;
	int copy_num;

	if (!bdev)
		return;

	for (copy_num = 0; copy_num < BTRFS_SUPER_MIRROR_MAX; copy_num++) {
		struct page *page;
		int ret;

		disk_super = btrfs_read_dev_one_super(bdev, copy_num);
		if (IS_ERR(disk_super))
			continue;

		if (bdev_is_zoned(bdev)) {
			btrfs_reset_sb_log_zones(bdev, copy_num);
			continue;
		}

		memset(&disk_super->magic, 0, sizeof(disk_super->magic));

		page = virt_to_page(disk_super);
		set_page_dirty(page);
		lock_page(page);
		/* write_on_page() unlocks the page */
		ret = write_one_page(page);
		if (ret)
			btrfs_warn(fs_info,
				"error clearing superblock number %d (%d)",
				copy_num, ret);
		btrfs_release_disk_super(disk_super);

	}

	/* Notify udev that device has changed */
	btrfs_kobject_uevent(bdev, KOBJ_CHANGE);

	/* Update ctime/mtime for device path for libblkid */
	update_dev_time(device_path);
}