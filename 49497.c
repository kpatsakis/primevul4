struct crypto_tfm *__crypto_alloc_tfm(struct crypto_alg *alg, u32 type,
				      u32 mask)
{
	struct crypto_tfm *tfm = NULL;
	unsigned int tfm_size;
	int err = -ENOMEM;

	tfm_size = sizeof(*tfm) + crypto_ctxsize(alg, type, mask);
	tfm = kzalloc(tfm_size, GFP_KERNEL);
	if (tfm == NULL)
		goto out_err;

	tfm->__crt_alg = alg;

	err = crypto_init_ops(tfm, type, mask);
	if (err)
		goto out_free_tfm;

	if (!tfm->exit && alg->cra_init && (err = alg->cra_init(tfm)))
		goto cra_init_failed;

	goto out;

cra_init_failed:
	crypto_exit_ops(tfm);
out_free_tfm:
	if (err == -EAGAIN)
		crypto_shoot_alg(alg);
	kfree(tfm);
out_err:
	tfm = ERR_PTR(err);
out:
	return tfm;
}
