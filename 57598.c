static int skcipher_encrypt_ablkcipher(struct skcipher_request *req)
{
	struct crypto_skcipher *skcipher = crypto_skcipher_reqtfm(req);
	struct crypto_tfm *tfm = crypto_skcipher_tfm(skcipher);
	struct ablkcipher_alg *alg = &tfm->__crt_alg->cra_ablkcipher;

	return skcipher_crypt_ablkcipher(req, alg->encrypt);
}
