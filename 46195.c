static int btrfs_submit_direct_hook(int rw, struct btrfs_dio_private *dip,
				    int skip_sum)
{
	struct inode *inode = dip->inode;
	struct btrfs_root *root = BTRFS_I(inode)->root;
	struct bio *bio;
	struct bio *orig_bio = dip->orig_bio;
	struct bio_vec *bvec = orig_bio->bi_io_vec;
	u64 start_sector = orig_bio->bi_iter.bi_sector;
	u64 file_offset = dip->logical_offset;
	u64 submit_len = 0;
	u64 map_length;
	int nr_pages = 0;
	int ret;
	int async_submit = 0;

	map_length = orig_bio->bi_iter.bi_size;
	ret = btrfs_map_block(root->fs_info, rw, start_sector << 9,
			      &map_length, NULL, 0);
	if (ret)
		return -EIO;

	if (map_length >= orig_bio->bi_iter.bi_size) {
		bio = orig_bio;
		dip->flags |= BTRFS_DIO_ORIG_BIO_SUBMITTED;
		goto submit;
	}

	/* async crcs make it difficult to collect full stripe writes. */
	if (btrfs_get_alloc_profile(root, 1) & BTRFS_BLOCK_GROUP_RAID56_MASK)
		async_submit = 0;
	else
		async_submit = 1;

	bio = btrfs_dio_bio_alloc(orig_bio->bi_bdev, start_sector, GFP_NOFS);
	if (!bio)
		return -ENOMEM;

	bio->bi_private = dip;
	bio->bi_end_io = btrfs_end_dio_bio;
	btrfs_io_bio(bio)->logical = file_offset;
	atomic_inc(&dip->pending_bios);

	while (bvec <= (orig_bio->bi_io_vec + orig_bio->bi_vcnt - 1)) {
		if (map_length < submit_len + bvec->bv_len ||
		    bio_add_page(bio, bvec->bv_page, bvec->bv_len,
				 bvec->bv_offset) < bvec->bv_len) {
			/*
			 * inc the count before we submit the bio so
			 * we know the end IO handler won't happen before
			 * we inc the count. Otherwise, the dip might get freed
			 * before we're done setting it up
			 */
			atomic_inc(&dip->pending_bios);
			ret = __btrfs_submit_dio_bio(bio, inode, rw,
						     file_offset, skip_sum,
						     async_submit);
			if (ret) {
				bio_put(bio);
				atomic_dec(&dip->pending_bios);
				goto out_err;
			}

			start_sector += submit_len >> 9;
			file_offset += submit_len;

			submit_len = 0;
			nr_pages = 0;

			bio = btrfs_dio_bio_alloc(orig_bio->bi_bdev,
						  start_sector, GFP_NOFS);
			if (!bio)
				goto out_err;
			bio->bi_private = dip;
			bio->bi_end_io = btrfs_end_dio_bio;
			btrfs_io_bio(bio)->logical = file_offset;

			map_length = orig_bio->bi_iter.bi_size;
			ret = btrfs_map_block(root->fs_info, rw,
					      start_sector << 9,
					      &map_length, NULL, 0);
			if (ret) {
				bio_put(bio);
				goto out_err;
			}
		} else {
			submit_len += bvec->bv_len;
			nr_pages++;
			bvec++;
		}
	}

submit:
	ret = __btrfs_submit_dio_bio(bio, inode, rw, file_offset, skip_sum,
				     async_submit);
	if (!ret)
		return 0;

	bio_put(bio);
out_err:
	dip->errors = 1;
	/*
	 * before atomic variable goto zero, we must
	 * make sure dip->errors is perceived to be set.
	 */
	smp_mb__before_atomic();
	if (atomic_dec_and_test(&dip->pending_bios))
		bio_io_error(dip->orig_bio);

	/* bio_end_io() will handle error, so we needn't return it */
	return 0;
}
