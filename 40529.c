static int shash_async_export(struct ahash_request *req, void *out)
{
	return crypto_shash_export(ahash_request_ctx(req), out);
}
