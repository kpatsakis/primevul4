int memcpy_fromiovecend(unsigned char *kdata, const struct iovec *iov,
			int offset, int len)
{
	/* Skip over the finished iovecs */
	while (offset >= iov->iov_len) {
		offset -= iov->iov_len;
		iov++;
	}

	while (len > 0) {
		u8 __user *base = iov->iov_base + offset;
		int copy = min_t(unsigned int, len, iov->iov_len - offset);

		offset = 0;
		if (copy_from_user(kdata, base, copy))
			return -EFAULT;
		len -= copy;
		kdata += copy;
		iov++;
	}

	return 0;
}
