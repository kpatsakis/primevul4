struct shash_desc *cryptd_shash_desc(struct ahash_request *req)
{
	struct cryptd_hash_request_ctx *rctx = ahash_request_ctx(req);
	return &rctx->desc;
}
