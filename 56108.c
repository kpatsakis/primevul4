format_ip6addr(netdissect_options *ndo, const u_char *cp)
{
    if (is_ipv4_mapped_address(cp))
        return ipaddr_string(ndo, cp + IPV4_MAPPED_HEADING_LEN);
    else
        return ip6addr_string(ndo, cp);
}
