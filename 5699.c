int find_client_by_fd(int fd)
{
	int i;

	if (fd < 0)
		return -1;

	for (i = 0; i <= client_maxi; i++) {
		if (clients[i].fd == fd)
			return i;
	}
	return -1;
}