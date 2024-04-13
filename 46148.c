static void btrfs_endio_direct_read(struct bio *bio)
{
	struct btrfs_dio_private *dip = bio->bi_private;
	struct inode *inode = dip->inode;
	struct bio *dio_bio;
	struct btrfs_io_bio *io_bio = btrfs_io_bio(bio);
	int err = bio->bi_error;

	if (dip->flags & BTRFS_DIO_ORIG_BIO_SUBMITTED)
		err = btrfs_subio_endio_read(inode, io_bio, err);

	unlock_extent(&BTRFS_I(inode)->io_tree, dip->logical_offset,
		      dip->logical_offset + dip->bytes - 1);
	dio_bio = dip->dio_bio;

	kfree(dip);

	dio_end_io(dio_bio, bio->bi_error);

	if (io_bio->end_io)
		io_bio->end_io(io_bio, err);
	bio_put(bio);
}
