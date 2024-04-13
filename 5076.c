static inline void btrfs_end_bbio(struct btrfs_bio *bbio, struct bio *bio)
{
	bio->bi_private = bbio->private;
	bio->bi_end_io = bbio->end_io;
	bio_endio(bio);

	btrfs_put_bbio(bbio);
}