static void __gcm_hash_len_done(struct aead_request *req, int err)
{
	struct crypto_gcm_req_priv_ctx *pctx = crypto_gcm_reqctx(req);

	if (!err) {
		err = gcm_hash_final(req, pctx);
		if (err == -EINPROGRESS || err == -EBUSY)
			return;
	}

	__gcm_hash_final_done(req, err);
}
