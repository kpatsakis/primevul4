is_ipv4_mapped_address(const u_char *addr)
{
    /* The value of the prefix */
    static const u_char ipv4_mapped_heading[IPV4_MAPPED_HEADING_LEN] =
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xFF, 0xFF };

    return memcmp(addr, ipv4_mapped_heading, IPV4_MAPPED_HEADING_LEN) == 0;
}
