static int skcipher_encrypt_blkcipher(struct skcipher_request *req)
{
	struct crypto_skcipher *skcipher = crypto_skcipher_reqtfm(req);
	struct crypto_tfm *tfm = crypto_skcipher_tfm(skcipher);
	struct blkcipher_alg *alg = &tfm->__crt_alg->cra_blkcipher;

	return skcipher_crypt_blkcipher(req, alg->encrypt);
}
