void __bio_clone_fast(struct bio *bio, struct bio *bio_src)
{
	BUG_ON(bio->bi_pool && BVEC_POOL_IDX(bio));

	/*
	 * most users will be overriding ->bi_disk with a new target,
	 * so we don't set nor calculate new physical/hw segment counts here
	 */
	bio->bi_disk = bio_src->bi_disk;
	bio_set_flag(bio, BIO_CLONED);
	bio->bi_opf = bio_src->bi_opf;
	bio->bi_write_hint = bio_src->bi_write_hint;
	bio->bi_iter = bio_src->bi_iter;
	bio->bi_io_vec = bio_src->bi_io_vec;

	bio_clone_blkcg_association(bio, bio_src);
}
