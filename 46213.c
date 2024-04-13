static int dio_read_error(struct inode *inode, struct bio *failed_bio,
			  struct page *page, u64 start, u64 end,
			  int failed_mirror, bio_end_io_t *repair_endio,
			  void *repair_arg)
{
	struct io_failure_record *failrec;
	struct bio *bio;
	int isector;
	int read_mode;
	int ret;

	BUG_ON(failed_bio->bi_rw & REQ_WRITE);

	ret = btrfs_get_io_failure_record(inode, start, end, &failrec);
	if (ret)
		return ret;

	ret = btrfs_check_dio_repairable(inode, failed_bio, failrec,
					 failed_mirror);
	if (!ret) {
		free_io_failure(inode, failrec);
		return -EIO;
	}

	if (failed_bio->bi_vcnt > 1)
		read_mode = READ_SYNC | REQ_FAILFAST_DEV;
	else
		read_mode = READ_SYNC;

	isector = start - btrfs_io_bio(failed_bio)->logical;
	isector >>= inode->i_sb->s_blocksize_bits;
	bio = btrfs_create_repair_bio(inode, failed_bio, failrec, page,
				      0, isector, repair_endio, repair_arg);
	if (!bio) {
		free_io_failure(inode, failrec);
		return -EIO;
	}

	btrfs_debug(BTRFS_I(inode)->root->fs_info,
		    "Repair DIO Read Error: submitting new dio read[%#x] to this_mirror=%d, in_validation=%d\n",
		    read_mode, failrec->this_mirror, failrec->in_validation);

	ret = submit_dio_repair_bio(inode, bio, read_mode,
				    failrec->this_mirror);
	if (ret) {
		free_io_failure(inode, failrec);
		bio_put(bio);
	}

	return ret;
}
