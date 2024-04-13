static int packet_snd_vnet_gso(struct sk_buff *skb,
			       struct virtio_net_hdr *vnet_hdr)
{
	if (vnet_hdr->flags & VIRTIO_NET_HDR_F_NEEDS_CSUM) {
		u16 s = __virtio16_to_cpu(vio_le(), vnet_hdr->csum_start);
		u16 o = __virtio16_to_cpu(vio_le(), vnet_hdr->csum_offset);

		if (!skb_partial_csum_set(skb, s, o))
			return -EINVAL;
	}

	skb_shinfo(skb)->gso_size =
		__virtio16_to_cpu(vio_le(), vnet_hdr->gso_size);
	skb_shinfo(skb)->gso_type = vnet_hdr->gso_type;

	/* Header must be checked, and gso_segs computed. */
	skb_shinfo(skb)->gso_type |= SKB_GSO_DODGY;
	skb_shinfo(skb)->gso_segs = 0;
	return 0;
}
