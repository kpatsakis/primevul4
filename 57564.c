int dns_packet_read_name(
                DnsPacket *p,
                char **_ret,
                bool allow_compression,
                size_t *start) {

        _cleanup_(rewind_dns_packet) DnsPacketRewinder rewinder;
        size_t after_rindex = 0, jump_barrier;
        _cleanup_free_ char *ret = NULL;
        size_t n = 0, allocated = 0;
        bool first = true;
        int r;

        assert(p);
        assert(_ret);
        INIT_REWINDER(rewinder, p);
        jump_barrier = p->rindex;

        if (p->refuse_compression)
                allow_compression = false;

        for (;;) {
                uint8_t c, d;

                r = dns_packet_read_uint8(p, &c, NULL);
                if (r < 0)
                        return r;

                if (c == 0)
                        /* End of name */
                        break;
                else if (c <= 63) {
                        const char *label;

                        /* Literal label */
                        r = dns_packet_read(p, c, (const void**) &label, NULL);
                        if (r < 0)
                                return r;

                        if (!GREEDY_REALLOC(ret, allocated, n + !first + DNS_LABEL_ESCAPED_MAX))
                                return -ENOMEM;

                        if (first)
                                first = false;
                        else
                                ret[n++] = '.';

                        r = dns_label_escape(label, c, ret + n, DNS_LABEL_ESCAPED_MAX);
                        if (r < 0)
                                return r;

                        n += r;
                        continue;
                } else if (allow_compression && (c & 0xc0) == 0xc0) {
                        uint16_t ptr;

                        /* Pointer */
                        r = dns_packet_read_uint8(p, &d, NULL);
                        if (r < 0)
                                return r;

                        ptr = (uint16_t) (c & ~0xc0) << 8 | (uint16_t) d;
                        if (ptr < DNS_PACKET_HEADER_SIZE || ptr >= jump_barrier)
                                return -EBADMSG;

                        if (after_rindex == 0)
                                after_rindex = p->rindex;

                        /* Jumps are limited to a "prior occurrence" (RFC-1035 4.1.4) */
                        jump_barrier = ptr;
                        p->rindex = ptr;
                } else
                        return -EBADMSG;
        }

        if (!GREEDY_REALLOC(ret, allocated, n + 1))
                return -ENOMEM;

        ret[n] = 0;

        if (after_rindex != 0)
                p->rindex= after_rindex;

        *_ret = ret;
        ret = NULL;

        if (start)
                *start = rewinder.saved_rindex;
        CANCEL_REWINDER(rewinder);

        return 0;
}
