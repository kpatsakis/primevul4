static int sctp_v4_addr_to_user(struct sctp_sock *sp, union sctp_addr *addr)
{
	/* No address mapping for V4 sockets */
	return sizeof(struct sockaddr_in);
}
