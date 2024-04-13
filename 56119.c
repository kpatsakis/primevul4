static int icmp6_cksum(netdissect_options *ndo, const struct ip6_hdr *ip6,
	const struct icmp6_hdr *icp, u_int len)
{
	return nextproto6_cksum(ndo, ip6, (const uint8_t *)(const void *)icp, len, len,
				IPPROTO_ICMPV6);
}
