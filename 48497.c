static void gcm_hash_final_done(struct crypto_async_request *areq, int err)
{
	struct aead_request *req = areq->data;

	__gcm_hash_final_done(req, err);
}
