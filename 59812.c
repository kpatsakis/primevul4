struct key *rxrpc_get_null_key(const char *keyname)
{
	const struct cred *cred = current_cred();
	struct key *key;
	int ret;

	key = key_alloc(&key_type_rxrpc, keyname,
			GLOBAL_ROOT_UID, GLOBAL_ROOT_GID, cred,
			KEY_POS_SEARCH, KEY_ALLOC_NOT_IN_QUOTA);
	if (IS_ERR(key))
		return key;

	ret = key_instantiate_and_link(key, NULL, 0, NULL, NULL);
	if (ret < 0) {
		key_revoke(key);
		key_put(key);
		return ERR_PTR(ret);
	}

	return key;
}
