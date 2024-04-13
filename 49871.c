static int hash_init(struct ahash_request *req)
{
	struct crypto_ahash *tfm = crypto_ahash_reqtfm(req);
	struct hash_ctx *ctx = crypto_ahash_ctx(tfm);
	struct hash_req_ctx *req_ctx = ahash_request_ctx(req);

	if (!ctx->key)
		ctx->keylen = 0;

	memset(&req_ctx->state, 0, sizeof(struct hash_state));
	req_ctx->updated = 0;
	if (hash_mode == HASH_MODE_DMA) {
		if (req->nbytes < HASH_DMA_ALIGN_SIZE) {
			req_ctx->dma_mode = false; /* Don't use DMA */

			pr_debug("%s: DMA mode, but direct to CPU mode for data size < %d\n",
				 __func__, HASH_DMA_ALIGN_SIZE);
		} else {
			if (req->nbytes >= HASH_DMA_PERFORMANCE_MIN_SIZE &&
			    hash_dma_valid_data(req->src, req->nbytes)) {
				req_ctx->dma_mode = true;
			} else {
				req_ctx->dma_mode = false;
				pr_debug("%s: DMA mode, but use CPU mode for datalength < %d or non-aligned data, except in last nent\n",
					 __func__,
					 HASH_DMA_PERFORMANCE_MIN_SIZE);
			}
		}
	}
	return 0;
}
