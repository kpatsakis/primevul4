int sync_page_io(struct md_rdev *rdev, sector_t sector, int size,
		 struct page *page, int rw, bool metadata_op)
{
	struct bio *bio = bio_alloc_mddev(GFP_NOIO, 1, rdev->mddev);
	int ret;

	bio->bi_bdev = (metadata_op && rdev->meta_bdev) ?
		rdev->meta_bdev : rdev->bdev;
	if (metadata_op)
		bio->bi_iter.bi_sector = sector + rdev->sb_start;
	else if (rdev->mddev->reshape_position != MaxSector &&
		 (rdev->mddev->reshape_backwards ==
		  (sector >= rdev->mddev->reshape_position)))
		bio->bi_iter.bi_sector = sector + rdev->new_data_offset;
	else
		bio->bi_iter.bi_sector = sector + rdev->data_offset;
	bio_add_page(bio, page, size, 0);
	submit_bio_wait(rw, bio);

	ret = test_bit(BIO_UPTODATE, &bio->bi_flags);
	bio_put(bio);
	return ret;
}
