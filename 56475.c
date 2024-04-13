void bio_free_pages(struct bio *bio)
{
	struct bio_vec *bvec;
	int i;

	bio_for_each_segment_all(bvec, bio, i)
		__free_page(bvec->bv_page);
}
