static unsigned int nf_hash_frag(__be32 id, const struct in6_addr *saddr,
				 const struct in6_addr *daddr)
{
	net_get_random_once(&nf_frags.rnd, sizeof(nf_frags.rnd));
	return jhash_3words(ipv6_addr_hash(saddr), ipv6_addr_hash(daddr),
			    (__force u32)id, nf_frags.rnd);
}
