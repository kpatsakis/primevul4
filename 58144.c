static void crypto_ccm_encrypt_done(struct crypto_async_request *areq, int err)
{
	struct aead_request *req = areq->data;
	struct crypto_aead *aead = crypto_aead_reqtfm(req);
	struct crypto_ccm_req_priv_ctx *pctx = crypto_ccm_reqctx(req);
	u8 *odata = pctx->odata;

	if (!err)
		scatterwalk_map_and_copy(odata, req->dst,
					 req->assoclen + req->cryptlen,
					 crypto_aead_authsize(aead), 1);
	aead_request_complete(req, err);
}
