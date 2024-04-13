static int shash_async_final(struct ahash_request *req)
{
	return crypto_shash_final(ahash_request_ctx(req), req->result);
}
