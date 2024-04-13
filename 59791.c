int ceph_crypto_init(void) {
	return register_key_type(&key_type_ceph);
}
