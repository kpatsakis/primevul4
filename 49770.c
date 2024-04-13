static int ecb_aes_encrypt(struct blkcipher_desc *desc,
			   struct scatterlist *dst, struct scatterlist *src,
			   unsigned int nbytes)
{
	struct aes_ctx *ctx = blk_aes_ctx(desc->tfm);
	struct blkcipher_walk walk;
	int err;
	int ts_state;

	padlock_reset_key(&ctx->cword.encrypt);

	blkcipher_walk_init(&walk, dst, src, nbytes);
	err = blkcipher_walk_virt(desc, &walk);

	ts_state = irq_ts_save();
	while ((nbytes = walk.nbytes)) {
		padlock_xcrypt_ecb(walk.src.virt.addr, walk.dst.virt.addr,
				   ctx->E, &ctx->cword.encrypt,
				   nbytes / AES_BLOCK_SIZE);
		nbytes &= AES_BLOCK_SIZE - 1;
		err = blkcipher_walk_done(desc, &walk, nbytes);
	}
	irq_ts_restore(ts_state);

	padlock_store_cword(&ctx->cword.encrypt);

	return err;
}
