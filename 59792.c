int ceph_crypto_key_clone(struct ceph_crypto_key *dst,
			  const struct ceph_crypto_key *src)
{
	memcpy(dst, src, sizeof(struct ceph_crypto_key));
	dst->key = kmemdup(src->key, src->len, GFP_NOFS);
	if (!dst->key)
		return -ENOMEM;
	return 0;
}
