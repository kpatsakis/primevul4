struct shash_desc *mcryptd_shash_desc(struct ahash_request *req)
{
	struct mcryptd_hash_request_ctx *rctx = ahash_request_ctx(req);
	return &rctx->desc;
}
