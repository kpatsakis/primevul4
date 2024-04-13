int dns_packet_append_blob(DnsPacket *p, const void *d, size_t l, size_t *start) {
        void *q;
        int r;

        assert(p);

        r = dns_packet_extend(p, l, &q, start);
        if (r < 0)
                return r;

        memcpy(q, d, l);
        return 0;
}
