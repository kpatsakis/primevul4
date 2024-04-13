int sctp_inet_listen(struct socket *sock, int backlog)
{
	struct sock *sk = sock->sk;
	struct crypto_hash *tfm = NULL;
	int err = -EINVAL;

	if (unlikely(backlog < 0))
		goto out;

	sctp_lock_sock(sk);

	if (sock->state != SS_UNCONNECTED)
		goto out;

	/* Allocate HMAC for generating cookie. */
	if (sctp_hmac_alg) {
		tfm = crypto_alloc_hash(sctp_hmac_alg, 0, CRYPTO_ALG_ASYNC);
		if (!tfm) {
			err = -ENOSYS;
			goto out;
		}
	}

	switch (sock->type) {
	case SOCK_SEQPACKET:
		err = sctp_seqpacket_listen(sk, backlog);
		break;
	case SOCK_STREAM:
		err = sctp_stream_listen(sk, backlog);
		break;
	default:
		break;
	};
	if (err)
		goto cleanup;

	/* Store away the transform reference. */
	sctp_sk(sk)->hmac = tfm;
out:
	sctp_release_sock(sk);
	return err;
cleanup:
	crypto_free_hash(tfm);
	goto out;
}
