cifs_idmap_key_instantiate(struct key *key, struct key_preparsed_payload *prep)
{
	char *payload;

	/*
	 * If the payload is less than or equal to the size of a pointer, then
	 * an allocation here is wasteful. Just copy the data directly to the
	 * payload.value union member instead.
	 *
	 * With this however, you must check the datalen before trying to
	 * dereference payload.data!
	 */
	if (prep->datalen <= sizeof(key->payload)) {
		key->payload.value = 0;
		memcpy(&key->payload.value, prep->data, prep->datalen);
		key->datalen = prep->datalen;
		return 0;
	}
	payload = kmemdup(prep->data, prep->datalen, GFP_KERNEL);
	if (!payload)
		return -ENOMEM;

	key->payload.data = payload;
	key->datalen = prep->datalen;
	return 0;
}
