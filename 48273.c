static void crypto_authenc_givencrypt_done(struct crypto_async_request *req,
					   int err)
{
	struct aead_request *areq = req->data;

	if (!err) {
		struct skcipher_givcrypt_request *greq = aead_request_ctx(areq);

		err = crypto_authenc_genicv(areq, greq->giv, 0);
	}

	authenc_request_complete(areq, err);
}
