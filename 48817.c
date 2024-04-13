int sb_is_blkdev_sb(struct super_block *sb)
{
	return sb == blockdev_superblock;
}
