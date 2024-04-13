static void bio_chain_endio(struct bio *bio)
{
	bio_endio(__bio_chain_endio(bio));
}
