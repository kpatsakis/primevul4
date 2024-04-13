static int cbc3_decrypt(struct blkcipher_desc *desc,
			struct scatterlist *dst, struct scatterlist *src,
			unsigned int nbytes)
{
	struct des3_ede_sparc64_ctx *ctx = crypto_blkcipher_ctx(desc->tfm);
	struct blkcipher_walk walk;
	const u64 *K;
	int err;

	blkcipher_walk_init(&walk, dst, src, nbytes);
	err = blkcipher_walk_virt(desc, &walk);
	desc->flags &= ~CRYPTO_TFM_REQ_MAY_SLEEP;

	K = &ctx->decrypt_expkey[0];
	des3_ede_sparc64_load_keys(K);
	while ((nbytes = walk.nbytes)) {
		unsigned int block_len = nbytes & DES_BLOCK_MASK;

		if (likely(block_len)) {
			const u64 *src64 = (const u64 *)walk.src.virt.addr;
			des3_ede_sparc64_cbc_decrypt(K, src64,
						     (u64 *) walk.dst.virt.addr,
						     block_len,
						     (u64 *) walk.iv);
		}
		nbytes &= DES_BLOCK_SIZE - 1;
		err = blkcipher_walk_done(desc, &walk, nbytes);
	}
	fprs_write(0);
	return err;
}
