static void __gcm_hash_final_done(struct aead_request *req, int err)
{
	struct crypto_gcm_req_priv_ctx *pctx = crypto_gcm_reqctx(req);
	struct crypto_gcm_ghash_ctx *gctx = &pctx->ghash_ctx;

	if (!err)
		crypto_xor(pctx->auth_tag, pctx->iauth_tag, 16);

	gctx->complete(req, err);
}
