static void skcipher_sock_destruct(struct sock *sk)
{
	struct alg_sock *ask = alg_sk(sk);
	struct skcipher_ctx *ctx = ask->private;
	struct crypto_ablkcipher *tfm = crypto_ablkcipher_reqtfm(&ctx->req);

	skcipher_free_sgl(sk);
	sock_kfree_s(sk, ctx->iv, crypto_ablkcipher_ivsize(tfm));
	sock_kfree_s(sk, ctx, ctx->len);
	af_alg_release_parent(sk);
}
