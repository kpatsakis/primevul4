static int run_filter(struct tap_filter *filter, const struct sk_buff *skb)
{
	/* Cannot use eth_hdr(skb) here because skb_mac_hdr() is incorrect
	 * at this point. */
	struct ethhdr *eh = (struct ethhdr *) skb->data;
	int i;

	/* Exact match */
	for (i = 0; i < filter->count; i++)
		if (ether_addr_equal(eh->h_dest, filter->addr[i]))
			return 1;

	/* Inexact match (multicast only) */
	if (is_multicast_ether_addr(eh->h_dest))
		return addr_hash_test(filter->mask, eh->h_dest);

	return 0;
}
