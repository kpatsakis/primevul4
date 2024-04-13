static int ipx_rcv(struct sk_buff *skb, struct net_device *dev, struct packet_type *pt, struct net_device *orig_dev)
{
	/* NULL here for pt means the packet was looped back */
	struct ipx_interface *intrfc;
	struct ipxhdr *ipx;
	u16 ipx_pktsize;
	int rc = 0;

	if (!net_eq(dev_net(dev), &init_net))
		goto drop;

	/* Not ours */
	if (skb->pkt_type == PACKET_OTHERHOST)
		goto drop;

	if ((skb = skb_share_check(skb, GFP_ATOMIC)) == NULL)
		goto out;

	if (!pskb_may_pull(skb, sizeof(struct ipxhdr)))
		goto drop;

	ipx_pktsize = ntohs(ipx_hdr(skb)->ipx_pktsize);

	/* Too small or invalid header? */
	if (ipx_pktsize < sizeof(struct ipxhdr) ||
	    !pskb_may_pull(skb, ipx_pktsize))
		goto drop;

	ipx = ipx_hdr(skb);
	if (ipx->ipx_checksum != IPX_NO_CHECKSUM &&
	   ipx->ipx_checksum != ipx_cksum(ipx, ipx_pktsize))
		goto drop;

	IPX_SKB_CB(skb)->ipx_tctrl	= ipx->ipx_tctrl;
	IPX_SKB_CB(skb)->ipx_dest_net	= ipx->ipx_dest.net;
	IPX_SKB_CB(skb)->ipx_source_net = ipx->ipx_source.net;

	/* Determine what local ipx endpoint this is */
	intrfc = ipxitf_find_using_phys(dev, pt->type);
	if (!intrfc) {
		if (ipxcfg_auto_create_interfaces &&
		   IPX_SKB_CB(skb)->ipx_dest_net) {
			intrfc = ipxitf_auto_create(dev, pt->type);
			if (intrfc)
				ipxitf_hold(intrfc);
		}

		if (!intrfc)	/* Not one of ours */
				/* or invalid packet for auto creation */
			goto drop;
	}

	rc = ipxitf_rcv(intrfc, skb);
	ipxitf_put(intrfc);
	goto out;
drop:
	kfree_skb(skb);
out:
	return rc;
}
