static void authenc_esn_verify_ahash_update_done(struct crypto_async_request *areq,
						 int err)
{
	u8 *ihash;
	unsigned int authsize;
	struct ablkcipher_request *abreq;
	struct aead_request *req = areq->data;
	struct crypto_aead *authenc_esn = crypto_aead_reqtfm(req);
	struct crypto_authenc_esn_ctx *ctx = crypto_aead_ctx(authenc_esn);
	struct authenc_esn_request_ctx *areq_ctx = aead_request_ctx(req);
	struct ahash_request *ahreq = (void *)(areq_ctx->tail + ctx->reqoff);
	unsigned int cryptlen = req->cryptlen;

	if (err)
		goto out;

	ahash_request_set_crypt(ahreq, areq_ctx->sg, ahreq->result,
				areq_ctx->cryptlen);

	ahash_request_set_callback(ahreq,
				   aead_request_flags(req) &
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

	authsize = crypto_aead_authsize(authenc_esn);
	cryptlen -= authsize;
	ihash = ahreq->result + authsize;
	scatterwalk_map_and_copy(ihash, areq_ctx->sg, areq_ctx->cryptlen,
				 authsize, 0);

	err = crypto_memneq(ihash, ahreq->result, authsize) ? -EBADMSG : 0;
	if (err)
		goto out;

	abreq = aead_request_ctx(req);
	ablkcipher_request_set_tfm(abreq, ctx->enc);
	ablkcipher_request_set_callback(abreq, aead_request_flags(req),
					req->base.complete, req->base.data);
	ablkcipher_request_set_crypt(abreq, req->src, req->dst,
				     cryptlen, req->iv);

	err = crypto_ablkcipher_decrypt(abreq);

out:
	authenc_esn_request_complete(req, err);
}
