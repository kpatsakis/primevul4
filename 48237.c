int crypto_init_spawn(struct crypto_spawn *spawn, struct crypto_alg *alg,
		      struct crypto_instance *inst, u32 mask)
{
	int err = -EAGAIN;

	spawn->inst = inst;
	spawn->mask = mask;

	down_write(&crypto_alg_sem);
	if (!crypto_is_moribund(alg)) {
		list_add(&spawn->list, &alg->cra_users);
		spawn->alg = alg;
		err = 0;
	}
	up_write(&crypto_alg_sem);

	return err;
}
