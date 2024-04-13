struct ip_vs_dest *ip_vs_find_dest(struct net  *net, int af,
				   const union nf_inet_addr *daddr,
				   __be16 dport,
				   const union nf_inet_addr *vaddr,
				   __be16 vport, __u16 protocol, __u32 fwmark,
				   __u32 flags)
{
	struct ip_vs_dest *dest;
	struct ip_vs_service *svc;
	__be16 port = dport;

	svc = ip_vs_service_get(net, af, fwmark, protocol, vaddr, vport);
	if (!svc)
		return NULL;
	if (fwmark && (flags & IP_VS_CONN_F_FWD_MASK) != IP_VS_CONN_F_MASQ)
		port = 0;
	dest = ip_vs_lookup_dest(svc, daddr, port);
	if (!dest)
		dest = ip_vs_lookup_dest(svc, daddr, port ^ dport);
	if (dest)
		atomic_inc(&dest->refcnt);
	ip_vs_service_put(svc);
	return dest;
}
