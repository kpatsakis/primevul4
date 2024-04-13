static int hash_sendmsg(struct socket *sock, struct msghdr *msg,
			size_t ignored)
{
	int limit = ALG_MAX_PAGES * PAGE_SIZE;
	struct sock *sk = sock->sk;
	struct alg_sock *ask = alg_sk(sk);
	struct hash_ctx *ctx = ask->private;
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

	while (msg_data_left(msg)) {
		int len = msg_data_left(msg);

		if (len > limit)
			len = limit;

		len = af_alg_make_sg(&ctx->sgl, &msg->msg_iter, len);
		if (len < 0) {
			err = copied ? 0 : len;
			goto unlock;
		}

		ahash_request_set_crypt(&ctx->req, ctx->sgl.sg, NULL, len);

		err = af_alg_wait_for_completion(crypto_ahash_update(&ctx->req),
						 &ctx->completion);
		af_alg_free_sg(&ctx->sgl);
		if (err)
			goto unlock;

		copied += len;
		iov_iter_advance(&msg->msg_iter, len);
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
