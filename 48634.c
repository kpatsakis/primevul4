static int encrypt(struct blkcipher_desc *desc, struct scatterlist *dst,
		   struct scatterlist *src, unsigned int nbytes)
{
	struct priv *ctx = crypto_blkcipher_ctx(desc->tfm);
	struct blkcipher_walk w;

	blkcipher_walk_init(&w, dst, src, nbytes);
	return crypt(desc, &w, ctx, crypto_cipher_alg(ctx->tweak)->cia_encrypt,
		     crypto_cipher_alg(ctx->child)->cia_encrypt);
}
