int skcipher_null_givencrypt(struct skcipher_givcrypt_request *req)
{
	return crypto_ablkcipher_encrypt(&req->creq);
}
