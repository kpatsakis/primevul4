static void bio_map_kern_endio(struct bio *bio)
{
	bio_put(bio);
}
