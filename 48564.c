int shash_ahash_mcryptd_finup(struct ahash_request *req,
			      struct shash_desc *desc)
{
	struct crypto_shash *tfm = desc->tfm;
	struct shash_alg *shash = crypto_shash_alg(tfm);

	/* alignment is to be done by multi-buffer crypto algorithm if needed */

	return shash->finup(desc, NULL, 0, req->result);
}
