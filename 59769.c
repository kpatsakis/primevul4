static int nfs_idmap_instantiate(struct key *key, struct key *authkey, char *data, size_t datalen)
{
	return key_instantiate_and_link(key, data, datalen,
					id_resolver_cache->thread_keyring,
					authkey);
}
