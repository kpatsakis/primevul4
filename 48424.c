static void crypto_rfc3686_free(struct crypto_instance *inst)
{
	struct crypto_skcipher_spawn *spawn = crypto_instance_ctx(inst);

	crypto_drop_skcipher(spawn);
	kfree(inst);
}
