static int crypto_ccm_decrypt(struct aead_request *req)
{
	struct crypto_aead *aead = crypto_aead_reqtfm(req);
	struct crypto_ccm_ctx *ctx = crypto_aead_ctx(aead);
	struct crypto_ccm_req_priv_ctx *pctx = crypto_ccm_reqctx(req);
	struct ablkcipher_request *abreq = &pctx->abreq;
	struct scatterlist *dst;
	unsigned int authsize = crypto_aead_authsize(aead);
	unsigned int cryptlen = req->cryptlen;
	u8 *authtag = pctx->auth_tag;
	u8 *odata = pctx->odata;
	u8 *iv = req->iv;
	int err;

	if (cryptlen < authsize)
		return -EINVAL;
	cryptlen -= authsize;

	err = crypto_ccm_check_iv(iv);
	if (err)
		return err;

	pctx->flags = aead_request_flags(req);

	scatterwalk_map_and_copy(authtag, req->src, cryptlen, authsize, 0);

	memset(iv + 15 - iv[0], 0, iv[0] + 1);

	sg_init_table(pctx->src, 2);
	sg_set_buf(pctx->src, authtag, 16);
	scatterwalk_sg_chain(pctx->src, 2, req->src);

	dst = pctx->src;
	if (req->src != req->dst) {
		sg_init_table(pctx->dst, 2);
		sg_set_buf(pctx->dst, authtag, 16);
		scatterwalk_sg_chain(pctx->dst, 2, req->dst);
		dst = pctx->dst;
	}

	ablkcipher_request_set_tfm(abreq, ctx->ctr);
	ablkcipher_request_set_callback(abreq, pctx->flags,
					crypto_ccm_decrypt_done, req);
	ablkcipher_request_set_crypt(abreq, pctx->src, dst, cryptlen + 16, iv);
	err = crypto_ablkcipher_decrypt(abreq);
	if (err)
		return err;

	err = crypto_ccm_auth(req, req->dst, cryptlen);
	if (err)
		return err;

	/* verify */
	if (crypto_memneq(authtag, odata, authsize))
		return -EBADMSG;

	return err;
}
