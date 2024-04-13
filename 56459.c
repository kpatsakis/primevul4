void bio_check_pages_dirty(struct bio *bio)
{
	struct bio_vec *bvec;
	int nr_clean_pages = 0;
	int i;

	bio_for_each_segment_all(bvec, bio, i) {
		struct page *page = bvec->bv_page;

		if (PageDirty(page) || PageCompound(page)) {
			put_page(page);
			bvec->bv_page = NULL;
		} else {
			nr_clean_pages++;
		}
	}

	if (nr_clean_pages) {
		unsigned long flags;

		spin_lock_irqsave(&bio_dirty_lock, flags);
		bio->bi_private = bio_dirty_list;
		bio_dirty_list = bio;
		spin_unlock_irqrestore(&bio_dirty_lock, flags);
		schedule_work(&bio_dirty_work);
	} else {
		bio_put(bio);
	}
}
