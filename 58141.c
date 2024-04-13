static int crypto_ccm_decrypt(struct aead_request *req)
{
	struct crypto_aead *aead = crypto_aead_reqtfm(req);
	struct crypto_ccm_ctx *ctx = crypto_aead_ctx(aead);
	struct crypto_ccm_req_priv_ctx *pctx = crypto_ccm_reqctx(req);
	struct skcipher_request *skreq = &pctx->skreq;
	struct scatterlist *dst;
	unsigned int authsize = crypto_aead_authsize(aead);
	unsigned int cryptlen = req->cryptlen;
	u8 *authtag = pctx->auth_tag;
	u8 *odata = pctx->odata;
	u8 *iv = req->iv;
	int err;

	cryptlen -= authsize;

	err = crypto_ccm_init_crypt(req, authtag);
	if (err)
		return err;

	scatterwalk_map_and_copy(authtag, sg_next(pctx->src), cryptlen,
				 authsize, 0);

	dst = pctx->src;
	if (req->src != req->dst)
		dst = pctx->dst;

	skcipher_request_set_tfm(skreq, ctx->ctr);
	skcipher_request_set_callback(skreq, pctx->flags,
				      crypto_ccm_decrypt_done, req);
	skcipher_request_set_crypt(skreq, pctx->src, dst, cryptlen + 16, iv);
	err = crypto_skcipher_decrypt(skreq);
	if (err)
		return err;

	err = crypto_ccm_auth(req, sg_next(dst), cryptlen);
	if (err)
		return err;

	/* verify */
	if (crypto_memneq(authtag, odata, authsize))
		return -EBADMSG;

	return err;
}
