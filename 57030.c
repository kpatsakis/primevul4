static void destroy_device_list(struct f2fs_sb_info *sbi)
{
	int i;

	for (i = 0; i < sbi->s_ndevs; i++) {
		blkdev_put(FDEV(i).bdev, FMODE_EXCL);
#ifdef CONFIG_BLK_DEV_ZONED
		kfree(FDEV(i).blkz_type);
#endif
	}
	kfree(sbi->devs);
}
