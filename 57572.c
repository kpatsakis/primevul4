void dns_packet_truncate(DnsPacket *p, size_t sz) {
        Iterator i;
        char *s;
        void *n;

        assert(p);

        if (p->size <= sz)
                return;

        HASHMAP_FOREACH_KEY(n, s, p->names, i) {

                if (PTR_TO_SIZE(n) < sz)
                        continue;

                hashmap_remove(p->names, s);
                free(s);
        }

        p->size = sz;
}
