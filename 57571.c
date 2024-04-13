void dns_packet_rewind(DnsPacket *p, size_t idx) {
        assert(p);
        assert(idx <= p->size);
        assert(idx >= DNS_PACKET_HEADER_SIZE);

        p->rindex = idx;
}
