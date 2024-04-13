static int dns_packet_read_type_window(DnsPacket *p, Bitmap **types, size_t *start) {
        uint8_t window;
        uint8_t length;
        const uint8_t *bitmap;
        uint8_t bit = 0;
        unsigned i;
        bool found = false;
        _cleanup_(rewind_dns_packet) DnsPacketRewinder rewinder;
        int r;

        assert(p);
        assert(types);
        INIT_REWINDER(rewinder, p);

        r = bitmap_ensure_allocated(types);
        if (r < 0)
                return r;

        r = dns_packet_read_uint8(p, &window, NULL);
        if (r < 0)
                return r;

        r = dns_packet_read_uint8(p, &length, NULL);
        if (r < 0)
                return r;

        if (length == 0 || length > 32)
                return -EBADMSG;

        r = dns_packet_read(p, length, (const void **)&bitmap, NULL);
        if (r < 0)
                return r;

        for (i = 0; i < length; i++) {
                uint8_t bitmask = 1 << 7;

                if (!bitmap[i]) {
                        found = false;
                        bit += 8;
                        continue;
                }

                found = true;

                while (bitmask) {
                        if (bitmap[i] & bitmask) {
                                uint16_t n;

                                n = (uint16_t) window << 8 | (uint16_t) bit;

                                /* Ignore pseudo-types. see RFC4034 section 4.1.2 */
                                if (dns_type_is_pseudo(n))
                                        continue;

                                r = bitmap_set(*types, n);
                                if (r < 0)
                                        return r;
                        }

                        bit++;
                        bitmask >>= 1;
                }
        }

        if (!found)
                return -EBADMSG;

        if (start)
                *start = rewinder.saved_rindex;
        CANCEL_REWINDER(rewinder);

        return 0;
}
