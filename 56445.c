static struct bio *__bio_chain_endio(struct bio *bio)
{
	struct bio *parent = bio->bi_private;

	if (!parent->bi_status)
		parent->bi_status = bio->bi_status;
	bio_put(bio);
	return parent;
}
