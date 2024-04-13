static inline int sctp_wspace(struct sctp_association *asoc)
{
	struct sock *sk = asoc->base.sk;
	int amt = 0;

	if (asoc->ep->sndbuf_policy) {
		/* make sure that no association uses more than sk_sndbuf */
		amt = sk->sk_sndbuf - asoc->sndbuf_used;
	} else {
		/* do socket level accounting */
		amt = sk->sk_sndbuf - atomic_read(&sk->sk_wmem_alloc);
	}

	if (amt < 0)
		amt = 0;

	return amt;
}
