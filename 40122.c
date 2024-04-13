static ssize_t hash_sendpage(struct socket *sock, struct page *page,
			     int offset, size_t size, int flags)
{
	struct sock *sk = sock->sk;
	struct alg_sock *ask = alg_sk(sk);
	struct hash_ctx *ctx = ask->private;
	int err;

	lock_sock(sk);
	sg_init_table(ctx->sgl.sg, 1);
	sg_set_page(ctx->sgl.sg, page, size, offset);

	ahash_request_set_crypt(&ctx->req, ctx->sgl.sg, ctx->result, size);

	if (!(flags & MSG_MORE)) {
		if (ctx->more)
			err = crypto_ahash_finup(&ctx->req);
		else
			err = crypto_ahash_digest(&ctx->req);
	} else {
		if (!ctx->more) {
			err = crypto_ahash_init(&ctx->req);
			if (err)
				goto unlock;
		}

		err = crypto_ahash_update(&ctx->req);
	}

	err = af_alg_wait_for_completion(err, &ctx->completion);
	if (err)
		goto unlock;

	ctx->more = flags & MSG_MORE;

unlock:
	release_sock(sk);

	return err ?: size;
}
