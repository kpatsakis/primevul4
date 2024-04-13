nf_nat_redirect_ipv6(struct sk_buff *skb, const struct nf_nat_range *range,
		     unsigned int hooknum)
{
	struct nf_nat_range newrange;
	struct in6_addr newdst;
	enum ip_conntrack_info ctinfo;
	struct nf_conn *ct;

	ct = nf_ct_get(skb, &ctinfo);
	if (hooknum == NF_INET_LOCAL_OUT) {
		newdst = loopback_addr;
	} else {
		struct inet6_dev *idev;
		struct inet6_ifaddr *ifa;
		bool addr = false;

		rcu_read_lock();
		idev = __in6_dev_get(skb->dev);
		if (idev != NULL) {
			list_for_each_entry(ifa, &idev->addr_list, if_list) {
				newdst = ifa->addr;
				addr = true;
				break;
			}
		}
		rcu_read_unlock();

		if (!addr)
			return NF_DROP;
	}

	newrange.flags		= range->flags | NF_NAT_RANGE_MAP_IPS;
	newrange.min_addr.in6	= newdst;
	newrange.max_addr.in6	= newdst;
	newrange.min_proto	= range->min_proto;
	newrange.max_proto	= range->max_proto;

	return nf_nat_setup_info(ct, &newrange, NF_NAT_MANIP_DST);
}
