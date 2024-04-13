__ip_vs_service_find(struct net *net, int af, __u16 protocol,
		     const union nf_inet_addr *vaddr, __be16 vport)
{
	unsigned int hash;
	struct ip_vs_service *svc;

	/* Check for "full" addressed entries */
	hash = ip_vs_svc_hashkey(net, af, protocol, vaddr, vport);

	list_for_each_entry(svc, &ip_vs_svc_table[hash], s_list){
		if ((svc->af == af)
		    && ip_vs_addr_equal(af, &svc->addr, vaddr)
		    && (svc->port == vport)
		    && (svc->protocol == protocol)
		    && net_eq(svc->net, net)) {
			/* HIT */
			return svc;
		}
	}

	return NULL;
}
