int key_instantiate_and_link(struct key *key,
			     const void *data,
			     size_t datalen,
			     struct key *keyring,
			     struct key *authkey)
{
	struct key_preparsed_payload prep;
	struct assoc_array_edit *edit;
	int ret;

	memset(&prep, 0, sizeof(prep));
	prep.data = data;
	prep.datalen = datalen;
	prep.quotalen = key->type->def_datalen;
	prep.expiry = TIME_T_MAX;
	if (key->type->preparse) {
		ret = key->type->preparse(&prep);
		if (ret < 0)
			goto error;
	}

	if (keyring) {
		if (keyring->restrict_link) {
			ret = keyring->restrict_link(keyring, key->type,
						     &prep.payload);
			if (ret < 0)
				goto error;
		}
		ret = __key_link_begin(keyring, &key->index_key, &edit);
		if (ret < 0)
			goto error;
	}

	ret = __key_instantiate_and_link(key, &prep, keyring, authkey, &edit);

	if (keyring)
		__key_link_end(keyring, &key->index_key, edit);

error:
	if (key->type->preparse)
		key->type->free_preparse(&prep);
	return ret;
}
