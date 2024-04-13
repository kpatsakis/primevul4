static int packet_snd_vnet_parse(struct msghdr *msg, size_t *len,
				 struct virtio_net_hdr *vnet_hdr)
{
	int n;

	if (*len < sizeof(*vnet_hdr))
		return -EINVAL;
	*len -= sizeof(*vnet_hdr);

	n = copy_from_iter(vnet_hdr, sizeof(*vnet_hdr), &msg->msg_iter);
	if (n != sizeof(*vnet_hdr))
		return -EFAULT;

	return __packet_snd_vnet_parse(vnet_hdr, *len);
}
