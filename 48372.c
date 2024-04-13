static void cryptd_blkcipher_decrypt(struct crypto_async_request *req, int err)
{
	struct cryptd_blkcipher_ctx *ctx = crypto_tfm_ctx(req->tfm);
	struct crypto_blkcipher *child = ctx->child;

	cryptd_blkcipher_crypt(ablkcipher_request_cast(req), child, err,
			       crypto_blkcipher_crt(child)->decrypt);
}
