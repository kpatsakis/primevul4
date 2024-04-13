static void gcm_hash_init_done(struct crypto_async_request *areq, int err)
{
	struct aead_request *req = areq->data;

	__gcm_hash_init_done(req, err);
}
