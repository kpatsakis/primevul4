cifs_spnego_key_instantiate(struct key *key, struct key_preparsed_payload *prep)
{
	char *payload;
	int ret;

	ret = -ENOMEM;
	payload = kmemdup(prep->data, prep->datalen, GFP_KERNEL);
	if (!payload)
		goto error;

	/* attach the data */
	key->payload.data = payload;
	ret = 0;

error:
	return ret;
}
