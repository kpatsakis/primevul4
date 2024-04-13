nextproto4_cksum(netdissect_options *ndo,
                 const struct ip *ip, const uint8_t *data,
                 u_int len, u_int covlen, u_int next_proto)
{
	struct phdr {
		uint32_t src;
		uint32_t dst;
		u_char mbz;
		u_char proto;
		uint16_t len;
	} ph;
	struct cksum_vec vec[2];

	/* pseudo-header.. */
	ph.len = htons((uint16_t)len);
	ph.mbz = 0;
	ph.proto = next_proto;
	UNALIGNED_MEMCPY(&ph.src, &ip->ip_src, sizeof(uint32_t));
	if (IP_HL(ip) == 5)
		UNALIGNED_MEMCPY(&ph.dst, &ip->ip_dst, sizeof(uint32_t));
	else
		ph.dst = ip_finddst(ndo, ip);

	vec[0].ptr = (const uint8_t *)(void *)&ph;
	vec[0].len = sizeof(ph);
	vec[1].ptr = data;
	vec[1].len = covlen;
 	return (in_cksum(vec, 2));
 }
