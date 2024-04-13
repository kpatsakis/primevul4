int socket_receive_timeout(int fd, void *data, size_t length, int flags,
					 unsigned int timeout)
{
	int res;
	int result;

	res = socket_check_fd(fd, FDM_READ, timeout);
	if (res <= 0) {
		return res;
	}
	result = recv(fd, data, length, flags);
	if (res > 0 && result == 0) {
		if (verbose >= 3)
			fprintf(stderr, "%s: fd=%d recv returned 0\n", __func__, fd);
		return -EAGAIN;
	}
	if (result < 0) {
		return -errno;
	}
	return result;
}
