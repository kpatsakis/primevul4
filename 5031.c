static void submit_stripe_bio(struct btrfs_bio *bbio, struct bio *bio,
			      u64 physical, struct btrfs_device *dev)
{
	struct btrfs_fs_info *fs_info = bbio->fs_info;

	bio->bi_private = bbio;
	btrfs_io_bio(bio)->device = dev;
	bio->bi_end_io = btrfs_end_bio;
	bio->bi_iter.bi_sector = physical >> 9;
	/*
	 * For zone append writing, bi_sector must point the beginning of the
	 * zone
	 */
	if (bio_op(bio) == REQ_OP_ZONE_APPEND) {
		if (btrfs_dev_is_sequential(dev, physical)) {
			u64 zone_start = round_down(physical, fs_info->zone_size);

			bio->bi_iter.bi_sector = zone_start >> SECTOR_SHIFT;
		} else {
			bio->bi_opf &= ~REQ_OP_ZONE_APPEND;
			bio->bi_opf |= REQ_OP_WRITE;
		}
	}
	btrfs_debug_in_rcu(fs_info,
	"btrfs_map_bio: rw %d 0x%x, sector=%llu, dev=%lu (%s id %llu), size=%u",
		bio_op(bio), bio->bi_opf, bio->bi_iter.bi_sector,
		(unsigned long)dev->bdev->bd_dev, rcu_str_deref(dev->name),
		dev->devid, bio->bi_iter.bi_size);
	bio_set_dev(bio, dev->bdev);

	btrfs_bio_counter_inc_noblocked(fs_info);

	btrfsic_submit_bio(bio);
}