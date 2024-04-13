static int cbc_decrypt(struct blkcipher_desc *desc,
		       struct scatterlist *dst, struct scatterlist *src,
		       unsigned int nbytes)
{
	struct crypto_sparc64_aes_ctx *ctx = crypto_blkcipher_ctx(desc->tfm);
	struct blkcipher_walk walk;
	u64 *key_end;
	int err;

	blkcipher_walk_init(&walk, dst, src, nbytes);
	err = blkcipher_walk_virt(desc, &walk);
	desc->flags &= ~CRYPTO_TFM_REQ_MAY_SLEEP;

	ctx->ops->load_decrypt_keys(&ctx->key[0]);
	key_end = &ctx->key[ctx->expanded_key_length / sizeof(u64)];
	while ((nbytes = walk.nbytes)) {
		unsigned int block_len = nbytes & AES_BLOCK_MASK;

		if (likely(block_len)) {
			ctx->ops->cbc_decrypt(key_end,
					      (const u64 *) walk.src.virt.addr,
					      (u64 *) walk.dst.virt.addr,
					      block_len, (u64 *) walk.iv);
		}
		nbytes &= AES_BLOCK_SIZE - 1;
		err = blkcipher_walk_done(desc, &walk, nbytes);
	}
	fprs_write(0);

	return err;
}
