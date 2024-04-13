static int __ecb_crypt(struct blkcipher_desc *desc,
		       struct scatterlist *dst, struct scatterlist *src,
		       unsigned int nbytes, bool encrypt)
{
	struct des_sparc64_ctx *ctx = crypto_blkcipher_ctx(desc->tfm);
	struct blkcipher_walk walk;
	int err;

	blkcipher_walk_init(&walk, dst, src, nbytes);
	err = blkcipher_walk_virt(desc, &walk);
	desc->flags &= ~CRYPTO_TFM_REQ_MAY_SLEEP;

	if (encrypt)
		des_sparc64_load_keys(&ctx->encrypt_expkey[0]);
	else
		des_sparc64_load_keys(&ctx->decrypt_expkey[0]);
	while ((nbytes = walk.nbytes)) {
		unsigned int block_len = nbytes & DES_BLOCK_MASK;

		if (likely(block_len)) {
			des_sparc64_ecb_crypt((const u64 *)walk.src.virt.addr,
					      (u64 *) walk.dst.virt.addr,
					      block_len);
		}
		nbytes &= DES_BLOCK_SIZE - 1;
		err = blkcipher_walk_done(desc, &walk, nbytes);
	}
	fprs_write(0);
	return err;
}
