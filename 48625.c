static int crypto_xcbc_digest_setkey(struct crypto_shash *parent,
				     const u8 *inkey, unsigned int keylen)
{
	unsigned long alignmask = crypto_shash_alignmask(parent);
	struct xcbc_tfm_ctx *ctx = crypto_shash_ctx(parent);
	int bs = crypto_shash_blocksize(parent);
	u8 *consts = PTR_ALIGN(&ctx->ctx[0], alignmask + 1);
	int err = 0;
	u8 key1[bs];

	if ((err = crypto_cipher_setkey(ctx->child, inkey, keylen)))
		return err;

	crypto_cipher_encrypt_one(ctx->child, consts, (u8 *)ks + bs);
	crypto_cipher_encrypt_one(ctx->child, consts + bs, (u8 *)ks + bs * 2);
	crypto_cipher_encrypt_one(ctx->child, key1, (u8 *)ks);

	return crypto_cipher_setkey(ctx->child, key1, bs);

}
