int dns_packet_append_uint32(DnsPacket *p, uint32_t v, size_t *start) {
        void *d;
        int r;

        assert(p);

        r = dns_packet_extend(p, sizeof(uint32_t), &d, start);
        if (r < 0)
                return r;

        unaligned_write_be32(d, v);

        return 0;
}
