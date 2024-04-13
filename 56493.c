static int __init init_bio(void)
{
	bio_slab_max = 2;
	bio_slab_nr = 0;
	bio_slabs = kzalloc(bio_slab_max * sizeof(struct bio_slab), GFP_KERNEL);
	if (!bio_slabs)
		panic("bio: can't allocate bios\n");

	bio_integrity_init();
	biovec_init_slabs();

	fs_bio_set = bioset_create(BIO_POOL_SIZE, 0, BIOSET_NEED_BVECS);
	if (!fs_bio_set)
		panic("bio: can't allocate bios\n");

	if (bioset_integrity_create(fs_bio_set, BIO_POOL_SIZE))
		panic("bio: can't create integrity pool\n");

	return 0;
}
