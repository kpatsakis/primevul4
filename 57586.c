 static void crypto_skcipher_exit_tfm(struct crypto_tfm *tfm)
 {
 	struct crypto_skcipher *skcipher = __crypto_skcipher_cast(tfm);
	struct skcipher_alg *alg = crypto_skcipher_alg(skcipher);

	alg->exit(skcipher);
}
