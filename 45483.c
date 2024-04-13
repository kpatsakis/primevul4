int memcpy_toiovecend(const struct iovec *iov, unsigned char *kdata,
		      int offset, int len)
{
	int copy;
	for (; len > 0; ++iov) {
		/* Skip over the finished iovecs */
		if (unlikely(offset >= iov->iov_len)) {
			offset -= iov->iov_len;
			continue;
		}
		copy = min_t(unsigned int, iov->iov_len - offset, len);
		if (copy_to_user(iov->iov_base + offset, kdata, copy))
			return -EFAULT;
		offset = 0;
		kdata += copy;
		len -= copy;
	}

	return 0;
}
