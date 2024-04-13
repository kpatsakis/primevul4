static int hmac_sha256_init(struct ahash_request *req)
{
	struct crypto_ahash *tfm = crypto_ahash_reqtfm(req);
	struct hash_ctx *ctx = crypto_ahash_ctx(tfm);

	ctx->config.data_format	= HASH_DATA_8_BITS;
	ctx->config.algorithm	= HASH_ALGO_SHA256;
	ctx->config.oper_mode	= HASH_OPER_MODE_HMAC;
	ctx->digestsize		= SHA256_DIGEST_SIZE;

	return hash_init(req);
}
