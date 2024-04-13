static int crypto_rfc4106_encrypt(struct aead_request *req)
{
	req = crypto_rfc4106_crypt(req);

	return crypto_aead_encrypt(req);
}
