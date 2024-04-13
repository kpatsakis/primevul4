int dns_packet_append_label(DnsPacket *p, const char *d, size_t l, bool canonical_candidate, size_t *start) {
        uint8_t *w;
        int r;

        /* Append a label to a packet. Optionally, does this in DNSSEC
         * canonical form, if this label is marked as a candidate for
         * it, and the canonical form logic is enabled for the
         * packet */

        assert(p);
        assert(d);

        if (l > DNS_LABEL_MAX)
                return -E2BIG;

        r = dns_packet_extend(p, 1 + l, (void**) &w, start);
        if (r < 0)
                return r;

        *(w++) = (uint8_t) l;

        if (p->canonical_form && canonical_candidate) {
                size_t i;

                /* Generate in canonical form, as defined by DNSSEC
                 * RFC 4034, Section 6.2, i.e. all lower-case. */

                for (i = 0; i < l; i++)
                        w[i] = (uint8_t) ascii_tolower(d[i]);
        } else
                /* Otherwise, just copy the string unaltered. This is
                 * essential for DNS-SD, where the casing of labels
                 * matters and needs to be retained. */
                memcpy(w, d, l);

        return 0;
}
