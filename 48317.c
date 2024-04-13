static int crypto_ccm_init_tfm(struct crypto_tfm *tfm)
{
	struct crypto_instance *inst = (void *)tfm->__crt_alg;
	struct ccm_instance_ctx *ictx = crypto_instance_ctx(inst);
	struct crypto_ccm_ctx *ctx = crypto_tfm_ctx(tfm);
	struct crypto_cipher *cipher;
	struct crypto_ablkcipher *ctr;
	unsigned long align;
	int err;

	cipher = crypto_spawn_cipher(&ictx->cipher);
	if (IS_ERR(cipher))
		return PTR_ERR(cipher);

	ctr = crypto_spawn_skcipher(&ictx->ctr);
	err = PTR_ERR(ctr);
	if (IS_ERR(ctr))
		goto err_free_cipher;

	ctx->cipher = cipher;
	ctx->ctr = ctr;

	align = crypto_tfm_alg_alignmask(tfm);
	align &= ~(crypto_tfm_ctx_alignment() - 1);
	tfm->crt_aead.reqsize = align +
				sizeof(struct crypto_ccm_req_priv_ctx) +
				crypto_ablkcipher_reqsize(ctr);

	return 0;

err_free_cipher:
	crypto_free_cipher(cipher);
	return err;
}
