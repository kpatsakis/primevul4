int dns_packet_new(DnsPacket **ret, DnsProtocol protocol, size_t mtu) {
        DnsPacket *p;
        size_t a;

        assert(ret);

        if (mtu <= UDP_PACKET_HEADER_SIZE)
                a = DNS_PACKET_SIZE_START;
        else
                a = mtu - UDP_PACKET_HEADER_SIZE;

        if (a < DNS_PACKET_HEADER_SIZE)
                a = DNS_PACKET_HEADER_SIZE;

        /* round up to next page size */
        a = PAGE_ALIGN(ALIGN(sizeof(DnsPacket)) + a) - ALIGN(sizeof(DnsPacket));

        /* make sure we never allocate more than useful */
        if (a > DNS_PACKET_SIZE_MAX)
                a = DNS_PACKET_SIZE_MAX;

        p = malloc0(ALIGN(sizeof(DnsPacket)) + a);
        if (!p)
                return -ENOMEM;

        p->size = p->rindex = DNS_PACKET_HEADER_SIZE;
        p->allocated = a;
        p->protocol = protocol;
        p->opt_start = p->opt_size = (size_t) -1;
        p->n_ref = 1;

        *ret = p;

        return 0;
}
