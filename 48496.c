static int gcm_hash_final(struct aead_request *req,
			  struct crypto_gcm_req_priv_ctx *pctx)
{
	struct ahash_request *ahreq = &pctx->u.ahreq;

	ahash_request_set_callback(ahreq, aead_request_flags(req),
				   gcm_hash_final_done, req);
	ahash_request_set_crypt(ahreq, NULL, pctx->iauth_tag, 0);

	return crypto_ahash_final(ahreq);
}
