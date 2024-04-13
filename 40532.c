static int shash_async_import(struct ahash_request *req, const void *in)
{
	struct crypto_shash **ctx = crypto_ahash_ctx(crypto_ahash_reqtfm(req));
	struct shash_desc *desc = ahash_request_ctx(req);

	desc->tfm = *ctx;
	desc->flags = req->base.flags;

	return crypto_shash_import(desc, in);
}
