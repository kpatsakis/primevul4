static int skcipher_crypt_ablkcipher(struct skcipher_request *req,
				     int (*crypt)(struct ablkcipher_request *))
{
	struct crypto_skcipher *tfm = crypto_skcipher_reqtfm(req);
	struct crypto_ablkcipher **ctx = crypto_skcipher_ctx(tfm);
	struct ablkcipher_request *subreq = skcipher_request_ctx(req);

	ablkcipher_request_set_tfm(subreq, *ctx);
	ablkcipher_request_set_callback(subreq, skcipher_request_flags(req),
					req->base.complete, req->base.data);
	ablkcipher_request_set_crypt(subreq, req->src, req->dst, req->cryptlen,
				     req->iv);

	return crypt(subreq);
}
