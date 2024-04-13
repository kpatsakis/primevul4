static struct aead_request *crypto_rfc4543_crypt(struct aead_request *req,
						 bool enc)
{
	struct crypto_aead *aead = crypto_aead_reqtfm(req);
	struct crypto_rfc4543_ctx *ctx = crypto_aead_ctx(aead);
	struct crypto_rfc4543_req_ctx *rctx = crypto_rfc4543_reqctx(req);
	struct aead_request *subreq = &rctx->subreq;
	struct scatterlist *src = req->src;
	struct scatterlist *cipher = rctx->cipher;
	struct scatterlist *payload = rctx->payload;
	struct scatterlist *assoc = rctx->assoc;
	unsigned int authsize = crypto_aead_authsize(aead);
	unsigned int assoclen = req->assoclen;
	struct page *srcp;
	u8 *vsrc;
	u8 *iv = PTR_ALIGN((u8 *)(rctx + 1) + crypto_aead_reqsize(ctx->child),
			   crypto_aead_alignmask(ctx->child) + 1);

	memcpy(iv, ctx->nonce, 4);
	memcpy(iv + 4, req->iv, 8);

	/* construct cipher/plaintext */
	if (enc)
		memset(rctx->auth_tag, 0, authsize);
	else
		scatterwalk_map_and_copy(rctx->auth_tag, src,
					 req->cryptlen - authsize,
					 authsize, 0);

	sg_init_one(cipher, rctx->auth_tag, authsize);

	/* construct the aad */
	srcp = sg_page(src);
	vsrc = PageHighMem(srcp) ? NULL : page_address(srcp) + src->offset;

	sg_init_table(payload, 2);
	sg_set_buf(payload, req->iv, 8);
	scatterwalk_crypto_chain(payload, src, vsrc == req->iv + 8, 2);
	assoclen += 8 + req->cryptlen - (enc ? 0 : authsize);

	if (req->assoc->length == req->assoclen) {
		sg_init_table(assoc, 2);
		sg_set_page(assoc, sg_page(req->assoc), req->assoc->length,
			    req->assoc->offset);
	} else {
		BUG_ON(req->assoclen > sizeof(rctx->assocbuf));

		scatterwalk_map_and_copy(rctx->assocbuf, req->assoc, 0,
					 req->assoclen, 0);

		sg_init_table(assoc, 2);
		sg_set_buf(assoc, rctx->assocbuf, req->assoclen);
	}
	scatterwalk_crypto_chain(assoc, payload, 0, 2);

	aead_request_set_tfm(subreq, ctx->child);
	aead_request_set_callback(subreq, req->base.flags, crypto_rfc4543_done,
				  req);
	aead_request_set_crypt(subreq, cipher, cipher, enc ? 0 : authsize, iv);
	aead_request_set_assoc(subreq, assoc, assoclen);

	return subreq;
}
