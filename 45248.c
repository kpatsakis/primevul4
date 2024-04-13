static int dgram_bind(struct sock *sk, struct sockaddr *uaddr, int len)
{
	struct sockaddr_ieee802154 *addr = (struct sockaddr_ieee802154 *)uaddr;
	struct dgram_sock *ro = dgram_sk(sk);
	int err = -EINVAL;
	struct net_device *dev;

	lock_sock(sk);

	ro->bound = 0;

	if (len < sizeof(*addr))
		goto out;

	if (addr->family != AF_IEEE802154)
		goto out;

	dev = ieee802154_get_dev(sock_net(sk), &addr->addr);
	if (!dev) {
		err = -ENODEV;
		goto out;
	}

	if (dev->type != ARPHRD_IEEE802154) {
		err = -ENODEV;
		goto out_put;
	}

	memcpy(&ro->src_addr, &addr->addr, sizeof(struct ieee802154_addr));

	ro->bound = 1;
	err = 0;
out_put:
	dev_put(dev);
out:
	release_sock(sk);

	return err;
}
