static struct kmem_cache *bio_find_or_create_slab(unsigned int extra_size)
{
	unsigned int sz = sizeof(struct bio) + extra_size;
	struct kmem_cache *slab = NULL;
	struct bio_slab *bslab, *new_bio_slabs;
	unsigned int new_bio_slab_max;
	unsigned int i, entry = -1;

	mutex_lock(&bio_slab_lock);

	i = 0;
	while (i < bio_slab_nr) {
		bslab = &bio_slabs[i];

		if (!bslab->slab && entry == -1)
			entry = i;
		else if (bslab->slab_size == sz) {
			slab = bslab->slab;
			bslab->slab_ref++;
			break;
		}
		i++;
	}

	if (slab)
		goto out_unlock;

	if (bio_slab_nr == bio_slab_max && entry == -1) {
		new_bio_slab_max = bio_slab_max << 1;
		new_bio_slabs = krealloc(bio_slabs,
					 new_bio_slab_max * sizeof(struct bio_slab),
					 GFP_KERNEL);
		if (!new_bio_slabs)
			goto out_unlock;
		bio_slab_max = new_bio_slab_max;
		bio_slabs = new_bio_slabs;
	}
	if (entry == -1)
		entry = bio_slab_nr++;

	bslab = &bio_slabs[entry];

	snprintf(bslab->name, sizeof(bslab->name), "bio-%d", entry);
	slab = kmem_cache_create(bslab->name, sz, ARCH_KMALLOC_MINALIGN,
				 SLAB_HWCACHE_ALIGN, NULL);
	if (!slab)
		goto out_unlock;

	bslab->slab = slab;
	bslab->slab_ref = 1;
	bslab->slab_size = sz;
out_unlock:
	mutex_unlock(&bio_slab_lock);
	return slab;
}
