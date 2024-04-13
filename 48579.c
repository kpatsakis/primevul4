static int pcrypt_aead_givencrypt(struct aead_givcrypt_request *req)
{
	int err;
	struct aead_request *areq = &req->areq;
	struct pcrypt_request *preq = aead_request_ctx(areq);
	struct aead_givcrypt_request *creq = pcrypt_request_ctx(preq);
	struct padata_priv *padata = pcrypt_request_padata(preq);
	struct crypto_aead *aead = aead_givcrypt_reqtfm(req);
	struct pcrypt_aead_ctx *ctx = crypto_aead_ctx(aead);
	u32 flags = aead_request_flags(areq);

	memset(padata, 0, sizeof(struct padata_priv));

	padata->parallel = pcrypt_aead_givenc;
	padata->serial = pcrypt_aead_giv_serial;

	aead_givcrypt_set_tfm(creq, ctx->child);
	aead_givcrypt_set_callback(creq, flags & ~CRYPTO_TFM_REQ_MAY_SLEEP,
				   pcrypt_aead_done, areq);
	aead_givcrypt_set_crypt(creq, areq->src, areq->dst,
				areq->cryptlen, areq->iv);
	aead_givcrypt_set_assoc(creq, areq->assoc, areq->assoclen);
	aead_givcrypt_set_giv(creq, req->giv, req->seq);

	err = pcrypt_do_parallel(padata, &ctx->cb_cpu, &pencrypt);
	if (!err)
		return -EINPROGRESS;

	return err;
}
