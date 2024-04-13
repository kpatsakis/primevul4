static void gcm_enc_hash_done(struct aead_request *req, int err)
{
	struct crypto_gcm_req_priv_ctx *pctx = crypto_gcm_reqctx(req);

	if (!err)
		gcm_enc_copy_hash(req, pctx);

	aead_request_complete(req, err);
}
