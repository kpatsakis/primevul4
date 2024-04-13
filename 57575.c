int dns_packet_validate(DnsPacket *p) {
        assert(p);

        if (p->size < DNS_PACKET_HEADER_SIZE)
                return -EBADMSG;

        if (p->size > DNS_PACKET_SIZE_MAX)
                return -EBADMSG;

        return 1;
}
