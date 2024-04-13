int skcipher_walk_aead_encrypt(struct skcipher_walk *walk,
			       struct aead_request *req, bool atomic)
{
	walk->total = req->cryptlen;

	return skcipher_walk_aead_common(walk, req, atomic);
}
