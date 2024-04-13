static int shash_async_update(struct ahash_request *req)
{
	return shash_ahash_update(req, ahash_request_ctx(req));
}
