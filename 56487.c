void bio_unmap_user(struct bio *bio)
{
	__bio_unmap_user(bio);
	bio_put(bio);
}
