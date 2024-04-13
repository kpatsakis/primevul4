static void eseqiv_complete(struct crypto_async_request *base, int err)
{
	struct skcipher_givcrypt_request *req = base->data;

	if (err)
		goto out;

	eseqiv_complete2(req);

out:
	skcipher_givcrypt_complete(req, err);
}
