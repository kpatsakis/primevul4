static void authenc_esn_geniv_ahash_done(struct crypto_async_request *areq,
					 int err)
{
	struct aead_request *req = areq->data;
	struct crypto_aead *authenc_esn = crypto_aead_reqtfm(req);
	struct crypto_authenc_esn_ctx *ctx = crypto_aead_ctx(authenc_esn);
	struct authenc_esn_request_ctx *areq_ctx = aead_request_ctx(req);
	struct ahash_request *ahreq = (void *)(areq_ctx->tail + ctx->reqoff);

	if (err)
		goto out;

	scatterwalk_map_and_copy(ahreq->result, areq_ctx->sg,
				 areq_ctx->cryptlen,
				 crypto_aead_authsize(authenc_esn), 1);

out:
	aead_request_complete(req, err);
}
