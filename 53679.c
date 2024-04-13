static inline int fuse_iter_npages(const struct iov_iter *ii_p)
{
	return iov_iter_npages(ii_p, FUSE_MAX_PAGES_PER_REQ);
}
