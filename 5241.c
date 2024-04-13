static int digest_update_from_fd(struct digest *d, int fd,
				 loff_t start, loff_t size)
{
	unsigned char *buf = xmalloc(PAGE_SIZE);
	int ret = 0;

	if (lseek(fd, start, SEEK_SET) != start) {
		perror("lseek");
		ret = -errno;
		goto out_free;
	}

	while (size) {
		unsigned long now = min_t(typeof(size), PAGE_SIZE, size);

		ret = read(fd, buf, now);
		if (ret < 0) {
			perror("read");
			goto out_free;
		}

		if (!ret)
			break;

		ret = digest_update_interruptible(d, buf, ret);
		if (ret)
			goto out_free;

		size -= now;
	}

out_free:
	free(buf);
	return ret;
}