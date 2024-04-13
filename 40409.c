static int aead_null_givencrypt(struct aead_givcrypt_request *req)
{
	return crypto_aead_encrypt(&req->areq);
}
