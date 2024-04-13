int dns_packet_read_raw_string(DnsPacket *p, const void **ret, size_t *size, size_t *start) {
        _cleanup_(rewind_dns_packet) DnsPacketRewinder rewinder;
        uint8_t c;
        int r;

        assert(p);
        INIT_REWINDER(rewinder, p);

        r = dns_packet_read_uint8(p, &c, NULL);
        if (r < 0)
                return r;

        r = dns_packet_read(p, c, ret, NULL);
        if (r < 0)
                return r;

        if (size)
                *size = c;
        if (start)
                *start = rewinder.saved_rindex;
        CANCEL_REWINDER(rewinder);

        return 0;
}
