int dns_packet_new_query(DnsPacket **ret, DnsProtocol protocol, size_t mtu, bool dnssec_checking_disabled) {
        DnsPacket *p;
        int r;

        assert(ret);

        r = dns_packet_new(&p, protocol, mtu);
        if (r < 0)
                return r;

        /* Always set the TC bit to 0 initially.
         * If there are multiple packets later, we'll update the bit shortly before sending.
         */
        dns_packet_set_flags(p, dnssec_checking_disabled, false);

        *ret = p;
        return 0;
}
