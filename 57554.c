int dns_packet_append_uint16(DnsPacket *p, uint16_t v, size_t *start) {
        void *d;
        int r;

        assert(p);

        r = dns_packet_extend(p, sizeof(uint16_t), &d, start);
        if (r < 0)
                return r;

        unaligned_write_be16(d, v);

        return 0;
}
