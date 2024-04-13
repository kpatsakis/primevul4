static void btrfs_retry_endio_nocsum(struct bio *bio)
{
	struct btrfs_retry_complete *done = bio->bi_private;
	struct bio_vec *bvec;
	int i;

	if (bio->bi_error)
		goto end;

	done->uptodate = 1;
	bio_for_each_segment_all(bvec, bio, i)
		clean_io_failure(done->inode, done->start, bvec->bv_page, 0);
end:
	complete(&done->done);
	bio_put(bio);
}
