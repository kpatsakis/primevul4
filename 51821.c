int socket_send(int fd, void *data, size_t length)
{
	return send(fd, data, length, 0);
}
