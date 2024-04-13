static int init_blkz_info(struct f2fs_sb_info *sbi, int devi)
{
	struct block_device *bdev = FDEV(devi).bdev;
	sector_t nr_sectors = bdev->bd_part->nr_sects;
	sector_t sector = 0;
	struct blk_zone *zones;
	unsigned int i, nr_zones;
	unsigned int n = 0;
	int err = -EIO;

	if (!f2fs_sb_mounted_blkzoned(sbi->sb))
		return 0;

	if (sbi->blocks_per_blkz && sbi->blocks_per_blkz !=
				SECTOR_TO_BLOCK(bdev_zone_sectors(bdev)))
		return -EINVAL;
	sbi->blocks_per_blkz = SECTOR_TO_BLOCK(bdev_zone_sectors(bdev));
	if (sbi->log_blocks_per_blkz && sbi->log_blocks_per_blkz !=
				__ilog2_u32(sbi->blocks_per_blkz))
		return -EINVAL;
	sbi->log_blocks_per_blkz = __ilog2_u32(sbi->blocks_per_blkz);
	FDEV(devi).nr_blkz = SECTOR_TO_BLOCK(nr_sectors) >>
					sbi->log_blocks_per_blkz;
	if (nr_sectors & (bdev_zone_sectors(bdev) - 1))
		FDEV(devi).nr_blkz++;

	FDEV(devi).blkz_type = kmalloc(FDEV(devi).nr_blkz, GFP_KERNEL);
	if (!FDEV(devi).blkz_type)
		return -ENOMEM;

#define F2FS_REPORT_NR_ZONES   4096

	zones = kcalloc(F2FS_REPORT_NR_ZONES, sizeof(struct blk_zone),
			GFP_KERNEL);
	if (!zones)
		return -ENOMEM;

	/* Get block zones type */
	while (zones && sector < nr_sectors) {

		nr_zones = F2FS_REPORT_NR_ZONES;
		err = blkdev_report_zones(bdev, sector,
					  zones, &nr_zones,
					  GFP_KERNEL);
		if (err)
			break;
		if (!nr_zones) {
			err = -EIO;
			break;
		}

		for (i = 0; i < nr_zones; i++) {
			FDEV(devi).blkz_type[n] = zones[i].type;
			sector += zones[i].len;
			n++;
		}
	}

	kfree(zones);

	return err;
}
