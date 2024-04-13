int dns_packet_read_blob(DnsPacket *p, void *d, size_t sz, size_t *start) {
        const void *q;
        int r;

        assert(p);
        assert(d);

        r = dns_packet_read(p, sz, &q, start);
        if (r < 0)
                return r;

        memcpy(d, q, sz);
        return 0;
}
