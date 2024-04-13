static int hash_sendmsg(struct kiocb *unused, struct socket *sock,
			struct msghdr *msg, size_t ignored)
{
	int limit = ALG_MAX_PAGES * PAGE_SIZE;
	struct sock *sk = sock->sk;
	struct alg_sock *ask = alg_sk(sk);
	struct hash_ctx *ctx = ask->private;
	unsigned long iovlen;
	struct iovec *iov;
	long copied = 0;
	int err;

	if (limit > sk->sk_sndbuf)
		limit = sk->sk_sndbuf;

	lock_sock(sk);
	if (!ctx->more) {
		err = crypto_ahash_init(&ctx->req);
		if (err)
			goto unlock;
	}

	ctx->more = 0;

	for (iov = msg->msg_iov, iovlen = msg->msg_iovlen; iovlen > 0;
	     iovlen--, iov++) {
		unsigned long seglen = iov->iov_len;
		char __user *from = iov->iov_base;

		while (seglen) {
			int len = min_t(unsigned long, seglen, limit);
			int newlen;

			newlen = af_alg_make_sg(&ctx->sgl, from, len, 0);
			if (newlen < 0) {
				err = copied ? 0 : newlen;
				goto unlock;
			}

			ahash_request_set_crypt(&ctx->req, ctx->sgl.sg, NULL,
						newlen);

			err = af_alg_wait_for_completion(
				crypto_ahash_update(&ctx->req),
				&ctx->completion);

			af_alg_free_sg(&ctx->sgl);

			if (err)
				goto unlock;

			seglen -= newlen;
			from += newlen;
			copied += newlen;
		}
	}

	err = 0;

	ctx->more = msg->msg_flags & MSG_MORE;
	if (!ctx->more) {
		ahash_request_set_crypt(&ctx->req, NULL, ctx->result, 0);
		err = af_alg_wait_for_completion(crypto_ahash_final(&ctx->req),
						 &ctx->completion);
	}

unlock:
	release_sock(sk);

	return err ?: copied;
}
