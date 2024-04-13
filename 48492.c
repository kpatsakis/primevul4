static void gcm_encrypt_done(struct crypto_async_request *areq, int err)
{
	struct aead_request *req = areq->data;
	struct crypto_gcm_req_priv_ctx *pctx = crypto_gcm_reqctx(req);

	if (!err) {
		err = gcm_hash(req, pctx);
		if (err == -EINPROGRESS || err == -EBUSY)
			return;
		else if (!err) {
			crypto_xor(pctx->auth_tag, pctx->iauth_tag, 16);
			gcm_enc_copy_hash(req, pctx);
		}
	}

	aead_request_complete(req, err);
}
