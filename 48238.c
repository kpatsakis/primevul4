int crypto_init_spawn2(struct crypto_spawn *spawn, struct crypto_alg *alg,
		       struct crypto_instance *inst,
		       const struct crypto_type *frontend)
{
	int err = -EINVAL;

	if ((alg->cra_flags ^ frontend->type) & frontend->maskset)
		goto out;

	spawn->frontend = frontend;
	err = crypto_init_spawn(spawn, alg, inst, frontend->maskset);

out:
	return err;
}
