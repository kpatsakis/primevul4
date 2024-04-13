int dns_packet_append_string(DnsPacket *p, const char *s, size_t *start) {
        assert(p);
        assert(s);

        return dns_packet_append_raw_string(p, s, strlen(s), start);
}
