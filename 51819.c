int socket_receive(int fd, void *data, size_t length)
{
	return socket_receive_timeout(fd, data, length, 0, RECV_TIMEOUT);
}
