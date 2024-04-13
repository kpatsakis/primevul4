static int mcryptd_hash_import(struct ahash_request *req, const void *in)
{
	struct mcryptd_hash_request_ctx *rctx = ahash_request_ctx(req);

	return crypto_shash_import(&rctx->desc, in);
}
