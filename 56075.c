bgp_vpn_ip_print(netdissect_options *ndo,
                 const u_char *pptr, u_int addr_length)
{

    /* worst case string is s fully formatted v6 address */
    static char addr[sizeof("1234:5678:89ab:cdef:1234:5678:89ab:cdef")];
    char *pos = addr;

    switch(addr_length) {
    case (sizeof(struct in_addr) << 3): /* 32 */
        ND_TCHECK2(pptr[0], sizeof(struct in_addr));
        snprintf(pos, sizeof(addr), "%s", ipaddr_string(ndo, pptr));
        break;
    case (sizeof(struct in6_addr) << 3): /* 128 */
        ND_TCHECK2(pptr[0], sizeof(struct in6_addr));
        snprintf(pos, sizeof(addr), "%s", ip6addr_string(ndo, pptr));
        break;
    default:
        snprintf(pos, sizeof(addr), "bogus address length %u", addr_length);
        break;
    }
    pos += strlen(pos);

trunc:
    *(pos) = '\0';
    return (addr);
}
