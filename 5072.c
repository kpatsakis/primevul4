static void bbio_error(struct btrfs_bio *bbio, struct bio *bio, u64 logical)
{
	atomic_inc(&bbio->error);
	if (atomic_dec_and_test(&bbio->stripes_pending)) {
		/* Should be the original bio. */
		WARN_ON(bio != bbio->orig_bio);

		btrfs_io_bio(bio)->mirror_num = bbio->mirror_num;
		bio->bi_iter.bi_sector = logical >> 9;
		if (atomic_read(&bbio->error) > bbio->max_errors)
			bio->bi_status = BLK_STS_IOERR;
		else
			bio->bi_status = BLK_STS_OK;
		btrfs_end_bbio(bbio, bio);
	}
}