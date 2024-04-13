vhost_scsi_calc_sgls(struct iov_iter *iter, size_t bytes, int max_sgls)
{
	int sgl_count = 0;

	if (!iter || !iter->iov) {
		pr_err("%s: iter->iov is NULL, but expected bytes: %zu"
		       " present\n", __func__, bytes);
		return -EINVAL;
	}

	sgl_count = iov_iter_npages(iter, 0xffff);
	if (sgl_count > max_sgls) {
		pr_err("%s: requested sgl_count: %d exceeds pre-allocated"
		       " max_sgls: %d\n", __func__, sgl_count, max_sgls);
		return -EINVAL;
	}
	return sgl_count;
}
