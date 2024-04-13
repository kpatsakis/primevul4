static int ghash_async_final(struct ahash_request *req)
{
	struct ahash_request *cryptd_req = ahash_request_ctx(req);

	if (!irq_fpu_usable()) {
		struct crypto_ahash *tfm = crypto_ahash_reqtfm(req);
		struct ghash_async_ctx *ctx = crypto_ahash_ctx(tfm);
		struct cryptd_ahash *cryptd_tfm = ctx->cryptd_tfm;

		memcpy(cryptd_req, req, sizeof(*req));
		ahash_request_set_tfm(cryptd_req, &cryptd_tfm->base);
		return crypto_ahash_final(cryptd_req);
	} else {
		struct shash_desc *desc = cryptd_shash_desc(cryptd_req);
		return crypto_shash_final(desc, req->result);
	}
}
