static void __gcm_hash_crypt_remain_done(struct aead_request *req, int err)
{
	struct crypto_gcm_req_priv_ctx *pctx = crypto_gcm_reqctx(req);

	if (!err) {
		err = gcm_hash_len(req, pctx);
		if (err == -EINPROGRESS || err == -EBUSY)
			return;
	}

	__gcm_hash_len_done(req, err);
}
