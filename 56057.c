lldp_extract_latlon(const u_char *tptr)
{
    uint64_t latlon;

    latlon = *tptr & 0x3;
    latlon = (latlon << 32) | EXTRACT_32BITS(tptr+1);

    return latlon;
}
