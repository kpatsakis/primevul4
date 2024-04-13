static int crypto_authenc_iverify(struct aead_request *req, u8 *iv,
				  unsigned int cryptlen)
{
	struct crypto_aead *authenc = crypto_aead_reqtfm(req);
	struct authenc_request_ctx *areq_ctx = aead_request_ctx(req);
	struct scatterlist *src = req->src;
	struct scatterlist *assoc = req->assoc;
	struct scatterlist *cipher = areq_ctx->cipher;
	struct scatterlist *asg = areq_ctx->asg;
	unsigned int ivsize = crypto_aead_ivsize(authenc);
	authenc_ahash_t authenc_ahash_fn = crypto_authenc_ahash_fb;
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

	if (req->assoclen && sg_is_last(assoc)) {
		authenc_ahash_fn = crypto_authenc_ahash;
		sg_init_table(asg, 2);
		sg_set_page(asg, sg_page(assoc), assoc->length, assoc->offset);
		scatterwalk_crypto_chain(asg, src, 0, 2);
		src = asg;
		cryptlen += req->assoclen;
	}

	areq_ctx->cryptlen = cryptlen;
	areq_ctx->sg = src;

	return crypto_authenc_verify(req, authenc_ahash_fn);
}
