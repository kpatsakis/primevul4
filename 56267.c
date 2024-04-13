nextproto6_cksum(netdissect_options *ndo,
                 const struct ip6_hdr *ip6, const uint8_t *data,
		 u_int len, u_int covlen, u_int next_proto)
{
        struct {
                struct in6_addr ph_src;
                struct in6_addr ph_dst;
                uint32_t       ph_len;
                uint8_t        ph_zero[3];
                uint8_t        ph_nxt;
        } ph;
        struct cksum_vec vec[2];

        /* pseudo-header */
        memset(&ph, 0, sizeof(ph));
        UNALIGNED_MEMCPY(&ph.ph_src, &ip6->ip6_src, sizeof (struct in6_addr));
        switch (ip6->ip6_nxt) {

        case IPPROTO_HOPOPTS:
        case IPPROTO_DSTOPTS:
        case IPPROTO_MOBILITY_OLD:
        case IPPROTO_MOBILITY:
        case IPPROTO_FRAGMENT:
        case IPPROTO_ROUTING:
                /*
                 * The next header is either a routing header or a header
                 * after which there might be a routing header, so scan
                 * for a routing header.
                 */
                ip6_finddst(ndo, &ph.ph_dst, ip6);
                break;

        default:
                UNALIGNED_MEMCPY(&ph.ph_dst, &ip6->ip6_dst, sizeof (struct in6_addr));
                break;
        }
        ph.ph_len = htonl(len);
        ph.ph_nxt = next_proto;

        vec[0].ptr = (const uint8_t *)(void *)&ph;
        vec[0].len = sizeof(ph);
        vec[1].ptr = data;
        vec[1].len = covlen;

        return in_cksum(vec, 2);
}
