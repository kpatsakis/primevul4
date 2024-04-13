static int gcm_hash_remain(struct aead_request *req,
			   struct crypto_gcm_req_priv_ctx *pctx,
			   unsigned int remain,
			   crypto_completion_t compl)
{
	struct ahash_request *ahreq = &pctx->u.ahreq;

	ahash_request_set_callback(ahreq, aead_request_flags(req),
				   compl, req);
	sg_init_one(pctx->src, gcm_zeroes, remain);
	ahash_request_set_crypt(ahreq, pctx->src, NULL, remain);

	return crypto_ahash_update(ahreq);
}
