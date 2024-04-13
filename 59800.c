static void ceph_key_free_preparse(struct key_preparsed_payload *prep)
{
	struct ceph_crypto_key *ckey = prep->payload[0];
	ceph_crypto_key_destroy(ckey);
	kfree(ckey);
}
