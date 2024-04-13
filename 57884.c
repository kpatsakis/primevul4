static int ipv6_rcv_saddr_equal(const struct in6_addr *sk1_rcv_saddr6,
				const struct in6_addr *sk2_rcv_saddr6,
				__be32 sk1_rcv_saddr, __be32 sk2_rcv_saddr,
				bool sk1_ipv6only, bool sk2_ipv6only,
				bool match_wildcard)
{
	int addr_type = ipv6_addr_type(sk1_rcv_saddr6);
	int addr_type2 = sk2_rcv_saddr6 ? ipv6_addr_type(sk2_rcv_saddr6) : IPV6_ADDR_MAPPED;

	/* if both are mapped, treat as IPv4 */
	if (addr_type == IPV6_ADDR_MAPPED && addr_type2 == IPV6_ADDR_MAPPED) {
		if (!sk2_ipv6only) {
			if (sk1_rcv_saddr == sk2_rcv_saddr)
				return 1;
			if (!sk1_rcv_saddr || !sk2_rcv_saddr)
				return match_wildcard;
		}
		return 0;
	}

	if (addr_type == IPV6_ADDR_ANY && addr_type2 == IPV6_ADDR_ANY)
		return 1;

	if (addr_type2 == IPV6_ADDR_ANY && match_wildcard &&
	    !(sk2_ipv6only && addr_type == IPV6_ADDR_MAPPED))
		return 1;

	if (addr_type == IPV6_ADDR_ANY && match_wildcard &&
	    !(sk1_ipv6only && addr_type2 == IPV6_ADDR_MAPPED))
		return 1;

	if (sk2_rcv_saddr6 &&
	    ipv6_addr_equal(sk1_rcv_saddr6, sk2_rcv_saddr6))
		return 1;

	return 0;
}
