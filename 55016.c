static int packet_bind_spkt(struct socket *sock, struct sockaddr *uaddr,
			    int addr_len)
{
	struct sock *sk = sock->sk;
	char name[sizeof(uaddr->sa_data) + 1];

	/*
	 *	Check legality
	 */

	if (addr_len != sizeof(struct sockaddr))
		return -EINVAL;
	/* uaddr->sa_data comes from the userspace, it's not guaranteed to be
	 * zero-terminated.
	 */
	memcpy(name, uaddr->sa_data, sizeof(uaddr->sa_data));
	name[sizeof(uaddr->sa_data)] = 0;

	return packet_do_bind(sk, name, 0, pkt_sk(sk)->num);
}
