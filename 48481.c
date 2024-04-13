static int crypto_rfc4543_copy_src_to_dst(struct aead_request *req, bool enc)
{
	struct crypto_aead *aead = crypto_aead_reqtfm(req);
	struct crypto_rfc4543_ctx *ctx = crypto_aead_ctx(aead);
	unsigned int authsize = crypto_aead_authsize(aead);
	unsigned int nbytes = req->cryptlen - (enc ? 0 : authsize);
	struct blkcipher_desc desc = {
		.tfm = ctx->null,
	};

	return crypto_blkcipher_encrypt(&desc, req->dst, req->src, nbytes);
}
