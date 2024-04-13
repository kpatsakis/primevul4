static off_t copyfd_sparse(int src_fd, int dst_fd1, int dst_fd2, off_t size2)
{
	off_t total = 0;
	int last_was_seek = 0;
#if CONFIG_FEATURE_COPYBUF_KB <= 4
	char buffer[CONFIG_FEATURE_COPYBUF_KB * 1024];
	enum { buffer_size = sizeof(buffer) };
#else
	char *buffer;
	int buffer_size;

	/* We want page-aligned buffer, just in case kernel is clever
	 * and can do page-aligned io more efficiently */
	buffer = mmap(NULL, CONFIG_FEATURE_COPYBUF_KB * 1024,
			PROT_READ | PROT_WRITE,
			MAP_PRIVATE | MAP_ANON,
			/* ignored: */ -1, 0);
	buffer_size = CONFIG_FEATURE_COPYBUF_KB * 1024;
	if (buffer == MAP_FAILED) {
		buffer = alloca(4 * 1024);
		buffer_size = 4 * 1024;
	}
#endif

	while (1) {
		ssize_t rd = safe_read(src_fd, buffer, buffer_size);
		if (!rd) { /* eof */
			if (last_was_seek) {
				if (lseek(dst_fd1, -1, SEEK_CUR) < 0
				 || safe_write(dst_fd1, "", 1) != 1
				 || (dst_fd2 >= 0
				     && (lseek(dst_fd2, -1, SEEK_CUR) < 0
					 || safe_write(dst_fd2, "", 1) != 1
				        )
				    )
				) {
					perror_msg("Write error");
					total = -1;
					goto out;
				}
			}
			/* all done */
			goto out;
		}
		if (rd < 0) {
			perror_msg("Read error");
			total = -1;
			goto out;
		}

		/* checking sparseness */
		ssize_t cnt = rd;
		while (--cnt >= 0) {
			if (buffer[cnt] != 0) {
				/* not sparse */
				errno = 0;
				ssize_t wr1 = full_write(dst_fd1, buffer, rd);
				ssize_t wr2 = (dst_fd2 >= 0 ? full_write(dst_fd2, buffer, rd) : rd);
				if (wr1 < rd || wr2 < rd) {
					perror_msg("Write error");
					total = -1;
					goto out;
				}
				last_was_seek = 0;
				goto adv;
			}
		}
		/* sparse */
		xlseek(dst_fd1, rd, SEEK_CUR);
		if (dst_fd2 >= 0)
			xlseek(dst_fd2, rd, SEEK_CUR);
		last_was_seek = 1;
 adv:
		total += rd;
		size2 -= rd;
		if (size2 < 0)
			dst_fd2 = -1;
	}
 out:

#if CONFIG_FEATURE_COPYBUF_KB > 4
	if (buffer_size != 4 * 1024)
		munmap(buffer, buffer_size);
#endif
	return total;
}
