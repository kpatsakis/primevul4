void zero_fill_bio(struct bio *bio)
{
	unsigned long flags;
	struct bio_vec bv;
	struct bvec_iter iter;

	bio_for_each_segment(bv, bio, iter) {
		char *data = bvec_kmap_irq(&bv, &flags);
		memset(data, 0, bv.bv_len);
		flush_dcache_page(bv.bv_page);
		bvec_kunmap_irq(data, &flags);
	}
}
