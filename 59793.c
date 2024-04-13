int ceph_crypto_key_decode(struct ceph_crypto_key *key, void **p, void *end)
{
	ceph_decode_need(p, end, 2*sizeof(u16) + sizeof(key->created), bad);
	key->type = ceph_decode_16(p);
	ceph_decode_copy(p, &key->created, sizeof(key->created));
	key->len = ceph_decode_16(p);
	ceph_decode_need(p, end, key->len, bad);
	key->key = kmalloc(key->len, GFP_NOFS);
	if (!key->key)
		return -ENOMEM;
	ceph_decode_copy(p, key->key, key->len);
	return 0;

bad:
	dout("failed to decode crypto key\n");
	return -EINVAL;
}
