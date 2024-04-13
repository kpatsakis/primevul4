int rxrpc_get_server_data_key(struct rxrpc_connection *conn,
			      const void *session_key,
			      time_t expiry,
			      u32 kvno)
{
	const struct cred *cred = current_cred();
	struct key *key;
	int ret;

	struct {
		u32 kver;
		struct rxrpc_key_data_v1 v1;
	} data;

	_enter("");

	key = key_alloc(&key_type_rxrpc, "x",
			GLOBAL_ROOT_UID, GLOBAL_ROOT_GID, cred, 0,
			KEY_ALLOC_NOT_IN_QUOTA);
	if (IS_ERR(key)) {
		_leave(" = -ENOMEM [alloc %ld]", PTR_ERR(key));
		return -ENOMEM;
	}

	_debug("key %d", key_serial(key));

	data.kver = 1;
	data.v1.security_index = RXRPC_SECURITY_RXKAD;
	data.v1.ticket_length = 0;
	data.v1.expiry = expiry;
	data.v1.kvno = 0;

	memcpy(&data.v1.session_key, session_key, sizeof(data.v1.session_key));

	ret = key_instantiate_and_link(key, &data, sizeof(data), NULL, NULL);
	if (ret < 0)
		goto error;

	conn->key = key;
	_leave(" = 0 [%d]", key_serial(key));
	return 0;

error:
	key_revoke(key);
	key_put(key);
	_leave(" = -ENOMEM [ins %d]", ret);
	return -ENOMEM;
}
