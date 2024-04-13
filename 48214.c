static void crypto_fpu_free(struct crypto_instance *inst)
{
	crypto_drop_spawn(crypto_instance_ctx(inst));
	kfree(inst);
}
