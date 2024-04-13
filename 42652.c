SCTP_STATIC int sctp_connect(struct sock *sk, struct sockaddr *addr,
			     int addr_len)
{
	int err = 0;
	struct sctp_af *af;

	sctp_lock_sock(sk);

	SCTP_DEBUG_PRINTK("%s - sk: %p, sockaddr: %p, addr_len: %d\n",
			  __FUNCTION__, sk, addr, addr_len);

	/* Validate addr_len before calling common connect/connectx routine. */
	af = sctp_get_af_specific(addr->sa_family);
	if (!af || addr_len < af->sockaddr_len) {
		err = -EINVAL;
	} else {
		/* Pass correct addr len to common routine (so it knows there
		 * is only one address being passed.
		 */
		err = __sctp_connect(sk, addr, af->sockaddr_len);
	}

	sctp_release_sock(sk);
	return err;
}
