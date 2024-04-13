static int aead_null_givdecrypt(struct aead_givcrypt_request *req)
{
	return crypto_aead_decrypt(&req->areq);
}
