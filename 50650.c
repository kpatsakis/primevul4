static int __packet_rcv_vnet(const struct sk_buff *skb,
			     struct virtio_net_hdr *vnet_hdr)
{
	*vnet_hdr = (const struct virtio_net_hdr) { 0 };

	if (virtio_net_hdr_from_skb(skb, vnet_hdr, vio_le()))
		BUG();

	return 0;
}
