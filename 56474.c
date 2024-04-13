static void bio_free(struct bio *bio)
{
	struct bio_set *bs = bio->bi_pool;
	void *p;

	bio_uninit(bio);

	if (bs) {
		bvec_free(bs->bvec_pool, bio->bi_io_vec, BVEC_POOL_IDX(bio));

		/*
		 * If we have front padding, adjust the bio pointer before freeing
		 */
		p = bio;
		p -= bs->front_pad;

		mempool_free(p, bs->bio_pool);
	} else {
		/* Bio was allocated by bio_kmalloc() */
		kfree(bio);
	}
}
