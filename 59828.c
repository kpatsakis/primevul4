void big_key_revoke(struct key *key)
{
	struct path *path = (struct path *)&key->payload.data2;

	/* clear the quota */
	key_payload_reserve(key, 0);
	if (key_is_instantiated(key) && key->type_data.x[1] > BIG_KEY_FILE_THRESHOLD)
		vfs_truncate(path, 0);
}
