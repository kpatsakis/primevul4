static int crypto_authenc_genicv(struct aead_request *req, u8 *iv,
				 unsigned int flags)
{
	struct crypto_aead *authenc = crypto_aead_reqtfm(req);
	struct authenc_request_ctx *areq_ctx = aead_request_ctx(req);
	struct scatterlist *dst = req->dst;
	struct scatterlist *assoc = req->assoc;
	struct scatterlist *cipher = areq_ctx->cipher;
	struct scatterlist *asg = areq_ctx->asg;
	unsigned int ivsize = crypto_aead_ivsize(authenc);
	unsigned int cryptlen = req->cryptlen;
	authenc_ahash_t authenc_ahash_fn = crypto_authenc_ahash_fb;
	struct page *dstp;
	u8 *vdst;
	u8 *hash;

	dstp = sg_page(dst);
	vdst = PageHighMem(dstp) ? NULL : page_address(dstp) + dst->offset;

	if (ivsize) {
		sg_init_table(cipher, 2);
		sg_set_buf(cipher, iv, ivsize);
		scatterwalk_crypto_chain(cipher, dst, vdst == iv + ivsize, 2);
		dst = cipher;
		cryptlen += ivsize;
	}

	if (req->assoclen && sg_is_last(assoc)) {
		authenc_ahash_fn = crypto_authenc_ahash;
		sg_init_table(asg, 2);
		sg_set_page(asg, sg_page(assoc), assoc->length, assoc->offset);
		scatterwalk_crypto_chain(asg, dst, 0, 2);
		dst = asg;
		cryptlen += req->assoclen;
	}

	areq_ctx->cryptlen = cryptlen;
	areq_ctx->sg = dst;

	areq_ctx->complete = authenc_geniv_ahash_done;
	areq_ctx->update_complete = authenc_geniv_ahash_update_done;

	hash = authenc_ahash_fn(req, flags);
	if (IS_ERR(hash))
		return PTR_ERR(hash);

	scatterwalk_map_and_copy(hash, dst, cryptlen,
				 crypto_aead_authsize(authenc), 1);
	return 0;
}
