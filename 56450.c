void bio_advance(struct bio *bio, unsigned bytes)
{
	if (bio_integrity(bio))
		bio_integrity_advance(bio, bytes);

	bio_advance_iter(bio, &bio->bi_iter, bytes);
}
