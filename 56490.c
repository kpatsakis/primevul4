struct bio_vec *bvec_alloc(gfp_t gfp_mask, int nr, unsigned long *idx,
			   mempool_t *pool)
{
	struct bio_vec *bvl;

	/*
	 * see comment near bvec_array define!
	 */
	switch (nr) {
	case 1:
		*idx = 0;
		break;
	case 2 ... 4:
		*idx = 1;
		break;
	case 5 ... 16:
		*idx = 2;
		break;
	case 17 ... 64:
		*idx = 3;
		break;
	case 65 ... 128:
		*idx = 4;
		break;
	case 129 ... BIO_MAX_PAGES:
		*idx = 5;
		break;
	default:
		return NULL;
	}

	/*
	 * idx now points to the pool we want to allocate from. only the
	 * 1-vec entry pool is mempool backed.
	 */
	if (*idx == BVEC_POOL_MAX) {
fallback:
		bvl = mempool_alloc(pool, gfp_mask);
	} else {
		struct biovec_slab *bvs = bvec_slabs + *idx;
		gfp_t __gfp_mask = gfp_mask & ~(__GFP_DIRECT_RECLAIM | __GFP_IO);

		/*
		 * Make this allocation restricted and don't dump info on
		 * allocation failures, since we'll fallback to the mempool
		 * in case of failure.
		 */
		__gfp_mask |= __GFP_NOMEMALLOC | __GFP_NORETRY | __GFP_NOWARN;

		/*
		 * Try a slab allocation. If this fails and __GFP_DIRECT_RECLAIM
		 * is set, retry with the 1-entry mempool
		 */
		bvl = kmem_cache_alloc(bvs->slab, __gfp_mask);
		if (unlikely(!bvl && (gfp_mask & __GFP_DIRECT_RECLAIM))) {
			*idx = BVEC_POOL_MAX;
			goto fallback;
		}
	}

	(*idx)++;
	return bvl;
}
