static int cryptd_aead_encrypt_enqueue(struct aead_request *req)
{
	return cryptd_aead_enqueue(req, cryptd_aead_encrypt );
}
