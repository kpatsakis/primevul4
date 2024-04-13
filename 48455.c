static void __gcm_hash_assoc_remain_done(struct aead_request *req, int err)
{
	struct crypto_gcm_req_priv_ctx *pctx = crypto_gcm_reqctx(req);
	struct crypto_gcm_ghash_ctx *gctx = &pctx->ghash_ctx;
	crypto_completion_t compl;
	unsigned int remain = 0;

	if (!err && gctx->cryptlen) {
		remain = gcm_remain(gctx->cryptlen);
		compl = remain ? gcm_hash_crypt_done :
			gcm_hash_crypt_remain_done;
		err = gcm_hash_update(req, pctx, compl,
				      gctx->src, gctx->cryptlen);
		if (err == -EINPROGRESS || err == -EBUSY)
			return;
	}

	if (remain)
		__gcm_hash_crypt_done(req, err);
	else
		__gcm_hash_crypt_remain_done(req, err);
}
