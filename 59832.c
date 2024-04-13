static struct key *request_user_key(const char *master_desc, u8 **master_key,
				    size_t *master_keylen)
{
	struct user_key_payload *upayload;
	struct key *ukey;

	ukey = request_key(&key_type_user, master_desc, NULL);
	if (IS_ERR(ukey))
		goto error;

	down_read(&ukey->sem);
	upayload = ukey->payload.data;
	*master_key = upayload->data;
	*master_keylen = upayload->datalen;
error:
	return ukey;
}
