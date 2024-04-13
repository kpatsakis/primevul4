int ipxitf_send(struct ipx_interface *intrfc, struct sk_buff *skb, char *node)
{
	struct ipxhdr *ipx = ipx_hdr(skb);
	struct net_device *dev = intrfc->if_dev;
	struct datalink_proto *dl = intrfc->if_dlink;
	char dest_node[IPX_NODE_LEN];
	int send_to_wire = 1;
	int addr_len;

	ipx->ipx_tctrl = IPX_SKB_CB(skb)->ipx_tctrl;
	ipx->ipx_dest.net = IPX_SKB_CB(skb)->ipx_dest_net;
	ipx->ipx_source.net = IPX_SKB_CB(skb)->ipx_source_net;

	/* see if we need to include the netnum in the route list */
	if (IPX_SKB_CB(skb)->last_hop.index >= 0) {
		__be32 *last_hop = (__be32 *)(((u8 *) skb->data) +
				sizeof(struct ipxhdr) +
				IPX_SKB_CB(skb)->last_hop.index *
				sizeof(__be32));
		*last_hop = IPX_SKB_CB(skb)->last_hop.netnum;
		IPX_SKB_CB(skb)->last_hop.index = -1;
	}

	/*
	 * We need to know how many skbuffs it will take to send out this
	 * packet to avoid unnecessary copies.
	 */

	if (!dl || !dev || dev->flags & IFF_LOOPBACK)
		send_to_wire = 0;	/* No non looped */

	/*
	 * See if this should be demuxed to sockets on this interface
	 *
	 * We want to ensure the original was eaten or that we only use
	 * up clones.
	 */

	if (ipx->ipx_dest.net == intrfc->if_netnum) {
		/*
		 * To our own node, loop and free the original.
		 * The internal net will receive on all node address.
		 */
		if (intrfc == ipx_internal_net ||
		    !memcmp(intrfc->if_node, node, IPX_NODE_LEN)) {
			/* Don't charge sender */
			skb_orphan(skb);

			/* Will charge receiver */
			return ipxitf_demux_socket(intrfc, skb, 0);
		}

		/* Broadcast, loop and possibly keep to send on. */
		if (!memcmp(ipx_broadcast_node, node, IPX_NODE_LEN)) {
			if (!send_to_wire)
				skb_orphan(skb);
			ipxitf_demux_socket(intrfc, skb, send_to_wire);
			if (!send_to_wire)
				goto out;
		}
	}

	/*
	 * If the originating net is not equal to our net; this is routed
	 * We are still charging the sender. Which is right - the driver
	 * free will handle this fairly.
	 */
	if (ipx->ipx_source.net != intrfc->if_netnum) {
		/*
		 * Unshare the buffer before modifying the count in
		 * case it's a flood or tcpdump
		 */
		skb = skb_unshare(skb, GFP_ATOMIC);
		if (!skb)
			goto out;
		if (++ipx->ipx_tctrl > ipxcfg_max_hops)
			send_to_wire = 0;
	}

	if (!send_to_wire) {
		kfree_skb(skb);
		goto out;
	}

	/* Determine the appropriate hardware address */
	addr_len = dev->addr_len;
	if (!memcmp(ipx_broadcast_node, node, IPX_NODE_LEN))
		memcpy(dest_node, dev->broadcast, addr_len);
	else
		memcpy(dest_node, &(node[IPX_NODE_LEN-addr_len]), addr_len);

	/* Make any compensation for differing physical/data link size */
	skb = ipxitf_adjust_skbuff(intrfc, skb);
	if (!skb)
		goto out;

	/* set up data link and physical headers */
	skb->dev	= dev;
	skb->protocol	= htons(ETH_P_IPX);

	/* Send it out */
	dl->request(dl, skb, dest_node);
out:
	return 0;
}
