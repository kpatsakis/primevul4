int dns_packet_append_question(DnsPacket *p, DnsQuestion *q) {
        DnsResourceKey *key;
        int r;

        assert(p);

        DNS_QUESTION_FOREACH(key, q) {
                r = dns_packet_append_key(p, key, 0, NULL);
                if (r < 0)
                        return r;
        }

        return 0;
}
