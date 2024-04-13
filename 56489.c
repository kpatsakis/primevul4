void bioset_free(struct bio_set *bs)
{
	if (bs->rescue_workqueue)
		destroy_workqueue(bs->rescue_workqueue);

	if (bs->bio_pool)
		mempool_destroy(bs->bio_pool);

	if (bs->bvec_pool)
		mempool_destroy(bs->bvec_pool);

	bioset_integrity_free(bs);
	bio_put_slab(bs);

	kfree(bs);
}
