int dns_packet_append_name(
                DnsPacket *p,
                const char *name,
                bool allow_compression,
                bool canonical_candidate,
                size_t *start) {

        size_t saved_size;
        int r;

        assert(p);
        assert(name);

        if (p->refuse_compression)
                allow_compression = false;

        saved_size = p->size;

        while (!dns_name_is_root(name)) {
                const char *z = name;
                char label[DNS_LABEL_MAX];
                size_t n = 0;

                if (allow_compression)
                        n = PTR_TO_SIZE(hashmap_get(p->names, name));
                if (n > 0) {
                        assert(n < p->size);

                        if (n < 0x4000) {
                                r = dns_packet_append_uint16(p, 0xC000 | n, NULL);
                                if (r < 0)
                                        goto fail;

                                goto done;
                        }
                }

                r = dns_label_unescape(&name, label, sizeof(label));
                if (r < 0)
                        goto fail;

                r = dns_packet_append_label(p, label, r, canonical_candidate, &n);
                if (r < 0)
                        goto fail;

                if (allow_compression) {
                        _cleanup_free_ char *s = NULL;

                        s = strdup(z);
                        if (!s) {
                                r = -ENOMEM;
                                goto fail;
                        }

                        r = hashmap_ensure_allocated(&p->names, &dns_name_hash_ops);
                        if (r < 0)
                                goto fail;

                        r = hashmap_put(p->names, s, SIZE_TO_PTR(n));
                        if (r < 0)
                                goto fail;

                        s = NULL;
                }
        }

        r = dns_packet_append_uint8(p, 0, NULL);
        if (r < 0)
                return r;

done:
        if (start)
                *start = saved_size;

        return 0;

fail:
        dns_packet_truncate(p, saved_size);
        return r;
}
