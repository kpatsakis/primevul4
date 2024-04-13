int bt_sock_unregister(int proto)
{
	int err = 0;

	if (proto < 0 || proto >= BT_MAX_PROTO)
		return -EINVAL;

	write_lock(&bt_proto_lock);

	if (!bt_proto[proto])
		err = -ENOENT;
	else
		bt_proto[proto] = NULL;

	write_unlock(&bt_proto_lock);

	return err;
}
