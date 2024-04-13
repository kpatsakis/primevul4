static void __gcm_hash_init_done(struct aead_request *req, int err)
{
	struct crypto_gcm_req_priv_ctx *pctx = crypto_gcm_reqctx(req);
	crypto_completion_t compl;
	unsigned int remain = 0;

	if (!err && req->assoclen) {
		remain = gcm_remain(req->assoclen);
		compl = remain ? gcm_hash_assoc_done :
			gcm_hash_assoc_remain_done;
		err = gcm_hash_update(req, pctx, compl,
				      req->assoc, req->assoclen);
		if (err == -EINPROGRESS || err == -EBUSY)
			return;
	}

	if (remain)
		__gcm_hash_assoc_done(req, err);
	else
		__gcm_hash_assoc_remain_done(req, err);
}
