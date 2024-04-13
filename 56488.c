struct bio_set *bioset_create(unsigned int pool_size,
			      unsigned int front_pad,
			      int flags)
{
	unsigned int back_pad = BIO_INLINE_VECS * sizeof(struct bio_vec);
	struct bio_set *bs;

	bs = kzalloc(sizeof(*bs), GFP_KERNEL);
	if (!bs)
		return NULL;

	bs->front_pad = front_pad;

	spin_lock_init(&bs->rescue_lock);
	bio_list_init(&bs->rescue_list);
	INIT_WORK(&bs->rescue_work, bio_alloc_rescue);

	bs->bio_slab = bio_find_or_create_slab(front_pad + back_pad);
	if (!bs->bio_slab) {
		kfree(bs);
		return NULL;
	}

	bs->bio_pool = mempool_create_slab_pool(pool_size, bs->bio_slab);
	if (!bs->bio_pool)
		goto bad;

	if (flags & BIOSET_NEED_BVECS) {
		bs->bvec_pool = biovec_create_pool(pool_size);
		if (!bs->bvec_pool)
			goto bad;
	}

	if (!(flags & BIOSET_NEED_RESCUER))
		return bs;

	bs->rescue_workqueue = alloc_workqueue("bioset", WQ_MEM_RECLAIM, 0);
	if (!bs->rescue_workqueue)
		goto bad;

	return bs;
bad:
	bioset_free(bs);
	return NULL;
}
