static int cbc_decrypt(struct blkcipher_desc *desc, struct scatterlist *dst,
		       struct scatterlist *src, unsigned int nbytes)
{
	bool fpu_enabled = false;
	struct blkcipher_walk walk;
	int err;

	blkcipher_walk_init(&walk, dst, src, nbytes);
	err = blkcipher_walk_virt(desc, &walk);
	desc->flags &= ~CRYPTO_TFM_REQ_MAY_SLEEP;

	while ((nbytes = walk.nbytes)) {
		fpu_enabled = cast5_fpu_begin(fpu_enabled, nbytes);
		nbytes = __cbc_decrypt(desc, &walk);
		err = blkcipher_walk_done(desc, &walk, nbytes);
	}

	cast5_fpu_end(fpu_enabled);
	return err;
}
