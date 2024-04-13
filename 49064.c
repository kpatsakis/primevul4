static int xts_encrypt(struct blkcipher_desc *desc, struct scatterlist *dst,
		       struct scatterlist *src, unsigned int nbytes)
{
	struct crypto_aes_xts_ctx *ctx = crypto_blkcipher_ctx(desc->tfm);
	int err, first, rounds = 6 + ctx->key1.key_length / 4;
	struct blkcipher_walk walk;
	unsigned int blocks;

	desc->flags &= ~CRYPTO_TFM_REQ_MAY_SLEEP;
	blkcipher_walk_init(&walk, dst, src, nbytes);
	err = blkcipher_walk_virt(desc, &walk);

	kernel_neon_begin();
	for (first = 1; (blocks = (walk.nbytes / AES_BLOCK_SIZE)); first = 0) {
		aes_xts_encrypt(walk.dst.virt.addr, walk.src.virt.addr,
				(u8 *)ctx->key1.key_enc, rounds, blocks,
				(u8 *)ctx->key2.key_enc, walk.iv, first);
		err = blkcipher_walk_done(desc, &walk, walk.nbytes % AES_BLOCK_SIZE);
	}
	kernel_neon_end();

	return err;
}
