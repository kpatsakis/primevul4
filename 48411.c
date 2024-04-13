static int crypto_ctr_crypt(struct blkcipher_desc *desc,
			      struct scatterlist *dst, struct scatterlist *src,
			      unsigned int nbytes)
{
	struct blkcipher_walk walk;
	struct crypto_blkcipher *tfm = desc->tfm;
	struct crypto_ctr_ctx *ctx = crypto_blkcipher_ctx(tfm);
	struct crypto_cipher *child = ctx->child;
	unsigned int bsize = crypto_cipher_blocksize(child);
	int err;

	blkcipher_walk_init(&walk, dst, src, nbytes);
	err = blkcipher_walk_virt_block(desc, &walk, bsize);

	while (walk.nbytes >= bsize) {
		if (walk.src.virt.addr == walk.dst.virt.addr)
			nbytes = crypto_ctr_crypt_inplace(&walk, child);
		else
			nbytes = crypto_ctr_crypt_segment(&walk, child);

		err = blkcipher_walk_done(desc, &walk, nbytes);
	}

	if (walk.nbytes) {
		crypto_ctr_crypt_final(&walk, child);
		err = blkcipher_walk_done(desc, &walk, 0);
	}

	return err;
}
