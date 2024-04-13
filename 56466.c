static void bio_copy_kern_endio_read(struct bio *bio)
{
	char *p = bio->bi_private;
	struct bio_vec *bvec;
	int i;

	bio_for_each_segment_all(bvec, bio, i) {
		memcpy(p, page_address(bvec->bv_page), bvec->bv_len);
		p += bvec->bv_len;
	}

	bio_copy_kern_endio(bio);
}
