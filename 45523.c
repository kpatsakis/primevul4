static int ipxitf_pprop(struct ipx_interface *intrfc, struct sk_buff *skb)
{
	struct ipxhdr *ipx = ipx_hdr(skb);
	int i, rc = -EINVAL;
	struct ipx_interface *ifcs;
	char *c;
	__be32 *l;

	/* Illegal packet - too many hops or too short */
	/* We decide to throw it away: no broadcasting, no local processing.
	 * NetBIOS unaware implementations route them as normal packets -
	 * tctrl <= 15, any data payload... */
	if (IPX_SKB_CB(skb)->ipx_tctrl > IPX_MAX_PPROP_HOPS ||
	    ntohs(ipx->ipx_pktsize) < sizeof(struct ipxhdr) +
					IPX_MAX_PPROP_HOPS * sizeof(u32))
		goto out;
	/* are we broadcasting this damn thing? */
	rc = 0;
	if (!sysctl_ipx_pprop_broadcasting)
		goto out;
	/* We do broadcast packet on the IPX_MAX_PPROP_HOPS hop, but we
	 * process it locally. All previous hops broadcasted it, and process it
	 * locally. */
	if (IPX_SKB_CB(skb)->ipx_tctrl == IPX_MAX_PPROP_HOPS)
		goto out;

	c = ((u8 *) ipx) + sizeof(struct ipxhdr);
	l = (__be32 *) c;

	/* Don't broadcast packet if already seen this net */
	for (i = 0; i < IPX_SKB_CB(skb)->ipx_tctrl; i++)
		if (*l++ == intrfc->if_netnum)
			goto out;

	/* < IPX_MAX_PPROP_HOPS hops && input interface not in list. Save the
	 * position where we will insert recvd netnum into list, later on,
	 * in ipxitf_send */
	IPX_SKB_CB(skb)->last_hop.index = i;
	IPX_SKB_CB(skb)->last_hop.netnum = intrfc->if_netnum;
	/* xmit on all other interfaces... */
	spin_lock_bh(&ipx_interfaces_lock);
	list_for_each_entry(ifcs, &ipx_interfaces, node) {
		/* Except unconfigured interfaces */
		if (!ifcs->if_netnum)
			continue;

		/* That aren't in the list */
		if (ifcs == intrfc)
			continue;
		l = (__be32 *) c;
		/* don't consider the last entry in the packet list,
		 * it is our netnum, and it is not there yet */
		for (i = 0; i < IPX_SKB_CB(skb)->ipx_tctrl; i++)
			if (ifcs->if_netnum == *l++)
				break;
		if (i == IPX_SKB_CB(skb)->ipx_tctrl) {
			struct sk_buff *s = skb_copy(skb, GFP_ATOMIC);

			if (s) {
				IPX_SKB_CB(s)->ipx_dest_net = ifcs->if_netnum;
				ipxrtr_route_skb(s);
			}
		}
	}
	spin_unlock_bh(&ipx_interfaces_lock);
out:
	return rc;
}
