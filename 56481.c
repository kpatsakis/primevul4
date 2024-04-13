static void bio_release_pages(struct bio *bio)
{
	struct bio_vec *bvec;
	int i;

	bio_for_each_segment_all(bvec, bio, i) {
		struct page *page = bvec->bv_page;

		if (page)
			put_page(page);
	}
}
