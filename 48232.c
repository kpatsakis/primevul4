void crypto_drop_spawn(struct crypto_spawn *spawn)
{
	if (!spawn->alg)
		return;

	down_write(&crypto_alg_sem);
	list_del(&spawn->list);
	up_write(&crypto_alg_sem);
}
