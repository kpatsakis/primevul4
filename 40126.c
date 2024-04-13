static int skcipher_accept_parent(void *private, struct sock *sk)
{
	struct skcipher_ctx *ctx;
	struct alg_sock *ask = alg_sk(sk);
	unsigned int len = sizeof(*ctx) + crypto_ablkcipher_reqsize(private);

	ctx = sock_kmalloc(sk, len, GFP_KERNEL);
	if (!ctx)
		return -ENOMEM;

	ctx->iv = sock_kmalloc(sk, crypto_ablkcipher_ivsize(private),
			       GFP_KERNEL);
	if (!ctx->iv) {
		sock_kfree_s(sk, ctx, len);
		return -ENOMEM;
	}

	memset(ctx->iv, 0, crypto_ablkcipher_ivsize(private));

	INIT_LIST_HEAD(&ctx->tsgl);
	ctx->len = len;
	ctx->used = 0;
	ctx->more = 0;
	ctx->merge = 0;
	ctx->enc = 0;
	af_alg_init_completion(&ctx->completion);

	ask->private = ctx;

	ablkcipher_request_set_tfm(&ctx->req, private);
	ablkcipher_request_set_callback(&ctx->req, CRYPTO_TFM_REQ_MAY_BACKLOG,
					af_alg_complete, &ctx->completion);

	sk->sk_destruct = skcipher_sock_destruct;

	return 0;
}
