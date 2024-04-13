static void ipxitf_discover_netnum(struct ipx_interface *intrfc,
				   struct sk_buff *skb)
{
	const struct ipx_cb *cb = IPX_SKB_CB(skb);

	/* see if this is an intra packet: source_net == dest_net */
	if (cb->ipx_source_net == cb->ipx_dest_net && cb->ipx_source_net) {
		struct ipx_interface *i =
				ipxitf_find_using_net(cb->ipx_source_net);
		/* NB: NetWare servers lie about their hop count so we
		 * dropped the test based on it. This is the best way
		 * to determine this is a 0 hop count packet. */
		if (!i) {
			intrfc->if_netnum = cb->ipx_source_net;
			ipxitf_add_local_route(intrfc);
		} else {
			printk(KERN_WARNING "IPX: Network number collision "
				"%lx\n        %s %s and %s %s\n",
				(unsigned long) ntohl(cb->ipx_source_net),
				ipx_device_name(i),
				ipx_frame_name(i->if_dlink_type),
				ipx_device_name(intrfc),
				ipx_frame_name(intrfc->if_dlink_type));
			ipxitf_put(i);
		}
	}
}
