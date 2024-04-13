static int cryptd_hash_digest_enqueue(struct ahash_request *req)
{
	return cryptd_hash_enqueue(req, cryptd_hash_digest);
}
