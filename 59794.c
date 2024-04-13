int ceph_crypto_key_encode(struct ceph_crypto_key *key, void **p, void *end)
{
	if (*p + sizeof(u16) + sizeof(key->created) +
	    sizeof(u16) + key->len > end)
		return -ERANGE;
	ceph_encode_16(p, key->type);
	ceph_encode_copy(p, &key->created, sizeof(key->created));
	ceph_encode_16(p, key->len);
	ceph_encode_copy(p, key->key, key->len);
	return 0;
}
