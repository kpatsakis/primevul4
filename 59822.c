int rxrpc_server_keyring(struct rxrpc_sock *rx, char __user *optval,
			 int optlen)
{
	struct key *key;
	char *description;

	_enter("");

	if (optlen <= 0 || optlen > PAGE_SIZE - 1)
		return -EINVAL;

	description = kmalloc(optlen + 1, GFP_KERNEL);
	if (!description)
		return -ENOMEM;

	if (copy_from_user(description, optval, optlen)) {
		kfree(description);
		return -EFAULT;
	}
	description[optlen] = 0;

	key = request_key(&key_type_keyring, description, NULL);
	if (IS_ERR(key)) {
		kfree(description);
		_leave(" = %ld", PTR_ERR(key));
		return PTR_ERR(key);
	}

	rx->securities = key;
	kfree(description);
	_leave(" = 0 [key %x]", key->serial);
	return 0;
}
