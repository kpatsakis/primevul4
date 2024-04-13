static int ahash_final(struct ahash_request *req)
{
	int ret = 0;
	struct hash_req_ctx *req_ctx = ahash_request_ctx(req);

	pr_debug("%s: data size: %d\n", __func__, req->nbytes);

	if ((hash_mode == HASH_MODE_DMA) && req_ctx->dma_mode)
		ret = hash_dma_final(req);
	else
		ret = hash_hw_final(req);

	if (ret) {
		pr_err("%s: hash_hw/dma_final() failed\n", __func__);
	}

	return ret;
}
