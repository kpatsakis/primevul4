int cdc_ncm_change_mtu(struct net_device *net, int new_mtu)
{
	struct usbnet *dev = netdev_priv(net);
	struct cdc_ncm_ctx *ctx = (struct cdc_ncm_ctx *)dev->data[0];
	int maxmtu = ctx->max_datagram_size - cdc_ncm_eth_hlen(dev);

	if (new_mtu <= 0 || new_mtu > maxmtu)
		return -EINVAL;
	net->mtu = new_mtu;
	return 0;
}
