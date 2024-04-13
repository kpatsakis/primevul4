static int gcm_hash_len(struct aead_request *req,
			struct crypto_gcm_req_priv_ctx *pctx)
{
	struct ahash_request *ahreq = &pctx->u.ahreq;
	struct crypto_gcm_ghash_ctx *gctx = &pctx->ghash_ctx;
	u128 lengths;

	lengths.a = cpu_to_be64(req->assoclen * 8);
	lengths.b = cpu_to_be64(gctx->cryptlen * 8);
	memcpy(pctx->iauth_tag, &lengths, 16);
	sg_init_one(pctx->src, pctx->iauth_tag, 16);
	ahash_request_set_callback(ahreq, aead_request_flags(req),
				   gcm_hash_len_done, req);
	ahash_request_set_crypt(ahreq, pctx->src,
				NULL, sizeof(lengths));

	return crypto_ahash_update(ahreq);
}
