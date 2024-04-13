static void crypto_rfc4543_done(struct crypto_async_request *areq, int err)
{
	struct aead_request *req = areq->data;
	struct crypto_aead *aead = crypto_aead_reqtfm(req);
	struct crypto_rfc4543_req_ctx *rctx = crypto_rfc4543_reqctx(req);

	if (!err) {
		scatterwalk_map_and_copy(rctx->auth_tag, req->dst,
					 req->cryptlen,
					 crypto_aead_authsize(aead), 1);
	}

	aead_request_complete(req, err);
}
