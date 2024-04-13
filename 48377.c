static int cryptd_blkcipher_init_tfm(struct crypto_tfm *tfm)
{
	struct crypto_instance *inst = crypto_tfm_alg_instance(tfm);
	struct cryptd_instance_ctx *ictx = crypto_instance_ctx(inst);
	struct crypto_spawn *spawn = &ictx->spawn;
	struct cryptd_blkcipher_ctx *ctx = crypto_tfm_ctx(tfm);
	struct crypto_blkcipher *cipher;

	cipher = crypto_spawn_blkcipher(spawn);
	if (IS_ERR(cipher))
		return PTR_ERR(cipher);

	ctx->child = cipher;
	tfm->crt_ablkcipher.reqsize =
		sizeof(struct cryptd_blkcipher_request_ctx);
	return 0;
}
