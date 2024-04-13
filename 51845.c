static int tipc_send_packet(struct socket *sock, struct msghdr *m, size_t dsz)
{
	if (dsz > TIPC_MAX_USER_MSG_SIZE)
		return -EMSGSIZE;

	return tipc_send_stream(sock, m, dsz);
}
