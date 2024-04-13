static int dns_packet_append_types(DnsPacket *p, Bitmap *types, size_t *start) {
        Iterator i;
        uint8_t window = 0;
        uint8_t entry = 0;
        uint8_t bitmaps[32] = {};
        unsigned n;
        size_t saved_size;
        int r;

        assert(p);

        saved_size = p->size;

        BITMAP_FOREACH(n, types, i) {
                assert(n <= 0xffff);

                if ((n >> 8) != window && bitmaps[entry / 8] != 0) {
                        r = dns_packet_append_type_window(p, window, entry / 8 + 1, bitmaps, NULL);
                        if (r < 0)
                                goto fail;

                        zero(bitmaps);
                }

                window = n >> 8;
                entry = n & 255;

                bitmaps[entry / 8] |= 1 << (7 - (entry % 8));
        }

        if (bitmaps[entry / 8] != 0) {
                r = dns_packet_append_type_window(p, window, entry / 8 + 1, bitmaps, NULL);
                if (r < 0)
                        goto fail;
        }

        if (start)
                *start = saved_size;

        return 0;
fail:
        dns_packet_truncate(p, saved_size);
        return r;
}
