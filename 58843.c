int __skb_vlan_pop(struct sk_buff *skb, u16 *vlan_tci)
{
	struct vlan_hdr *vhdr;
	int offset = skb->data - skb_mac_header(skb);
	int err;

	if (WARN_ONCE(offset,
		      "__skb_vlan_pop got skb with skb->data not at mac header (offset %d)\n",
		      offset)) {
		return -EINVAL;
	}

	err = skb_ensure_writable(skb, VLAN_ETH_HLEN);
	if (unlikely(err))
		return err;

	skb_postpull_rcsum(skb, skb->data + (2 * ETH_ALEN), VLAN_HLEN);

	vhdr = (struct vlan_hdr *)(skb->data + ETH_HLEN);
	*vlan_tci = ntohs(vhdr->h_vlan_TCI);

	memmove(skb->data + VLAN_HLEN, skb->data, 2 * ETH_ALEN);
	__skb_pull(skb, VLAN_HLEN);

	vlan_set_encap_proto(skb, vhdr);
	skb->mac_header += VLAN_HLEN;

	if (skb_network_offset(skb) < ETH_HLEN)
		skb_set_network_header(skb, ETH_HLEN);

	skb_reset_mac_len(skb);

	return err;
}
