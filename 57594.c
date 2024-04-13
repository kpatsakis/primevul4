static int skcipher_crypt_blkcipher(struct skcipher_request *req,
				    int (*crypt)(struct blkcipher_desc *,
						 struct scatterlist *,
						 struct scatterlist *,
						 unsigned int))
{
	struct crypto_skcipher *tfm = crypto_skcipher_reqtfm(req);
	struct crypto_blkcipher **ctx = crypto_skcipher_ctx(tfm);
	struct blkcipher_desc desc = {
		.tfm = *ctx,
		.info = req->iv,
		.flags = req->base.flags,
	};


	return crypt(&desc, req->dst, req->src, req->cryptlen);
}
