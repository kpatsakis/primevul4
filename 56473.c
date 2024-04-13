void bio_flush_dcache_pages(struct bio *bi)
{
	struct bio_vec bvec;
	struct bvec_iter iter;

	bio_for_each_segment(bvec, bi, iter)
		flush_dcache_page(bvec.bv_page);
}
