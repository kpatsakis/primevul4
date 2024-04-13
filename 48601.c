static void seqiv_complete(struct crypto_async_request *base, int err)
{
	struct skcipher_givcrypt_request *req = base->data;

	seqiv_complete2(req, err);
	skcipher_givcrypt_complete(req, err);
}
