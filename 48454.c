static void __gcm_hash_assoc_done(struct aead_request *req, int err)
{
	struct crypto_gcm_req_priv_ctx *pctx = crypto_gcm_reqctx(req);
	unsigned int remain;

	if (!err) {
		remain = gcm_remain(req->assoclen);
		BUG_ON(!remain);
		err = gcm_hash_remain(req, pctx, remain,
				      gcm_hash_assoc_remain_done);
		if (err == -EINPROGRESS || err == -EBUSY)
			return;
	}

	__gcm_hash_assoc_remain_done(req, err);
}
