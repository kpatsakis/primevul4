void bio_set_pages_dirty(struct bio *bio)
{
	struct bio_vec *bvec;
	int i;

	bio_for_each_segment_all(bvec, bio, i) {
		struct page *page = bvec->bv_page;

		if (page && !PageCompound(page))
			set_page_dirty_lock(page);
	}
}
