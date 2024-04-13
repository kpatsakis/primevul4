void bt_sock_unregister(int proto)
{
	if (proto < 0 || proto >= BT_MAX_PROTO)
		return;

	write_lock(&bt_proto_lock);
	bt_proto[proto] = NULL;
	write_unlock(&bt_proto_lock);
}
