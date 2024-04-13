static void __copy_skb_header(struct sk_buff *new, const struct sk_buff *old)
{
	new->tstamp		= old->tstamp;
	/* We do not copy old->sk */
	new->dev		= old->dev;
	memcpy(new->cb, old->cb, sizeof(old->cb));
	skb_dst_copy(new, old);
#ifdef CONFIG_XFRM
	new->sp			= secpath_get(old->sp);
#endif
	__nf_copy(new, old, false);

	/* Note : this field could be in headers_start/headers_end section
	 * It is not yet because we do not want to have a 16 bit hole
	 */
	new->queue_mapping = old->queue_mapping;

	memcpy(&new->headers_start, &old->headers_start,
	       offsetof(struct sk_buff, headers_end) -
	       offsetof(struct sk_buff, headers_start));
	CHECK_SKB_FIELD(protocol);
	CHECK_SKB_FIELD(csum);
	CHECK_SKB_FIELD(hash);
	CHECK_SKB_FIELD(priority);
	CHECK_SKB_FIELD(skb_iif);
	CHECK_SKB_FIELD(vlan_proto);
	CHECK_SKB_FIELD(vlan_tci);
	CHECK_SKB_FIELD(transport_header);
	CHECK_SKB_FIELD(network_header);
	CHECK_SKB_FIELD(mac_header);
	CHECK_SKB_FIELD(inner_protocol);
	CHECK_SKB_FIELD(inner_transport_header);
	CHECK_SKB_FIELD(inner_network_header);
	CHECK_SKB_FIELD(inner_mac_header);
	CHECK_SKB_FIELD(mark);
#ifdef CONFIG_NETWORK_SECMARK
	CHECK_SKB_FIELD(secmark);
#endif
#ifdef CONFIG_NET_RX_BUSY_POLL
	CHECK_SKB_FIELD(napi_id);
#endif
#ifdef CONFIG_XPS
	CHECK_SKB_FIELD(sender_cpu);
#endif
#ifdef CONFIG_NET_SCHED
	CHECK_SKB_FIELD(tc_index);
#endif

}
