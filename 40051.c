static int bt_sock_create(struct net *net, struct socket *sock, int proto,
			  int kern)
{
	int err;

	if (net != &init_net)
		return -EAFNOSUPPORT;

	if (proto < 0 || proto >= BT_MAX_PROTO)
		return -EINVAL;

	if (!bt_proto[proto])
		request_module("bt-proto-%d", proto);

	err = -EPROTONOSUPPORT;

	read_lock(&bt_proto_lock);

	if (bt_proto[proto] && try_module_get(bt_proto[proto]->owner)) {
		err = bt_proto[proto]->create(net, sock, proto, kern);
		if (!err)
			bt_sock_reclassify_lock(sock->sk, proto);
		module_put(bt_proto[proto]->owner);
	}

	read_unlock(&bt_proto_lock);

	return err;
}
