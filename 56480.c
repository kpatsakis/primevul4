static void bio_put_slab(struct bio_set *bs)
{
	struct bio_slab *bslab = NULL;
	unsigned int i;

	mutex_lock(&bio_slab_lock);

	for (i = 0; i < bio_slab_nr; i++) {
		if (bs->bio_slab == bio_slabs[i].slab) {
			bslab = &bio_slabs[i];
			break;
		}
	}

	if (WARN(!bslab, KERN_ERR "bio: unable to find slab!\n"))
		goto out;

	WARN_ON(!bslab->slab_ref);

	if (--bslab->slab_ref)
		goto out;

	kmem_cache_destroy(bslab->slab);
	bslab->slab = NULL;

out:
	mutex_unlock(&bio_slab_lock);
}
