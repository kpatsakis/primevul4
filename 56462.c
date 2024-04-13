void bio_copy_data(struct bio *dst, struct bio *src)
{
	struct bvec_iter src_iter, dst_iter;
	struct bio_vec src_bv, dst_bv;
	void *src_p, *dst_p;
	unsigned bytes;

	src_iter = src->bi_iter;
	dst_iter = dst->bi_iter;

	while (1) {
		if (!src_iter.bi_size) {
			src = src->bi_next;
			if (!src)
				break;

			src_iter = src->bi_iter;
		}

		if (!dst_iter.bi_size) {
			dst = dst->bi_next;
			if (!dst)
				break;

			dst_iter = dst->bi_iter;
		}

		src_bv = bio_iter_iovec(src, src_iter);
		dst_bv = bio_iter_iovec(dst, dst_iter);

		bytes = min(src_bv.bv_len, dst_bv.bv_len);

		src_p = kmap_atomic(src_bv.bv_page);
		dst_p = kmap_atomic(dst_bv.bv_page);

		memcpy(dst_p + dst_bv.bv_offset,
		       src_p + src_bv.bv_offset,
		       bytes);

		kunmap_atomic(dst_p);
		kunmap_atomic(src_p);

		bio_advance_iter(src, &src_iter, bytes);
		bio_advance_iter(dst, &dst_iter, bytes);
	}
}
