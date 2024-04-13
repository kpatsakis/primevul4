static int packet_rcv_vnet(struct msghdr *msg, const struct sk_buff *skb,
			   size_t *len)
{
	struct virtio_net_hdr vnet_hdr;

	if (*len < sizeof(vnet_hdr))
		return -EINVAL;
	*len -= sizeof(vnet_hdr);

	if (virtio_net_hdr_from_skb(skb, &vnet_hdr, vio_le(), true))
		return -EINVAL;

	return memcpy_to_msg(msg, (void *)&vnet_hdr, sizeof(vnet_hdr));
}
