static struct bio *btrfs_dio_bio_alloc(struct block_device *bdev,
				       u64 first_sector, gfp_t gfp_flags)
{
	struct bio *bio;
	bio = btrfs_bio_alloc(bdev, first_sector, BIO_MAX_PAGES, gfp_flags);
	if (bio)
		bio_associate_current(bio);
	return bio;
}
