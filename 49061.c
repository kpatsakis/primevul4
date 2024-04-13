static int ctr_encrypt(struct blkcipher_desc *desc, struct scatterlist *dst,
		       struct scatterlist *src, unsigned int nbytes)
{
	struct crypto_aes_ctx *ctx = crypto_blkcipher_ctx(desc->tfm);
	int err, first, rounds = 6 + ctx->key_length / 4;
	struct blkcipher_walk walk;
	int blocks;

	desc->flags &= ~CRYPTO_TFM_REQ_MAY_SLEEP;
	blkcipher_walk_init(&walk, dst, src, nbytes);
	err = blkcipher_walk_virt_block(desc, &walk, AES_BLOCK_SIZE);

	first = 1;
	kernel_neon_begin();
	while ((blocks = (walk.nbytes / AES_BLOCK_SIZE))) {
		aes_ctr_encrypt(walk.dst.virt.addr, walk.src.virt.addr,
				(u8 *)ctx->key_enc, rounds, blocks, walk.iv,
				first);
		first = 0;
		nbytes -= blocks * AES_BLOCK_SIZE;
		if (nbytes && nbytes == walk.nbytes % AES_BLOCK_SIZE)
			break;
		err = blkcipher_walk_done(desc, &walk,
					  walk.nbytes % AES_BLOCK_SIZE);
	}
	if (nbytes) {
		u8 *tdst = walk.dst.virt.addr + blocks * AES_BLOCK_SIZE;
		u8 *tsrc = walk.src.virt.addr + blocks * AES_BLOCK_SIZE;
		u8 __aligned(8) tail[AES_BLOCK_SIZE];

		/*
		 * Minimum alignment is 8 bytes, so if nbytes is <= 8, we need
		 * to tell aes_ctr_encrypt() to only read half a block.
		 */
		blocks = (nbytes <= 8) ? -1 : 1;

		aes_ctr_encrypt(tail, tsrc, (u8 *)ctx->key_enc, rounds,
				blocks, walk.iv, first);
		memcpy(tdst, tail, nbytes);
		err = blkcipher_walk_done(desc, &walk, 0);
	}
	kernel_neon_end();

	return err;
}
