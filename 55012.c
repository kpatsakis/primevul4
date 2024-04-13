static int __packet_snd_vnet_parse(struct virtio_net_hdr *vnet_hdr, size_t len)
{
	if ((vnet_hdr->flags & VIRTIO_NET_HDR_F_NEEDS_CSUM) &&
	    (__virtio16_to_cpu(vio_le(), vnet_hdr->csum_start) +
	     __virtio16_to_cpu(vio_le(), vnet_hdr->csum_offset) + 2 >
	      __virtio16_to_cpu(vio_le(), vnet_hdr->hdr_len)))
		vnet_hdr->hdr_len = __cpu_to_virtio16(vio_le(),
			 __virtio16_to_cpu(vio_le(), vnet_hdr->csum_start) +
			__virtio16_to_cpu(vio_le(), vnet_hdr->csum_offset) + 2);

	if (__virtio16_to_cpu(vio_le(), vnet_hdr->hdr_len) > len)
		return -EINVAL;

	return 0;
}
