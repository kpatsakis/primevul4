static int sctp_get_port(struct sock *sk, unsigned short snum)
{
	long ret;
	union sctp_addr addr;
	struct sctp_af *af = sctp_sk(sk)->pf->af;

	/* Set up a dummy address struct from the sk. */
	af->from_sk(&addr, sk);
	addr.v4.sin_port = htons(snum);

	/* Note: sk->sk_num gets filled in if ephemeral port request. */
	ret = sctp_get_port_local(sk, &addr);

	return ret ? 1 : 0;
}
