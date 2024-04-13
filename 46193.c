static int btrfs_submit_bio_hook(struct inode *inode, int rw, struct bio *bio,
			  int mirror_num, unsigned long bio_flags,
			  u64 bio_offset)
{
	struct btrfs_root *root = BTRFS_I(inode)->root;
	int ret = 0;
	int skip_sum;
	int metadata = 0;
	int async = !atomic_read(&BTRFS_I(inode)->sync_writers);

	skip_sum = BTRFS_I(inode)->flags & BTRFS_INODE_NODATASUM;

	if (btrfs_is_free_space_inode(inode))
		metadata = 2;

	if (!(rw & REQ_WRITE)) {
		ret = btrfs_bio_wq_end_io(root->fs_info, bio, metadata);
		if (ret)
			goto out;

		if (bio_flags & EXTENT_BIO_COMPRESSED) {
			ret = btrfs_submit_compressed_read(inode, bio,
							   mirror_num,
							   bio_flags);
			goto out;
		} else if (!skip_sum) {
			ret = btrfs_lookup_bio_sums(root, inode, bio, NULL);
			if (ret)
				goto out;
		}
		goto mapit;
	} else if (async && !skip_sum) {
		/* csum items have already been cloned */
		if (root->root_key.objectid == BTRFS_DATA_RELOC_TREE_OBJECTID)
			goto mapit;
		/* we're doing a write, do the async checksumming */
		ret = btrfs_wq_submit_bio(BTRFS_I(inode)->root->fs_info,
				   inode, rw, bio, mirror_num,
				   bio_flags, bio_offset,
				   __btrfs_submit_bio_start,
				   __btrfs_submit_bio_done);
		goto out;
	} else if (!skip_sum) {
		ret = btrfs_csum_one_bio(root, inode, bio, 0, 0);
		if (ret)
			goto out;
	}

mapit:
	ret = btrfs_map_bio(root, rw, bio, mirror_num, 0);

out:
	if (ret < 0) {
		bio->bi_error = ret;
		bio_endio(bio);
	}
	return ret;
}
