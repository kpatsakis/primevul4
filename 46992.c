static struct sock *sctp_v4_create_accept_sk(struct sock *sk,
					     struct sctp_association *asoc)
{
	struct sock *newsk = sk_alloc(sock_net(sk), PF_INET, GFP_KERNEL,
			sk->sk_prot, 0);
	struct inet_sock *newinet;

	if (!newsk)
		goto out;

	sock_init_data(NULL, newsk);

	sctp_copy_sock(newsk, sk, asoc);
	sock_reset_flag(newsk, SOCK_ZAPPED);

	newinet = inet_sk(newsk);

	newinet->inet_daddr = asoc->peer.primary_addr.v4.sin_addr.s_addr;

	sk_refcnt_debug_inc(newsk);

	if (newsk->sk_prot->init(newsk)) {
		sk_common_release(newsk);
		newsk = NULL;
	}

out:
	return newsk;
}
