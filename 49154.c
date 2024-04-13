static int cbc_decrypt(struct blkcipher_desc *desc,
		       struct scatterlist *dst, struct scatterlist *src,
		       unsigned int nbytes)
{
	struct camellia_sparc64_ctx *ctx = crypto_blkcipher_ctx(desc->tfm);
	struct blkcipher_walk walk;
	cbc_crypt_op *op;
	const u64 *key;
	int err;

	op = camellia_sparc64_cbc_decrypt_3_grand_rounds;
	if (ctx->key_len != 16)
		op = camellia_sparc64_cbc_decrypt_4_grand_rounds;

	blkcipher_walk_init(&walk, dst, src, nbytes);
	err = blkcipher_walk_virt(desc, &walk);
	desc->flags &= ~CRYPTO_TFM_REQ_MAY_SLEEP;

	key = &ctx->decrypt_key[0];
	camellia_sparc64_load_keys(key, ctx->key_len);
	while ((nbytes = walk.nbytes)) {
		unsigned int block_len = nbytes & CAMELLIA_BLOCK_MASK;

		if (likely(block_len)) {
			const u64 *src64;
			u64 *dst64;

			src64 = (const u64 *)walk.src.virt.addr;
			dst64 = (u64 *) walk.dst.virt.addr;
			op(src64, dst64, block_len, key,
			   (u64 *) walk.iv);
		}
		nbytes &= CAMELLIA_BLOCK_SIZE - 1;
		err = blkcipher_walk_done(desc, &walk, nbytes);
	}
	fprs_write(0);
	return err;
}
