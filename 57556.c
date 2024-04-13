static int dns_packet_extend(DnsPacket *p, size_t add, void **ret, size_t *start) {
        assert(p);

        if (p->size + add > p->allocated) {
                size_t a;

                a = PAGE_ALIGN((p->size + add) * 2);
                if (a > DNS_PACKET_SIZE_MAX)
                        a = DNS_PACKET_SIZE_MAX;

                if (p->size + add > a)
                        return -EMSGSIZE;

                if (p->_data) {
                        void *d;

                        d = realloc(p->_data, a);
                        if (!d)
                                return -ENOMEM;

                        p->_data = d;
                } else {
                        p->_data = malloc(a);
                        if (!p->_data)
                                return -ENOMEM;

                        memcpy(p->_data, (uint8_t*) p + ALIGN(sizeof(DnsPacket)), p->size);
                        memzero((uint8_t*) p->_data + p->size, a - p->size);
                }

                p->allocated = a;
        }

        if (start)
                *start = p->size;

        if (ret)
                *ret = (uint8_t*) DNS_PACKET_DATA(p) + p->size;

        p->size += add;
        return 0;
}
