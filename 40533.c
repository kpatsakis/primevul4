static int shash_async_init(struct ahash_request *req)
{
	struct crypto_shash **ctx = crypto_ahash_ctx(crypto_ahash_reqtfm(req));
	struct shash_desc *desc = ahash_request_ctx(req);

	desc->tfm = *ctx;
	desc->flags = req->base.flags;

	return crypto_shash_init(desc);
}
