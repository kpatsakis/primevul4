static void btrfs_retry_endio(struct bio *bio)
{
	struct btrfs_retry_complete *done = bio->bi_private;
	struct btrfs_io_bio *io_bio = btrfs_io_bio(bio);
	struct bio_vec *bvec;
	int uptodate;
	int ret;
	int i;

	if (bio->bi_error)
		goto end;

	uptodate = 1;
	bio_for_each_segment_all(bvec, bio, i) {
		ret = __readpage_endio_check(done->inode, io_bio, i,
					     bvec->bv_page, 0,
					     done->start, bvec->bv_len);
		if (!ret)
			clean_io_failure(done->inode, done->start,
					 bvec->bv_page, 0);
		else
			uptodate = 0;
	}

	done->uptodate = uptodate;
end:
	complete(&done->done);
	bio_put(bio);
}
