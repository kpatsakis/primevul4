static int eseqiv_init(struct crypto_tfm *tfm)
{
	struct crypto_ablkcipher *geniv = __crypto_ablkcipher_cast(tfm);
	struct eseqiv_ctx *ctx = crypto_ablkcipher_ctx(geniv);
	unsigned long alignmask;
	unsigned int reqsize;

	spin_lock_init(&ctx->lock);

	alignmask = crypto_tfm_ctx_alignment() - 1;
	reqsize = sizeof(struct eseqiv_request_ctx);

	if (alignmask & reqsize) {
		alignmask &= reqsize;
		alignmask--;
	}

	alignmask = ~alignmask;
	alignmask &= crypto_ablkcipher_alignmask(geniv);

	reqsize += alignmask;
	reqsize += crypto_ablkcipher_ivsize(geniv);
	reqsize = ALIGN(reqsize, crypto_tfm_ctx_alignment());

	ctx->reqoff = reqsize - sizeof(struct eseqiv_request_ctx);

	tfm->crt_ablkcipher.reqsize = reqsize +
				      sizeof(struct ablkcipher_request);

	return skcipher_geniv_init(tfm);
}
