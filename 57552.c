static int dns_packet_append_type_window(DnsPacket *p, uint8_t window, uint8_t length, const uint8_t *types, size_t *start) {
        size_t saved_size;
        int r;

        assert(p);
        assert(types);
        assert(length > 0);

        saved_size = p->size;

        r = dns_packet_append_uint8(p, window, NULL);
        if (r < 0)
                goto fail;

        r = dns_packet_append_uint8(p, length, NULL);
        if (r < 0)
                goto fail;

        r = dns_packet_append_blob(p, types, length, NULL);
        if (r < 0)
                goto fail;

        if (start)
                *start = saved_size;

        return 0;
fail:
        dns_packet_truncate(p, saved_size);
        return r;
}
