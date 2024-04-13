int socket_peek(int fd, void *data, size_t length)
{
	return socket_receive_timeout(fd, data, length, MSG_PEEK, RECV_TIMEOUT);
}
