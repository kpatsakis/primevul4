static int crypto_gcm_verify(struct aead_request *req,
			     struct crypto_gcm_req_priv_ctx *pctx)
{
	struct crypto_aead *aead = crypto_aead_reqtfm(req);
	u8 *auth_tag = pctx->auth_tag;
	u8 *iauth_tag = pctx->iauth_tag;
	unsigned int authsize = crypto_aead_authsize(aead);
	unsigned int cryptlen = req->cryptlen - authsize;

	crypto_xor(auth_tag, iauth_tag, 16);
	scatterwalk_map_and_copy(iauth_tag, req->src, cryptlen, authsize, 0);
	return crypto_memneq(iauth_tag, auth_tag, authsize) ? -EBADMSG : 0;
}
