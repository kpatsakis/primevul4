int dns_packet_truncate_opt(DnsPacket *p) {
        assert(p);

        if (p->opt_start == (size_t) -1) {
                assert(p->opt_size == (size_t) -1);
                return 0;
        }

        assert(p->opt_size != (size_t) -1);
        assert(DNS_PACKET_ARCOUNT(p) > 0);

        if (p->opt_start + p->opt_size != p->size)
                return -EBUSY;

        dns_packet_truncate(p, p->opt_start);
        DNS_PACKET_HEADER(p)->arcount = htobe16(DNS_PACKET_ARCOUNT(p) - 1);
        p->opt_start = p->opt_size = (size_t) -1;

        return 1;
}
