static int btrfs_check_dio_repairable(struct inode *inode,
				      struct bio *failed_bio,
				      struct io_failure_record *failrec,
				      int failed_mirror)
{
	int num_copies;

	num_copies = btrfs_num_copies(BTRFS_I(inode)->root->fs_info,
				      failrec->logical, failrec->len);
	if (num_copies == 1) {
		/*
		 * we only have a single copy of the data, so don't bother with
		 * all the retry and error correction code that follows. no
		 * matter what the error is, it is very likely to persist.
		 */
		pr_debug("Check DIO Repairable: cannot repair, num_copies=%d, next_mirror %d, failed_mirror %d\n",
			 num_copies, failrec->this_mirror, failed_mirror);
		return 0;
	}

	failrec->failed_mirror = failed_mirror;
	failrec->this_mirror++;
	if (failrec->this_mirror == failed_mirror)
		failrec->this_mirror++;

	if (failrec->this_mirror > num_copies) {
		pr_debug("Check DIO Repairable: (fail) num_copies=%d, next_mirror %d, failed_mirror %d\n",
			 num_copies, failrec->this_mirror, failed_mirror);
		return 0;
	}

	return 1;
}
