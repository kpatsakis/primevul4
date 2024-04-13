void bvec_free(mempool_t *pool, struct bio_vec *bv, unsigned int idx)
{
	if (!idx)
		return;
	idx--;

	BIO_BUG_ON(idx >= BVEC_POOL_NR);

	if (idx == BVEC_POOL_MAX) {
		mempool_free(bv, pool);
	} else {
		struct biovec_slab *bvs = bvec_slabs + idx;

		kmem_cache_free(bvs->slab, bv);
	}
}
