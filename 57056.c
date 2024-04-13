static int f2fs_scan_devices(struct f2fs_sb_info *sbi)
{
	struct f2fs_super_block *raw_super = F2FS_RAW_SUPER(sbi);
	unsigned int max_devices = MAX_DEVICES;
	int i;

	/* Initialize single device information */
	if (!RDEV(0).path[0]) {
		if (!bdev_is_zoned(sbi->sb->s_bdev))
			return 0;
		max_devices = 1;
	}

	/*
	 * Initialize multiple devices information, or single
	 * zoned block device information.
	 */
	sbi->devs = kcalloc(max_devices, sizeof(struct f2fs_dev_info),
				GFP_KERNEL);
	if (!sbi->devs)
		return -ENOMEM;

	for (i = 0; i < max_devices; i++) {

		if (i > 0 && !RDEV(i).path[0])
			break;

		if (max_devices == 1) {
			/* Single zoned block device mount */
			FDEV(0).bdev =
				blkdev_get_by_dev(sbi->sb->s_bdev->bd_dev,
					sbi->sb->s_mode, sbi->sb->s_type);
		} else {
			/* Multi-device mount */
			memcpy(FDEV(i).path, RDEV(i).path, MAX_PATH_LEN);
			FDEV(i).total_segments =
				le32_to_cpu(RDEV(i).total_segments);
			if (i == 0) {
				FDEV(i).start_blk = 0;
				FDEV(i).end_blk = FDEV(i).start_blk +
				    (FDEV(i).total_segments <<
				    sbi->log_blocks_per_seg) - 1 +
				    le32_to_cpu(raw_super->segment0_blkaddr);
			} else {
				FDEV(i).start_blk = FDEV(i - 1).end_blk + 1;
				FDEV(i).end_blk = FDEV(i).start_blk +
					(FDEV(i).total_segments <<
					sbi->log_blocks_per_seg) - 1;
			}
			FDEV(i).bdev = blkdev_get_by_path(FDEV(i).path,
					sbi->sb->s_mode, sbi->sb->s_type);
		}
		if (IS_ERR(FDEV(i).bdev))
			return PTR_ERR(FDEV(i).bdev);

		/* to release errored devices */
		sbi->s_ndevs = i + 1;

#ifdef CONFIG_BLK_DEV_ZONED
		if (bdev_zoned_model(FDEV(i).bdev) == BLK_ZONED_HM &&
				!f2fs_sb_mounted_blkzoned(sbi->sb)) {
			f2fs_msg(sbi->sb, KERN_ERR,
				"Zoned block device feature not enabled\n");
			return -EINVAL;
		}
		if (bdev_zoned_model(FDEV(i).bdev) != BLK_ZONED_NONE) {
			if (init_blkz_info(sbi, i)) {
				f2fs_msg(sbi->sb, KERN_ERR,
					"Failed to initialize F2FS blkzone information");
				return -EINVAL;
			}
			if (max_devices == 1)
				break;
			f2fs_msg(sbi->sb, KERN_INFO,
				"Mount Device [%2d]: %20s, %8u, %8x - %8x (zone: %s)",
				i, FDEV(i).path,
				FDEV(i).total_segments,
				FDEV(i).start_blk, FDEV(i).end_blk,
				bdev_zoned_model(FDEV(i).bdev) == BLK_ZONED_HA ?
				"Host-aware" : "Host-managed");
			continue;
		}
#endif
		f2fs_msg(sbi->sb, KERN_INFO,
			"Mount Device [%2d]: %20s, %8u, %8x - %8x",
				i, FDEV(i).path,
				FDEV(i).total_segments,
				FDEV(i).start_blk, FDEV(i).end_blk);
	}
	f2fs_msg(sbi->sb, KERN_INFO,
			"IO Block Size: %8d KB", F2FS_IO_SIZE_KB(sbi));
	return 0;
}
