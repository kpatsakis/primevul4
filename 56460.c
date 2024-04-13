struct bio *bio_clone_bioset(struct bio *bio_src, gfp_t gfp_mask,
			     struct bio_set *bs)
{
	struct bvec_iter iter;
	struct bio_vec bv;
	struct bio *bio;

	/*
	 * Pre immutable biovecs, __bio_clone() used to just do a memcpy from
	 * bio_src->bi_io_vec to bio->bi_io_vec.
	 *
	 * We can't do that anymore, because:
	 *
	 *  - The point of cloning the biovec is to produce a bio with a biovec
	 *    the caller can modify: bi_idx and bi_bvec_done should be 0.
	 *
	 *  - The original bio could've had more than BIO_MAX_PAGES biovecs; if
	 *    we tried to clone the whole thing bio_alloc_bioset() would fail.
	 *    But the clone should succeed as long as the number of biovecs we
	 *    actually need to allocate is fewer than BIO_MAX_PAGES.
	 *
	 *  - Lastly, bi_vcnt should not be looked at or relied upon by code
	 *    that does not own the bio - reason being drivers don't use it for
	 *    iterating over the biovec anymore, so expecting it to be kept up
	 *    to date (i.e. for clones that share the parent biovec) is just
	 *    asking for trouble and would force extra work on
	 *    __bio_clone_fast() anyways.
	 */

	bio = bio_alloc_bioset(gfp_mask, bio_segments(bio_src), bs);
	if (!bio)
		return NULL;
	bio->bi_disk		= bio_src->bi_disk;
	bio->bi_opf		= bio_src->bi_opf;
	bio->bi_write_hint	= bio_src->bi_write_hint;
	bio->bi_iter.bi_sector	= bio_src->bi_iter.bi_sector;
	bio->bi_iter.bi_size	= bio_src->bi_iter.bi_size;

	switch (bio_op(bio)) {
	case REQ_OP_DISCARD:
	case REQ_OP_SECURE_ERASE:
	case REQ_OP_WRITE_ZEROES:
		break;
	case REQ_OP_WRITE_SAME:
		bio->bi_io_vec[bio->bi_vcnt++] = bio_src->bi_io_vec[0];
		break;
	default:
		bio_for_each_segment(bv, bio_src, iter)
			bio->bi_io_vec[bio->bi_vcnt++] = bv;
		break;
	}

	if (bio_integrity(bio_src)) {
		int ret;

		ret = bio_integrity_clone(bio, bio_src, gfp_mask);
		if (ret < 0) {
			bio_put(bio);
			return NULL;
		}
	}

	bio_clone_blkcg_association(bio, bio_src);

	return bio;
}
