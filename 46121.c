static inline int __btrfs_submit_dio_bio(struct bio *bio, struct inode *inode,
					 int rw, u64 file_offset, int skip_sum,
					 int async_submit)
{
	struct btrfs_dio_private *dip = bio->bi_private;
	int write = rw & REQ_WRITE;
	struct btrfs_root *root = BTRFS_I(inode)->root;
	int ret;

	if (async_submit)
		async_submit = !atomic_read(&BTRFS_I(inode)->sync_writers);

	bio_get(bio);

	if (!write) {
		ret = btrfs_bio_wq_end_io(root->fs_info, bio,
				BTRFS_WQ_ENDIO_DATA);
		if (ret)
			goto err;
	}

	if (skip_sum)
		goto map;

	if (write && async_submit) {
		ret = btrfs_wq_submit_bio(root->fs_info,
				   inode, rw, bio, 0, 0,
				   file_offset,
				   __btrfs_submit_bio_start_direct_io,
				   __btrfs_submit_bio_done);
		goto err;
	} else if (write) {
		/*
		 * If we aren't doing async submit, calculate the csum of the
		 * bio now.
		 */
		ret = btrfs_csum_one_bio(root, inode, bio, file_offset, 1);
		if (ret)
			goto err;
	} else {
		ret = btrfs_lookup_and_bind_dio_csum(root, inode, dip, bio,
						     file_offset);
		if (ret)
			goto err;
	}
map:
	ret = btrfs_map_bio(root, rw, bio, 0, async_submit);
err:
	bio_put(bio);
	return ret;
}
