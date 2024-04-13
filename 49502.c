void *crypto_create_tfm(struct crypto_alg *alg,
			const struct crypto_type *frontend)
{
	char *mem;
	struct crypto_tfm *tfm = NULL;
	unsigned int tfmsize;
	unsigned int total;
	int err = -ENOMEM;

	tfmsize = frontend->tfmsize;
	total = tfmsize + sizeof(*tfm) + frontend->extsize(alg);

	mem = kzalloc(total, GFP_KERNEL);
	if (mem == NULL)
		goto out_err;

	tfm = (struct crypto_tfm *)(mem + tfmsize);
	tfm->__crt_alg = alg;

	err = frontend->init_tfm(tfm);
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
	kfree(mem);
out_err:
	mem = ERR_PTR(err);
out:
	return mem;
}
