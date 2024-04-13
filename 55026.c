static int packet_snd_vnet_parse(struct msghdr *msg, size_t *len,
				 struct virtio_net_hdr *vnet_hdr)
{
	if (*len < sizeof(*vnet_hdr))
		return -EINVAL;
	*len -= sizeof(*vnet_hdr);

	if (!copy_from_iter_full(vnet_hdr, sizeof(*vnet_hdr), &msg->msg_iter))
		return -EFAULT;

	return __packet_snd_vnet_parse(vnet_hdr, *len);
}
