struct bio *bio_split(struct bio *bio, int sectors,
		      gfp_t gfp, struct bio_set *bs)
{
	struct bio *split = NULL;

	BUG_ON(sectors <= 0);
	BUG_ON(sectors >= bio_sectors(bio));

	split = bio_clone_fast(bio, gfp, bs);
	if (!split)
		return NULL;

	split->bi_iter.bi_size = sectors << 9;

	if (bio_integrity(split))
		bio_integrity_trim(split);

	bio_advance(bio, split->bi_iter.bi_size);

	if (bio_flagged(bio, BIO_TRACE_COMPLETION))
		bio_set_flag(bio, BIO_TRACE_COMPLETION);

	return split;
}
