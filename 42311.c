static void tun_free_netdev(struct net_device *dev)
{
	struct tun_struct *tun = netdev_priv(dev);

	BUG_ON(!test_bit(SOCK_EXTERNALLY_ALLOCATED, &tun->socket.flags));

	sk_release_kernel(tun->socket.sk);
}
