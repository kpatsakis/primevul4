static int gcm_hash_update(struct aead_request *req,
			   struct crypto_gcm_req_priv_ctx *pctx,
			   crypto_completion_t compl,
			   struct scatterlist *src,
			   unsigned int len)
{
	struct ahash_request *ahreq = &pctx->u.ahreq;

	ahash_request_set_callback(ahreq, aead_request_flags(req),
				   compl, req);
	ahash_request_set_crypt(ahreq, src, NULL, len);

	return crypto_ahash_update(ahreq);
}
