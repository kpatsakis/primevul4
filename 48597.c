static int seqiv_aead_givencrypt(struct aead_givcrypt_request *req)
{
	struct crypto_aead *geniv = aead_givcrypt_reqtfm(req);
	struct seqiv_ctx *ctx = crypto_aead_ctx(geniv);
	struct aead_request *areq = &req->areq;
	struct aead_request *subreq = aead_givcrypt_reqctx(req);
	crypto_completion_t compl;
	void *data;
	u8 *info;
	unsigned int ivsize;
	int err;

	aead_request_set_tfm(subreq, aead_geniv_base(geniv));

	compl = areq->base.complete;
	data = areq->base.data;
	info = areq->iv;

	ivsize = crypto_aead_ivsize(geniv);

	if (unlikely(!IS_ALIGNED((unsigned long)info,
				 crypto_aead_alignmask(geniv) + 1))) {
		info = kmalloc(ivsize, areq->base.flags &
				       CRYPTO_TFM_REQ_MAY_SLEEP ? GFP_KERNEL:
								  GFP_ATOMIC);
		if (!info)
			return -ENOMEM;

		compl = seqiv_aead_complete;
		data = req;
	}

	aead_request_set_callback(subreq, areq->base.flags, compl, data);
	aead_request_set_crypt(subreq, areq->src, areq->dst, areq->cryptlen,
			       info);
	aead_request_set_assoc(subreq, areq->assoc, areq->assoclen);

	seqiv_geniv(ctx, info, req->seq, ivsize);
	memcpy(req->giv, info, ivsize);

	err = crypto_aead_encrypt(subreq);
	if (unlikely(info != areq->iv))
		seqiv_aead_complete2(req, err);
	return err;
}
