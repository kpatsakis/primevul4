static void eseqiv_free(struct crypto_instance *inst)
{
	skcipher_geniv_free(inst);
	crypto_put_default_rng();
}
