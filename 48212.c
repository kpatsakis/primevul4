static int crypto_fpu_encrypt(struct blkcipher_desc *desc_in,
			      struct scatterlist *dst, struct scatterlist *src,
			      unsigned int nbytes)
{
	int err;
	struct crypto_fpu_ctx *ctx = crypto_blkcipher_ctx(desc_in->tfm);
	struct crypto_blkcipher *child = ctx->child;
	struct blkcipher_desc desc = {
		.tfm = child,
		.info = desc_in->info,
		.flags = desc_in->flags & ~CRYPTO_TFM_REQ_MAY_SLEEP,
	};

	kernel_fpu_begin();
	err = crypto_blkcipher_crt(desc.tfm)->encrypt(&desc, dst, src, nbytes);
	kernel_fpu_end();
	return err;
}
