static int ahash_sha256_digest(struct ahash_request *req)
{
	int ret2, ret1;

	ret1 = ahash_sha256_init(req);
	if (ret1)
		goto out;

	ret1 = ahash_update(req);
	ret2 = ahash_final(req);

out:
	return ret1 ? ret1 : ret2;
}
