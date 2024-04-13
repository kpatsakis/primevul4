SCTP_STATIC int sctp_bind(struct sock *sk, struct sockaddr *addr, int addr_len)
{
	int retval = 0;

	sctp_lock_sock(sk);

	SCTP_DEBUG_PRINTK("sctp_bind(sk: %p, addr: %p, addr_len: %d)\n",
			  sk, addr, addr_len);

	/* Disallow binding twice. */
	if (!sctp_sk(sk)->ep->base.bind_addr.port)
		retval = sctp_do_bind(sk, (union sctp_addr *)addr,
				      addr_len);
	else
		retval = -EINVAL;

	sctp_release_sock(sk);

	return retval;
}
