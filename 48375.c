static int cryptd_blkcipher_encrypt_enqueue(struct ablkcipher_request *req)
{
	return cryptd_blkcipher_enqueue(req, cryptd_blkcipher_encrypt);
}
