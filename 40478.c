void skcipher_geniv_exit(struct crypto_tfm *tfm)
{
	crypto_free_ablkcipher(tfm->crt_ablkcipher.base);
}
