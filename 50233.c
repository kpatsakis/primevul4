static enum ip6_defrag_users nf_ct6_defrag_user(unsigned int hooknum,
						struct sk_buff *skb)
{
	u16 zone_id = NF_CT_DEFAULT_ZONE_ID;
#if IS_ENABLED(CONFIG_NF_CONNTRACK)
	if (skb->nfct) {
		enum ip_conntrack_info ctinfo;
		const struct nf_conn *ct = nf_ct_get(skb, &ctinfo);

		zone_id = nf_ct_zone_id(nf_ct_zone(ct), CTINFO2DIR(ctinfo));
	}
#endif
	if (nf_bridge_in_prerouting(skb))
		return IP6_DEFRAG_CONNTRACK_BRIDGE_IN + zone_id;

	if (hooknum == NF_INET_PRE_ROUTING)
		return IP6_DEFRAG_CONNTRACK_IN + zone_id;
	else
		return IP6_DEFRAG_CONNTRACK_OUT + zone_id;
}
