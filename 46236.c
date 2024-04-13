static inline int submit_dio_repair_bio(struct inode *inode, struct bio *bio,
					int rw, int mirror_num)
{
	struct btrfs_root *root = BTRFS_I(inode)->root;
	int ret;

	BUG_ON(rw & REQ_WRITE);

	bio_get(bio);

	ret = btrfs_bio_wq_end_io(root->fs_info, bio,
				  BTRFS_WQ_ENDIO_DIO_REPAIR);
	if (ret)
		goto err;

	ret = btrfs_map_bio(root, rw, bio, mirror_num, 0);
err:
	bio_put(bio);
	return ret;
}
