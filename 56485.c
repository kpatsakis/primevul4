void bio_trim(struct bio *bio, int offset, int size)
{
	/* 'bio' is a cloned bio which we need to trim to match
	 * the given offset and size.
	 */

	size <<= 9;
	if (offset == 0 && size == bio->bi_iter.bi_size)
		return;

	bio_clear_flag(bio, BIO_SEG_VALID);

	bio_advance(bio, offset << 9);

	bio->bi_iter.bi_size = size;

	if (bio_integrity(bio))
		bio_integrity_trim(bio);

}
