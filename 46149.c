static void btrfs_endio_direct_write(struct bio *bio)
{
	struct btrfs_dio_private *dip = bio->bi_private;
	struct inode *inode = dip->inode;
	struct btrfs_root *root = BTRFS_I(inode)->root;
	struct btrfs_ordered_extent *ordered = NULL;
	u64 ordered_offset = dip->logical_offset;
	u64 ordered_bytes = dip->bytes;
	struct bio *dio_bio;
	int ret;

again:
	ret = btrfs_dec_test_first_ordered_pending(inode, &ordered,
						   &ordered_offset,
						   ordered_bytes,
						   !bio->bi_error);
	if (!ret)
		goto out_test;

	btrfs_init_work(&ordered->work, btrfs_endio_write_helper,
			finish_ordered_fn, NULL, NULL);
	btrfs_queue_work(root->fs_info->endio_write_workers,
			 &ordered->work);
out_test:
	/*
	 * our bio might span multiple ordered extents.  If we haven't
	 * completed the accounting for the whole dio, go back and try again
	 */
	if (ordered_offset < dip->logical_offset + dip->bytes) {
		ordered_bytes = dip->logical_offset + dip->bytes -
			ordered_offset;
		ordered = NULL;
		goto again;
	}
	dio_bio = dip->dio_bio;

	kfree(dip);

	dio_end_io(dio_bio, bio->bi_error);
	bio_put(bio);
}
