static int crypto_authenc_esn_iverify(struct aead_request *req, u8 *iv,
				      unsigned int cryptlen)
{
	struct crypto_aead *authenc_esn = crypto_aead_reqtfm(req);
	struct authenc_esn_request_ctx *areq_ctx = aead_request_ctx(req);
	struct scatterlist *src = req->src;
	struct scatterlist *assoc = req->assoc;
	struct scatterlist *cipher = areq_ctx->cipher;
	struct scatterlist *hsg = areq_ctx->hsg;
	struct scatterlist *tsg = areq_ctx->tsg;
	struct scatterlist *assoc1;
	struct scatterlist *assoc2;
	unsigned int ivsize = crypto_aead_ivsize(authenc_esn);
	struct page *srcp;
	u8 *vsrc;

	srcp = sg_page(src);
	vsrc = PageHighMem(srcp) ? NULL : page_address(srcp) + src->offset;

	if (ivsize) {
		sg_init_table(cipher, 2);
		sg_set_buf(cipher, iv, ivsize);
		scatterwalk_crypto_chain(cipher, src, vsrc == iv + ivsize, 2);
		src = cipher;
		cryptlen += ivsize;
	}

	if (sg_is_last(assoc))
		return -EINVAL;

	assoc1 = assoc + 1;
	if (sg_is_last(assoc1))
		return -EINVAL;

	assoc2 = assoc + 2;
	if (!sg_is_last(assoc2))
		return -EINVAL;

	sg_init_table(hsg, 2);
	sg_set_page(hsg, sg_page(assoc), assoc->length, assoc->offset);
	sg_set_page(hsg + 1, sg_page(assoc2), assoc2->length, assoc2->offset);

	sg_init_table(tsg, 1);
	sg_set_page(tsg, sg_page(assoc1), assoc1->length, assoc1->offset);

	areq_ctx->cryptlen = cryptlen;
	areq_ctx->headlen = assoc->length + assoc2->length;
	areq_ctx->trailen = assoc1->length;
	areq_ctx->sg = src;

	areq_ctx->complete = authenc_esn_verify_ahash_done;
	areq_ctx->update_complete = authenc_esn_verify_ahash_update_done;
	areq_ctx->update_complete2 = authenc_esn_verify_ahash_update_done2;

	return crypto_authenc_esn_verify(req);
}
