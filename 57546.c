int dns_packet_append_answer(DnsPacket *p, DnsAnswer *a) {
        DnsResourceRecord *rr;
        DnsAnswerFlags flags;
        int r;

        assert(p);

        DNS_ANSWER_FOREACH_FLAGS(rr, flags, a) {
                r = dns_packet_append_rr(p, rr, flags, NULL, NULL);
                if (r < 0)
                        return r;
        }

        return 0;
}
