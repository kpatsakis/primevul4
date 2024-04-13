int crypto_ahash_finup(struct ahash_request *req)
{
	return crypto_ahash_op(req, crypto_ahash_reqtfm(req)->finup);
}
