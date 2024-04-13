ip_vs_lookup_real_service(struct net *net, int af, __u16 protocol,
			  const union nf_inet_addr *daddr,
			  __be16 dport)
{
	struct netns_ipvs *ipvs = net_ipvs(net);
	unsigned int hash;
	struct ip_vs_dest *dest;

	/*
	 *	Check for "full" addressed entries
	 *	Return the first found entry
	 */
	hash = ip_vs_rs_hashkey(af, daddr, dport);

	read_lock(&ipvs->rs_lock);
	list_for_each_entry(dest, &ipvs->rs_table[hash], d_list) {
		if ((dest->af == af)
		    && ip_vs_addr_equal(af, &dest->addr, daddr)
		    && (dest->port == dport)
		    && ((dest->protocol == protocol) ||
			dest->vfwmark)) {
			/* HIT */
			read_unlock(&ipvs->rs_lock);
			return dest;
		}
	}
	read_unlock(&ipvs->rs_lock);

	return NULL;
}
