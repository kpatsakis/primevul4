int compat_import_iovec(int type, const struct compat_iovec __user * uvector,
		 unsigned nr_segs, unsigned fast_segs,
		 struct iovec **iov, struct iov_iter *i)
{
	ssize_t n;
	struct iovec *p;
	n = compat_rw_copy_check_uvector(type, uvector, nr_segs, fast_segs,
				  *iov, &p);
	if (n < 0) {
		if (p != *iov)
			kfree(p);
		*iov = NULL;
		return n;
	}
	iov_iter_init(i, type, p, nr_segs, n);
	*iov = p == *iov ? NULL : p;
	return 0;
}
