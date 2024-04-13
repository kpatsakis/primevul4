static void seqiv_aead_complete(struct crypto_async_request *base, int err)
{
	struct aead_givcrypt_request *req = base->data;

	seqiv_aead_complete2(req, err);
	aead_givcrypt_complete(req, err);
}
