int dns_packet_read_key(DnsPacket *p, DnsResourceKey **ret, bool *ret_cache_flush, size_t *start) {
        _cleanup_(rewind_dns_packet) DnsPacketRewinder rewinder;
        _cleanup_free_ char *name = NULL;
        bool cache_flush = false;
        uint16_t class, type;
        DnsResourceKey *key;
        int r;

        assert(p);
        assert(ret);
        INIT_REWINDER(rewinder, p);

        r = dns_packet_read_name(p, &name, true, NULL);
        if (r < 0)
                return r;

        r = dns_packet_read_uint16(p, &type, NULL);
        if (r < 0)
                return r;

        r = dns_packet_read_uint16(p, &class, NULL);
        if (r < 0)
                return r;

        if (p->protocol == DNS_PROTOCOL_MDNS) {
                /* See RFC6762, Section 10.2 */

                if (type != DNS_TYPE_OPT && (class & MDNS_RR_CACHE_FLUSH)) {
                        class &= ~MDNS_RR_CACHE_FLUSH;
                        cache_flush = true;
                }
        }

        key = dns_resource_key_new_consume(class, type, name);
        if (!key)
                return -ENOMEM;

        name = NULL;
        *ret = key;

        if (ret_cache_flush)
                *ret_cache_flush = cache_flush;
        if (start)
                *start = rewinder.saved_rindex;
        CANCEL_REWINDER(rewinder);

        return 0;
}
