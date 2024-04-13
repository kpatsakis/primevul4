static int fuse_verify_ioctl_iov(struct iovec *iov, size_t count)
{
	size_t n;
	u32 max = FUSE_MAX_PAGES_PER_REQ << PAGE_SHIFT;

	for (n = 0; n < count; n++, iov++) {
		if (iov->iov_len > (size_t) max)
			return -ENOMEM;
		max -= iov->iov_len;
	}
	return 0;
}
