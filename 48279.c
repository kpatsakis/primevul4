static void authenc_esn_geniv_ahash_update_done(struct crypto_async_request *areq,
						int err)
{
	struct aead_request *req = areq->data;
	struct crypto_aead *authenc_esn = crypto_aead_reqtfm(req);
	struct crypto_authenc_esn_ctx *ctx = crypto_aead_ctx(authenc_esn);
	struct authenc_esn_request_ctx *areq_ctx = aead_request_ctx(req);
	struct ahash_request *ahreq = (void *)(areq_ctx->tail + ctx->reqoff);

	if (err)
		goto out;

	ahash_request_set_crypt(ahreq, areq_ctx->sg, ahreq->result,
				areq_ctx->cryptlen);
	ahash_request_set_callback(ahreq, aead_request_flags(req) &
					  CRYPTO_TFM_REQ_MAY_SLEEP,
				   areq_ctx->update_complete2, req);

	err = crypto_ahash_update(ahreq);
	if (err)
		goto out;

	ahash_request_set_crypt(ahreq, areq_ctx->tsg, ahreq->result,
				areq_ctx->trailen);
	ahash_request_set_callback(ahreq, aead_request_flags(req) &
					  CRYPTO_TFM_REQ_MAY_SLEEP,
				   areq_ctx->complete, req);

	err = crypto_ahash_finup(ahreq);
	if (err)
		goto out;

	scatterwalk_map_and_copy(ahreq->result, areq_ctx->sg,
				 areq_ctx->cryptlen,
				 crypto_aead_authsize(authenc_esn), 1);

out:
	authenc_esn_request_complete(req, err);
}
