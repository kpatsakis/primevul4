static void eseqiv_complete2(struct skcipher_givcrypt_request *req)
{
	struct crypto_ablkcipher *geniv = skcipher_givcrypt_reqtfm(req);
	struct eseqiv_request_ctx *reqctx = skcipher_givcrypt_reqctx(req);

	memcpy(req->giv, PTR_ALIGN((u8 *)reqctx->tail,
			 crypto_ablkcipher_alignmask(geniv) + 1),
	       crypto_ablkcipher_ivsize(geniv));
}
