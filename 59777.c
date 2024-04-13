static void nfs_idmap_quit_keyring(void)
{
	key_revoke(id_resolver_cache->thread_keyring);
	unregister_key_type(&key_type_id_resolver);
	unregister_key_type(&key_type_id_resolver_legacy);
	put_cred(id_resolver_cache);
}
