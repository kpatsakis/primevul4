static inline int btrfs_lookup_and_bind_dio_csum(struct btrfs_root *root,
						 struct inode *inode,
						 struct btrfs_dio_private *dip,
						 struct bio *bio,
						 u64 file_offset)
{
	struct btrfs_io_bio *io_bio = btrfs_io_bio(bio);
	struct btrfs_io_bio *orig_io_bio = btrfs_io_bio(dip->orig_bio);
	int ret;

	/*
	 * We load all the csum data we need when we submit
	 * the first bio to reduce the csum tree search and
	 * contention.
	 */
	if (dip->logical_offset == file_offset) {
		ret = btrfs_lookup_bio_sums_dio(root, inode, dip->orig_bio,
						file_offset);
		if (ret)
			return ret;
	}

	if (bio == dip->orig_bio)
		return 0;

	file_offset -= dip->logical_offset;
	file_offset >>= inode->i_sb->s_blocksize_bits;
	io_bio->csum = (u8 *)(((u32 *)orig_io_bio->csum) + file_offset);

	return 0;
}
