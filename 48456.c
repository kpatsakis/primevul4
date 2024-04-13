static void __gcm_hash_crypt_done(struct aead_request *req, int err)
{
	struct crypto_gcm_req_priv_ctx *pctx = crypto_gcm_reqctx(req);
	struct crypto_gcm_ghash_ctx *gctx = &pctx->ghash_ctx;
	unsigned int remain;

	if (!err) {
		remain = gcm_remain(gctx->cryptlen);
		BUG_ON(!remain);
		err = gcm_hash_remain(req, pctx, remain,
				      gcm_hash_crypt_remain_done);
		if (err == -EINPROGRESS || err == -EBUSY)
			return;
	}

	__gcm_hash_crypt_remain_done(req, err);
}
