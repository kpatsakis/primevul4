static inline sector_t calc_dev_sboffset(struct md_rdev *rdev)
{
	sector_t num_sectors = i_size_read(rdev->bdev->bd_inode) / 512;
	return MD_NEW_SIZE_SECTORS(num_sectors);
}
