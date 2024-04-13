static int crypto_ccm_init_tfm(struct crypto_aead *tfm)
{
	struct aead_instance *inst = aead_alg_instance(tfm);
	struct ccm_instance_ctx *ictx = aead_instance_ctx(inst);
	struct crypto_ccm_ctx *ctx = crypto_aead_ctx(tfm);
	struct crypto_ahash *mac;
	struct crypto_skcipher *ctr;
	unsigned long align;
	int err;

	mac = crypto_spawn_ahash(&ictx->mac);
	if (IS_ERR(mac))
		return PTR_ERR(mac);

	ctr = crypto_spawn_skcipher(&ictx->ctr);
	err = PTR_ERR(ctr);
	if (IS_ERR(ctr))
		goto err_free_mac;

	ctx->mac = mac;
	ctx->ctr = ctr;

	align = crypto_aead_alignmask(tfm);
	align &= ~(crypto_tfm_ctx_alignment() - 1);
	crypto_aead_set_reqsize(
		tfm,
		align + sizeof(struct crypto_ccm_req_priv_ctx) +
		crypto_skcipher_reqsize(ctr));

	return 0;

err_free_mac:
	crypto_free_ahash(mac);
	return err;
}
