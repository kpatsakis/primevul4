static int bio_copy_from_iter(struct bio *bio, struct iov_iter iter)
{
	int i;
	struct bio_vec *bvec;

	bio_for_each_segment_all(bvec, bio, i) {
		ssize_t ret;

		ret = copy_page_from_iter(bvec->bv_page,
					  bvec->bv_offset,
					  bvec->bv_len,
					  &iter);

		if (!iov_iter_count(&iter))
			break;

		if (ret < bvec->bv_len)
			return -EFAULT;
	}

	return 0;
}
