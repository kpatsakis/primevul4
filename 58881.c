int skb_vlan_pop(struct sk_buff *skb)
{
	u16 vlan_tci;
	__be16 vlan_proto;
	int err;

	if (likely(skb_vlan_tag_present(skb))) {
		skb->vlan_tci = 0;
	} else {
		if (unlikely(!eth_type_vlan(skb->protocol)))
			return 0;

		err = __skb_vlan_pop(skb, &vlan_tci);
		if (err)
			return err;
	}
	/* move next vlan tag to hw accel tag */
	if (likely(!eth_type_vlan(skb->protocol)))
		return 0;

	vlan_proto = skb->protocol;
	err = __skb_vlan_pop(skb, &vlan_tci);
	if (unlikely(err))
		return err;

	__vlan_hwaccel_put_tag(skb, vlan_proto, vlan_tci);
	return 0;
}
