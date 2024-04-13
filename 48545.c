static int mcryptd_hash_digest_enqueue(struct ahash_request *req)
{
	return mcryptd_hash_enqueue(req, mcryptd_hash_digest);
}
