static int ctr_crypt(struct blkcipher_desc *desc,
		     struct scatterlist *dst, struct scatterlist *src,
		     unsigned int nbytes)
{
	struct crypto_sparc64_aes_ctx *ctx = crypto_blkcipher_ctx(desc->tfm);
	struct blkcipher_walk walk;
	int err;

	blkcipher_walk_init(&walk, dst, src, nbytes);
	err = blkcipher_walk_virt_block(desc, &walk, AES_BLOCK_SIZE);
	desc->flags &= ~CRYPTO_TFM_REQ_MAY_SLEEP;

	ctx->ops->load_encrypt_keys(&ctx->key[0]);
	while ((nbytes = walk.nbytes) >= AES_BLOCK_SIZE) {
		unsigned int block_len = nbytes & AES_BLOCK_MASK;

		if (likely(block_len)) {
			ctx->ops->ctr_crypt(&ctx->key[0],
					    (const u64 *)walk.src.virt.addr,
					    (u64 *) walk.dst.virt.addr,
					    block_len, (u64 *) walk.iv);
		}
		nbytes &= AES_BLOCK_SIZE - 1;
		err = blkcipher_walk_done(desc, &walk, nbytes);
	}
	if (walk.nbytes) {
		ctr_crypt_final(ctx, &walk);
		err = blkcipher_walk_done(desc, &walk, 0);
	}
	fprs_write(0);
	return err;
}
