static inline int ip6_rt_check(const struct rt6key *rt_key,
			       const struct in6_addr *fl_addr,
			       const struct in6_addr *addr_cache)
{
	return (rt_key->plen != 128 || !ipv6_addr_equal(fl_addr, &rt_key->addr)) &&
		(!addr_cache || !ipv6_addr_equal(fl_addr, addr_cache));
}
