void ahash_free_instance(struct crypto_instance *inst)
{
	crypto_drop_spawn(crypto_instance_ctx(inst));
	kfree(ahash_instance(inst));
}
