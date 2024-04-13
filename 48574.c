static int pcrypt_aead_decrypt(struct aead_request *req)
{
	int err;
	struct pcrypt_request *preq = aead_request_ctx(req);
	struct aead_request *creq = pcrypt_request_ctx(preq);
	struct padata_priv *padata = pcrypt_request_padata(preq);
	struct crypto_aead *aead = crypto_aead_reqtfm(req);
	struct pcrypt_aead_ctx *ctx = crypto_aead_ctx(aead);
	u32 flags = aead_request_flags(req);

	memset(padata, 0, sizeof(struct padata_priv));

	padata->parallel = pcrypt_aead_dec;
	padata->serial = pcrypt_aead_serial;

	aead_request_set_tfm(creq, ctx->child);
	aead_request_set_callback(creq, flags & ~CRYPTO_TFM_REQ_MAY_SLEEP,
				  pcrypt_aead_done, req);
	aead_request_set_crypt(creq, req->src, req->dst,
			       req->cryptlen, req->iv);
	aead_request_set_assoc(creq, req->assoc, req->assoclen);

	err = pcrypt_do_parallel(padata, &ctx->cb_cpu, &pdecrypt);
	if (!err)
		return -EINPROGRESS;

	return err;
}
